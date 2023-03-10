game = Game.GetInstance();
game:UseSteam(false);
game:UseEditor(true);
--game:SetReferenceRes(1024,768);
--game:SetWindowParams(1920,1080,"GAME",FALSE);
game:SetWindowParams(1280,720,"GAME",FALSE);
game:SetStartScript("script/game.lua");
--game:SetStartScript("script/diorama.lua");
--game:SetStartScript("script/test_mesh.lua");
game:SetMainThemeSoundEvent("event:/Musiques/Track");



fontmanager = FontManager.GetInstance();
fontmanager:LoadFont("debug","debug/courbd.ttf");
fontmanager:LoadFont("title","font/OpenSans-Light.ttf");
fontmanager:LoadFont("hud","font/OpenSans-Regular.ttf");
fontmanager:LoadFont("Tuto","font/OpenSans-Light.ttf");

texturemamanger = TextureManager.GetInstance();
texturemamanger:RegisterAtlas("textures/Menu");