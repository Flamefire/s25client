// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "commonDefines.h" // IWYU pragma: keep
#include "BuildingNames.h"
#include "mygettext/mygettext.h"

const boost::array<const char*, NUM_BUILDING_TYPES> BUILDING_NAMES = {{gettext_noop("Headquarters"),
                                                                       gettext_noop("Barracks"),
                                                                       gettext_noop("Guardhouse"),
                                                                       gettext_noop("Watchtower"),
                                                                       gettext_noop("Fortress"),
                                                                       gettext_noop("Granite mine"),
                                                                       gettext_noop("Coal mine"),
                                                                       gettext_noop("Iron mine"),
                                                                       gettext_noop("Gold mine"),
                                                                       gettext_noop("Lookout tower"),
                                                                       gettext_noop("Catapult"),
                                                                       gettext_noop("Woodcutter"),
                                                                       gettext_noop("Fishery"),
                                                                       gettext_noop("Quarry"),
                                                                       gettext_noop("Forester"),
                                                                       gettext_noop("Slaughterhouse"),
                                                                       gettext_noop("Hunter"),
                                                                       gettext_noop("Brewery"),
                                                                       gettext_noop("Armory"),
                                                                       gettext_noop("Metalworks"),
                                                                       gettext_noop("Iron smelter"),
                                                                       gettext_noop("Pig farm"),
                                                                       gettext_noop("Storehouse"),
                                                                       gettext_noop("Mill"),
                                                                       gettext_noop("Bakery"),
                                                                       gettext_noop("Sawmill"),
                                                                       gettext_noop("Mint"),
                                                                       gettext_noop("Well"),
                                                                       gettext_noop("Shipyard"),
                                                                       gettext_noop("Farm"),
                                                                       gettext_noop("Donkey breeding"),
                                                                       gettext_noop("Harbor building"),
                                                                       gettext_noop("Charburner")}};
