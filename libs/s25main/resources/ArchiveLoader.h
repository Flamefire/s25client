// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ResolvedFile.h"
#include <memory>

namespace libsiedler2 {
class Archiv;
class ArchivItem_Palette;
} // namespace libsiedler2

struct ArchiveLoader
{
    explicit ArchiveLoader(const libsiedler2::ArchivItem_Palette* palette = NULL) : palette_(palette) {}
    std::shared_ptr<libsiedler2::Archiv> load(const ResolvedFile& file) const;

private:
    const libsiedler2::ArchivItem_Palette* palette_;
};

