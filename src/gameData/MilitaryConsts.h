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

#ifndef MILITARY_CONSTS_H_
#define MILITARY_CONSTS_H_

#include "PositionPtInit.h"
#include "gameTypes/Nation.h"
#include <boost/array.hpp>

/// Größe der Militärquadrate (in Knotenpunkten), in die die Welt eingeteilt wurde für Militärgebäude
const unsigned short MILITARY_SQUARE_SIZE = 20;

/// Maximale Entfernungen für "nahe Militärgebäudedistanz" und "mittlere Militärgebäudedistanz"
const unsigned MAX_MILITARY_DISTANCE_NEAR = 18;
const unsigned MAX_MILITARY_DISTANCE_MIDDLE = 26;

/// highest military rank - currently ranks 0-4 available
const unsigned MAX_MILITARY_RANK = 4;

/// Basisangriffsreichweite (Angriff mit allen Soldaten möglich)
const unsigned BASE_ATTACKING_DISTANCE = 21;

/// Erweiterte Reichweite, für die jeweils ein Soldat von der Angriffsarmee abgezogen wird
const unsigned EXTENDED_ATTACKING_DISTANCE = 1;

/// Maximale Länge für den Laufweg beim Angriff
const unsigned MAX_ATTACKING_RUN_DISTANCE = 40;

/// Distanz zwischen zwei Gegnern, sodass diese aufeinander zugehen
const unsigned MEET_FOR_FIGHT_DISTANCE = 5;

/// Besatzung in den einzelnen Militärgebäuden
const boost::array<int, 4> NUM_TROOPS = {{2, 3, 6, 9}};

/// Radien der Militärgebäude
const boost::array<unsigned, 4> SUPPRESS_UNUSED MILITARY_RADIUS = {{8, 9, 10, 11}};
// Radius für einzelne Hafen(baustellen)
const unsigned HARBOR_RADIUS = 8;
const unsigned HQ_RADIUS = 9;

/// maximale Hitpoints der Soldaten
const boost::array<unsigned, 5> HITPOINTS = {{3, 4, 5, 6, 7}};

/// Max distance for an attacker to reach a building and join in capturing
const unsigned MAX_FAR_AWAY_CAPTURING_DISTANCE = 15;

/// Sichtweite der Militärgebäude (relativ); wird auf die normale Grenzweite draufaddiert
const unsigned VISUALRANGE_MILITARY = 3;
/// Sichtweite von Spähtürmen (absolut)
const unsigned VISUALRANGE_LOOKOUTTOWER = 20;
/// Sichtweite von Spähern
const unsigned VISUALRANGE_SCOUT = 3;
/// Sichtweite von Soldaten
const unsigned VISUALRANGE_SOLDIER = 2;
/// Sichtweite von Schiffen
const unsigned VISUALRANGE_SHIP = 2;
/// Sichtweite von Erkundungs-Schiffen
const unsigned VISUALRANGE_EXPLORATION_SHIP = 12;

/// Beförderungszeit von Soldaten ( =UPGRADE_TIME + rand(UPGRADE_TIME_RANDOM) )
const unsigned UPGRADE_TIME = 100;
const unsigned UPGRADE_TIME_RANDOM = 300;
/// Genesungszeit von Soldaten in Häusern, Zeit, die gebraucht wird um sich um einen Hitpoint zu erholen
// ( =CONVALESCE_TIME + rand(CONVALESCE_TIME_RANDOM) )
const unsigned CONVALESCE_TIME = 500;
const unsigned CONVALESCE_TIME_RANDOM = 500;

/// Maximale Entfernung des Militärgebäudes von dem Hafen bei Seeangriffen
const unsigned SEAATTACK_DISTANCE = 15;

#endif
