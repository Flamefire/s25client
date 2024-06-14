// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "iwBuildings.h"
#include "GamePlayer.h"
#include "GlobalGameSettings.h"
#include "Loader.h"
#include "WindowManager.h"
#include "WineLoader.h"
#include "addons/const_addons.h"
#include "buildings/nobBaseWarehouse.h"
#include "buildings/nobHarborBuilding.h"
#include "buildings/nobMilitary.h"
#include "buildings/nobUsual.h"
#include "files.h"
#include "iwBaseWarehouse.h"
#include "iwBuilding.h"
#include "iwHarborBuilding.h"
#include "iwHelp.h"
#include "iwMilitaryBuilding.h"
#include "iwTempleBuilding.h"
#include "ogl/FontStyle.h"
#include "ogl/glFont.h"
#include "world/GameWorldBase.h"
#include "world/GameWorldView.h"
#include "world/GameWorldViewer.h"
#include "gameTypes/BuildingCount.h"
#include "gameData/BuildingConsts.h"
#include "gameData/BuildingProperties.h"
#include "gameData/const_gui_ids.h"

/// Reihenfolge der Gebäude
void iwBuildings::setBuildingOrder()
{
    bts = {
      BuildingType::Barracks,       BuildingType::Guardhouse, BuildingType::Watchtower,     BuildingType::Fortress,
      BuildingType::GraniteMine,    BuildingType::CoalMine,   BuildingType::IronMine,       BuildingType::GoldMine,
      BuildingType::LookoutTower,   BuildingType::Catapult,   BuildingType::Woodcutter,     BuildingType::Fishery,
      BuildingType::Quarry,         BuildingType::Forester,   BuildingType::Slaughterhouse, BuildingType::Hunter,
      BuildingType::Brewery,        BuildingType::Armory,     BuildingType::Metalworks,     BuildingType::Ironsmelter,
      BuildingType::PigFarm,
      BuildingType::Storehouse, // entry 21
      BuildingType::Mill,           BuildingType::Bakery,     BuildingType::Sawmill,        BuildingType::Mint,
      BuildingType::Well,           BuildingType::Shipyard,   BuildingType::Farm,           BuildingType::DonkeyBreeder,
      BuildingType::Charburner,
      BuildingType::HarborBuilding,                                                 // entry 31
      BuildingType::Vineyard,       BuildingType::Winery,     BuildingType::Temple, // entry 34
    };
    removeUnusedBuildings();
}

void iwBuildings::removeUnusedBuildings()
{
    auto removeNotUsedBuilding = [=](BuildingType const& bts) {
        if(!wineaddon::isAddonActive(gwv.GetWorld()) && wineaddon::isWineAddonBuildingType(bts))
            return true;
        else if(!gwv.GetWorld().GetGGS().isEnabled(AddonId::CHARBURNER) && bts == BuildingType::Charburner)
            return true;
        else
            return false;
    };
    bts.erase(std::remove_if(std::begin(bts), std::end(bts), removeNotUsedBuilding), std::end(bts));
}

// Abstand des ersten Icons vom linken oberen Fensterrand
const Extent bldContentOffset(30, 40);
// Abstand der einzelnen Symbole untereinander
const Extent iconSpacing(40, 48);
// Abstand der Schriften unter den Icons
const unsigned short font_distance_y = 20;

