#include "renderer_driver_gl.h"

#include "2d/font.h"
#include "renderer/shader.h"
#include "core/sort.h"
#include "window/window_3dscreen.h"
#include "math/spline.h"
#include "resource/resource_manager.h"

#include "3d/material.h"
#include "gl_extension.h"
#include "3d/vertex_buffer.h"
#include "3d/index_buffer.h"
#include "renderer/shader_group.h"
#include "platform/platform_win32.h"

S32 g_GLDefaultFrameBuffer = 0;
S32 g_nbFrameBuffer = 0;
S32 g_nbTexture = 0;

#define GLATTRIB_Position 0
#define GLATTRIB_Normal 1
#define GLATTRIB_UV 2
#define GLATTRIB_Color 3

struct SDriverGLRendererData
{
	HDC		m_hDC = NULL;
	HGLRC	m_hRC = NULL;
	S32		m_iWidth = 0;
	S32		m_iHeight = 0;
};

struct SDriverGLResourceData
{
	GLuint	m_ID;
};

struct SDriverGLPrimitiveData
{
	GLuint	m_VBID;
	GLuint	m_IBID;
	GLuint	m_ID;
};

#ifdef USE_DEBUGOPENGL

CGLCheckError::CGLCheckError(const Char * _name, const Char* _file, S64 _line)
{
	m_Name = _name;
	m_File = _file;
	m_Line = _line;

	GLint error = glGetError();
	if (error)
	{
		Str sError;
		switch (error)
		{
		case GL_INVALID_ENUM: sError = "Invalid Enum"; break;
		case GL_INVALID_VALUE: sError = "Invalid Value"; break;
		case GL_INVALID_OPERATION: sError = "Invalid Operation"; break;
		case GL_STACK_OVERFLOW: sError = "Stack Overflow"; break;
		case GL_STACK_UNDERFLOW: sError = "Stack Underflow"; break;
		case GL_OUT_OF_MEMORY: sError = "Out Of Memory"; break;
		}
		Logger::GetInstance().Log(eLOG_Error, m_File, m_Line, "%s() glError : BEFORE %s(0x%x)\n", m_Name, sError.GetArray(), error);
		DEBUG_Break();
	}
}

//----------------------------------

CGLCheckError::~CGLCheckError()
{
	GLint error = glGetError();
    if(error) 
	{
		Str sError;
		switch (error)
		{
		case GL_INVALID_ENUM: sError = "Invalid Enum"; break;
		case GL_INVALID_VALUE: sError = "Invalid Value"; break;
		case GL_INVALID_OPERATION: sError = "Invalid Operation"; break;
		case GL_STACK_OVERFLOW: sError = "Stack Overflow"; break;
		case GL_STACK_UNDERFLOW: sError = "Stack Underflow"; break;
		case GL_OUT_OF_MEMORY: sError = "Out Of Memory"; break;
		}
		Logger::GetInstance().Log(eLOG_Error, m_File, m_Line, "%s() glError : %s(0x%x)\n", m_Name, sError.GetArray(), error);
		DEBUG_Break();
    }
}

//----------------------------------

void APIENTRY GLErrorMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	Str sSource = "";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:				sSource = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sSource = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	sSource = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		sSource = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:		sSource = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:				sSource = "Other"; break;
	}

	Str sType = "";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:				sType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	sType = "Deprecated Behavior"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	sType = "Undefined behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY:			sType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:			sType = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER:				sType = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:			sType = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:			sType = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:				sType = "Other"; break;
	}

	Str sSeverity = "";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
	{
		LOGGER_LogError("%s %s error (%d)- %s\n", sSource.GetArray(), sType.GetArray(), id, message);
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM:
	{
		LOGGER_LogWarning("%s %s warning (%d)- %s\n", sSource.GetArray(), sType.GetArray(), id, message);
		break;
	}
	case GL_DEBUG_SEVERITY_LOW:
	{
		LOGGER_Log("%s %s problem (%d)- %s\n", sSource.GetArray(), sType.GetArray(), id, message);
		break;
	}
	case GL_DEBUG_SEVERITY_NOTIFICATION:
	{
		LOGGER_Log("%s %s info (%d)- %s\n", sSource.GetArray(), sType.GetArray(), id, message);
		break;
	}
	}
}

#define CHECKERROR(_function)	CGLCheckError error_##_function(#_function,__FILE__,__LINE__);
#else
#define CHECKERROR(_function)	
#endif

//----------------------------------

RendererDriverGL::RendererDriverGL():SUPER()
{
	m_CurrentRenderState = RenderState_None;
	m_iCurrentFBO = 0;
	m_SharedRC = NULL;
}

//----------------------------------

