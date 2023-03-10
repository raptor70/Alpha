function Init(self)
	
	self:SetAlwaysDraw(true);
	
end

function Update(self,dtime)
	comp = World.GetInstance():GetComponent("gamemultiplayer");
	comp:DrawDebug();
	
	-- update start game button
	mustdisplay = false;
	if comp:IsInLobby() then
		lobby = comp:GetLobby();
		if lobby:IsMaster() and lobby:GetNbPlayers() >= 1  then
			mustdisplay = true;
		end
	end	
	
	to_game = World.GetInstance():GetEntity("to_game");
	if mustdisplay then
		to_game:Show();
	else
		to_game:Hide();
	end
end


function OnEvent(self,event)
	comp = World.GetInstance():GetComponent("gamemultiplayer");
	if event == "CREATE_LOBBY" then	
		if not comp:IsInLobby() then
			comp:CreateLobby();
		end
	elseif  event == "STOP_LOBBY" then
		if comp:IsInLobby() then
			comp:StopLobby();
		end
	elseif  event == "INVITE_LOBBY" then
		if comp:IsInLobby() then
			comp:InviteToLobby();
		end
	elseif  event == "TO_GAME" then
		if comp:IsInLobby() then
			lobby = comp:GetLobby();
			lobby:GoToGame();
		end
	elseif  event == "TO_GAME_SCRIPT" then
		self:GoToScript("script/game.lua");
	end
end