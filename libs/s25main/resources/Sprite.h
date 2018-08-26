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

#ifndef Sprite_h__
#define Sprite_h__

#include "Point.h"
#include "Rect.h"
#include "ogl/TextureHandle.h"

namespace libsiedler2 {
class PixelBufferARGB;
}

class Sprite
{
    /// Offset where the image should be drawn (shifts origin)
    Position offset_;
    /// Position and size of the image in the texture
    Rect rect_;
    /// Size of the underlying texture (might be different due to expansion of texture atlas)
    Extent texSize_;
    ogl::TextureHandle texture_;

    void init(Extent size);

public:
    explicit Sprite(Extent size);
    Sprite(const libsiedler2::PixelBufferARGB& data, Position offset);
    /// Render the sprite at the given screen position
    void render(Position pos) const;
};

#endif // Sprite_h__
