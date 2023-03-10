#import "View.h"

#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "game.h"
#include "renderer.h"
#include "ShaderTypes.h"

#define IS_OS_8_OR_LATER ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)

//#define FORCE_OPENGL

 void MetalInitialize(View* self)			{ [(id) self Initialize]; }
 void MetalClear(View* self, Bool _color, Bool _depth)				{ [(id) self Clear]; }
 void MetalSwap(View* self)					{ [(id) self Swap]; }
 void MetalEnableDepthTest(View* self)		{ [(id) self EnableDepthTest]; }
 void MetalDisableDepthTest(View* self)		{ [(id) self DisableDepthTest]; }
 void MetalEnableAlphaBlend(View* self)		{ [(id) self EnableAlphaBlend]; }
 void MetalDisableAlphaBlend(View* self)	{ [(id) self DisableAlphaBlend]; }
 void MetalSetMVPMatrix(View* self,const Mat44& _mat)	{ [(id) self SetMVPMatrix:_mat]; }
 void MetalBeginMaterial(View* self, const CMaterial* _pMaterial,const Color& _mainColor)	{ [(id) self BeginMaterial:_pMaterial color:_mainColor]; }
 void MetalAddTexture(View* self, CTexture* _pTexture)	{ [(id) self AddTexture:_pTexture]; }
 void MetalRemoveTexture(View* self, CTexture* _pTexture)	{ [(id) self RemoveTexture:_pTexture]; }
 void MetalDraw(View* self, GeometryType _type, const CVertexBuffer* _vb, const CIndexBuffer* _ib) { [(id) self Draw:_type vb:_vb ib:_ib]; }

static const uint32_t kInFlightCommandBuffers = 3;

static const float triangle[] =
{
    0.0, 0.5, 0.0,   0.0, 0.5,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,
    0.5, 0.0, 0.0,   0.0, 0.0,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0,
    -0.5, 0.0, 0.0,   1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 1.0, 1.0, 1.0
};

struct BufferMetal
{
    void* _basePointer;
    id<MTLBuffer> buffer;
    Bool used;
    
    BufferMetal()
    {
        _basePointer = NULL;
        buffer = nil;
        used = FALSE;
    }
};

@interface View ()
{   
	// OPENGL
    CAEAGLLayer *glLayer;
    EAGLContext *glContext;
	GLuint defaultFramebuffer;
	GLuint colorRenderbuffer;
	GLuint depthRenderbuffer;
}

@end

@implementation View
{
@private
	BOOL _layerSizeDidUpdate;
	
	// METAL
	__weak CAMetalLayer *_metalLayer;
	id <MTLDevice> _device;
    id <MTLTexture>  _depthTex;
    id <MTLTexture>  _stencilTex;
    id <MTLTexture>  _msaaTex;
    id <MTLCommandQueue> _commandQueue;
	id <MTLCommandBuffer> _commandBuffer;
    id <MTLLibrary> _defaultLibrary;
	id <MTLRenderCommandEncoder> _renderEncoder;
	id <MTLDepthStencilState> _depthState;
	id <MTLDepthStencilState> _nodepthState;
	id <MTLRenderPipelineState> _opaquePipeline;
	id <MTLRenderPipelineState> _transpPipeline;
	id <MTLRenderPipelineState> _additivePipeline;
	id <MTLRenderPipelineState> _currentPipeline;
	id <MTLTexture>	_textures[128];
	id <MTLSamplerState> _sampler;
    id <MTLBuffer>  _vertexBuffer;
    int32_t _vertexBufferNext;
    id <MTLBuffer>  _indexBuffer;
    int32_t _indexBufferNext;
	BOOL _useBlending;
	BOOL _useAdditive;
	int16_t _currentDraw;
	uniforms_t _uniforms[256];
    id <MTLBuffer> _uniformBuffer;
    dispatch_semaphore_t  m_InflightSemaphore;
    //BufferMetal _buffers[1024];
}
@synthesize currentDrawable      = _currentDrawable;
@synthesize renderPassDescriptor = _renderPassDescriptor;
    /**
 * Overrides a method in UIView that specifies the backing layer class
 * for the view. UIView is responsible for managing an instance.
 */