Bool RendererDriverGL::Initialize()
{
	if (!SUPER::Initialize())
		return FALSE;

#ifdef WIN32
	WNDCLASSA windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = DefWindowProcA;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.lpszClassName = "Dummy_WGL";
	if (!RegisterClassA(&windowClass))
	{
		LOGGER_LogError("Can't Register Dummy window class\n");
		return FALSE;
	}

	HWND hDummyWnd = CreateWindowExA(
		0,
		windowClass.lpszClassName,
		"Dummy OpenGL Window",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		windowClass.hInstance,
		0);
	if (!hDummyWnd)
	{
		LOGGER_LogError("Can't Create a Dummy window.\n");
		return FALSE;
	}

	HDC hDummyDC = GetDC(hDummyWnd);
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 32;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixel_format = ChoosePixelFormat(hDummyDC, &pfd);
	if (!pixel_format)
	{
		LOGGER_LogError("Can't Find A Suitable PixelFormat.\n");
		return FALSE;
	}

	if (!SetPixelFormat(hDummyDC, pixel_format, &pfd)) 
	{
		LOGGER_LogError("Can't Set The PixelFormat.\n");
		return FALSE;
	}

	HGLRC dummy_context = wglCreateContext(hDummyDC);
	if (!dummy_context) 
	{
		LOGGER_LogError("Can't Create a GL Rendering Context.\n");
		return FALSE;
	}

	if (!wglMakeCurrent(hDummyDC, dummy_context)) 
	{
		LOGGER_LogError("Can't Activate The GL Rendering Context.\n");
		return FALSE;
	}

	GL_EXTENSION_Init();

	wglMakeCurrent(hDummyDC, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(hDummyWnd, hDummyDC);
	DestroyWindow(hDummyWnd);
#endif

	return TRUE;
}

//----------------------------------

Bool RendererDriverGL::Finalize()
{
	if (!SUPER::Finalize())
		return FALSE;
	return TRUE;
}

//----------------------------------

Bool RendererDriverGL::CreateContext(const Window3DScreen* _pScreen, Renderer* _pRenderer)
{
	if (!SUPER::CreateContext(_pScreen, _pRenderer))
		return FALSE;

	SDriverGLRendererData* pRendererData = NEW SDriverGLRendererData;
	_pRenderer->m_pDriverData = pRendererData;

#ifdef WIN32
	static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		32,							    // Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		32,								// 32Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	pRendererData->m_hDC = GetDC(_pScreen->GetPlatformData().GetDynamicCastPtr<WIN32ComponentPlatformData>()->m_hWnd);
	if (!pRendererData->m_hDC)							// Did We Get A Device Context?
	{
		LOGGER_LogError("Can't Create a Device Context.\n");
		return FALSE;
	}

	S32 pixel_format = ChoosePixelFormat(pRendererData->m_hDC, &pfd);
	if (!pixel_format)				// Did Windows Find A Matching Pixel Format?
	{
		LOGGER_LogError("Can't Find A Suitable PixelFormat.\n");
		return FALSE;
	}

	if (!SetPixelFormat(pRendererData->m_hDC, pixel_format, &pfd))				// Are We Able To Set The Pixel Format?
	{
		LOGGER_LogError("Can't Set The PixelFormat.\n");
		return FALSE;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (!m_SharedRC)
	{
		pRendererData->m_hRC = wglCreateContextAttribsARB(pRendererData->m_hDC, m_SharedRC, attribs);
		if (!pRendererData->m_hRC)					// Are We Able To Get A Rendering Context?
		{
			LOGGER_LogError("Can't Create a GL Rendering Context.\n");
			return FALSE;
		}
		m_SharedRC = pRendererData->m_hRC;
	}
	else
		pRendererData->m_hRC = m_SharedRC; // shared because PixelFormatDecriptor are comptabible
		

	if (!wglMakeCurrent(pRendererData->m_hDC, pRendererData->m_hRC))						// Try To Activate The Rendering Context
	{
		LOGGER_LogError("Can't Activate The GL Rendering Context.\n");
		return FALSE;
	}

	//wglSwapIntervalEXT(0);	// vsync (todo : check opengl version)
#endif

	glEnableVertexAttribArray(GLATTRIB_Position);
	glEnableVertexAttribArray(GLATTRIB_Normal);
	glEnableVertexAttribArray(GLATTRIB_UV);
	glEnableVertexAttribArray(GLATTRIB_Color);
	
	// init 
	m_iCurrentFBO = g_GLDefaultFrameBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, g_GLDefaultFrameBuffer);
	glViewport(0, 0, pRendererData->m_iWidth, pRendererData->m_iHeight);
	glClearDepth(1.f);

	return TRUE;
}

//----------------------------------

Bool RendererDriverGL::DestroyContext(const Window3DScreen* _pScreen, Renderer* _pRenderer)
{
	if (!SUPER::DestroyContext(_pScreen, _pRenderer))
		return FALSE;

	SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;

#ifdef WIN32
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(pRendererData->m_hRC);
	ReleaseDC(_pScreen->GetPlatformData().GetDynamicCastPtr<WIN32ComponentPlatformData>()->m_hWnd, pRendererData->m_hDC);
#endif

	DELETE pRendererData;
	_pRenderer->m_pDriverData = NULL;

	return TRUE;
}

//----------------------------------

Bool RendererDriverGL::ChangeViewport(Renderer* _pRenderer, S32 _width, S32 _height)
{
	if (!SUPER::ChangeViewport(_pRenderer, _width, _height))
		return FALSE;

	CHECKERROR(ChangeViewport);

	SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;
	pRendererData->m_iWidth= _width;
	pRendererData->m_iHeight = _height;

	//eglSwapInterval(eglGetCurrentDisplay(), 0);

	glClearColor( 0.f, 0.f, 0.f, 0.f );
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glPolygonMode( GL_FRONT, GL_FILL );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	m_CurrentRenderState = RenderState_None;
	SetRenderStateValue(RenderState_DepthTest,	FALSE);
	SetRenderStateValue(RenderState_DepthWrite, FALSE);
	SetRenderStateValue(RenderState_AlphaBlend,	FALSE);
	SetRenderStateValue(RenderState_FaceCulling, FALSE);
	SetRenderStateValue(RenderState_InvertCulling, FALSE);
	SetRenderStateValue(RenderState_Additive, FALSE);
		
	return TRUE;
}

//----------------------------------

S32 RendererDriverGL::GetMaxTextureSize()
{
	CHECKERROR(GetMaxTextureSize);
	GLint maxSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
	LOGGER_Log("OPENGL Texture max size : %d\n",maxSize);
	return maxSize;
}


//----------------------------------

void RendererDriverGL::BeginFrame(Renderer* _pRenderer) 
{
	CHECKERROR(BeginFrame);

	SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;

#if defined(WIN32)
	wglMakeCurrent(pRendererData->m_hDC, pRendererData->m_hRC);
#endif

#ifdef USE_DEBUGOPENGL
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLErrorMessage, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
#endif
}

//----------------------------------

void RendererDriverGL::SetWireframe(Bool _bWireframe)
{
	CHECKERROR(SetWireframe);
	#ifdef OPENGL_ES
	#else
	if ( _bWireframe )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else
	{
		glPolygonMode( GL_FRONT, GL_FILL );
	}
	#endif
}

//----------------------------------

void RendererDriverGL::EnableAdditive()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

//----------------------------------

void RendererDriverGL::DisableAdditive()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//----------------------------------

void RendererDriverGL::EnableClipRect(Renderer* _pRenderer, const Rect& _rect)
{
	glEnable(GL_SCISSOR_TEST);

	SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;
	Float botLeft = pRendererData->m_iHeight - (_rect.m_vTopLeft.y + _rect.m_vSize.y);
	glScissor((GLint)_rect.m_vTopLeft.x, (GLint)botLeft, (GLsizei)_rect.m_vSize.x, (GLsizei)_rect.m_vSize.y);
}

//----------------------------------

void RendererDriverGL::DisableClipRect()
{
	glDisable(GL_SCISSOR_TEST);
}

//----------------------------------

void RendererDriverGL::Swap(Renderer* _pRenderer)
{
	SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;

	CHECKERROR(Swap);
	//PROFILER_Begin(DRAW_Swap);
	//glFinish();

	#if defined(WIN32)
	SwapBuffers(pRendererData->m_hDC);
	#else
	#endif
	//PROFILER_End(DRAW_Swap);
}

//----------------------------------

void RendererDriverGL::Draw(GeometryType _type, const Primitive3D* _pPrimitive, U32 _indiceCount, U32 _ibOffset)
{
	UpdatePrimitive((Primitive3D*)_pPrimitive);

	CHECKERROR(Draw);
	SDriverGLPrimitiveData* pData = (SDriverGLPrimitiveData*)_pPrimitive->m_pDriverData;
	glBindVertexArray(pData->m_ID);

	U32 type = GL_TRIANGLES;
	switch (_type)
	{
		case Geometry_TrianglesList: type = GL_TRIANGLES; break;
		case Geometry_TrianglesStrip: type = GL_TRIANGLE_STRIP; break;
		case Geometry_Lines: type = GL_LINES; break;
		default:	DEBUG_Forbidden("Not supported"); break;
	}

	{
		CHECKERROR(glDrawElements);
		glDrawElements(type, _indiceCount, GL_UNSIGNED_SHORT, ((U16*)NULL + _ibOffset));
	}
}

//----------------------------------

void RendererDriverGL::AddTexture(Texture* _texture)
{
    if( _texture->GetNbLevels() < 1)
        return;

	g_nbTexture++;
    
	CHECKERROR(AddTexture);
	GLuint id = 0;
	glGenTextures(1, &id);
	if( id == 0 )
	{
		LOGGER_LogError("AddTexture : failed %s\n",_texture->GetName().GetStr().GetArray());
		return;
	}

	glBindTexture(GL_TEXTURE_2D, id);
	if( _texture->GetNbLevels() > 1 )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, _texture->GetNbLevels()-1);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//wrapping
	if( _texture->GetWrapping() == Texture::TEXTUREWRAP_Repeat )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT );
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	}

	// add mipmap levels
	for(S32 l=0; l<_texture->GetNbLevels(); l++ )
	{
		U32 sizeX = _texture->GetSizeX(l);
		U32 sizeY = _texture->GetSizeY(l);
		GLint internalFormat = 0;
		GLint format = 0;
		GLint datatype = 0;
		U32 bytePerPixel = 0;
		switch (_texture->GetFormat())
		{
			case Texture::TEXTUREFORMAT_DepthStencil:
			{
				datatype = GL_UNSIGNED_INT_24_8;
				internalFormat = GL_DEPTH24_STENCIL8;
				format = GL_DEPTH_STENCIL;
				bytePerPixel = sizeof(U32);
				break;
			}

			case Texture::TEXTUREFORMAT_R8:
			{
				datatype = GL_UNSIGNED_BYTE;
				internalFormat = GL_R8;
				format = GL_RED;
				bytePerPixel = sizeof(U8);
				break;
			}

			case Texture::TEXTUREFORMAT_RG8:
			{
				datatype = GL_UNSIGNED_BYTE;
				internalFormat = GL_RG8;
				format = GL_RG;
				bytePerPixel = 2 * sizeof(U8);
				break;
			}

			case Texture::TEXTUREFORMAT_RGB8:
			{
				datatype = GL_UNSIGNED_BYTE;
				internalFormat = GL_RGB8;
				format = GL_RGB;
				bytePerPixel = 3 * sizeof(U8);
				break;
			}
			case Texture::TEXTUREFORMAT_RGBA8:
			{
				datatype = GL_UNSIGNED_BYTE;
				internalFormat = GL_RGBA8;
				format = GL_RGBA;
				bytePerPixel = 4 * sizeof(U8);
				break;
			}

			case Texture::TEXTUREFORMAT_R16F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_R16F;
				format = GL_RED;
				bytePerPixel = sizeof(U16);
				break;
			}

			case Texture::TEXTUREFORMAT_RG16F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_RG16F;
				format = GL_RG;
				bytePerPixel = 2 * sizeof(U16);
				break;
			}

			case Texture::TEXTUREFORMAT_RGB16F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_RGB16F;
				format = GL_RGB;
				bytePerPixel = 3 * sizeof(U16);
				break;
			}

			case Texture::TEXTUREFORMAT_RGBA16F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_RGBA16F;
				format = GL_RGBA;
				bytePerPixel = 4 * sizeof(U16);
				break;
			}

			case Texture::TEXTUREFORMAT_R32F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_R32F;
				format = GL_RED;
				bytePerPixel = sizeof(U32);
				break;
			}

			case Texture::TEXTUREFORMAT_RGB32F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_RGB32F;
				format = GL_RGB;
				bytePerPixel = 3 * sizeof(U32);
				break;
			}

			case Texture::TEXTUREFORMAT_RGBA32F:
			{
				datatype = GL_FLOAT;
				internalFormat = GL_RGBA32F;
				format = GL_RGBA;
				bytePerPixel = 4 * sizeof(U32);
				break;
			}

			default: DEBUG_Forbidden("Unknown format"); break;
		}

		const U8* pSrcData = _texture->GetData(l);
		U8* pTempBuffer = NULL;
		if (pSrcData)
		{
			// copy from topleft first to lowerleft first
			pTempBuffer = NEW U8[_texture->GetDataSize(l)];
			U32 rowSize = sizeX * bytePerPixel;
			for (U32 i = 0; i < sizeY; i++)
			{
				memcpy(pTempBuffer + i * rowSize, pSrcData + (sizeY - 1 - i) * rowSize, rowSize);
			}
		}

		glTexImage2D(GL_TEXTURE_2D, l, internalFormat, sizeX, sizeY, 0, format, datatype, pTempBuffer);

		DELETE[] pTempBuffer;

		_texture->ReleaseData(l);
	}

	//if( _texture->GetNbLevels() == 1 )
	//	glGenerateMipmap(GL_TEXTURE_2D);

