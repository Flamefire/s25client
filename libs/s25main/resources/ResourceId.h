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

#ifndef ResourceId_h__
#define ResourceId_h__

#include "RTTR_Assert.h"
#include <string>

/// Identifies a resource. Normally an entry in an archive, but may also be a plain file (e.g. ogg file)
/// Only filenames are allowed as overriding files work on filenames only, hence filenames must be unique
struct ResourceId
{
    std::string filename;
    unsigned index;
    explicit ResourceId(const std::string& possibleFilename, unsigned index = 0) : index(index)
    {
        // Due to legacy reasons we can have a full path here. So extract the filename
        // Disallow windows style separators (we own the ids, so this should not occur)
        RTTR_Assert(possibleFilename.find_first_of('\\') == std::string::npos);
        size_t pos = possibleFilename.find_last_of('/');
        if(pos != std::string::npos)
            filename = possibleFilename.substr(pos + 1u);
        else
            filename = possibleFilename;
    }
    bool operator<(const ResourceId& rhs) const { return filename < rhs.filename || (filename == rhs.filename && index < rhs.index); }
};

#endif // ResourceId_h__