+ (Class)layerClass
{
#ifndef FORCE_OPENGL
	if( IS_OS_8_OR_LATER )
	{
		id <MTLDevice> device = MTLCreateSystemDefaultDevice();
		if( device )
			return [CAMetalLayer class];
	}
#endif
	
    return [CAEAGLLayer class];
}

- (id)init 
{
    self = [super init];
    if (self) 
    {
        _useMetal = NO;
    }
    return self;
}

/**
 * Custom view initialization goes here. See http://developer.apple.com/library/ios/documentation/uikit/reference/uiview_class/uiview/uiview.html#//apple_ref/occ/instm/UIView/initWithFrame:
 */
- (id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
    if (self)
    {
        [self initCommon];
    }
    
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    
    if(self)
    {
        //[self initCommon];
    }
    return self;
}

/**
 * Clears the screen to a particular color.
 */
- (void)setGLContext
{
	if( !_useMetal )
		[EAGLContext setCurrentContext:glContext];
}

/**
 * Clears the screen to a particular color.
 */
- (void)setNOGLContext
{
	if( !_useMetal )
		[EAGLContext setCurrentContext:nil];
}

- (void)beginrender
{
	if( !_useMetal )
		[self setGLContext];
		
	if(_layerSizeDidUpdate)
	{
		CGSize drawableSize = self.bounds.size;
		drawableSize.width  *= self.contentScaleFactor;
		drawableSize.height *= self.contentScaleFactor;
		
		if( _useMetal )
			_metalLayer.drawableSize = drawableSize;
		
		CGame::GetInstance().SetWindowParams(drawableSize.width,drawableSize.height,"Sob",TRUE);
		
		_layerSizeDidUpdate = NO;
	}
}

/**
 * Clears the screen to a particular color.
 */
