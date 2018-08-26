// Copyright (c) 2005 - 2018 Settlers Freaks (sf-team at siedler25.org)
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

#include "SoundLoader.h"
#include "SoundEffect.h"
#include "driver/SoundHandle.h"
#include "drivers/AudioDriverWrapper.h"
#include "libsiedler2/ArchivItem.h"
#include "libsiedler2/ArchivItem_Sound.h"
#include "libutil/tmpFile.h"

std::shared_ptr<SoundEffect> SoundLoader::load(const ResolvedFile& file) const
{
    if(file.back().extension() != ".wav")
        return NULL;
    SoundHandle handle = AUDIODRIVER.LoadEffect(file.back().string());
    if(handle.isValid())
        return std::shared_ptr<SoundEffect>(new SoundEffect(handle));
    else
        return NULL;
}

std::shared_ptr<SoundEffect> SoundLoader::load(const libsiedler2::ArchivItem& item) const
{
    if(item.getBobType() != libsiedler2::BobType::BOBTYPE_SOUND)
        return NULL;
    const libsiedler2::ArchivItem_Sound& sound = static_cast<const libsiedler2::ArchivItem_Sound&>(item);
    if(sound.getType() != libsiedler2::SoundType::SOUNDTYPE_WAVE)
        return NULL;
    TmpFile tmp(".wav");
    if(tmp.isValid() && sound.write(tmp.getStream()) == 0)
        return load(ResolvedFile(1, tmp.filePath));
    else
        return NULL;
}
