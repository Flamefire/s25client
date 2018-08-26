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

#include "SpriteLoader.h"
#include "Sprite.h"
#include <libsiedler2/ArchivItem_Bitmap.h>
#include <libsiedler2/PixelBufferARGB.h>

std::shared_ptr<Sprite> SpriteLoader::load(const libsiedler2::ArchivItem& item) const
{
    if(item.getBobType() != libsiedler2::BOBTYPE_BITMAP_RAW && item.getBobType() != libsiedler2::BOBTYPE_BITMAP_RLE)
        return NULL;
    const libsiedler2::baseArchivItem_Bitmap& bmp = dynamic_cast<const libsiedler2::baseArchivItem_Bitmap&>(item);
    libsiedler2::PixelBufferARGB buffer(bmp.getWidth(), bmp.getHeight());
    bmp.print(buffer);
    return std::shared_ptr<Sprite>(new Sprite(buffer, Position(bmp.getNx(), bmp.getNy())));
}

std::shared_ptr<Sprite> SpriteLoader::load(const ResolvedFile& file) const
{
    return NULL;
}
