function SetParamToTable(paramtable,paramName,paramValue)
	-- set params
	if type(paramValue) == "boolean" then
		paramtable:GetOrAddValue(paramName):SetBool(paramValue);
	elseif type(paramValue) == "number" then
		paramtable:GetOrAddValue(paramName):SetFloat(paramValue);
	elseif type(paramValue) == "string" then
		paramtable:GetOrAddValue(paramName):SetStr(paramValue);
	elseif type(paramValue) == "userdata" then
		if paramValue.typeName == "Vec3f" then
			paramtable:GetOrAddValue(paramName):SetVec3f(paramValue);
		elseif paramValue.typeName == "Color" then
			paramtable:GetOrAddValue(paramName):SetColor(paramValue);
		end
	else
		print("Unknow type : "..type(paramValue).. " for param "..paramName);
	end
end

