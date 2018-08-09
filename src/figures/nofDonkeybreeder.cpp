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
#include "nofDonkeybreeder.h"

#include "GamePlayer.h"
#include "Loader.h"
#include "buildings/nobUsual.h"
#include "network/GameClient.h"
#include "nofCarrier.h"
#include "ogl/glArchivItem_Bitmap.h"
#include "ogl/glArchivItem_Bitmap_Player.h"
#include "ogl/glSmartBitmap.h"
#include "world/GameWorldGame.h"
#include "libutil/colors.h"
#include "gameData/BuildingDesc.h"

nofDonkeybreeder::nofDonkeybreeder(const MapPoint pos, unsigned char player, nobUsual* workplace)
    : nofWorkman(JOB_DONKEYBREEDER, pos, player, workplace)
{}

nofDonkeybreeder::nofDonkeybreeder(SerializedGameData& sgd, unsigned obj_id) : nofWorkman(sgd, obj_id) {}

/**
 *  Zeichnet ihn beim Arbeiten.
 */
void nofDonkeybreeder::DrawWorking(DrawPoint drawPt)
{
    const Nation nation = workplace->GetNation();
    const unsigned color = gwg->GetPlayer(player).color;

    unsigned now_id = GAMECLIENT.Interpolate(96, current_ev);
    DrawPoint walkBasePos = drawPt + workplace->GetDescription().getWorkOffset("start");
    DrawPoint workPos = drawPt + workplace->GetDescription().getWorkOffset("work");

    if(now_id < 4)
    {
        workplace->DrawDoor(drawPt);
        LOADER.bob_jobs_cache[nation.value][JOB_DONKEYBREEDER][4][now_id % 8].draw(
          walkBasePos + DrawPoint(now_id, now_id), COLOR_WHITE, color);
    } else if(now_id < 12)
    {
        DrawPoint walkPos = Interpolate(walkBasePos + DrawPoint::all(4), workPos, now_id - 4, 8u);
        LOADER.bob_jobs_cache[nation.value][JOB_DONKEYBREEDER][3][now_id % 8].draw(
            walkPos, COLOR_WHITE, color);
    } else if(now_id < 20)
        LOADER.GetPlayerImage("rom_bobs", 291 + (now_id - 12))
          ->DrawFull(walkBasePos + workPos, COLOR_WHITE, color);
    else if(now_id < 28)
    {
        DrawPoint walkPos = Interpolate(workPos, walkBasePos + DrawPoint::all(4), now_id - 4, 8u);
        LOADER.bob_jobs_cache[nation.value][JOB_DONKEYBREEDER][0][(now_id - 20) % 8].draw(
            walkPos, COLOR_WHITE, color);
    }
    else if(now_id < 32)
    {
        workplace->DrawDoor(drawPt);
        LOADER.bob_jobs_cache[nation.value][JOB_DONKEYBREEDER][1][(now_id - 28) % 8].draw(
          walkBasePos + DrawPoint(32 - now_id, 32 - now_id), COLOR_WHITE, color);
    }
}

/**
 *  Der Arbeiter erzeugt eine Ware.
 */
GoodType nofDonkeybreeder::ProduceWare()
{
    /// @todo Wie kann ich hier eine Person erzeugen?
    return GD_NOTHING;
}

void nofDonkeybreeder::WorkFinished()
{
    // Straße und Zielflagge für Esel suchen
    noRoadNode* flag_goal;
    RoadSegment* road = gwg->GetPlayer(player).FindRoadForDonkey(workplace, &flag_goal);

    // Esel erzeugen und zum Ziel beordern
    nofCarrier* donkey = new nofCarrier(nofCarrier::CT_DONKEY, pos, player, road, flag_goal);
    gwg->GetPlayer(player).IncreaseInventoryJob(JOB_PACKDONKEY, 1);
    donkey->InitializeRoadWalking(gwg->GetSpecObj<noRoadNode>(pos)->GetRoute(Direction::SOUTHEAST), 0, true);

    // Wenn keine Straße gefunden wurde, muss er nach Hause gehen
    if(!road)
        donkey->GoHome();
    else
        // ansonsten Arbeitsplatz Bescheid sagen
        road->GotDonkey(donkey);

    // Esel absetzen
    gwg->AddFigure(pos, donkey);

    // In die neue Welt laufen
    donkey->ActAtFirst();
}
