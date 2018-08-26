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

#include "MusicLoader.h"
#include "Music.h"
#include "driver/SoundHandle.h"
#include "drivers/AudioDriverWrapper.h"
#include "mygettext/mygettext.h"
#include "libsiedler2/ArchivItem.h"
#include "libsiedler2/ArchivItem_Sound.h"
#include "libsiedler2/ArchivItem_Sound_Midi.h"
#include "libsiedler2/ArchivItem_Sound_XMidi.h"
#include "libutil/Log.h"
#include "libutil/tmpFile.h"

std::shared_ptr<Music> MusicLoader::load(const ResolvedFile& file) const
{
    const bfs::path extension = file.back().extension();
    if(extension != ".midi" && extension != ".mp3" && extension != "ogg")
        return NULL;
    SoundHandle handle = AUDIODRIVER.LoadMusic(file.back().string());
    if(handle.isValid())
        return std::shared_ptr<Music>(new Music(handle));
    else
        return NULL;
}

std::shared_ptr<Music> MusicLoader::load(const libsiedler2::ArchivItem& item) const
{
    if(item.getBobType() != libsiedler2::BobType::BOBTYPE_SOUND)
        return NULL;
    const libsiedler2::ArchivItem_Sound& sound = static_cast<const libsiedler2::ArchivItem_Sound&>(item);
    std::string ext;
    switch(sound.getType())
    {
        default: return NULL;
        case libsiedler2::SoundType::SOUNDTYPE_XMIDI:
        {
            libsiedler2::ArchivItem_Sound_Midi midiFile;
            try
            {
                const libsiedler2::MIDI_Track midiTrack = static_cast<const libsiedler2::ArchivItem_Sound_XMidi&>(sound).getMidiTrack(0);
                midiFile.addTrack(midiTrack);
            } catch(std::exception& e)
            {
                LOG.write(_("Failed to load XMIDI track: %1%")) % e.what();
                return NULL;
            }
            return load(midiFile);
        }
        case libsiedler2::SoundType::SOUNDTYPE_MIDI: ext = ".midi"; break;
        case libsiedler2::SoundType::SOUNDTYPE_MP3: ext = ".mp3"; break;
        case libsiedler2::SoundType::SOUNDTYPE_OGG: ext = ".ogg"; break;
    }
    TmpFile tmp(ext);
    if(tmp.isValid() && sound.write(tmp.getStream()) == 0)
        return load(ResolvedFile(1, tmp.filePath));
    else
        return NULL;
}
