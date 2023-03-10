params = {}
----------------------------------------------------

-- input
params.InputDistInToSpline = 40;
params.InputDistOutToSpline = 60;

-- line
params.LineSpeedFactor = 0.1;			-- doit etre entre 0 et 1
params.LineInertieFactor = 5;

-- burner
params.BurnerSwitchStartDelay = 0.5;
params.BurnerSwitchActiveDelay = 2.5;
params.BurnerSwitchInactiveDelay = 0;
params.BurnerDelay = 0.1;

-- monster
params.MonsterDistance = 88;
params.MonsterSmoothDuration = .2;
params.MonsterMergeDuration = .1;
params.MonsterToSpawn = 40;
params.MonsterColorCount = 4;

-- missions
params.MissionDefeatCount = 4;
params.MissionTimer = 400;

-- ability
params.SuperAbilityRandomMonstersPercentage = .4;		-- pourcentage de ceux qui reste
params.SuperAbilityFreezeDuration = 5;
params.SuperAbilityFreezeFactor = 0.5;
params.SuperAbilityBurnerDuration = 3;

-- score & combo
params.ScorePerMonster = 10;
params.ComboDelay = 2;
params.MonsterBadBurnedDelay = 2;
params.MalusPerMonster = 2;

----------------------------------------------------
 --DONT REMOVE THAT !!!
for k,v in pairs(params) do 
	if( type(v) == "string" ) then
		CParams.GetInstance():AddParamStr(k,v);
	else
		CParams.GetInstance():AddParamFloat(k,v);
	end
end