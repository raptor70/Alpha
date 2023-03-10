
function Init(self)

	self:SetAlwaysDraw(true);
	self:SetAlwaysUpdate(true);
	
	-- lobby ready ok
	comp = World.GetInstance():GetComponent("gamemultiplayer");
	--if not comp:IsInLobby() then
		--self:GoToScript("script/UI/lobby.lua");
	--else					
		-- add hud
		--self:PushScript("script/UI/HUD.lua");	
	--end
end

function OnEvent(self,event)
end