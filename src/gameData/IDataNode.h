// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef IDataNode_h__
#define IDataNode_h__

#include <string>
#include <sstream>
#include <limits>

using std::string;

class IDataNode{
    template <typename T>
    std::string ToString(T const &data){
        std::stringstream Io;
        Io << data;
        return Io.str();
    }
    std::string ToString(signed char const& data){
        return ToString<int>(data);
    }
    std::string ToString(unsigned char const& data){
        return ToString<unsigned>(data);
    }
public:
    virtual IDataNode* addNode(const string& name) = 0;
    virtual IDataNode* addNode(const string& name, const string& value) = 0;
    template<typename T>
    inline IDataNode* addNode(const string& name, const T& value){
        return addNode(name, ToString(value));
    }
    virtual void addAttribute(const string& name, const string& value) = 0;

    virtual IDataNode* getChild(const string& name = string(), bool throwOnError = true) const = 0;
    virtual IDataNode* getNextSibling(const string& name = string()) const = 0;

    virtual string getName() const = 0;
    virtual string getValue() const = 0;
    string getValue(const string& name) const;
    template<typename T>
    T getIntValue(const string& name, bool fromNode = true, T minVal = numeric_limits<T>::min(), T maxVal = numeric_limits<T>::max()) const;
    bool getBoolValue(const string& name, bool fromNode = true) const;
    virtual string getAttribute(const string& name) const = 0;
};

#endif // IDataNode_h__