- (void)endrender
{
	_currentDrawable    = nil;
	
	if( !_useMetal )
		[glContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)initCommon
{    
	self.opaque          = YES;
    self.backgroundColor = nil;
	self.contentScaleFactor = [UIScreen mainScreen].scale;
	
	_drawableSize = self.bounds.size;
	
    [self initMetal];
	if( !_useMetal )
	{
		[self initGL];
	}
}

- (void)initMetal
{
#ifndef FORCE_OPENGL
	if( IS_OS_8_OR_LATER )
	{
		_device = MTLCreateSystemDefaultDevice();
	}
#endif
	if( !_device )
	{
		return;
	}
	
    _metalLayer = (CAMetalLayer *)self.layer;
	
	// PAS dans le sample (peut etre Synchro (YES, YES)
    _metalLayer.presentsWithTransaction = NO;
    _metalLayer.drawsAsynchronously     = NO;
	
    _metalLayer.device          = _device;
    _metalLayer.pixelFormat     = MTLPixelFormatBGRA8Unorm;
    _metalLayer.framebufferOnly = YES;
    _useMetal = YES;
	
	_sampleCount = 4;
    _depthPixelFormat = MTLPixelFormatDepth32Float;
    _stencilPixelFormat = MTLPixelFormatInvalid;
    
    _defaultLibrary = [_device newDefaultLibrary];
    id <MTLFunction> fragmentProgram;
    id <MTLFunction> vertexProgram;
    if(_defaultLibrary)
    {
        fragmentProgram = [_defaultLibrary newFunctionWithName:@"lighting_fragment"];
        vertexProgram = [_defaultLibrary newFunctionWithName:@"lighting_vertex"];
    }
    
    MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStateDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthStateDesc.depthWriteEnabled = YES;
    _depthState = [_device newDepthStencilStateWithDescriptor:depthStateDesc];
    
    MTLDepthStencilDescriptor *nodepthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
    nodepthStateDesc.depthCompareFunction = MTLCompareFunctionAlways;
    nodepthStateDesc.depthWriteEnabled = YES;
    
    _nodepthState = [_device newDepthStencilStateWithDescriptor:nodepthStateDesc];
    
    NSError *errors = nil;
    MTLRenderPipelineDescriptor *renderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    renderPipelineDesc.sampleCount                     = _sampleCount;
    renderPipelineDesc.vertexFunction                  = vertexProgram;
    renderPipelineDesc.fragmentFunction                = fragmentProgram;
    renderPipelineDesc.depthAttachmentPixelFormat      = _depthPixelFormat;
    renderPipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    renderPipelineDesc.colorAttachments[0].blendingEnabled = NO;
    _opaquePipeline = [_device newRenderPipelineStateWithDescriptor:renderPipelineDesc error:&errors];
    
    MTLRenderPipelineDescriptor *transprenderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    transprenderPipelineDesc.sampleCount                     = _sampleCount;
    transprenderPipelineDesc.vertexFunction                  = vertexProgram;
    transprenderPipelineDesc.fragmentFunction                = fragmentProgram;
    transprenderPipelineDesc.depthAttachmentPixelFormat      = _depthPixelFormat;
    transprenderPipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    transprenderPipelineDesc.colorAttachments[0].blendingEnabled = YES;
    transprenderPipelineDesc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    transprenderPipelineDesc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    transprenderPipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    transprenderPipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    transprenderPipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    transprenderPipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    _transpPipeline = [_device newRenderPipelineStateWithDescriptor:transprenderPipelineDesc error:&errors];
    
    MTLRenderPipelineDescriptor *additiverenderPipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    additiverenderPipelineDesc.sampleCount                     = _sampleCount;
    additiverenderPipelineDesc.vertexFunction                  = vertexProgram;
    additiverenderPipelineDesc.fragmentFunction                = fragmentProgram;
    additiverenderPipelineDesc.depthAttachmentPixelFormat      = _depthPixelFormat;
    additiverenderPipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    additiverenderPipelineDesc.colorAttachments[0].blendingEnabled = YES;
    additiverenderPipelineDesc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    additiverenderPipelineDesc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    additiverenderPipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    additiverenderPipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorSourceAlpha;
    additiverenderPipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
    additiverenderPipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
    _additivePipeline = [_device newRenderPipelineStateWithDescriptor:additiverenderPipelineDesc error:&errors];
    
    MTLSamplerDescriptor* sampleDesc = [[MTLSamplerDescriptor alloc] init];
    sampleDesc.minFilter = MTLSamplerMinMagFilterLinear;
    sampleDesc.magFilter = MTLSamplerMinMagFilterLinear;
	sampleDesc.sAddressMode = MTLSamplerAddressModeRepeat;
	sampleDesc.rAddressMode = MTLSamplerAddressModeRepeat;
	sampleDesc.tAddressMode = MTLSamplerAddressModeRepeat;
    _sampler = [_device newSamplerStateWithDescriptor:sampleDesc];
    
    _uniformBuffer = [_device newBufferWithLength:128*256 options:0];
    _vertexBuffer = [_device newBufferWithLength:1024*1024 options:0];
    _indexBuffer = [_device newBufferWithLength:1024*1024 options:0];
    
    m_InflightSemaphore = dispatch_semaphore_create(kInFlightCommandBuffers);
}

- (void) setupRenderPassDescriptorForTexture:(id <MTLTexture>) texture
{
    // create lazily
    if (_renderPassDescriptor == nil)
        _renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // create a color attachment every frame since we have to recreate the texture every frame
    MTLRenderPassColorAttachmentDescriptor *colorAttachment = _renderPassDescriptor.colorAttachments[0];
    colorAttachment.texture = texture;
    
    // make sure to clear every frame for best performance
    colorAttachment.loadAction = MTLLoadActionClear;
    colorAttachment.clearColor = MTLClearColorMake(0.f, 0.f, 0.f, 1.0f);
    
    // if sample count is greater than 1, render into using MSAA, then resolve into our color texture
    if(_sampleCount > 1)
    {
        BOOL doUpdate =     ( _msaaTex.width       != texture.width  )
                        ||  ( _msaaTex.height      != texture.height )
                        ||  ( _msaaTex.sampleCount != _sampleCount   );
        
        if(!_msaaTex || (_msaaTex && doUpdate))
        {
            MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                            width: texture.width
                                                                                           height: texture.height
                                                                                        mipmapped: NO];
            desc.textureType = MTLTextureType2DMultisample;
            
            // sample count was specified to the view by the renderer.
            // this must match the sample count given to any pipeline state using this render pass descriptor
            desc.sampleCount = _sampleCount;
            
            _msaaTex = [_device newTextureWithDescriptor: desc];
        }
        
        // When multisampling, perform rendering to _msaaTex, then resolve
        // to 'texture' at the end of the scene
        colorAttachment.texture = _msaaTex;
        colorAttachment.resolveTexture = texture;
        
        // set store action to resolve in this case
        colorAttachment.storeAction = MTLStoreActionMultisampleResolve;
    }
    else
    {
        // store only attachments that will be presented to the screen, as in this case
        colorAttachment.storeAction = MTLStoreActionStore;
    } // color0
    
    // Now create the depth and stencil attachments
    
    if(_depthPixelFormat != MTLPixelFormatInvalid)
    {
        BOOL doUpdate =     ( _depthTex.width       != texture.width  )
                        ||  ( _depthTex.height      != texture.height )
                        ||  ( _depthTex.sampleCount != _sampleCount   );
        
        if(!_depthTex || doUpdate)
        {
            //  If we need a depth texture and don't have one, or if the depth texture we have is the wrong size
            //  Then allocate one of the proper size
            MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: _depthPixelFormat
                                                                                            width: texture.width
                                                                                           height: texture.height
                                                                                        mipmapped: NO];
            
            desc.textureType = (_sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
            desc.sampleCount = _sampleCount;
            
            _depthTex = [_device newTextureWithDescriptor: desc];
        
            MTLRenderPassDepthAttachmentDescriptor *depthAttachment = _renderPassDescriptor.depthAttachment;
            depthAttachment.texture = _depthTex;
            depthAttachment.loadAction = MTLLoadActionClear;
            depthAttachment.storeAction = MTLStoreActionDontCare;
            depthAttachment.clearDepth = 1.0;
        }
    } // depth
    
    if(_stencilPixelFormat != MTLPixelFormatInvalid)
    {
        BOOL doUpdate  =    ( _stencilTex.width       != texture.width  )
                        ||  ( _stencilTex.height      != texture.height )
                        ||  ( _stencilTex.sampleCount != _sampleCount   );
        
        if(!_stencilTex || doUpdate)
        {
            //  If we need a stencil texture and don't have one, or if the depth texture we have is the wrong size
            //  Then allocate one of the proper size
            MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: _stencilPixelFormat
                                                                                            width: texture.width
                                                                                           height: texture.height
                                                                                        mipmapped: NO];
            
            desc.textureType = (_sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
            desc.sampleCount = _sampleCount;
            
            _stencilTex = [_device newTextureWithDescriptor: desc];
        
            MTLRenderPassStencilAttachmentDescriptor* stencilAttachment = _renderPassDescriptor.stencilAttachment;
            stencilAttachment.texture = _stencilTex;
            stencilAttachment.loadAction = MTLLoadActionClear;
            stencilAttachment.storeAction = MTLStoreActionDontCare;
            stencilAttachment.clearStencil = 0;
        }
    } //stencil
}

