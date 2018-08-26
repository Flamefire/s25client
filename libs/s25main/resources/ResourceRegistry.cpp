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

///////////////////////////////////////////////////////////////////////////////

#include "ResourceRegistry.h"
#include "ArchiveLoader.h"
#include "MusicLoader.h"
#include "SoundLoader.h"
#include "SpriteLoader.h"
#include "helpers/strUtils.h"
#include <libsiedler2/ArchivItem_Palette.h>
#include <sstream>

template<class T>
struct GetLoader;

template<>
struct GetLoader<libsiedler2::Archiv>
{
    using type = ArchiveLoader;
};

template<>
struct GetLoader<SoundEffect>
{
    using type = SoundLoader;
};

template<>
struct GetLoader<Music>
{
    using type = MusicLoader;
};

template<>
struct GetLoader<Sprite>
{
    using type = SpriteLoader;
};

std::out_of_range ResourceRegistry::makeNotFoundError(const ResourceId& id)
{
    std::stringstream ss;
    ss << "Resource " << id.filename << "#" << id.index << " not found";
    return std::out_of_range(ss.str());
}

struct PaletteLoader
{
    std::shared_ptr<libsiedler2::ArchivItem_Palette> load(libsiedler2::ArchivItem* pal) const
    {
        return std::shared_ptr<libsiedler2::ArchivItem_Palette>(dynamic_cast<libsiedler2::ArchivItem_Palette*>(pal->clone()));
    }
};

const libsiedler2::ArchivItem_Palette& ResourceRegistry::getPalette(const std::string& id)
{
    PaletteCache::ResPtr res = palettes_.tryGet(id);
    if(res)
        return *res;
    std::vector<bfs::path> resolvedFile = locator_.resolveFile(id);
    if(resolvedFile.empty())
        throw makeNotFoundError(ResourceId(id));
    ArchiveCache::ResPtr archive = archives_.load(ArchiveLoader(), id, resolvedFile);
    if(!archive)
        throw makeNotFoundError(ResourceId(id));
    res = palettes_.load(PaletteLoader(), id, archive->get(0));
    if(!res)
        throw makeNotFoundError(ResourceId(id));
    return *res;
}

template<class T_Cache>
const typename T_Cache::Resource& ResourceRegistry::get(T_Cache& cache, const ResourceId& id)
{
    typename T_Cache::ResPtr res = cache.tryGet(id);
    if(res)
        return *res;
    std::vector<bfs::path> resolvedFile = locator_.resolveFile(id.filename);
    if(resolvedFile.empty())
        throw makeNotFoundError(id);
    typename GetLoader<typename T_Cache::Resource>::type loader;
    if(id.index == 0u)
    {
        res = cache.load(loader, id, resolvedFile);
        if(res)
            return *res;
    }
    ArchiveCache::ResPtr archive = archives_.load(ArchiveLoader(&getPalette("PAL5.BBM")), id.filename, resolvedFile);
    if(!archive || !archive->get(id.index))
        throw makeNotFoundError(id);
    res = cache.load(loader, id, *archive->get(id.index));
    if(!res)
        throw makeNotFoundError(id);
    return *res;
}

const SoundEffect& ResourceRegistry::getEffect(const ResourceId& id)
{
    return get(soundEffects_, id);
}

const Music& ResourceRegistry::getMusic(const ResourceId& id)
{
    return get(musics_, id);
}

const Sprite& ResourceRegistry::getSprite(const ResourceId& id)
{
    return get(sprites_, id);
}
