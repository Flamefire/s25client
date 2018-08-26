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

#ifndef ResourceRegistry_h__
#define ResourceRegistry_h__

#include "ResourceCache.h"
#include "ResourceId.h"
#include "ArchiveLocator.h"
#include <libsiedler2/Archiv.h>
#include <string>

namespace libsiedler2 {
class ArchivItem_Palette;
class Archiv;
} // namespace libsiedler2
class SoundEffect;
class Music;
class Sprite;

class ResourceRegistry
{
    using ArchiveCache = ResourceCache<libsiedler2::Archiv, std::string>;
    using PaletteCache = ResourceCache<libsiedler2::ArchivItem_Palette, std::string>;
    using SoundCache = ResourceCache<SoundEffect, ResourceId>;
    using MusicCache = ResourceCache<Music, ResourceId>;
    using SpriteCache = ResourceCache<Sprite, ResourceId>;
    ArchiveLocator locator_;
    ArchiveCache archives_;
    PaletteCache palettes_;
    SoundCache soundEffects_;
    MusicCache musics_;
    SpriteCache sprites_;

    std::out_of_range makeNotFoundError(const ResourceId& id);
    template<class T_Cache>
    const typename T_Cache::Resource& get(T_Cache&, const ResourceId&);

public:
    const libsiedler2::ArchivItem_Palette& getPalette(const std::string& id);
    const SoundEffect& getEffect(const ResourceId& id);
    const Music& getMusic(const ResourceId& id);
    const Sprite& getSprite(const ResourceId& id);
};

#endif // ResourceRegistry_h__
