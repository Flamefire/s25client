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

#include "Sprite.h"
#include "drivers/VideoDriverWrapper.h"
#include <libsiedler2/PixelBufferARGB.h>
#include <glad/glad.h>

Sprite::Sprite(Extent size) : offset_(0, 0)
{
    init(size);
}

Sprite::Sprite(const libsiedler2::PixelBufferARGB& data, Position offset) : offset_(offset)
{
    init(Extent(data.getWidth(), data.getHeight()));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data.getWidth(), data.getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, data.getPixelPtr());
}

void Sprite::render(Position pos) const {}

void Sprite::init(Extent size)
{
    rect_.setSize(size);
    if(size.x * size.y == 0u)
    {
        texSize_ = Extent(0, 0);
        return;
    }
    texSize_ = VIDEODRIVER.calcPreferredTextureSize(size);
    texture_ = ogl::TextureHandle::create();
    VIDEODRIVER.BindTexture(texture_.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize_.x, texSize_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
}
