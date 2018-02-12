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

#pragma once

#ifndef PositionPtInit_h__
#define PositionPtInit_h__

#include "Point.h"

/// Helper struct to allow use in array initializers till C++11.
/// Implicitly convertible to Position
struct PositionPtInit
{
    typedef Position::ElementType ElementType;
    const ElementType x;
    const ElementType y;
    operator Position() const { return Position(x, y); }
};

// Helper for defining the operations
#define DEF_DRAWPOINT_OP(OP)                                                                                     \
    inline Position operator OP(const Position& lhs, const PositionPtInit& rhs) { return lhs OP Position(rhs); } \
    inline Position operator OP(const PositionPtInit& lhs, const Position& rhs) { return Position(lhs) OP rhs; } \
    inline Position operator OP(const PositionPtInit& lhs, const PositionPtInit& rhs) { return Position(lhs) OP rhs; }

DEF_DRAWPOINT_OP(+)
DEF_DRAWPOINT_OP(-)
DEF_DRAWPOINT_OP(*)
DEF_DRAWPOINT_OP(/)

#undef DEF_DRAWPOINT_OP

#endif // PositionPtInit_h__