- (MTLRenderPassDescriptor *) renderPassDescriptor
{
    id <CAMetalDrawable> drawable = self.currentDrawable;
    if(!drawable)
    {
        NSLog(@">> ERROR: Failed to get a drawable!");
        _renderPassDescriptor = nil;
    }
    else
    {
        [self setupRenderPassDescriptorForTexture: drawable.texture];
    }
    
    return _renderPassDescriptor;
}

- (id <CAMetalDrawable>) currentDrawable
{
    while (_currentDrawable == nil)
    {
        _currentDrawable = [_metalLayer nextDrawable];
        
        if(!_currentDrawable)
        {
            NSLog(@"CurrentDrawable is nil");
        }
    }
    
    return _currentDrawable;
}

/**
 * Sets up OpenGL.
 */
- (void)initGL
{    
    glLayer = (CAEAGLLayer *)self.layer;
    glLayer.contentsScale = [UIScreen mainScreen].scale;
    glLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking: @NO,
                                    kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8};
    glLayer.opaque = YES;

    glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!glContext) {
        NSLog(@"Unable to create EAGLContext");
        exit(1);
    }
    
    if (![EAGLContext setCurrentContext:glContext]) {
        NSLog(@"Unable to set current EAGLContext");
        exit(1);
    }
	
    [self createFramebufferGL];
}

