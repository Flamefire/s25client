iconFile = "<RTTR_GAME>/DATA/MBOB/VIK_ICON.LST"
summerTexFile = "<RTTR_GAME>/DATA/MBOB/VIK_Z.LST"
winterTexFile = "<RTTR_GAME>/DATA/MBOB/WVIK_Z.LST"

function getSummerAndWinterTex(idx)
	return { { filepath = summerTexFile, idx = idx }, { filepath = winterTexFile, idx = idx } }
end

nation:AddBuilding{
	-- Name used to reference this
	name = "Headquarters",
	-- Texture for the icon
	icon = { filepath = iconFile, idx = 0 },
	-- Textures for the building. Required: main, skeleton, door. Optional: shadow, skeletonShadow. Multiple values: 0 = summer, 1 = winter
	texture = {
		-- Textures for the building. Required: main, skeleton, door. Optional: shadow, skeletonShadow. Multiple values: 0 = summer, 1 = winter
		main = getSummerAndWinterTex(250),
		-- Shadow texture
		shadow = getSummerAndWinterTex(251),
		-- Skeleton (in construction) texture and its shadow
		skeleton = { filepath = "mis0bobs", idx = 6 },
		-- Skeleton (in construction) texture and its shadow
		skeletonShadow = { filepath = "mis0bobs", idx = 7 },
		-- Texture of the (closed) door
		door = getSummerAndWinterTex(254)
	},
	-- Y-Position of the door (X will be calculated by extending the path from the flag)
	doorPosY = 10,
	-- Smoke type (1-4, 0 = disabled) and offset relative to the buildings origin
	smoke = { type = 0, offset = {127, 127} }
}
nation:AddBuilding{
	name = "Barracks",
	icon = { filepath = iconFile, idx = 1 },
	texture = { main = getSummerAndWinterTex(255), shadow = getSummerAndWinterTex(256), skeleton = getSummerAndWinterTex(257), skeletonShadow = getSummerAndWinterTex(258), door = getSummerAndWinterTex(259) },
	doorPosY = 12
}
nation:AddBuilding{
	name = "Guardhouse",
	icon = { filepath = iconFile, idx = 2 },
	texture = { main = getSummerAndWinterTex(260), shadow = getSummerAndWinterTex(261), skeleton = getSummerAndWinterTex(262), skeletonShadow = getSummerAndWinterTex(263), door = getSummerAndWinterTex(264) },
	doorPosY = 11
}
nation:AddBuilding{
	name = "Watchtower",
	icon = { filepath = iconFile, idx = 4 },
	texture = { main = getSummerAndWinterTex(270), shadow = getSummerAndWinterTex(271), skeleton = getSummerAndWinterTex(272), skeletonShadow = getSummerAndWinterTex(273), door = getSummerAndWinterTex(274) },
	doorPosY = 11
}
nation:AddBuilding{
	name = "Fortress",
	icon = { filepath = iconFile, idx = 9 },
	texture = { main = getSummerAndWinterTex(295), shadow = getSummerAndWinterTex(296), skeleton = getSummerAndWinterTex(297), skeletonShadow = getSummerAndWinterTex(298), door = getSummerAndWinterTex(299) },
	doorPosY = 14
}
nation:AddBuilding{
	name = "Granite mine",
	icon = { filepath = iconFile, idx = 10 },
	texture = { main = getSummerAndWinterTex(300), shadow = getSummerAndWinterTex(301), skeleton = getSummerAndWinterTex(302), skeletonShadow = getSummerAndWinterTex(303), door = getSummerAndWinterTex(304) },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Coal mine",
	icon = { filepath = iconFile, idx = 11 },
	texture = { main = getSummerAndWinterTex(305), shadow = getSummerAndWinterTex(306), skeleton = getSummerAndWinterTex(307), skeletonShadow = getSummerAndWinterTex(308), door = getSummerAndWinterTex(309) },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Iron mine",
	icon = { filepath = iconFile, idx = 12 },
	texture = { main = getSummerAndWinterTex(310), shadow = getSummerAndWinterTex(311), skeleton = getSummerAndWinterTex(312), skeletonShadow = getSummerAndWinterTex(313), door = getSummerAndWinterTex(314) },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Gold mine",
	icon = { filepath = iconFile, idx = 13 },
	texture = { main = getSummerAndWinterTex(315), shadow = getSummerAndWinterTex(316), skeleton = getSummerAndWinterTex(317), skeletonShadow = getSummerAndWinterTex(318), door = getSummerAndWinterTex(319) },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Lookout tower",
	icon = { filepath = iconFile, idx = 14 },
	texture = { main = getSummerAndWinterTex(320), shadow = getSummerAndWinterTex(321), skeleton = getSummerAndWinterTex(322), skeletonShadow = getSummerAndWinterTex(323), door = getSummerAndWinterTex(324) },
	doorPosY = 4
}
nation:AddBuilding{
	name = "Catapult",
	icon = { filepath = iconFile, idx = 16 },
	texture = { main = getSummerAndWinterTex(330), shadow = getSummerAndWinterTex(331), skeleton = getSummerAndWinterTex(332), skeletonShadow = getSummerAndWinterTex(333), door = getSummerAndWinterTex(334) },
	doorPosY = 10
}
nation:AddBuilding{
	name = "Woodcutter",
	icon = { filepath = iconFile, idx = 17 },
	texture = { main = getSummerAndWinterTex(335), shadow = getSummerAndWinterTex(336), skeleton = getSummerAndWinterTex(337), skeletonShadow = getSummerAndWinterTex(338), door = getSummerAndWinterTex(339) },
	doorPosY = 10,
	smoke = { type = 1, offset = {2, -36} }
}
nation:AddBuilding{
	name = "Fishery",
	icon = { filepath = iconFile, idx = 18 },
	texture = { main = getSummerAndWinterTex(340), shadow = getSummerAndWinterTex(341), skeleton = getSummerAndWinterTex(342), skeletonShadow = getSummerAndWinterTex(343), door = getSummerAndWinterTex(344) },
	doorPosY = 10,
	smoke = { type = 1, offset = {4, -36} }
}
nation:AddBuilding{
	name = "Quarry",
	icon = { filepath = iconFile, idx = 19 },
	texture = { main = getSummerAndWinterTex(345), shadow = getSummerAndWinterTex(346), skeleton = getSummerAndWinterTex(347), skeletonShadow = getSummerAndWinterTex(348), door = getSummerAndWinterTex(349) },
	doorPosY = 12,
	smoke = { type = 1, offset = {0, -34} }
}
nation:AddBuilding{
	name = "Forester",
	icon = { filepath = iconFile, idx = 20 },
	texture = { main = getSummerAndWinterTex(350), shadow = getSummerAndWinterTex(351), skeleton = getSummerAndWinterTex(352), skeletonShadow = getSummerAndWinterTex(353), door = getSummerAndWinterTex(354) },
	doorPosY = 12,
	smoke = { type = 1, offset = {-5, -29} }
}
nation:AddBuilding{
	name = "Slaughterhouse",
	icon = { filepath = iconFile, idx = 21 },
	texture = { main = getSummerAndWinterTex(355), shadow = getSummerAndWinterTex(356), skeleton = getSummerAndWinterTex(357), skeletonShadow = getSummerAndWinterTex(358), door = getSummerAndWinterTex(359) },
	doorPosY = 19,
	smoke = { type = 1, offset = {7, -41} }
}
nation:AddBuilding{
	name = "Hunter",
	icon = { filepath = iconFile, idx = 22 },
	texture = { main = getSummerAndWinterTex(360), shadow = getSummerAndWinterTex(361), skeleton = getSummerAndWinterTex(362), skeletonShadow = getSummerAndWinterTex(363), door = getSummerAndWinterTex(364) },
	doorPosY = 11,
	smoke = { type = 1, offset = {-6, -38} }
}
nation:AddBuilding{
	name = "Brewery",
	icon = { filepath = iconFile, idx = 23 },
	texture = { main = getSummerAndWinterTex(365), shadow = getSummerAndWinterTex(366), skeleton = getSummerAndWinterTex(367), skeletonShadow = getSummerAndWinterTex(368), door = getSummerAndWinterTex(369) },
	doorPosY = 13,
	smoke = { type = 3, offset = {5, -39} }
}
nation:AddBuilding{
	name = "Armory",
	icon = { filepath = iconFile, idx = 24 },
	texture = { main = getSummerAndWinterTex(370), shadow = getSummerAndWinterTex(371), skeleton = getSummerAndWinterTex(372), skeletonShadow = getSummerAndWinterTex(373), door = getSummerAndWinterTex(374) },
	doorPosY = 6,
	smoke = { type = 3, offset = {-23, -36} }
}
nation:AddBuilding{
	name = "Metalworks",
	icon = { filepath = iconFile, idx = 25 },
	texture = { main = getSummerAndWinterTex(375), shadow = getSummerAndWinterTex(376), skeleton = getSummerAndWinterTex(377), skeletonShadow = getSummerAndWinterTex(378), door = getSummerAndWinterTex(379) },
	doorPosY = 11,
	smoke = { type = 1, offset = {-9, -35} }
}
nation:AddBuilding{
	name = "Iron smelter",
	icon = { filepath = iconFile, idx = 26 },
	texture = { main = getSummerAndWinterTex(380), shadow = getSummerAndWinterTex(381), skeleton = getSummerAndWinterTex(382), skeletonShadow = getSummerAndWinterTex(383), door = getSummerAndWinterTex(384) },
	doorPosY = 14,
	smoke = { type = 2, offset = {-2, -38} }
}
nation:AddBuilding{
	name = "Charburner",
	icon = { filepath = "charburner", idx = 32 },
	texture = { main = {{"charburner", 25}, {"charburner", 30}}, shadow = {"charburner", 26}, skeleton = {"charburner", 27}, skeletonShadow = {"charburner", 28}, door = {{"charburner", 29}, {"charburner", 31}} },
	doorPosY = 0,
	smoke = { type = 2, offset = {-22, -55} }
}
nation:AddBuilding{
	name = "Pig farm",
	icon = { filepath = iconFile, idx = 28 },
	texture = { main = getSummerAndWinterTex(390), shadow = getSummerAndWinterTex(391), skeleton = getSummerAndWinterTex(392), skeletonShadow = getSummerAndWinterTex(393), door = getSummerAndWinterTex(394) },
	doorPosY = -3,
	smoke = { type = 2, offset = {-30, -37} }
}
nation:AddBuilding{
	name = "Storehouse",
	icon = { filepath = iconFile, idx = 29 },
	texture = { main = getSummerAndWinterTex(395), shadow = getSummerAndWinterTex(396), skeleton = getSummerAndWinterTex(397), skeletonShadow = getSummerAndWinterTex(398), door = getSummerAndWinterTex(399) },
	doorPosY = 11
}
nation:AddBuilding{
	name = "Mill",
	icon = { filepath = iconFile, idx = 31 },
	texture = { main = getSummerAndWinterTex(405), shadow = getSummerAndWinterTex(406), skeleton = getSummerAndWinterTex(407), skeletonShadow = getSummerAndWinterTex(408), door = getSummerAndWinterTex(409) },
	doorPosY = 9
}
nation:AddBuilding{
	name = "Bakery",
	icon = { filepath = iconFile, idx = 32 },
	texture = { main = getSummerAndWinterTex(410), shadow = getSummerAndWinterTex(411), skeleton = getSummerAndWinterTex(412), skeletonShadow = getSummerAndWinterTex(413), door = getSummerAndWinterTex(414) },
	doorPosY = 11,
	smoke = { type = 4, offset = {-21, -26} }
}
nation:AddBuilding{
	name = "Sawmill",
	icon = { filepath = iconFile, idx = 33 },
	texture = { main = getSummerAndWinterTex(415), shadow = getSummerAndWinterTex(416), skeleton = getSummerAndWinterTex(417), skeletonShadow = getSummerAndWinterTex(418), door = getSummerAndWinterTex(419) },
	doorPosY = 10,
	smoke = { type = 1, offset = {-11, -45} }
}
nation:AddBuilding{
	name = "Mint",
	icon = { filepath = iconFile, idx = 34 },
	texture = { main = getSummerAndWinterTex(420), shadow = getSummerAndWinterTex(421), skeleton = getSummerAndWinterTex(422), skeletonShadow = getSummerAndWinterTex(423), door = getSummerAndWinterTex(424) },
	doorPosY = 16,
	smoke = { type = 1, offset = {16, -38} }
}
nation:AddBuilding{
	name = "Well",
	icon = { filepath = iconFile, idx = 35 },
	texture = { main = getSummerAndWinterTex(425), shadow = getSummerAndWinterTex(426), skeleton = getSummerAndWinterTex(427), skeletonShadow = getSummerAndWinterTex(428), door = getSummerAndWinterTex(429) },
	doorPosY = 0
}
nation:AddBuilding{
	name = "Shipyard",
	icon = { filepath = iconFile, idx = 36 },
	texture = { main = getSummerAndWinterTex(430), shadow = getSummerAndWinterTex(431), skeleton = getSummerAndWinterTex(432), skeletonShadow = getSummerAndWinterTex(433), door = getSummerAndWinterTex(434) },
	doorPosY = 16
}
nation:AddBuilding{
	name = "Farm",
	icon = { filepath = iconFile, idx = 37 },
	texture = { main = getSummerAndWinterTex(435), shadow = getSummerAndWinterTex(436), skeleton = getSummerAndWinterTex(437), skeletonShadow = getSummerAndWinterTex(438), door = getSummerAndWinterTex(439) },
	doorPosY = -6,
	smoke = { type = 1, offset = {-17, -48} }
}
nation:AddBuilding{
	name = "Donkey breeding",
	icon = { filepath = iconFile, idx = 38 },
	texture = { main = getSummerAndWinterTex(440), shadow = getSummerAndWinterTex(441), skeleton = getSummerAndWinterTex(442), skeletonShadow = getSummerAndWinterTex(443), door = getSummerAndWinterTex(444) },
	doorPosY = -2,
	smoke = { type = 4, offset = {-27, -40} }
}
nation:AddBuilding{
	name = "Harbor building",
	icon = { filepath = iconFile, idx = 39 },
	texture = { main = getSummerAndWinterTex(445), shadow = getSummerAndWinterTex(446), skeleton = getSummerAndWinterTex(447), skeletonShadow = getSummerAndWinterTex(448), door = getSummerAndWinterTex(449) },
	animations = {
		occupied = { filepath = "<Vikings>", frames = range(500, 540, 5), msPerFrame = 158 },
		work = { filepath = "<Map>", frames = range(740, 748), offset = {5, -80}, msPerFrame = 197 }
	},
	workOffsets = {
		builder = {-38, 17},
		boards = {-65, -5},
		stones = {-52, 10}
	},
	doorPosY = 10
}
