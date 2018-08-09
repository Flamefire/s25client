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
#include "nofBaker.h"
#include "GamePlayer.h"
#include "Loader.h"
#include "SoundManager.h"
#include "buildings/nobUsual.h"
#include "network/GameClient.h"
#include "ogl/glArchivItem_Bitmap.h"
#include "ogl/glArchivItem_Bitmap_Player.h"
#include "ogl/glSmartBitmap.h"
#include "world/GameWorldGame.h"
#include "gameData/NationData.h"

nofBaker::nofBaker(const MapPoint pos, const unsigned char player, nobUsual* workplace) : nofWorkman(JOB_BAKER, pos, player, workplace) {}

nofBaker::nofBaker(SerializedGameData& sgd, const unsigned obj_id) : nofWorkman(sgd, obj_id) {}

void nofBaker::DrawWorking(DrawPoint drawPt)
{
    unsigned max_id = 120;
    unsigned now_id = GAMECLIENT.Interpolate(max_id, current_ev);
    unsigned char wpNation = workplace->GetNation().value;
    unsigned plColor = gwg->GetPlayer(player).color;

    if(now_id < 8)
    {
        // position zum rauslaufen berechnen
        DrawPoint walkPos = drawPt + workplace->GetDescription().getWorkOffset()baker::walkOffsets[wpNation][now_id];
        if(now_id < 4)
            workplace->DrawDoor(drawPt);
        unsigned walkDir = baker::walkdirection[wpNation][std::min(now_id / 2u, 2u)];
        LOADER.bob_jobs_cache[wpNation][JOB_BAKER][walkDir][now_id].draw(walkPos, COLOR_WHITE, plColor);

    }else if(now_id < 16) // brot in den ofen schieben
    {
        LOADER.GetPlayerImage("rom_bobs", 182 + (now_id - 8))->DrawFull(drawPt + baker::workOffset[wpNation], COLOR_WHITE, plColor);

        // "Brot-rein/raus"-Sound
        if(now_id == 20)
        {
            SOUNDMANAGER.PlayNOSound(68, this, now_id);
            was_sounding = true;
        }
    }else if(now_id < max_id - 16) // warten
    {
        LOADER.GetPlayerImage("rom_bobs", 189)->DrawFull(drawPt + baker::workOffset[wpNation], COLOR_WHITE, plColor);
    }else if(now_id < max_id - 8) // brot aus dem ofen holen
    {
        LOADER.GetPlayerImage("rom_bobs", 182 + 7 - (now_id % 8))->DrawFull(drawPt + baker::workOffset[wpNation], COLOR_WHITE, plColor);

        // "Brot-rein/raus"-Sound
        if(now_id == max_id - 12)
        {
            SOUNDMANAGER.PlayNOSound(68, this, now_id);
            was_sounding = true;
        }
    } else
    {
        unsigned now_id = max_id - now_id - 1;
        RTTR_Assert(now_id < 8);
        // position zum reinlaufen berechnen
        DrawPoint walkPos = drawPt + baker::walkOffsets[wpNation][now_id];
        if(now_id < 4)
            workplace->DrawDoor(drawPt);
        unsigned walkDir = baker::walkdirection[wpNation][std::min(now_id / 2u, 2u)];
        LOADER.bob_jobs_cache[wpNation][JOB_BAKER][walkDir][now_id].draw(walkPos, COLOR_WHITE, plColor);
    }
}

GoodType nofBaker::ProduceWare()
{
    return GD_BREAD;
}
