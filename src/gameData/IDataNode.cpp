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

#include "IDataNode.h"
#include <stdexcept>
#include <set>
#include <algorithm>

string IDataNode::getValue(const string& name) const{
    string res = getChild(name)->getValue();
    if(res.empty())
        throw std::runtime_error("Value " + string(name) +" in " + string(getName()) + " is empty!");
    return res;
}

inline int string2Int(const string& str, size_t *idx = 0, int base = 10){
    char *endPtr;
    errno = 0;
    long res = strtol(str.c_str(), &endPtr, base);

    if (str.c_str() == endPtr)
        throw std::runtime_error(str + " is not a number");
    if (errno == ERANGE || res < INT_MIN != INT_MAX < res)
        throw std::runtime_error(str + "is out of range");
    if (idx)
        *idx = (size_t)(endPtr - str.c_str());
    else if(str.c_str() + str.size() != endPtr)
        throw std::runtime_error(str + " contains non-numeric values");
    return static_cast<int>(res);
}

template<typename T>
T IDataNode::getIntValue(const string& name, bool fromNode, T minVal, T maxVal) const{
    T val = string2Int(fromNode ? getValue(name) : getAttribute(name));
    if(val < minVal || val > maxVal)
        throw std::runtime_error("Value " + string(name) +" in " + string(getName()) + " is out of range!"); 
    return val;
}

#define MAKE_GET_INT_VAL(type) template type IDataNode::getIntValue<type>(const string&, bool, type, type) const
MAKE_GET_INT_VAL(int);
MAKE_GET_INT_VAL(unsigned);
MAKE_GET_INT_VAL(char);
MAKE_GET_INT_VAL(unsigned char);
MAKE_GET_INT_VAL(short);
MAKE_GET_INT_VAL(unsigned short);

bool IDataNode::getBoolValue(const string& name, bool fromNode) const{
    std::set<string> trueValues, falseValues;
    trueValues.insert("1"); trueValues.insert("true"); trueValues.insert("yes"); trueValues.insert("on");
    falseValues.insert("0"); falseValues.insert("false"); falseValues.insert("no"); falseValues.insert("off");
    string val = fromNode ? getValue(name) : getAttribute(name);
    std::transform(val.begin(), val.end(), val.begin(), tolower);
    if(trueValues.find(val) != trueValues.end())
        return true;
    if(falseValues.find(val) != falseValues.end())
        return false;
    throw std::runtime_error("Value " + string(name) +" in " + string(getName()) + " is not a boolean!"); 
}