- (void)createFramebufferGL
{
    [EAGLContext setCurrentContext:glContext];
    
    glGenFramebuffers(1, &defaultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    [glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&_framebufferwidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&_framebufferheight);
}

- (void)deleteFramebufferGL
{
    [EAGLContext setCurrentContext:glContext];
    
    if (defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if (colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
    if (depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

- (void)setContentScaleFactor:(CGFloat)contentScaleFactor
{
    [super setContentScaleFactor:contentScaleFactor];
	
	if( !_useMetal )
	{
		[self deleteFramebufferGL];
		[self createFramebufferGL];
	}

    _layerSizeDidUpdate = YES;
}
 
- (void)layoutSubviews
{
    [super layoutSubviews];
	
	if( !_useMetal )
	{
		[self deleteFramebufferGL];
		[self createFramebufferGL];
	}
    
    _layerSizeDidUpdate = YES;
}

//---------------------------------------------------------
// METAL
//---------------------------------------------------------

- (void)Initialize
{
	_commandQueue = [_device newCommandQueue];
	
	return;
}
/*
- (Bool)ChangeViewport(S32 _width, S32 _height)
{
	return TRUE;
}

- (Bool)Finalize(const CWindow3DScreen* _pScreen)
{
	return TRUE;
}

- (S32)GetMaxTextureSize()
{
}
*/
- (void)Clear
{
    
    dispatch_semaphore_wait(m_InflightSemaphore, DISPATCH_TIME_FOREVER);
	_commandBuffer = [_commandQueue commandBuffer];
	/*_renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:self.renderPassDescriptor];
	[_renderEncoder setDepthStencilState:_nodepthState];*/
	_currentDraw = 0;
										
	_useBlending = NO;
	_useAdditive = NO;
    
    _vertexBufferNext = 0;
    _indexBufferNext = 0;
    
}
/*
- (void)SetWireframe(Bool _bWireframe)
{
}*/

- (void)EnableDepthTest
{
	/*[_renderEncoder endEncoding];
	_renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:self.renderPassDescriptor];
	[_renderEncoder setDepthStencilState:_depthState];*/
}

- (void)DisableDepthTest
{
	//[_renderEncoder endEncoding];
	_renderEncoder = [_commandBuffer renderCommandEncoderWithDescriptor:self.renderPassDescriptor];
	[_renderEncoder setDepthStencilState:_nodepthState];
    
    [_renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    [_renderEncoder setFragmentSamplerState:_sampler atIndex:0];
}

- (void)EnableAlphaBlend
{
	_useBlending = YES;
}

- (void)DisableAlphaBlend
{
	_useBlending = NO;
}

- (void)BeginMaterial:(const CMaterial*)_pMaterial color:(const Color&)_mainColor
{
    Vec2f deltauv = Vec2f::Zero;
	Color col = Color::White;
	_useAdditive = NO;
	if( _pMaterial )
	{
		col = _pMaterial->GetColor() * _mainColor;
		
		deltauv = _pMaterial->GetUVDelta();
		if( _pMaterial->IsAdditive() )
			_useAdditive = YES;
		
		CTexture* pTexture = _pMaterial->GetTexture();
		if( pTexture && pTexture->GetNbLevels() > 0 )
		{
			//LOGGER_Log("BeginMaterial - Has Texture\n");
			deltauv.x *= pTexture->GetUVMaxX();
			deltauv.y *= pTexture->GetUVMaxY();

			if( pTexture->m_Id >= 0 )
			{
				pTexture->m_bUsed = TRUE;
                [_renderEncoder setFragmentTexture:_textures[pTexture->m_Id] atIndex:0];
			}
		}
		else
		{
			//glBindTexture(GL_TEXTURE_2D,0);
		}
	}
	
	_uniforms[_currentDraw].coorddelta.x = deltauv.x;
	_uniforms[_currentDraw].coorddelta.y = deltauv.y;
	_uniforms[_currentDraw].color.x = col.r;
	_uniforms[_currentDraw].color.y = col.g;
	_uniforms[_currentDraw].color.z = col.b;
	_uniforms[_currentDraw].color.w = col.a;
	
	if( _useBlending )
	{
		if( _useAdditive )
			[_renderEncoder setRenderPipelineState:_additivePipeline];
		else
			[_renderEncoder setRenderPipelineState:_transpPipeline];
	}
	else
		[_renderEncoder setRenderPipelineState:_opaquePipeline];
}

- (void)AddTexture:(CTexture*)_pTexture
{
	if( !_pTexture || _pTexture->GetNbLevels() == 0 || !_pTexture->m_MipMaps[0].m_Data)
        return;
		
	NSUInteger id = 0;
	for( id = 0; id < 128; id++)
	{
		if( _textures[id] == nil )
			break;
	}
	
	if( id == 128 )
		return;
		
	NSUInteger width = _pTexture->GetSizeX(0);
	NSUInteger height = _pTexture->GetSizeY(0);
	MTLTextureDescriptor *pTexDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                    width:width
                                                                                    height:height
                                                                                    mipmapped:NO];
	_textures[id] = [_device newTextureWithDescriptor:pTexDesc];
	
	const U8* buffer = _pTexture->GetData(0);
	U8* tmpbuffer = NEW U8[height*width*4];
	if( _pTexture->GetBytePerPixel(0) == 3 )
	{
		for(S32 y=0; y<height; y++)
			for(S32 x=0; x<width; x++)
			{
				tmpbuffer[y*width*4+x*4] = buffer[y*width*3+x*3];
				tmpbuffer[y*width*4+x*4+1] = buffer[y*width*3+x*3+1];
				tmpbuffer[y*width*4+x*4+2] = buffer[y*width*3+x*3+2];
				tmpbuffer[y*width*4+x*4+3] = 255;
			}
		buffer = tmpbuffer;
	}
	
	MTLRegion region = MTLRegionMake2D(0, 0, width, height);
	NSUInteger rowBytes = width*4;
	[_textures[id] replaceRegion:region
                    mipmapLevel:0
                    withBytes:buffer
                    bytesPerRow:rowBytes];
	DELETE[] _pTexture->m_MipMaps[0].m_Data;
	_pTexture->m_MipMaps[0].m_Data = NULL;
	
	/*MTLSamplerDescriptor *desc = [[MTLSamplerDescriptor alloc] init];
	desc.minFilter = MTLSamplerMinMagFilterLinear;
	desc.magFilter = MTLSamplerMinMagFilterLinear;
	desc.sAddressMode = MTLSamplerAddressModeRepeat;
	desc.tAddressMode = MTLSamplerAddressModeRepeat;
	_samplers[id] = [_device newSamplerStateWithDescriptor:desc];*/
	
	_pTexture->m_Id = id;
    
    DELETE[] tmpbuffer;
}

- (void)RemoveTexture:(CTexture*) _pTexture
{
	_textures[_pTexture->m_Id] = nil;
	_pTexture->m_Id = -1;
}

- (void)SetMVPMatrix:(const Mat44&) _mat
{
	memcpy(&(_uniforms[_currentDraw].mvp.columns[0]),&(_mat.xx),4*sizeof(Float));
    memcpy(&(_uniforms[_currentDraw].mvp.columns[1]),&(_mat.yx),4*sizeof(Float));
    memcpy(&(_uniforms[_currentDraw].mvp.columns[2]),&(_mat.zx),4*sizeof(Float));
    memcpy(&(_uniforms[_currentDraw].mvp.columns[3]),&(_mat.wx),4*sizeof(Float));
}

/*
- (void)UseShader()
{
}
*/
- (void)Swap
{
	[_renderEncoder endEncoding];
    __block dispatch_semaphore_t dispatchSemaphore = m_InflightSemaphore;
    [_commandBuffer addCompletedHandler:^(id <MTLCommandBuffer> cmdb){
        dispatch_semaphore_signal(dispatchSemaphore);
    }];
    [_commandBuffer presentDrawable:self.currentDrawable];
    [_commandBuffer commit];
}



- (void)Draw:(GeometryType)_type vb:(const CVertexBuffer*)_vb ib:(const CIndexBuffer*)_ib;
{
    if( !_vb || !_ib || _vb->GetVertexCount() == 0 )
		return;
		
	MTLPrimitiveType type = MTLPrimitiveTypeTriangle;
	switch(_type)
	{
	case Geometry_TrianglesList : type = MTLPrimitiveTypeTriangle; break;
	case Geometry_TrianglesStrip : type = MTLPrimitiveTypeTriangleStrip; break;
	}
    
    /*id <MTLBuffer>  vertexBuffer = [_device newBufferWithBytes:(void *)_vb->GetArray()
                                                        length:_vb->GetVertexCount()*VB_ValueCountPerVertex * sizeof( Float )
                                                       options:0];
    
    id <MTLBuffer>  indexBuffer = [_device newBufferWithBytes:(void *)_ib->GetPointer()
                                                       length:_ib->GetItemCount()*sizeof(uint32_t)
                                                      options:0];*/

    //void* test = (void*)NEW Float[2*4096];//malloc(16384);//_vb->GetVertexCount()*VB_ValueCountPerVertex * sizeof( Float ));
    
    uint8_t* content = (uint8_t *)[_vertexBuffer contents];
    memcpy(content+_vertexBufferNext,_vb->GetArray(),_vb->GetVertexCount()*VB_ValueCountPerVertex * sizeof( Float ));
    int32_t voffset =_vertexBufferNext;
    _vertexBufferNext+=_vb->GetVertexCount()*VB_ValueCountPerVertex * sizeof( Float );
    
    content = (uint8_t *)[_indexBuffer contents];
    memcpy(content+_indexBufferNext,_ib->GetPointer(),_ib->GetItemCount()*sizeof(uint32_t));
    int32_t ioffset =_indexBufferNext;
    _indexBufferNext+=_ib->GetItemCount()*sizeof(uint32_t);
    /*id <MTLBuffer>  vertexBuffer = [_device newBufferWithBytesNoCopy:(void *)_vb->GetArray()
												length:_vb->GetSize()*sizeof(Float)//_vb->GetVertexCount()*VB_ValueCountPerVertex * sizeof( Float )
                                                options:0
                                                deallocator:^(void *pointer, NSUInteger length) {}];*/
	
    /*id <MTLBuffer>  indexBuffer = [_device newBufferWithBytesNoCopy:(void *)_ib->GetPointer()
												length:_ib->GetSize()*sizeof(uint32_t)//_ib->GetItemCount()*sizeof(uint32_t)
                                                options:0
                                                deallocator:^(void *pointer, NSUInteger length) {}];*/
	
	[_renderEncoder setVertexBuffer:_vertexBuffer offset:voffset atIndex:0 ];
	uint8_t *bufferPointer = (uint8_t *)[_uniformBuffer contents];
	memcpy(bufferPointer+_currentDraw*sizeof(uniforms_t), &(_uniforms[_currentDraw]), sizeof(uniforms_t));
	[_renderEncoder setVertexBuffer:_uniformBuffer offset:_currentDraw*sizeof(uniforms_t) atIndex:1 ];
    [_renderEncoder 	drawIndexedPrimitives:type
					indexCount:_ib->GetItemCount() 
					indexType:MTLIndexTypeUInt32 
					indexBuffer:_indexBuffer
					indexBufferOffset:ioffset];
     
					
	_currentDraw++;
}
/*
- (void)DrawLine(const Vec3f& _a, const Vec3f& _b)
{
}

- (void)DrawLineStrip(const Vec3fArray& _vb)
{
}*/


@end