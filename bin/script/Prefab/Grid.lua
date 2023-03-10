require "script/entities"

function Init(self)

	randomNumber = function(x)
		local r = math.random(1,2)
		return x[r]
	end
	
	meshArray = {"Grid_Cell","Grid_Cell_Building"}
	xAxis = 0
	yAxis = 0

	r = 1
			
	repeat
		mesh = randomNumber(meshArray)
		param = GetEntityParam(mesh);
		entity = CreateEntity(self,mesh,param,Vec3f.new(yAxis,0,xAxis));
		xAxis = xAxis+1;
	r = r + 1;
	until r == 30
	
end

	--new_entity = self:CreateNewEntity("...");
	--new_entity = script:CreateNewEntityFromParent("...",parent_entity);
	--new_entity:SetPos(pos);
	--world:AddNewComponentToEntity(new_entity,"Mesh");
	--new_entity:SetMeshPath("mesh/Grid_Cell_Building.DAE");
	--new_entity:GetParamsTable():GetOrAddValue("..."):SetStr("...")
	--new_entity:GetParamsTable():GetOrAddValue("..."):SetVec3f(Vec3f.new())
	--new_entity:GetParamsTable():GetOrAddValue("..."):SetColor(Color.new())
	--new_entity:GetParamsTable():GetOrAddValue("..."):SetBool(true)
	--new_entity:GetParamsTable():GetOrAddValue("..."):SetFloat(0.2)
	--new_entity:SetParamStr("MeshPath","mesh/Grid_Cell_Building.DAE");