#ifdef USE_GPU_PROFILER
	glObjectLabel(GL_TEXTURE, id, -1, _texture->GetName().GetStr().GetArray());
#endif


	_texture->m_pDriverData = NEW SDriverGLResourceData({ id });
}

//----------------------------------

void RendererDriverGL::RemoveTexture(Texture* _pTexture)
{
	CHECKERROR(RemoveTexture);
	g_nbTexture--;
	SDriverGLResourceData* pData = (SDriverGLResourceData*)_pTexture->m_pDriverData;
	glDeleteTextures(1,&(pData->m_ID));
	DELETE pData;
	_pTexture->m_pDriverData = NULL;
}

//----------------------------------

void RendererDriverGL::UpdatePrimitive(Primitive3D* _pPrimitive)
{
	CHECKERROR(UpdatePrimitive);

	if (!_pPrimitive->m_pDriverData)
	{
		GLuint vaoId = 0;
		glGenVertexArrays(1, &vaoId);

		GLuint ids[2] = { 0 };
		glGenBuffers(2, ids);
		if (vaoId == 0 || ids[0] == 0 || ids[1] == 0)
		{
			LOGGER_LogError("AddPrimitive : failed %x\n", _pPrimitive);
			return;
		}

		VertexBuffer* pVB = (VertexBuffer*)_pPrimitive->GetVB().GetPtr();
		IndexBuffer* pIB = (IndexBuffer*)_pPrimitive->GetIB().GetPtr();
		GLenum usage = _pPrimitive->IsStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

		glBindVertexArray(vaoId);

		glBindBuffer(GL_ARRAY_BUFFER, ids[0]);
		glBufferData(GL_ARRAY_BUFFER, pVB->GetArraySize(), pVB->GetArray(), usage);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ids[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIB->m_Array.GetItemSize(), pIB->m_Array.GetPointer(), usage);

		glVertexAttribPointer(GLATTRIB_Position, 3, GL_FLOAT, GL_FALSE, VB_ValueCountPerVertex * sizeof(Float), ((Float*)NULL));
		glVertexAttribPointer(GLATTRIB_Normal, 3, GL_FLOAT, GL_FALSE, VB_ValueCountPerVertex * sizeof(Float), ((Float*)NULL + 5));
		glVertexAttribPointer(GLATTRIB_UV, 2, GL_FLOAT, GL_FALSE, VB_ValueCountPerVertex * sizeof(Float), ((Float*)NULL + 3));
		glVertexAttribPointer(GLATTRIB_Color, 4, GL_FLOAT, GL_FALSE, VB_ValueCountPerVertex * sizeof(Float), ((Float*)NULL + 8));

		glEnableVertexAttribArray(GLATTRIB_Position);
		glEnableVertexAttribArray(GLATTRIB_Normal);
		glEnableVertexAttribArray(GLATTRIB_UV);
		glEnableVertexAttribArray(GLATTRIB_Color);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef USE_GPU_PROFILER
		glObjectLabel(GL_VERTEX_ARRAY, vaoId, -1, _pPrimitive->GetName().GetStr().GetArray());
		glObjectLabel(GL_BUFFER, ids[0], -1, pVB->GetName().GetStr().GetArray());
		glObjectLabel(GL_BUFFER, ids[1], -1, pIB->GetName().GetStr().GetArray());
#endif

		_pPrimitive->m_pDriverData = NEW SDriverGLPrimitiveData({ ids[0], ids[1], vaoId });
	}
	else if (!_pPrimitive->IsStatic())// update data
	{
		SDriverGLPrimitiveData* pData = (SDriverGLPrimitiveData*)_pPrimitive->m_pDriverData;
		glBindVertexArray(pData->m_ID);

		VertexBuffer* pVB = (VertexBuffer*)_pPrimitive->GetVB().GetPtr();
		glBindBuffer(GL_ARRAY_BUFFER, pData->m_VBID);		
		glBufferData(GL_ARRAY_BUFFER, pVB->GetArraySize(), pVB->GetArray(), GL_DYNAMIC_DRAW);

		IndexBuffer* pIB = (IndexBuffer*)_pPrimitive->GetIB().GetPtr();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pData->m_IBID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIB->m_Array.GetItemSize(), pIB->m_Array.GetPointer(), GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
}

//----------------------------------

void RendererDriverGL::RemovePrimitive(void* _pDriverData)
{
	CHECKERROR(RemovePrimitive);
	SDriverGLPrimitiveData* pPrimitiveData = (SDriverGLPrimitiveData*)_pDriverData;

	GLuint ids[2] = { pPrimitiveData->m_VBID,  pPrimitiveData->m_IBID };
	glDeleteBuffers(2, ids);
	glDeleteVertexArrays(1, &(pPrimitiveData->m_ID));

	DELETE pPrimitiveData;
}

//----------------------------------

void RendererDriverGL::CompileShaderGroup(ShaderGroup* _shaderGroup)
{
	CHECKERROR(CompileShaderGroup);

	// already compiled
	if (_shaderGroup->m_pDriverData)
	{
		return;
	}

	// compiles each shaders
	Shader* vertex = (Shader*)_shaderGroup->m_Shaders[SHADER_Vertex].GetPtr();
	Shader* pixel = (Shader*)_shaderGroup->m_Shaders[SHADER_Pixel].GetPtr();
	Shader* header = (Shader*)_shaderGroup->m_Shaders[SHADER_Header].GetPtr();
	CompileShader(vertex, header);
	CompileShader(pixel, header);

	// link them into program
	GLuint program = glCreateProgram();
    if (program) 
	{
		SDriverGLResourceData* pVertexData = (SDriverGLResourceData*)vertex->m_pDriverData;
		SDriverGLResourceData* pPixelData = (SDriverGLResourceData*)pixel->m_pDriverData;
		glAttachShader(program, pVertexData->m_ID);
		glAttachShader(program, pPixelData->m_ID);

		glBindAttribLocation(program, GLATTRIB_Position, "a_vVertexPosition");
		glBindAttribLocation(program, GLATTRIB_Normal, "a_vVertexNormal");
		glBindAttribLocation(program, GLATTRIB_UV, "a_vVertexUV");
		glBindAttribLocation(program, GLATTRIB_Color, "a_vVertexColor");

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) 
		{
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) 
			{
                Char* buf = NEW Char[bufLength];
                if (buf) 
				{
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGGER_LogError("Could not link program:\n%s\n", buf);

                    DELETE[] buf;
                }
            }
            glDeleteProgram(program);
            program = 0;
			return;
        }
    }

