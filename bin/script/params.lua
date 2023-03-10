params = {
----------------------------------------------------

-- Tower
StepTowerPlacementDuration = 15;
StepShootDuration = 10;
StepWallsPlacementDuration = 30;


----------------------------------------------------
 --DONT REMOVE THAT !!!
 }
for k,v in pairs(params) do 
	if( type(v) == "string" ) then
		World.GetInstance():GetRoot():GetParamsTable():GetOrAddValue(k):SetStr(v);
	else
		World.GetInstance():GetRoot():GetParamsTable():GetOrAddValue(k):SetFloat(v);
	end
end