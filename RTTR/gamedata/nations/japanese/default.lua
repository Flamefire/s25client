nation = rttr:AddNation{
	-- Name used to reference this
	name = __"Japanese",
	-- Index in the original game
	s2Id = 2,
	-- Texture file for summer textures
	summerTexFile = "<RTTR_GAME>/DATA/MBOB/JAP_Z.LST",
	-- Texture file for winter textures
	winterTexFile = "<RTTR_GAME>/DATA/MBOB/WJAP_Z.LST",
	-- Default avatar texture
	defaultAvatar = { filepath = "io", idx = 253 }
}
include "buildings.lua"