#ifdef USE_GPU_PROFILER
	glObjectLabel(GL_PROGRAM, program, -1, _shaderGroup->GetName().GetStr().GetArray());
#endif

	_shaderGroup->m_pDriverData = NEW SDriverGLResourceData({ program });
}

//----------------------------------

void RendererDriverGL::RemoveShaderGroup(ShaderGroup* _shaderGroup) 
{
	CHECKERROR(RemoveShaderGroup);
	for (S32 i = 0; i < _countof(_shaderGroup->m_Shaders); i++)
	{
		if (_shaderGroup->m_Shaders[i].IsValid())
		{
			Shader* pShader = (Shader*)(_shaderGroup->m_Shaders[i].GetPtr());
			if (pShader->m_pDriverData)
			{
				SDriverGLResourceData* pData = (SDriverGLResourceData*)pShader->m_pDriverData;
				glDeleteShader(pData->m_ID);
				DELETE pData;
				pShader->m_pDriverData = NULL;
			}
		}
	}

	if(_shaderGroup->m_pDriverData )
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_shaderGroup->m_pDriverData;
		glDeleteProgram(pShaderGroupData->m_ID);		
		DELETE pShaderGroupData;
		_shaderGroup->m_pDriverData = NULL;
	}
}

//----------------------------------

