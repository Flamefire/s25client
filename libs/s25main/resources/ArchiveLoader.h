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

#ifndef ArchiveLoader_h__
#define ArchiveLoader_h__

#include "ResolvedFile.h"
#include <memory>

namespace libsiedler2 {
class Archiv;
class ArchivItem_Palette;
} // namespace libsiedler2

struct ArchiveLoader
{
    explicit ArchiveLoader(const libsiedler2::ArchivItem_Palette* palette = NULL) : palette_(palette) {}
    std::shared_ptr<libsiedler2::Archiv> load(const ResolvedFile& file) const;

private:
    const libsiedler2::ArchivItem_Palette* palette_;
};

#endif // ArchiveLoader_h__
