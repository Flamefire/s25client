nation = rttr:AddNation{
	-- Name used to reference this
	name = __"Babylonians",
	-- Index in the original game
	s2Id = 0,
	-- Texture file for summer textures
	summerTexFile = "<RTTR_RTTR>/LSTS/GAME/Babylonier/bab_z.lst",
	-- Texture file for winter textures
	winterTexFile = "<RTTR_RTTR>/LSTS/GAME/Babylonier/wbab_z.lst",
	-- Default avatar texture
	defaultAvatar = { filepath = "io_new", idx = 7 }
}
include "buildings.lua"
