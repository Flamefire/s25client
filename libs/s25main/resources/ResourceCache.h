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

#ifndef ResourceCache_h__
#define ResourceCache_h__

#include "RTTR_Assert.h"
#include <map>
#include <memory>
#include <stdexcept>

/// Caches already loaded resources of a given type
/// Works together with a loader that is expected to have no state and a function load(...)
/// returning a ptr to the resource constructed from the arguments
template<class T_Resource, class T_Key>
class ResourceCache
{
public:
    using Resource = T_Resource;
    using ResPtr = std::shared_ptr<Resource>;

private:
    using map = std::map<T_Key, ResPtr>;
    using value_type = typename map::value_type;
    map resources_;

public:
    /// Remove all entries deleting them
    void clear() { resources_.clear(); }
    bool contains(const T_Key& key) { return resources_.find(key) != resources_.end(); }
    /// Return the resource with the given key or throw if it does not exist
    T_Resource& get(const T_Key& key) const
    {
        ResPtr res = tryGet(key);
        if(!res)
            throw std::out_of_range("Did not find element");
        return *res;
    }
    /// Return the resource with the given key or NULL
    ResPtr tryGet(const T_Key& key) const
    {
        map::const_iterator it = resources_.find(key);
        return it == resources_.end() ? NULL : it->second;
    }
    /// Load a given resource if it does not exist yet and return it
    template<class T_Loader, class T_Arg>
    ResPtr load(const T_Loader& loader, const T_Key& key, const T_Arg& arg)
    {
        map::iterator it = resources_.find(key);
        if(it != resources_.end())
            return it->second;
        ResPtr res = loader.load(arg);
        if(res)
            add(key, res);
        return res;
    }
    /// Load a given resource if it does not exist yet and return it
    /// Assumes the key can be used directly to construct an instance
    template<class T_Loader>
    ResPtr load(const T_Loader& loader, const T_Key& argAndKey)
    {
        return load(loader, argAndKey, argAndKey);
    }
    void add(const T_Key& key, ResPtr res)
    {
        RTTR_Assert(!contains(key));
        resources_.insert(value_type(key, res));
    }
    /// Delete an entry from the cache
    void discard(const T_Key& key) { resources_.erase(key); }
};

#endif // ResourceCache_h__
