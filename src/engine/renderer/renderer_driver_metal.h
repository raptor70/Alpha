#ifndef __RENDERER_DRIVER_METAL_H__
#define __RENDERER_DRIVER_METAL_H__

#ifdef IOS
#include "View.h"
#endif

#ifdef USE_METAL
void MetalInitialize(View* self);
void MetalClear(View* self);
void MetalSwap(View* self);
void MetalEnableDepthTest(View* self);
void MetalDisableDepthTest(View* self);
void MetalEnableAlphaBlend(View* self);
void MetalDisableAlphaBlend(View* self);
void MetalBeginMaterial(View* self, const Material* _pMaterial,const Color& _mainColor);
void MetalDraw(View* self,GeometryType _type, const VertexBuffer* _vb, const IndexBuffer* _ib);
void MetalSetMVPMatrix(View* self,const Mat44& _mat);
void MetalAddTexture(View* self,Texture* _pTexture);
void MetalRemoveTexture(View* self,Texture* _pTexture);

BEGIN_SUPER_CLASS(RendererDriverMetal,RendererDriver)
public:
	RendererDriverMetal(){}
	virtual Bool Initialize(Window3DScreen* _pScreen) OVERRIDE		{ MetalInitialize(g_pView); return TRUE; }
	virtual Bool ChangeViewport(S32 _width, S32 _height) OVERRIDE			{ return TRUE; }
	virtual Bool Finalize(Window3DScreen* _pScreen) OVERRIDE			{ return TRUE; }

	virtual S32 GetMaxTextureSize() OVERRIDE	{ return 4096; }

	virtual void Clear(Bool _color, Bool _depth) OVERRIDE { MetalClear(g_pView,_color,_depth);}
	virtual void SetWireframe(Bool _bWireframe) OVERRIDE {}
	virtual void EnableDepthTest() OVERRIDE { MetalEnableDepthTest(g_pView); }
	virtual void DisableDepthTest() OVERRIDE { MetalDisableDepthTest(g_pView); }
	virtual void EnableAlphaBlend() OVERRIDE { MetalEnableAlphaBlend(g_pView); }
	virtual void DisableAlphaBlend() OVERRIDE { MetalDisableAlphaBlend(g_pView); }
	virtual void BeginMaterial(const Material* _pMaterial, const Color& _mainColor) OVERRIDE { MetalBeginMaterial(g_pView,_pMaterial,_mainColor); }
	virtual void AddTexture(Texture* _pTexture) OVERRIDE { MetalAddTexture(g_pView,_pTexture); }
	virtual void RemoveTexture(Texture* _pTexture) OVERRIDE { MetalRemoveTexture(g_pView,_pTexture);}
	virtual void SetMVPMatrix(const Mat44& _mat) OVERRIDE { MetalSetMVPMatrix(g_pView,_mat); }
	virtual void UseShader() OVERRIDE {}
	virtual void Swap(Renderer* _pRenderer) OVERRIDE { MetalSwap(g_pView);}

	virtual void Draw(GeometryType _type, const VertexBuffer* _vb, const IndexBuffer* _ib) OVERRIDE { MetalDraw(g_pView,_type,_vb,_ib); }
	virtual void DrawLine(const Vec3f& _a, const Vec3f& _b) OVERRIDE {}
	virtual void DrawLineStrip(const Vec3fArray& _vb) OVERRIDE {}
	
	static View* g_pView;
END_SUPER_CLASS
#endif

#endif