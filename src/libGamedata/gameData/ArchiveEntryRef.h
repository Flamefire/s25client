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

#ifndef ArchiveEntryRef_h__
#define ArchiveEntryRef_h__

#include <boost/flyweight.hpp>
#include <string>

struct ArchiveEntryRef
{
    boost::flyweight<std::string> filepath;
    unsigned index;
    ArchiveEntryRef(const std::string& filepath = "", unsigned index = 0) : filepath(filepath), index(index) {}
};

namespace kaguya {
template<class T, typename Enable>
struct lua_type_traits;
template<>
struct lua_type_traits<ArchiveEntryRef, void>;
} // namespace kaguya

#endif // ArchiveEntryRef_h__
