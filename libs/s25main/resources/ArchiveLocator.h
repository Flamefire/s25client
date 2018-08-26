// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ResolvedFile.h"
#include "addons/const_addons.h"
#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

/// Helper for locating archives.
/// An archive is uniquely identified by its name (file stem + extension) and may contain multiple resources.
/// A folder is considered an archive, each file a resource whose names need to start with a numeric index and a dot.
/// The resource system is layered:
/// If an archive exists in multiple folders the archives will be loaded in order with entries present in earlier
/// archives being replaced by those in later ones.
/// This allows modifying existing resources and adding new ones.
class ArchiveLocator
{
    struct FolderData
    {
        /// Path to the folder
        std::string path;
        /// Filenames in the folder
        std::vector<std::string> files;
    };

public:
    /// Convenience function to add standard paths
    void addStandardFolders();
    /// Add a folder to the list of folders containing resources.
    /// Files in folders added last will override prior ones
    /// Paths with macros will be resolved
    /// Setting atBack to false will prepend the folder to the existing ones
    void addFolder(std::string path, bool atBack = true);
    /// Add the folder from an addon
    void addAddonFolder(AddonId id);
    /// Remove all entries
    void clear();
    /// Get all file paths for a given filename
    ResolvedFile resolveFile(const std::string& filename);

private:
    std::vector<FolderData> folders_;
};

