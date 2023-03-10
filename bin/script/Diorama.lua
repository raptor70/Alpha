require "script/entities"

function Init(self)

	randomNumber = function(x)
		local r = math.random(1,30)
		return x[r]
	end
	
	randomNumberWater = function(x)
		local r = math.random(1,5)
		return x[r]
	end
	
	RowSize = 30;
	ColumnSize = 44;
	meshArray = {"Grid_Cell_B","Grid_Cell_C","Grid_Cell_D","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell","Grid_Cell"};
	bankArray = {"Grid_Cell_Water","Grid_Cell_Water_B","Grid_Cell_Water_C","Grid_Cell_Water_D","Grid_Cell_Water_E"};
	xAxis = 0;
	yAxis = 0;

	Row = 0;
	Column = 0;
	
	param = GetEntityParam("simple_mesh");
	GridGenerator = CreateEntity(self,"GridGenerator",param,Vec3f.new(0,0,0));
			comp = world:AddNewComponentToEntity(GridGenerator,"Animation");
			comp:SetDuration(1.5);
			comp:SetLoop(false);
			comp:AddPosKey(Vec3f.new(0,-1,0),0);
			comp:AddPosKey(Vec3f.new(0,-1,0),0.25);
			comp:AddPosKey(Vec3f.new(0,0.15,-0),1);

	
	param = GetEntityParam("Water");
	entity = CreateEntity(self,"Water",param,Vec3f.new((ColumnSize/2)-0.5,0,(RowSize/2)-0.5),GridGenerator);
	entity:SetScale(Vec3f.new(ColumnSize,1,RowSize));
	
	repeat
		if Column == ((ColumnSize/2)) then
			if Row < ((RowSize/2)+1) and Row > ((RowSize/2)-2) then
				param = GetEntityParam("Grid_Cell_Water_Center");
				entity = CreateEntity(self,"Grid_Cell_Water_Center",param,Vec3f.new(yAxis-0.5,0,xAxis+0.5),GridGenerator);
				xAxis = xAxis+2;
				Row = Row+2;
			else	
		
				mesh = randomNumberWater(bankArray);
				param = GetEntityParam(mesh);
				entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis),GridGenerator);
				rot = Quat.new(math.rad(270),Vec3f.new(0,1,0));
				entity:SetWorldRot(rot);
				
				if Row == 0 then
					param = GetEntityParam("Grid_Cell_Water_Border");
					entity = CreateEntity(self,"Grid_Cell_Water_Border",param,Vec3f.new(yAxis-0.5,0,xAxis-0.5),GridGenerator);
				end
				
				xAxis = xAxis+1;
				Row = Row+1;
				
				if Row == RowSize then
					yAxis = yAxis+1;
					xAxis = 0;
					Row = 0;
					Column = Column+1;
				end
			end
		
		elseif Column == ((ColumnSize/2)-1) then
		
			if Row < ((RowSize/2)+1) and Row > ((RowSize/2)-2) then
				xAxis = xAxis+2;
				Row = Row+2;
			else
				mesh = randomNumberWater(bankArray);
				param = GetEntityParam(mesh);
			entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis),GridGenerator);
			rot = Quat.new(math.rad(90),Vec3f.new(0,1,0));
			entity:SetWorldRot(rot);
			xAxis = xAxis+1;
			Row = Row+1;
			
				if Row == RowSize then
					yAxis = yAxis+1;
					xAxis = 0;
					Row = 0;
					Column = Column+1;
				end
			end
			
		else
			if Row < RowSize then
				if Row == ((RowSize/2)) then
					mesh = randomNumberWater(bankArray);
					param = GetEntityParam(mesh);
					entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis),GridGenerator);
					rot = Quat.new(math.rad(180),Vec3f.new(0,1,0));
					entity:SetWorldRot(rot);
					Row = Row + 1;
					xAxis = xAxis+1;
				elseif Row == ((RowSize/2)-1) then
					mesh = randomNumberWater(bankArray);
					param = GetEntityParam(mesh);
					entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis),GridGenerator);
					Row = Row + 1;
					xAxis = xAxis+1;
				else
					mesh = randomNumber(meshArray);
					param = GetEntityParam(mesh);
					entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis),GridGenerator);
					randRot = math.random(1, 4)
					--if randRot == 1 then
					--	rot = Quat.new(0,Vec3f.new(0,1,0));
					--	entity:SetWorldRot(rot);
					--elseif randRot == 2 then
					--	rot = Quat.new(90,Vec3f.new(0,1,0));
					--	entity:SetWorldRot(rot);
					--elseif randRot == 3 then
					--	rot = Quat.new(180,Vec3f.new(0,1,0));
					--	entity:SetWorldRot(rot);
					--else 	
					--	rot = Quat.new(270,Vec3f.new(0,1,0));
					--	entity:SetWorldRot(rot);
					--end
					
					xAxis = xAxis+1;
					Row = Row + 1;
				end
			end
			if Row == RowSize then
				yAxis = yAxis+1;
				xAxis = 0;
				Row = 0;
				Column = Column+1;
			end
		end
	until Column == ColumnSize;
end

