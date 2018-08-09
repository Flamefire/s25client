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
#include "iwBuildingProductivities.h"
#include "GamePlayer.h"
#include "Loader.h"
#include "files.h"
#include "helpers/mathFuncs.h"
#include "gameData/BuildingProperties.h"
#include "gameData/NationDesc.h"
#include "gameData/const_gui_ids.h"
#include "libutil/colors.h"

/// Abstand vom linken, oberen Fensterrand
const Extent bldProdContentOffset(50, 30);
/// Abstand vom rechten Fensterrand
const unsigned short right_x = 40;
/// Horizontaler Abstand zwischen Bild und Prozentbar
const unsigned short image_percent_x = 35;
/// Horizontaler Abstand zwischen Prozentbar und nächstem Bild
const unsigned short percent_image_x = 40;
/// Vertikaler Abstand zwischen 2 nacheinanderfolgenden "Zeilen"
const unsigned short distance_y = 35;

/// Größe der Prozentbalken
const Extent percentSize(100, 18);

iwBuildingProductivities::iwBuildingProductivities(const GamePlayer& player)
    : IngameWindow(CGI_BUILDINGSPRODUCTIVITY, IngameWindow::posLastOrCenter, Extent(10, 10), _("Productivity"),
                   LOADER.GetImageN("resource", 41)),
      player(player), percents(NUM_BUILDING_TYPES, 0)
{
    std::vector<BuildingType> productionBlds;
    for(unsigned i = 0; i < NUM_BUILDING_TYPES; i++)
    {
        BuildingType bld = BuildingType(i);
        if(BuildingProperties::IsProduction(bld))
            productionBlds.push_back(bld);
    }
    Extent iwSize =
      Extent(2 * percentSize.x + 2 * image_percent_x + percent_image_x + right_x, (productionBlds.size() / 2 + 1) * (distance_y + 1))
      + bldProdContentOffset;
    SetIwSize(iwSize);
    const NationDesc& nationDesc = player.GetNationDesc();
    unsigned curYPos = 0;
    for(unsigned y = 0; y < productionBlds.size();)
    {
        for(unsigned x = 0; x < 2 && y < productionBlds.size(); ++x, ++y)
        {
            BuildingType bld = productionBlds[y];
            unsigned imgId = y * 2;
            DrawPoint imgPos(x * (percent_image_x + percentSize.x + image_percent_x), curYPos + percentSize.y / 2);
            imgPos = imgPos + bldProdContentOffset;
            if(player.IsBuildingEnabled(bld))
            {
                ITexture* img = LOADER.GetTexFromFile(nationDesc.buildings[bld].icon);
                AddImage(imgId, imgPos, img, _(nationDesc.buildings[bld].name));
                DrawPoint percentPos = imgPos + DrawPoint(image_percent_x, percentSize.y / 2);
                AddPercent(imgId + 1, percentPos, percentSize, TC_GREY, COLOR_YELLOW, SmallFont, &percents[bld]);
            } else
                AddImage(imgId, imgPos, LOADER.GetImageN("io", 188));
        }
        curYPos += distance_y;
    }

    UpdatePercents();

    // Hilfe-Button
    // Original S2 does not have a Help button in this window. Add it if you have something to say.
    // AddImageButton(500, GetSize().x - 14 - 30, GetSize().y - 20 - 32, 30, 32, TC_GREY, LOADER.GetImageN("io", 225), _("Help"));
}

/// Aktualisieren der Prozente
void iwBuildingProductivities::UpdatePercents()
{
    player.GetBuildingRegister().CalcProductivities(percents);
}

/// Produktivitäts-percentbars aktualisieren
void iwBuildingProductivities::Msg_PaintAfter()
{
    UpdatePercents();
}