iwBuildings::iwBuildings(GameWorldView& gwv, GameCommandFactory& gcFactory)
    : IngameWindow(CGI_BUILDINGS, IngameWindow::posLastOrCenter, Extent(185, 480), _("Buildings"),
                   LOADER.GetImageN("resource", 41)),
      gwv(gwv), gcFactory(gcFactory)
{
    setBuildingOrder();
    const unsigned rowCount = helpers::divCeil(bts.size(), 4);
    Resize(iconSpacing * Extent(4, rowCount + 1) + bldContentOffset);

    const Nation playerNation = gwv.GetViewer().GetPlayer().nation;
    // Symbole für die einzelnen Gebäude erstellen
    for(unsigned y = 0; y < rowCount; y++)
    {
        for(unsigned x = 0; x < 4; x++)
        {
            if(y * 4 + x >= bts.size()) //-V547
                break;

            Extent btSize = Extent(32, 32);
            DrawPoint btPos = bldContentOffset - btSize / 2 + iconSpacing * DrawPoint(x, y);
            AddImageButton(y * 4 + x, btPos, btSize, TextureColor::Grey,
                           LOADER.GetNationIcon(playerNation, bts[y * 4 + x]), _(BUILDING_NAMES[bts[y * 4 + x]]));
        }
    }

    // "Help" button
    Extent btSize = Extent(30, 32);
    AddImageButton(35, GetFullSize() - DrawPoint(14, 20) - btSize, btSize, TextureColor::Grey,
                   LOADER.GetImageN("io", 225), _("Help"));
}

/// Anzahlen der Gebäude zeichnen
void iwBuildings::Msg_PaintAfter()
{
    static boost::format fmt("%1%/%2%");
    IngameWindow::Msg_PaintAfter();
    // Anzahlen herausfinden
    BuildingCount bc = gwv.GetViewer().GetPlayer().GetBuildingRegister().GetBuildingNums();

    // Anzahlen unter die Gebäude schreiben
    DrawPoint rowPos = GetDrawPos() + bldContentOffset + DrawPoint(0, font_distance_y);
    for(unsigned y = 0; y < helpers::divCeil(bts.size(), 4); ++y)
    {
        DrawPoint curPos = rowPos;
        for(unsigned x = 0; x < 4; x++)
        {
            if(y * 4 + x >= bts.size()) //-V547
                break;

            fmt % bc.buildings[bts[y * 4 + x]] % bc.buildingSites[bts[y * 4 + x]];
            NormalFont->Draw(curPos, fmt.str(), FontStyle::CENTER, COLOR_YELLOW);
            curPos.x += iconSpacing.x;
        }
        rowPos.y += iconSpacing.y;
    }
}

void iwBuildings::Msg_ButtonClick(const unsigned ctrl_id)
{
    if(ctrl_id == 35) // Help button
    {
        WINDOWMANAGER.ReplaceWindow(
          std::make_unique<iwHelp>(_("The building statistics window gives you an insight into "
                                     "the number of buildings you have, by type. The number on "
                                     "the left is the total number of this type of building "
                                     "completed, the number on the right shows how many are "
                                     "currently under construction.")));
        return;
    }

    // no buildings of type complete? -> do nothing
    const GamePlayer& localPlayer = gwv.GetViewer().GetPlayer();
    const BuildingRegister& buildingRegister = localPlayer.GetBuildingRegister();

    BuildingType bldType = bts[ctrl_id];
    if(BuildingProperties::IsMilitary(bldType))
        GoToFirstMatching<iwMilitaryBuilding>(bldType, buildingRegister.GetMilitaryBuildings());
    else if(bldType == BuildingType::HarborBuilding)
        GoToFirstMatching<iwHarborBuilding>(bldType, buildingRegister.GetHarbors());
    else if(BuildingProperties::IsWareHouse(bldType))
        GoToFirstMatching<iwBaseWarehouse>(bldType, buildingRegister.GetStorehouses());
    else if(bldType == BuildingType::Temple)
        GoToFirstMatching<iwTempleBuilding>(bldType, buildingRegister.GetBuildings(bldType));
    else
        GoToFirstMatching<iwBuilding>(bldType, buildingRegister.GetBuildings(bldType));
}

template<class T_Window, class T_Building>
void iwBuildings::GoToFirstMatching(BuildingType bldType, const std::list<T_Building*>& blds)
{
    for(T_Building* bld : blds)
    {
        if(bld->GetBuildingType() == bldType)
        {
            gwv.MoveToMapPt(bld->GetPos());
            auto nextscrn = std::make_unique<T_Window>(gwv, gcFactory, static_cast<T_Building*>(bld));
            nextscrn->SetPos(GetPos());
            WINDOWMANAGER.ReplaceWindow(std::move(nextscrn));
            return;
        }
    }
}
