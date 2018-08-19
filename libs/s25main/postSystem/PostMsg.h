// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ESoundEffect.h"
#include "PostCategory.h"
#include "gameTypes/MapCoordinates.h"
#include "gameTypes/PactTypes.h"
#include <string>

struct BasePlayerInfo;
class ITexture;

class PostMsg
{
public:
    /// Create a simple message
    PostMsg(unsigned sendFrame, std::string text, PostCategory cat, const MapPoint& pt,
            ESoundEffect soundEffect = ESoundEffect::Pidgeon);
    PostMsg(unsigned sendFrame, std::string text, PostCategory cat, ESoundEffect soundEffect = ESoundEffect::Pidgeon);
    /// Response to a diplomacy question. Last parameter states if the pact was accepted(true) or canceled(false)
    PostMsg(unsigned sendFrame, PactType pt, const BasePlayerInfo& otherPlayer, bool acceptedOrCanceled,
            ESoundEffect soundEffect = ESoundEffect::Pidgeon);
    virtual ~PostMsg() = default;

    unsigned GetSendFrame() const { return sendFrame_; }
    const std::string& GetText() const { return text_; }
    PostCategory GetCategory() const { return cat_; }
    /// Get position related to this message (optional, defaults to invalid)
    MapPoint GetPos() const { return pt_; }
    ESoundEffect GetSoundEffect() const { return soundEffect_; }
    /// Get Associated image or nullptr if none exists
    virtual ITexture* GetImage_() const { return nullptr; }

protected:
    void SetText(const std::string& text) { text_ = text; }

private:
    unsigned sendFrame_;
    std::string text_;
    PostCategory cat_;
    MapPoint pt_;
    ESoundEffect soundEffect_;
};
