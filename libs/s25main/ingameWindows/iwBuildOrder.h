// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "TransmitSettingsIgwAdapter.h"
#include "gameTypes/SettingsTypes.h"

class GameWorldViewer;

class iwBuildOrder : public TransmitSettingsIgwAdapter
{
    const GameWorldViewer& gwv;

    BuildOrders pendingBuildOrder;
    bool useCustomBuildOrder;

public:
    iwBuildOrder(const GameWorldViewer& gwv);

private:
    /// Updates the control elements with the pending build order
    void UpdateSettings() override;
    /// Sendet veränderte Einstellungen (an den Client), falls sie verändert wurden
    void TransmitSettings() override;

    void Msg_ListSelectItem(unsigned ctrl_id, int selection) override;
    void Msg_ButtonClick(unsigned ctrl_id) override;
    void Msg_ComboSelectItem(unsigned ctrl_id, unsigned selection) override;
};