void RendererDriverGL::UseShaderGroup(const ShaderGroup* _shaderGroup) 
{
	SUPER::UseShaderGroup(_shaderGroup);

	CHECKERROR(UseShaderGroup);
	if(_shaderGroup->m_pDriverData)
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_shaderGroup->m_pDriverData;
		glUseProgram(pShaderGroupData->m_ID);
	}
}

//----------------------------------

S32 RendererDriverGL::GetShaderAttribute(ShaderGroup* _shaderGroup, const Char* _name)
{
	CHECKERROR(GetShaderAttribute);
	if(_shaderGroup->m_pDriverData)
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_shaderGroup->m_pDriverData;
		return (S32)glGetAttribLocation(pShaderGroupData->m_ID, _name);
	}

	return -1;
}

//----------------------------------
	
S32 RendererDriverGL::GetShaderUniform(ShaderGroup* _shaderGroup, const Char* _name)
{
	CHECKERROR(GetShaderUniform);
	if(_shaderGroup->m_pDriverData)
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_shaderGroup->m_pDriverData;
		return (S32)glGetUniformLocation(pShaderGroupData->m_ID, _name);
	}

	return -1;
}

//----------------------------------

S32 RendererDriverGL::GetShaderUniformData(ShaderGroup* _shaderGroup, const Char* _name)
{
	CHECKERROR(GetShaderUniform);
	if (_shaderGroup->m_pDriverData)
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_shaderGroup->m_pDriverData;
		return (S32)glGetUniformBlockIndex(pShaderGroupData->m_ID, _name);
	}

	return -1;
}

