// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IngameWindow.h"
#include <string>

/// Soforthilfe-Fenster
class iwHelp : public IngameWindow
{
public:
    explicit iwHelp(const std::string& content);
};
