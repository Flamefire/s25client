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

#ifndef PointTraits_h__
#define PointTraits_h__

#include "Point.h"
#include "gameData/ArchiveEntryRef.h"
#include <kaguya/kaguya.hpp>

namespace kaguya {

template<typename T>
struct lua_type_traits<Point<T> >
{
    typedef Point<T> get_type;
    typedef const Point<T>& push_type;

    struct checkTypeForEach
    {
        checkTypeForEach(bool& valid) : valid_(valid) {}
        bool& valid_;
        bool operator()(const LuaStackRef& k, const LuaStackRef& v)
        {
            valid_ = k.isConvertible<size_t>() && v.isConvertible<T>();
            return valid_;
        }
    };
    struct strictCheckTypeForEach
    {
        strictCheckTypeForEach(bool& valid) : valid_(valid) {}
        bool& valid_;
        bool operator()(const LuaStackRef& k, const LuaStackRef& v)
        {
            valid_ = k.isType<size_t>() && v.isType<T>();
            return valid_;
        }
    };

    static bool checkType(lua_State* l, int index)
    {
        if(lua_type(l, index) != LUA_TTABLE)
            return false;

        LuaStackRef table(l, index);
        if(table.size() != 2)
            return false;
        bool valid = true;
        table.foreach_table_breakable<LuaStackRef, LuaStackRef>(checkTypeForEach(valid));
        return valid;
    }
    static bool strictCheckType(lua_State* l, int index)
    {
        if(lua_type(l, index) != LUA_TTABLE)
            return false;

        LuaStackRef table(l, index);
        if(table.size() != 2)
            return false;
        bool valid = true;
        table.foreach_table_breakable<LuaStackRef, LuaStackRef>(strictCheckTypeForEach(valid));
        return valid;
    }
    static get_type get(lua_State* l, int index)
    {
        if(lua_type(l, index) != LUA_TTABLE)
        {
            except::typeMismatchError(l, std::string("type mismatch"));
            return get_type();
        }
        LuaStackRef t(l, index);
        if(t.size() != 2)
        {
            except::typeMismatchError(l, std::string("type mismatch"));
            return get_type();
        }
        return get_type(t[1], t[2]);
    }
    static int push(lua_State* l, push_type v)
    {
        lua_createtable(l, 2, 0);
        LuaStackRef table(l, -1);
        table.setRawField(1, v.x);
        table.setRawField(2, v.y);
        return 1;
    }
};

template<>
struct lua_type_traits<ArchiveEntryRef>
{
    typedef ArchiveEntryRef get_type;
    typedef const ArchiveEntryRef& push_type;
    static bool checkType(lua_State* l, int index)
    {
        LuaStackRef t(l, index);
        if(!t.getField<optional<std::string> >("filepath"))
            return false;
        optional<LuaRef> idx = t.getField<optional<LuaRef> >("idx");
        return !idx || idx->isConvertible<unsigned>();
    }
    static bool strictCheckType(lua_State* l, int index)
    {
        LuaStackRef t(l, index);
        if(!t.getField<optional<std::string> >("filepath"))
            return false;
        optional<LuaRef> idx = t.getField<optional<LuaRef> >("idx");
        return !idx || idx->isType<unsigned>();
    }

    static get_type get(lua_State* l, int index)
    {
        LuaStackRef t(l, index);
        optional<LuaRef> idx = t.getField<optional<LuaRef> >("idx");
        return get_type(t["filepath"], idx ? *idx : 0u);
    }

    static int push(lua_State* l, push_type v)
    {
        lua_createtable(l, 0, 2);
        LuaStackRef table(l, -1);
        table.setRawField("filepath", v.filepath);
        if(v.index != 0u)
            table.setRawField("idx", v.index);
        return 1;
    }
};
} // namespace kaguya

#endif // PointTraits_h__
