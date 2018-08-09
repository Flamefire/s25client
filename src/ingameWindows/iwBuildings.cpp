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

#include "rttrDefines.h" // IWYU pragma: keep
#include "iwBuildings.h"
#include "GamePlayer.h"
#include "Loader.h"
#include "WindowManager.h"
#include "buildings/nobBaseWarehouse.h"
#include "buildings/nobHarborBuilding.h"
#include "buildings/nobMilitary.h"
#include "buildings/nobStorehouse.h"
#include "buildings/nobUsual.h"
#include "files.h"
#include "helpers/mathFuncs.h"
#include "iwBaseWarehouse.h"
#include "iwBuilding.h"
#include "iwHarborBuilding.h"
#include "iwHelp.h"
#include "iwMilitaryBuilding.h"
#include "ogl/FontStyle.h"
#include "ogl/glArchivItem_Font.h"
#include "world/GameWorldView.h"
#include "world/GameWorldViewer.h"
#include "gameTypes/BuildingCount.h"
#include "gameData/BuildingProperties.h"
#include "gameData/NationDesc.h"
#include "gameData/const_gui_ids.h"
#include <boost/foreach.hpp>
#include <cstdio>

const unsigned NUM_BUILDINGS = NUM_BUILDING_TYPES - 1; // All except HQ

// Abstand des ersten Icons vom linken oberen Fensterrand
const DrawPoint iconPadding(30, 40);
// Abstand der einzelnen Symbole untereinander
const DrawPoint iconSpacing(40, 48);
// Abstand der Schriften unter den Icons
const unsigned short font_distance_y = 20;
const unsigned helpID = NUM_BUILDING_TYPES;

iwBuildings::iwBuildings(GameWorldView& gwv, GameCommandFactory& gcFactory)
    : IngameWindow(CGI_BUILDINGS, IngameWindow::posLastOrCenter, Extent(185, 480), _("Buildings"), LOADER.GetImageN("resource", 41)),
      gwv(gwv), gcFactory(gcFactory)
{
    const NationDesc& natDesc = gwv.GetViewer().GetPlayer().GetNationDesc();
    // Symbole für die einzelnen Gebäude erstellen
    for(unsigned y = 0; y < helpers::ceilDiv(NUM_BUILDINGS, 4); ++y)
    {
        for(unsigned x = 0; x < 4; ++x)
        {
            if(y * 4 + x >= NUM_BUILDINGS) //-V547
                break;
            BuildingType bld = BuildingType(y * 4 + x + 1); // Skip HQ (0)
            ITexture* img = LOADER.GetTexFromFile(natDesc.buildings[bld].icon);
            Extent btSize = Extent(32, 32);
            DrawPoint btPos = iconPadding - btSize / 2 + iconSpacing * DrawPoint(x, y);
            AddImageButton(bld, btPos, btSize, TC_GREY, img, _(natDesc.buildings[bld].name));
        }
    }

    // Hilfe-Button
    Extent btSize = Extent(30, 32);
    AddImageButton(helpID, GetSize() - DrawPoint(14, 20) - btSize, btSize, TC_GREY, LOADER.GetImageN("io", 225), _("Help"));
}

/// Anzahlen der Gebäude zeichnen
void iwBuildings::Msg_PaintAfter()
{
    // Anzahlen herausfinden
    BuildingCount bc = gwv.GetViewer().GetPlayer().GetBuildingRegister().GetBuildingNums();

    // Anzahlen unter die Gebäude schreiben
    DrawPoint rowPos = GetDrawPos() + iconPadding + DrawPoint(0, font_distance_y);
    for(unsigned y = 0; y < helpers::ceilDiv(NUM_BUILDINGS, 4); ++y)
    {
        DrawPoint curPos = rowPos;
        for(unsigned x = 0; x < 4; ++x)
        {
            if(y * 4 + x >= NUM_BUILDINGS) //-V547
                break;
            BuildingType bld = BuildingType(y * 4 + x + 1); // Skip HQ (0)
            char txt[64];
            sprintf(txt, "%u/%u", bc.buildings[bld], bc.buildingSites[bld]);
            NormalFont->Draw(curPos, txt, FontStyle::CENTER, COLOR_YELLOW);
            curPos.x += iconSpacing.x;
        }
        rowPos.y += iconSpacing.y;
    }
}

void iwBuildings::Msg_ButtonClick(const unsigned ctrl_id)
{
    if(ctrl_id == helpID) // Help button
    {
        WINDOWMANAGER.Show(new iwHelp(GUI_ID(CGI_HELP), _("The building statistics window gives you an insight into "
                                                          "the number of buildings you have, by type. The number on "
                                                          "the left is the total number of this type of building "
                                                          "completed, the number on the right shows how many are "
                                                          "currently under construction.")));
        return;
    }

    const GamePlayer& localPlayer = gwv.GetViewer().GetPlayer();
    const BuildingRegister& buildingRegister = localPlayer.GetBuildingRegister();

    BuildingType bldType = BuildingType(ctrl_id);
    if(BuildingProperties::IsMilitary(bldType))
        GoToFirstMatching<iwMilitaryBuilding>(bldType, buildingRegister.GetMilitaryBuildings());
    else if(bldType == BLD_HARBORBUILDING)
        GoToFirstMatching<iwHarborBuilding>(bldType, buildingRegister.GetHarbors());
    else if(BuildingProperties::IsWareHouse(bldType))
        GoToFirstMatching<iwBaseWarehouse>(bldType, buildingRegister.GetStorehouses());
    else
        GoToFirstMatching<iwBuilding>(bldType, buildingRegister.GetBuildings(bldType));
}

template<class T_Window, class T_Building>
void iwBuildings::GoToFirstMatching(BuildingType bldType, const std::list<T_Building*>& blds)
{
    BOOST_FOREACH(T_Building* bld, blds)
    {
        if(bld->GetBuildingType() == bldType)
        {
            gwv.MoveToMapPt(bld->GetPos());
            T_Window* nextscrn = new T_Window(gwv, gcFactory, bld);
            nextscrn->SetPos(GetPos());
            WINDOWMANAGER.Show(nextscrn);
            return;
        }
    }
}
