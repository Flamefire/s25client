rttr:AddBuilding{
	-- Name used to reference this
	name = __"Headquarters",
	-- Text shown in the help window
	help = __"The headquarters represents the center of your realm. The large amount of storage space available means a great many things can be safely stored here. You can release certain merchandise from the headquarters, as and when required or stop its storage. To do this, first choose the corresponding icon followed by the desired merchandise or job symbol. On the third page, you can adjust the number of reserve soldiers who are responsible for guarding the headquarters. There are two values given: the first value indicates the current number of men, the second value indicates the desired number. ",
	-- Costs to build this building. Non set values default to 0
	costs = { boards = 0, stones = 0 },
	-- Required space for the building (hut, house, castle, mine, harbor)
	size = "castle",
	-- Describes the work that this building does (optional)
	work = {
		-- Worker belonging to the building
		worker = "none",
		-- Produced ware. 'none' for nothing or 'invalid' for special cases
		producedWare = "none",
		-- List of up to 3 wares required for production
		waresNeeded = {},
		-- How many wares of each type can be stored. Default: 6
		numSpacesPerWare = 6,
		-- true (default) if one of each waresNeeded is used per production cycle
		-- false if the ware type is used, that the building has the most of
		useOneWareEach = true
	}
}
rttr:AddBuilding{
	name = __"Barracks",
	help = __"The barracks is a very small hut which can be used by your soldiers as quarters. Using the gold coin button you can stop the delivery of gold coins to the soldiers stationed here. However, without gold coins the soldiers here can not train and improve their skills.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Private",
		waresNeeded = {"Coins"},
		numSpacesPerWare = 1
	}
}
rttr:AddBuilding{
	name = __"Guardhouse",
	help = __"The guardhouse is a comfortable place for the military which is also protected by solid stone walls. Using the gold coin button you can stop the delivery of gold coins to the soldiers stationed here. However, without gold coins the soldiers here can not train and improve their skills.",
	costs = { boards = 2, stones = 3 },
	size = "hut",
	work = {
		worker = "Private",
		waresNeeded = {"Coins"},
		numSpacesPerWare = 2
	}
}
rttr:AddBuilding{
	name = __"Watchtower",
	help = __"The watchtower with its large amount of space is best suited for stationing a large number of your troops. Using the gold coin button you can stop the delivery of gold coins to the soldiers stationed here. However, without gold coins the soldiers here can not train and improve their skills.",
	costs = { boards = 3, stones = 5 },
	size = "house",
	work = {
		worker = "Private",
		waresNeeded = {"Coins"},
		numSpacesPerWare = 4
	}
}
rttr:AddBuilding{
	name = __"Fortress",
	help = __"The defensive capabilities and size of the fortress are unsurpassed. This stronghold ensures that other valuable buildings and commodities are protected. Using the gold coin button you can stop the delivery of gold coins to the soldiers stationed here. However, without gold coins the soldiers here can not train and improve their skills.",
	costs = { boards = 4, stones = 7 },
	size = "castle",
	work = {
		worker = "Private",
		waresNeeded = {"Coins"}
	}
}
rttr:AddBuilding{
	name = __"Granite mine",
	help = __"The quarrying of stone in a granite mine guarantees the supply of stone for buildings. However, even a granite mine has to feed its workers.",
	costs = { boards = 4 },
	size = "mine",
	work = {
		worker = "Miner",
		producedWare = "Stones",
		waresNeeded = {"Fish", "Meat", "Bread"},
		numSpacesPerWare = 2,
		useOneWareEach = false
	}
}
rttr:AddBuilding{
	name = __"Coal mine",
	help = __"The mining of coal supports the metalworks and smithy. This hard work requires an adequate supply of food.",
	costs = { boards = 4 },
	size = "mine",
	work = {
		worker = "Miner",
		producedWare = "Coal",
		waresNeeded = {"Fish", "Meat", "Bread"},
		numSpacesPerWare = 2,
		useOneWareEach = false
	}
}
rttr:AddBuilding{
	name = __"Iron mine",
	help = __"Deep within the mountains, miners dig for iron ore. They will obviously need a lot of food for the strenuous work.",
	costs = { boards = 4 },
	size = "mine",
	work = {
		worker = "Miner",
		producedWare = "Iron",
		waresNeeded = {"Fish", "Meat", "Bread"},
		numSpacesPerWare = 2,
		useOneWareEach = false
	}
}
rttr:AddBuilding{
	name = __"Gold mine",
	help = __"A gold mine allows you to prospect for valuable gold deposits. For this, it is necessary to ensure that the miners are provided with plenty of food.",
	costs = { boards = 4 },
	size = "mine",
	work = {
		worker = "Miner",
		producedWare = "Gold",
		waresNeeded = {"Fish", "Meat", "Bread"},
		numSpacesPerWare = 2,
		useOneWareEach = false
	}
}
rttr:AddBuilding{
	name = __"Lookout tower",
	help = __"From the lookout tower you can see far into previously unexplored lands.",
	costs = { boards = 4 },
	size = "hut",
	work = {
		worker = "Scout"
	}
}
rttr:AddBuilding{
	name = __"Catapult",
	help = __"Thanks to its immense strength, the catapults represents an effective weapon against enemy military buildings.",
	costs = { boards = 4, stones = 2 },
	size = "house",
	work = {
		worker = "Helper",
		waresNeeded = {"Stones"},
		numSpacesPerWare = 4
	}
}
rttr:AddBuilding{
	name = __"Woodcutter",
	help = __"A woodcutter provides the sawmill with logs. A forester is able to replant the depleted forest.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Woodchopper",
		producedWare = "Wood"
	}
}
rttr:AddBuilding{
	name = __"Fishery",
	help = __"The fish man is responsible for finding water rich in fish. His fish feed a great many miners.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Fisher",
		producedWare = "Fish"
	}
}
rttr:AddBuilding{
	name = __"Quarry",
	help = __"The stonemason works the stone near his quarry into bricks. These are needed mainly for building houses and as ammunition for catapults.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Stonemason",
		producedWare = "Stones"
	}
}
rttr:AddBuilding{
	name = __"Forester",
	help = __"Within his area, the forester ensures the survival of the forest. He plants all types of trees.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Ranger"
	}
}
rttr:AddBuilding{
	name = __"Slaughterhouse",
	help = __"The butcher processes the livestock delivered into nutritious ham on which you miners are fed.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Butcher",
		producedWare = "Meat",
		waresNeeded = {"Ham"}
	}
}
rttr:AddBuilding{
	name = __"Hunter",
	help = __"Meat the hunter acquires is used to feed the miners.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Huntsman",
		producedWare = "Meat"
	}
}
rttr:AddBuilding{
	name = __"Brewery",
	help = __"The brewer produces fine beer from grain and water. This drink is needed to fill the soldiers with courage.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Brewer",
		producedWare = "Beer",
		waresNeeded = {"Grain", "Water"}
	}
}
rttr:AddBuilding{
	name = __"Armory",
	help = __"The armory produces swords and strong shields. This equipment is vital for your soldiers.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Armorer",
		producedWare = "Sword",
		waresNeeded = {"Iron", "Coal"}
	}
}
rttr:AddBuilding{
	name = __"Metalworks",
	help = __"The countless tools which your workers need are made in the metalworks. This requires boards and iron.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Metalworker",
		producedWare = "Tongs",
		waresNeeded = {"Iron", "Boards"}
	}
}
rttr:AddBuilding{
	name = __"Iron smelter",
	help = __"Raw iron ore is smelted in the iron smelters with the help of coal. The processed iron is then used to making weapons (in the Armory) and tools (in the metalworks).",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Iron founder",
		producedWare = "Iron",
		waresNeeded = {"Iron ore", "Coal"}
	}
}
rttr:AddBuilding{
	name = __"Charburner",
	help = __"The charburner stacks up piles of wood and straw which is then burned to create charcoal. This can be used just like the coal from the mine without any loss in quality.",
	costs = { boards = 4, stones = 3 },
	size = "castle",
	work = {
		worker = "Charburner",
		producedWare = "Coal",
		waresNeeded = {"Wood", "Grain"}
	}
}
rttr:AddBuilding{
	name = __"Pig farm",
	help = __"Grain and water are needed for rearing pigs. The meat thus obtained can then be processed by a butcher.",
	costs = { boards = 3, stones = 3 },
	size = "castle",
	work = {
		worker = "Pig breeder",
		producedWare = "Ham",
		waresNeeded = {"Grain", "Water"}
	}
}
rttr:AddBuilding{
	name = __"Storehouse",
	help = __"The storehouse can help reduce long transportation journeys and is suited to the temporary storage of merchandise and inhabitants. You can release certain merchandise from the storehouse, as and when required. Alternatively, the storage function can be disabled. To do this, first choose the relevant icon followed by the desired merchandise or job symbol.",
	costs = { boards = 4, stones = 3 },
	size = "house",
	work = {
	}
}
rttr:AddBuilding{
	name = __"Mill",
	help = __"The grain is ground in the windmill. The flour from the windmill is later used at the bakery to bake bread.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Miller",
		producedWare = "Flour",
		waresNeeded = {"Grain"}
	}
}
rttr:AddBuilding{
	name = __"Bakery",
	help = __"The flour produced at the windmill can be combined with water in the bakery to make oven-fresh bread. It's your miners' favorite!",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Baker",
		producedWare = "Bread",
		waresNeeded = {"Flour", "Water"}
	}
}
rttr:AddBuilding{
	name = __"Sawmill",
	help = __"The carpenter turns the woodcutter's logs into made-to-measure planks. These form the basic for building houses and ships.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Carpenter",
		producedWare = "Boards",
		waresNeeded = {"Wood"}
	}
}
rttr:AddBuilding{
	name = __"Mint",
	help = __"The mint is responsible for producing valuable gold coins. These precious objects are produced using coal and gold.",
	costs = { boards = 2, stones = 2 },
	size = "house",
	work = {
		worker = "Minter",
		producedWare = "Coins",
		waresNeeded = {"Gold", "Coal"}
	}
}
rttr:AddBuilding{
	name = __"Well",
	help = __"A well supplies water to the bakery, brewery, donkey breeder and pig farm.",
	costs = { boards = 2 },
	size = "hut",
	work = {
		worker = "Helper",
		producedWare = "Water"
	}
}
rttr:AddBuilding{
	name = __"Shipyard",
	help = __"It is possible to build small rowing boats as well as huge cargo ships in a shipyard. The boards required for this are skillfully worked by shipwrights.",
	costs = { boards = 2, stones = 3 },
	size = "house",
	work = {
		worker = "Shipwright",
		producedWare = "Boat",
		waresNeeded = {"Boards"}
	}
}
rttr:AddBuilding{
	name = __"Farm",
	help = __"The farmer plants and harvests grain in the surrounding fields. A windmill then processes the harvested grain into flour or can be used to feed the pigs.",
	costs = { boards = 3, stones = 3 },
	size = "castle",
	work = {
		worker = "Farmer",
		producedWare = "Grain"
	}
}
rttr:AddBuilding{
	name = __"Donkey breeding",
	help = __"The pack donkeys bred here are used to transport your merchandise more efficiently. They are reared on water and grain.",
	costs = { boards = 3, stones = 3 },
	size = "castle",
	work = {
		worker = "Donkey breeder",
		producedWare = "invalid",
		waresNeeded = {"Grain", "Water"}
	}
}
rttr:AddBuilding{
	name = __"Harbor building",
	help = __"Ships can only be loaded and unloaded in a harbor. Expeditions can also be prepared here. You can release certain merchandise from the storehouse, as and when required. Alternatively, the storage function can be disabled. To do this, first choose the relevant icon followed by the desired merchandise or job symbol.",
	costs = { boards = 4, stones = 6 },
	size = "harbor",
	work = {
	}
}
