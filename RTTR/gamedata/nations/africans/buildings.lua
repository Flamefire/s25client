nation:AddBuilding{
	-- Name used to reference this
	name = "Headquarters",
	-- Texture for the icon
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 0 },
	-- Y-Position of the door (X will be calculated by extending the path from the flag)
	doorPosY = 5,
	-- Smoke type (1-4, 0 = disabled) and offset relative to the buildings origin
	smoke = { type = 0, offset = {127, 127} }
}
nation:AddBuilding{
	name = "Barracks",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 1 },
	doorPosY = 10
}
nation:AddBuilding{
	name = "Guardhouse",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 2 },
	doorPosY = 13
}
nation:AddBuilding{
	name = "Watchtower",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 4 },
	doorPosY = 10
}
nation:AddBuilding{
	name = "Fortress",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 9 },
	doorPosY = 6
}
nation:AddBuilding{
	name = "Granite mine",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 10 },
	doorPosY = 8
}
nation:AddBuilding{
	name = "Coal mine",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 11 },
	doorPosY = 8
}
nation:AddBuilding{
	name = "Iron mine",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 12 },
	doorPosY = 8
}
nation:AddBuilding{
	name = "Gold mine",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 13 },
	doorPosY = 8
}
nation:AddBuilding{
	name = "Lookout tower",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 14 },
	doorPosY = 6
}
nation:AddBuilding{
	name = "Catapult",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 16 },
	doorPosY = 10
}
nation:AddBuilding{
	name = "Woodcutter",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 17 },
	doorPosY = 10
}
nation:AddBuilding{
	name = "Fishery",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 18 },
	doorPosY = 12
}
nation:AddBuilding{
	name = "Quarry",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 19 },
	doorPosY = 14,
	smoke = { type = 1, offset = {3, -32} }
}
nation:AddBuilding{
	name = "Forester",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 20 },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Slaughterhouse",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 21 },
	doorPosY = 5
}
nation:AddBuilding{
	name = "Hunter",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 22 },
	doorPosY = 11
}
nation:AddBuilding{
	name = "Brewery",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 23 },
	doorPosY = 19
}
nation:AddBuilding{
	name = "Armory",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 24 },
	doorPosY = 19,
	smoke = { type = 1, offset = {-32, -23} }
}
nation:AddBuilding{
	name = "Metalworks",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 25 },
	doorPosY = 12,
	smoke = { type = 4, offset = {-26, -47} }
}
nation:AddBuilding{
	name = "Iron smelter",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 26 },
	doorPosY = 18,
	smoke = { type = 2, offset = {-20, -37} }
}
nation:AddBuilding{
	name = "Charburner",
	icon = { filepath = "charburner", idx = 8 },
	doorPosY = 0,
	smoke = { type = 2, offset = {-18, -52} }
}
nation:AddBuilding{
	name = "Pig farm",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 28 },
	doorPosY = -6
}
nation:AddBuilding{
	name = "Storehouse",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 29 },
	doorPosY = 19
}
nation:AddBuilding{
	name = "Mill",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 31 },
	doorPosY = 12
}
nation:AddBuilding{
	name = "Bakery",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 32 },
	doorPosY = 11,
	smoke = { type = 4, offset = {27, -39} }
}
nation:AddBuilding{
	name = "Sawmill",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 33 },
	doorPosY = 6
}
nation:AddBuilding{
	name = "Mint",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 34 },
	doorPosY = 10,
	smoke = { type = 1, offset = {17, -52} }
}
nation:AddBuilding{
	name = "Well",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 35 },
	doorPosY = 0
}
nation:AddBuilding{
	name = "Shipyard",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 36 },
	doorPosY = 0
}
nation:AddBuilding{
	name = "Farm",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 37 },
	doorPosY = -1
}
nation:AddBuilding{
	name = "Donkey breeding",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 38 },
	doorPosY = 4
}
nation:AddBuilding{
	name = "Harbor building",
	icon = { filepath = "<RTTR_GAME>/DATA/MBOB/AFR_ICON.LST", idx = 39 },
	doorPosY = 13
}
