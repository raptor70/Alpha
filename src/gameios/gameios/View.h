#import <UIKit/UIKit.h>

/**
 * This class is a UIView that wraps a CAEAGLLayer, which is used by
 * Core Animation to render OpenGL content to the screen.
 *
 * An instance of this class will be attached to the main window as
 * a subview.
 */
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import "renderer.h"

@interface View : UIView

- (void)setGLContext;
- (void)setNOGLContext;
- (void)beginrender;
- (void)endrender;

- (void)Initialize;
- (void)Clear;
- (void)Swap;
- (void)EnableDepthTest;
- (void)DisableDepthTest;
- (void)SetMVPMatrix:(const Mat44&) _mat;
- (void)EnableAlphaBlend;
- (void)DisableAlphaBlend;
- (void)BeginMaterial:(const CMaterial*)_pMaterial color:(const Color&)_mainColor;
- (void)AddTexture:(CTexture*)_pTexture;
- (void)RemoveTexture:(CTexture*)_pTexture;
- (void)Draw:(GeometryType)_type vb:(const CVertexBuffer*)_vb ib:(const CIndexBuffer*)_ib;

// OPENGL
@property (nonatomic) GLint framebufferwidth;
@property (nonatomic) GLint framebufferheight;

// METAL
@property (nonatomic) BOOL useMetal;
@property (nonatomic, readonly) id <CAMetalDrawable> currentDrawable;
@property (nonatomic, readonly) CGSize drawableSize;
@property (nonatomic, readonly) MTLRenderPassDescriptor *renderPassDescriptor;
@property (nonatomic) MTLPixelFormat depthPixelFormat;
@property (nonatomic) MTLPixelFormat stencilPixelFormat;
@property (nonatomic) NSUInteger     sampleCount;

@end