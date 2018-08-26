// Copyright (c) 2018 - 2018 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef TextureHandle_h__
#define TextureHandle_h__

#include <memory>

namespace ogl {
/// RAII Handle of an OpenGL texture
class TextureHandle
{
    struct TextureDeleter
    {
        using pointer = unsigned;
        void operator()(pointer buffer) const;
    };

    std::unique_ptr<unsigned, TextureDeleter> handle_;

public:
    explicit TextureHandle(unsigned handle = 0u) : handle_(handle) {}
    void reset(unsigned handle = 0u) { handle_.reset(handle); }
    unsigned get() const { return handle_.get(); }
    bool isValid() const { return get() != 0u; }
    static TextureHandle create();
};
} // namespace ogl

#endif // TextureHandle_h__
