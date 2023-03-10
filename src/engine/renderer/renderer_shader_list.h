// Name, vfx, pfx, combination

SHADER(Transparent, common, transparent,"")
SHADER(Shadow, common, shadow, "")
SHADER(Skybox, fullscreen_quad, skybox, "")
SHADER(DeferredGeometry, deferred_geometry, deferred_geometry, "")
SHADER(DeferredLighting, fullscreen_quad, deferred_lighting, "")
SHADER(DeferredOmni, omni, omni, "")
SHADER(SSAO, fullscreen_quad, ssao, "")
SHADER(Blur, fullscreen_quad, blur, "HORIZONTAL")
SHADER(Outline, common, outline, "")
SHADER(OutlineHorizontal, fullscreen_quad, outline_horizontal, "")
SHADER(OutlineVertical, fullscreen_quad, outline_vertical, "")
SHADER(DOF, fullscreen_quad, dof, "FIRST|COC")
SHADER(ToneMapping, fullscreen_quad, tone_mapping, "")
SHADER(AA, fullscreen_quad, aa, "")
SHADER(2D, 2d, 2d, "")
SHADER(Debug, debug, debug, "")
SHADER(DisplayTexture, fullscreen_quad, display_texture, "")

#ifdef USE_EDITOR_V2
SHADER(EditorOutlineDraw, common, outline, "")
SHADER(EditorOutlineDraw2D, 2D, outline, "")
SHADER(EditorOutlineExtend, fullscreen_quad, editor_outline_extend, "")
SHADER(EditorPicking, common, editor_picking, "")
#endif