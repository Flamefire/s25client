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

#ifndef ArchiveLocator_h__
#define ArchiveLocator_h__

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

#endif // ArchiveLocator_h__
