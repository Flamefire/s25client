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
#include "NationDataLoader.h"
#include "CheckedLuaTable.h"
#include "gameData/NatBuildingDesc.h"
#include "gameData/NationDesc.h"
#include <kaguya/kaguya.hpp>

NationDataLoader::NationDataLoader(WorldDescription& worldDesc, NationDesc& nationDesc) : worldDesc_(worldDesc), nationDesc_(nationDesc) {}

void NationDataLoader::Register(kaguya::State& state)
{
    state["RTTRNation"].setClass(kaguya::UserdataMetatable<NationDataLoader>().addFunction("AddBuilding", &NationDataLoader::AddBuilding));
}

void NationDataLoader::AddBuilding(const kaguya::LuaTable& data)
{
    nationDesc_.buildings.add(NatBuildingDesc(data, worldDesc_));
}