//----------------------------------

void RendererDriverGL::ActiveTexture(U32 _slot, const ShaderGroup::Params& _params, Texture* _pTexture)
{
	CHECKERROR(ActiveTexture);
	glActiveTexture(GL_TEXTURE0 + _slot);
	if( _pTexture )
	{
		DEBUG_Require(_pTexture->m_pDriverData);
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_pTexture->m_pDriverData;
		glBindTexture(GL_TEXTURE_2D, pShaderGroupData->m_ID);
	}
	else
		glBindTexture(GL_TEXTURE_2D, 0);

	S32 id = m_pCurrentShader->GetParamId(_params);
	if (id >= 0)
		glUniform1i(id, (S32)_slot);
}

//----------------------------------

void RendererDriverGL::PushShaderData(ShaderData* _pData)
{
	CHECKERROR(PushShaderData);

	if (!_pData->m_pDriverData)
	{
		GLuint ubo = 0;
		glGenBuffers(1, &ubo);
		_pData->m_pDriverData = NEW SDriverGLResourceData({ ubo });	

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, _pData->GetDataSize(), _pData->GetDataPtr(), GL_DYNAMIC_DRAW);

#ifdef USE_GPU_PROFILER
		glObjectLabel(GL_BUFFER, ubo, -1, _pData->GetName().GetStr().GetArray());
#endif
	}
	else
	{
		SDriverGLResourceData* pDriverData = (SDriverGLResourceData*)_pData->m_pDriverData;
		glBindBuffer(GL_UNIFORM_BUFFER, pDriverData->m_ID);
		glBufferData(GL_UNIFORM_BUFFER, _pData->GetDataSize(), _pData->GetDataPtr(), GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//----------------------------------

void RendererDriverGL::RemoveShaderData(void* _pDriverData)
{
	SDriverGLResourceData* pShaderData = (SDriverGLResourceData*)_pDriverData;
	glDeleteBuffers(1, &pShaderData->m_ID);
	DELETE pShaderData;
}

//----------------------------------

void RendererDriverGL::UseShaderData(const ShaderGroup::Params& _params, ShaderData* _pData, U32 _iSlot)
{
	CHECKERROR(UseShaderData);
	S32 id = m_pCurrentShader->GetParamId(_params);
	if (id >= 0)
	{
		SDriverGLResourceData* pDriverData = (SDriverGLResourceData*)_pData->m_pDriverData;
		glBindBufferBase(GL_UNIFORM_BUFFER, _iSlot, pDriverData->m_ID);

		SDriverGLResourceData* pShaderData = (SDriverGLResourceData*)m_pCurrentShader->m_pDriverData;
		glUniformBlockBinding(pShaderData->m_ID, id, _iSlot);
	}
}

//----------------------------------

void RendererDriverGL::PushDrawShaderData(const ShaderGroup::Params& _params, ShaderData* _pData)
{
	CHECKERROR(PushDrawShaderData);
	PushShaderData(_pData);
	S32 id = m_pCurrentShader->GetParamId(_params);
	if (id >= 0)
	{
		SDriverGLResourceData* pDriverData = (SDriverGLResourceData*)_pData->m_pDriverData;
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, pDriverData->m_ID);

		SDriverGLResourceData* pShaderData = (SDriverGLResourceData*)m_pCurrentShader->m_pDriverData;
		glUniformBlockBinding(pShaderData->m_ID, id, 2);
	}
}

//----------------------------------

void RendererDriverGL::CompileShader(Shader* _shader, Shader* _header)
{
	CHECKERROR(CompileShader);

	// already compile
	if (_shader->m_pDriverData) 
	{
		return;
	}

	ShaderSource* pShaderSource = _shader->GetSource().GetCastPtr<ShaderSource>();
	ShaderSource* pHeaderSource = NULL;
	if(_header)
		pHeaderSource = _header->GetSource().GetCastPtr<ShaderSource>();

	GLenum shaderType = 0;
	Str sourceType = "";
	switch(pShaderSource->GetType())
	{
	case SHADER_Vertex: shaderType = GL_VERTEX_SHADER; sourceType = "#define VFX\n";  break;
	case SHADER_Pixel : shaderType = GL_FRAGMENT_SHADER; sourceType = "#define PFX\n"; break;
	default : 
		{
			DEBUG_Forbidden("Unknown shader type !!\n");
			return;
		}
	}

	// create new shader
	GLuint shader = glCreateShader(shaderType);
    if (shader) 
	{
		Str codeHeader = "#version 450\n";
		codeHeader += "#define TEXTURE_LOWERLEFT\n";
		codeHeader += sourceType;
		for (const Str& f : _shader->GetFlags())
			codeHeader += Str("#define %s 1\n", f.GetArray());
		codeHeader += "\n";
		Str sourceStr = codeHeader;
		if (pHeaderSource)
		{
			sourceStr += pHeaderSource->GetSource().GetArray();
			sourceStr += "\n";
		}
		sourceStr += pShaderSource->GetSource().GetArray();
		const Char* source = sourceStr.GetArray();
		glShaderSource(shader, 1, &source, NULL);

        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (!compiled) 
		{
			if (infoLen)
			{
				Char* buf = NEW Char[infoLen];
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGGER_LogError("Could not compile shader %s:\n", _shader->GetPath().GetArray());
					DisplayGLError(buf, codeHeader, _header, _shader, eLOG_Error);
					DELETE[] buf;
				}
			}
            glDeleteShader(shader);
            shader = 0;
        }
		else
		{
			if (infoLen)
			{
				Char* buf = NEW Char[infoLen];
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGGER_LogWarning("Warning in shader %s:\n%s\n", _shader->GetPath().GetArray(), buf);
					DisplayGLError(buf, codeHeader, _header, _shader, eLOG_Warning);
					DELETE[] buf;
				}
			}
		}
    }

