require "script/core"

params = {
----------------------------------------------------
-- engine
BlueNoiseTexture = "textures/BlueNoise.png";

-- sun / skybox
SunDir = Vec3f.new(-0.75,-1,-0.75);
SunColor = Color.new(0,0,0);
SkyboxTexture = "debug/UVChecker.png";

-- lighting
AmbientFactor = 0.30;
DiffuseFactor = 1.0;
SpecularFactor = 0.0;
EmissiveFactor = 1.0;
GlobalAmbient = Color.new(80,80,170);
ShadowStrength = 0.8;
NoLightBoost = 0.025;

-- shadow
ShadowSliceDist1 = 35;
ShadowSliceDist2 = 45;
ShadowSliceDist3 = 120;

-- bloom
BloomSize = 0.1;

-- outline
OutlineSize = 10; -- nb pixel

-- dof
DOFMinNearPlane = 0; -- meter
DOFMaxFarPlane = 100; -- meter
DOFNearPlaneFromFocus = 1; -- meter
DOFFarPlaneFromFocus = 100; -- meter

-- ssao
SSAORadius = 0.4; -- meter
SSAOScreenRadiusMin = 10; -- nb pixel
SSAOScreenRadiusMax = 100; -- nb pixel
SSAODotBias = 0.3;
SSAOMultiplier = 5.5;

-- vignette
VignetteColor = Color.new(25,25,25);
VignetteRadius = 0.7; -- radius in screen space (0.5=half screen)
VignetteSmoothness = 0.45; -- fade size (screen space)

----------------------------------------------------
 --DONT REMOVE THAT !!!
 };
 
function FillParamsWithTable(paramsTable)
	for k,v in pairs(params) do
		SetParamToTable(paramsTable,k,v);
	end
end