function Init(self)
	
	self:SetAlwaysDraw(true);
	self:SetAlwaysUpdate(true);
	
end

function Update(self,dtime)
	world = World.GetInstance();
	comp = world:GetComponent("gamemultiplayer");
	lobby = comp:GetLobby();
	timer = world:GetEntity("Timer");
	timer:SetParamStr("TextValue",string.format("%.2fs left", lobby:GetTimeLeft()));
end



function OnEvent(self,event)

end