#ifdef USE_GPU_PROFILER
	glObjectLabel(GL_SHADER, shader, -1, _shader->GetName().GetStr().GetArray());
#endif

	_shader->m_pDriverData = NEW SDriverGLResourceData({ shader });
}

//----------------------------------

void RendererDriverGL::DisplayGLError(const Str& _error, const Str& _codeHeader, const Shader* _header, const Shader* _shader, const LogType& _logType)
{
#ifdef USE_LOGGER
	StrArray lines;
	_error.Split("\n", lines);
	for (U32 i = 0; i < lines.GetCount(); i++)
	{
		Str& line = lines[i];
		StrArray words;
		U32 firstParenthesis = 1;
		U32 secondParenthesis = line.GetFirstIndexOfCharacter(')');
		U32 lineNumber = line.GetTextInsideRange(firstParenthesis + 1, secondParenthesis - 1).GetInteger();
		Str path = _shader->GetSource()->GetPath();

		U32 codeHeaderLineCount = _codeHeader.GetCharacterCount('\n') + 1;
		U32 HeaderLineCount = 0;
		Str headerPath;
		if (_header)
		{
			HeaderLineCount = _header->GetSource().GetCastPtr<ShaderSource>()->GetSource().GetCharacterCount('\n') + 1;
			headerPath = _header->GetPath();
		}

		if (lineNumber <= codeHeaderLineCount)
			path = "CODEHEADER";
		else if (lineNumber <= codeHeaderLineCount + HeaderLineCount)
		{
			lineNumber -= codeHeaderLineCount;
			path = headerPath;
		}
		else
		{
			lineNumber -= codeHeaderLineCount + HeaderLineCount;
		}
#ifdef WIN32 // TO REPLACE by a CFile::GetCurPath()
		HMODULE hModule = GetModuleHandleW(NULL);
		WCHAR WExePath[256];
		GetModuleFileNameW(hModule, WExePath, 256);
		Char ExePath[256];
		wcstombs(ExePath, WExePath, 256);
		Str dirPath = ExePath;
		S32 lastSlash = dirPath.GetLastIndexOfCharacter('\\');
		dirPath = dirPath.GetTextInsideRange(0, lastSlash);
		path = dirPath + path;
#endif

		Logger::GetInstance().Log(_logType, path.GetArray(), lineNumber, "%s\n", line.GetTextInsideRange(secondParenthesis + 1, line.GetLength() - 1).GetArray());
	}
#endif
}

//----------------------------------

void RendererDriverGL::AddFrameBuffer(FrameBuffer* _pFrameBuffer) 
{
	CHECKERROR(AddFrameBuffer);

	g_nbFrameBuffer++;

	GLuint id = 0;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	static GLuint g_attachments[17] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7, 
		GL_COLOR_ATTACHMENT8, 
		GL_COLOR_ATTACHMENT9, 
		GL_COLOR_ATTACHMENT10,
		GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15,
		//GL_COLOR_ATTACHMENT16,
		//GL_COLOR_ATTACHMENT17,
		//GL_COLOR_ATTACHMENT18,
		//GL_COLOR_ATTACHMENT19,
		//GL_COLOR_ATTACHMENT20,
		//GL_COLOR_ATTACHMENT21,
		//GL_COLOR_ATTACHMENT22,
		//GL_COLOR_ATTACHMENT23,
		//GL_COLOR_ATTACHMENT24,
		//GL_COLOR_ATTACHMENT25,
		//GL_COLOR_ATTACHMENT26,
		//GL_COLOR_ATTACHMENT27,
		//GL_COLOR_ATTACHMENT28,
		//GL_COLOR_ATTACHMENT29,
		//GL_COLOR_ATTACHMENT30,
		//GL_COLOR_ATTACHMENT31
	};

	DEBUG_Require(_pFrameBuffer->GetTextureCount()<=_countof(g_attachments));

	GLuint depthStencil = 0;
	S32 attachementsCount = 0;
	for(U32 i=0; i<_pFrameBuffer->GetTextureCount(); i++)
	{
		Texture* pTexture = _pFrameBuffer->GetTexture(i);
		SDriverGLResourceData* pTextureData = (SDriverGLResourceData*)pTexture->m_pDriverData;
		DEBUG_Require(pTextureData);
		if (pTexture->GetFormat() == Texture::TEXTUREFORMAT_DepthStencil)
		{
			CHECKERROR(IsDepthStencil);
			depthStencil = pTextureData->m_ID;
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, g_attachments[i], GL_TEXTURE_2D, pTextureData->m_ID, 0);
			attachementsCount++;
		}		
	}

	{
		CHECKERROR(glDrawBuffers);
		glDrawBuffers(attachementsCount, g_attachments);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
	}

#ifdef USE_GPU_PROFILER
	glObjectLabel(GL_FRAMEBUFFER, id, -1, _pFrameBuffer->GetName().GetStr().GetArray());
#endif

	_pFrameBuffer->m_pDriverData = NEW SDriverGLResourceData({ id });

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		Str sError;
		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: sError = "Incomplete Attachment"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: sError = "Missing Attachment"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: sError = "Incomplete Draw Buffer"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: sError = "Incomplete Read Buffer"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED: sError = "Unsupported"; break;
		}
		DEBUG_Forbidden("Unable to create Framebuffer !! - Error : %s",sError.GetArray());
	}
}

