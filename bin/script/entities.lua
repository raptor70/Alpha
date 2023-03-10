require "script/core"

entities = {

--	camera = {
--		components = {
--			"Camera";
--			"GameCamera";
--		};
--		
--		params = {
--			CameraIsOrtho = false;
--			CameraNear = 0.01;
--			CameraFar = 200;
--			CameraOrthoX = 15;
--			CameraOrthoY = 15;
--			CameraFov = math.rad(40);
--			GameCameraMoveAccel = 100;
--			GameCameraMoveSpeedMax = 30;
--			GameCameraMoveFrottement = 15;
--			GameCameraMoveMouseScreenRatio = 0.001; 		-- screen percentage to move camera with mouse
--			--GameCameraMoveMouseDistanceRatio = 0.5; 	-- reference screen percentage to move camera with drag & drop
--			GameCameraMoveMouseFactor = 100;
--			GameCameraZoomMin = 3;
--			GameCameraZoomMax = 50;
--			GameCameraZoomDefault = 20;
--			GameCameraZoomAccel = 75;
--			GameCameraZoomSpeedMax = 75;
--			GameCameraZoomFrottement = 12;
--			GameCameraZoomWheelDuration = 0.1; -- duration zoom after wheel tick
--			GameCameraPitchMin = math.rad(10);
--			GameCameraPitchMax = math.rad(55);
--			GameCameraPitchDefault = math.rad(50);
--			GameCameraPitchAccel = math.rad(800);
--			GameCameraPitchSpeedMax = math.rad(800);
--			GameCameraPitchFrottement = 12;
--			GameCameraPitchMouseFactor = 300;		-- mouse pitch factor
--			GameCameraYawAccel = math.rad(600);
--			GameCameraYawSpeedMax = math.rad(600);
--			GameCameraYawFrottement = 12;
--			GameCameraYawMouseFactor = 300;			-- mouse yaw factor
--			GameCameraYawDefault = math.rad(15); -- en radian
--		};
--	};
	
	simple_mesh = {
		components = {
			"Mesh";
		};
	};
	
	Grid_Cell = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell.DAE";
		}
	};
	
	Grid_Cell_B = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_B.DAE";
		}
	};
	
	Grid_Cell_C = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_C.DAE";
		}
	};
	
	Grid_Cell_D = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_D.DAE";
		}
	};
	
	Grid_Cell_Building = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Building.DAE";
		}
	};
	
	Grid_Cell_Water = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water.DAE";
		}
	};
	
	Grid_Cell_Water_B = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_B.DAE";
		}
	};
	
	Grid_Cell_Water_C = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_C.DAE";
		}
	};
	
	Grid_Cell_Water_D = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_D.DAE";
		}
	};
	
	Grid_Cell_Water_E = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_E.DAE";
		}
	};
	Grid_Cell_Water_Center = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_Center.DAE";
		}
	};
	Grid_Cell_Water_Border = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Grid_Cell_Water_Border.DAE";
		}
	};
	Water = {
		components = {
			"Mesh";
		};
		params = {
			MeshPath = "mesh/Water.DAE";
		}
	};
};

function GetEntityParam(name)
	return entities[name];
end

function ApplyParams(param,entity)
	-- parent check
	if param.parent ~= nil then
		parentParam = GetEntityParam(param.parent);
		ApplyParams(parentParam,entity);
	end
	
	-- add entity
	if param.components ~= nil then 
		world = World.GetInstance();
		for i,v in ipairs(param.components) do
			world:AddNewComponentToEntity(entity,v);
		end
	end
	
	-- set params
	if param.params ~= nil then 
		for k,v in pairs(param.params) do
			SetParamToTable(entity:GetParamsTable(),k,v);
		end
	end
end

function CreateEntity(script,name,param, pos, parent)
	-- create entity
	if parent ~= nil then
		new_entity = script:CreateNewEntityFromParent(name,parent);
	else 
		new_entity = script:CreateNewEntity(name);
	end
	
	-- params
	ApplyParams(param,new_entity);
	
	-- entity params
	if pos ~= nil then
		new_entity:SetPos(pos);
	end
	
	return new_entity;
end
