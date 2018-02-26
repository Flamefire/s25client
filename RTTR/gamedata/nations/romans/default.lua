nation = rttr:AddNation{
	-- Name used to reference this
	name = __"Romans",
	-- Index in the original game
	s2Id = 0,
	-- Texture file for summer textures
	summerTexFile = "<RTTR_GAME>/DATA/MBOB/ROM_Z.LST",
	-- Texture file for winter textures
	winterTexFile = "<RTTR_GAME>/DATA/MBOB/WROM_Z.LST",
	-- Default avatar texture
	defaultAvatar = { filepath = "io", idx = 252 }
}
include "buildings.lua"
