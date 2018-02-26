nation = rttr:AddNation{
	-- Name used to reference this
	name = __"Africans",
	-- Index in the original game
	s2Id = 3,
	-- Texture file for summer textures
	summerTexFile = "<RTTR_GAME>/DATA/MBOB/AFR_Z.LST",
	-- Texture file for winter textures
	winterTexFile = "<RTTR_GAME>/DATA/MBOB/WAFR_Z.LST",
	-- Default avatar texture
	defaultAvatar = { filepath = "io", idx = 257 }
}
include "buildings.lua"