//----------------------------------

void RendererDriverGL::RemoveFrameBuffer(FrameBuffer* _pFrameBuffer)
{
	CHECKERROR(RemoveFrameBuffer);
	g_nbFrameBuffer--;
	
	SDriverGLResourceData* pData = (SDriverGLResourceData*)_pFrameBuffer->m_pDriverData;
	glDeleteFramebuffers(1,&(pData->m_ID));
	DELETE pData;
	_pFrameBuffer->m_pDriverData = NULL;
}

//----------------------------------

void RendererDriverGL::BeginPass(Renderer* _pRenderer, RendererPass* _pPass)
{
	CHECKERROR(BeginPass);
	FrameBuffer* pFrameBuffer = _pPass->m_pFrameBuffer;
	if (pFrameBuffer)
	{
		if (!pFrameBuffer->m_pDriverData)
			AddFrameBuffer(pFrameBuffer);

		SDriverGLResourceData* pData = (SDriverGLResourceData*)pFrameBuffer->m_pDriverData;
		m_iCurrentFBO = pData->m_ID;
		glBindFramebuffer(GL_FRAMEBUFFER, pData->m_ID);
		glViewport(0, 0, pFrameBuffer->GetTexture(0)->GetSizeX(), pFrameBuffer->GetTexture(0)->GetSizeY());
	}
	else
	{
		SDriverGLRendererData* pRendererData = (SDriverGLRendererData*)_pRenderer->m_pDriverData;
		m_iCurrentFBO = g_GLDefaultFrameBuffer;
		glBindFramebuffer(GL_FRAMEBUFFER, g_GLDefaultFrameBuffer);
		glViewport(0, 0, pRendererData->m_iWidth, pRendererData->m_iHeight);
	}

	if (_pPass->m_ClearFlag)
	{
		CHECKERROR(Clear);
		glClearColor(_pPass->m_ClearColor.r, _pPass->m_ClearColor.g, _pPass->m_ClearColor.b, _pPass->m_ClearColor.a);
		glClearDepth(_pPass->m_ClearDepth);

		GLbitfield flag = 0;
		if (_pPass->m_ClearFlag & Clear_Color)
			flag |= GL_COLOR_BUFFER_BIT;
		if (_pPass->m_ClearFlag & Clear_Depth)
		{
			SetRenderStateValue(RenderState_DepthWrite, TRUE);
			flag |= GL_DEPTH_BUFFER_BIT;
		}

		glClear(flag);
	}
}

//----------------------------------

void RendererDriverGL::BeginSubPass(RendererSubpass* _pSubpass)
{
	CHECKERROR(BeginPass);
	
	SetRenderState(_pSubpass->m_RenderState);

	// shader
	if (_pSubpass->m_pShader->m_pDriverData)
	{
		SDriverGLResourceData* pShaderGroupData = (SDriverGLResourceData*)_pSubpass->m_pShader->m_pDriverData;
		glUseProgram(pShaderGroupData->m_ID);
		m_pCurrentShader = _pSubpass->m_pShader;
	}
}

//----------------------------------

void RendererDriverGL::SetRenderState(const RenderState& _newstate)
{
	Bool bChanged = FALSE;
	RenderState changedState = _newstate ^ m_CurrentRenderState;
	while (changedState)
	{
		RenderState mask = RenderState(U32(changedState) - 1) ^ changedState;
		RenderState changedFlag = changedState & mask;
		SetRenderStateValue(changedFlag, _newstate & changedFlag);
		changedState &= (~changedFlag);
		bChanged = TRUE;
	}
	m_CurrentRenderState = _newstate;
	if (bChanged)
	{
		if (m_CurrentRenderState & RenderState_AlphaBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else if (m_CurrentRenderState & RenderState_Additive)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		else
		{
			glDisable(GL_BLEND);			
		}
	}
}

//----------------------------------

void RendererDriverGL::SetRenderStateValue(const RenderState& _flag, Bool _bEnable)
{
	switch (_flag)
	{
	case RenderState_DepthTest:
	{
		if (_bEnable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		break;
	}
	case RenderState_DepthWrite:
	{
		glDepthMask(_bEnable);
		break;
	}
	case RenderState_FaceCulling:
	{
		if (_bEnable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
		break;
	}
	case RenderState_InvertCulling:
	{
		if (_bEnable)
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);
		break;
	}
	}
}

//----------------------------------

U32 RendererDriverGL::ReadPixel(FrameBuffer* _pFrameBuffer, const Vec2i& _vPos)
{
	CHECKERROR(ReadPixel);
	DEBUG_Require(_pFrameBuffer->GetTexture(0)->m_pDriverData);
	DEBUG_Require(_pFrameBuffer->GetTexture(0)->GetFormat()==Texture::TEXTUREFORMAT_RGBA8);

	SDriverGLResourceData* pData = (SDriverGLResourceData*)_pFrameBuffer->m_pDriverData;
	if (_pFrameBuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, pData->m_ID);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, g_GLDefaultFrameBuffer);

	U32 data;
	glReadPixels(_vPos.x, _vPos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data);
	return data;
}

#ifdef USE_GPU_PROFILER
void RendererDriverGL::ProfilerBegin(const Char* _pEvent)
{
	CHECKERROR(ProfilerBegin);
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION,0,-1,_pEvent);
}

//----------------------------------

void RendererDriverGL::ProfilerEnd()
{
	CHECKERROR(ProfilerEnd);
	glPopDebugGroup();
}
#endif