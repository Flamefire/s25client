nation = rttr:AddNation{
	-- Name used to reference this
	name = __"Vikings",
	-- Index in the original game
	s2Id = 1,
	-- Texture file for summer textures
	summerTexFile = "<RTTR_GAME>/DATA/MBOB/VIK_Z.LST",
	-- Texture file for winter textures
	winterTexFile = "<RTTR_GAME>/DATA/MBOB/WVIK_Z.LST",
	-- Default avatar texture
	defaultAvatar = { filepath = "io", idx = 256 }
}
include "buildings.lua"
