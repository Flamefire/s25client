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

#include "ArchiveLoader.h"
#include "helpers/chronoIO.h"
#include <libsiedler2/Archiv.h>
#include <libsiedler2/ArchivItem.h>
#include <libsiedler2/ErrorCodes.h>
#include <libsiedler2/libsiedler2.h>
#include <libutil/Log.h>
#include <boost/filesystem.hpp>
#include <chrono>
#include <mygettext/mygettext.h>

namespace bfs = boost::filesystem;

namespace {
/// Load a file into an archive
int loadFile(libsiedler2::Archiv& archive, const bfs::path& filepath, const libsiedler2::ArchivItem_Palette* palette)
{
    LOG.write(_("Loading \"%s\": ")) % filepath;
    return libsiedler2::Load(filepath.string(), archive, palette);
}

/// Load a file into an archive
int loadDirectory(libsiedler2::Archiv& archive, const bfs::path& path, const libsiedler2::ArchivItem_Palette* palette)
{
    LOG.write(_("Loading directory %s\n")) % path;
    std::vector<libsiedler2::FileEntry> files = libsiedler2::ReadFolderInfo(path.string());
    LOG.write(_("  Loading %1% entries: ")) % files.size();
    return libsiedler2::LoadFolder(files, archive, palette);
}

/// Load a file or directory into the archive
bool loadFileOrDir(libsiedler2::Archiv& to, const bfs::path& filePath, const libsiedler2::ArchivItem_Palette* palette)
{
    using namespace std::chrono;

    if(!bfs::exists(filePath))
    {
        LOG.write(_("File or directory does not exist: %s\n")) % filePath;
        return false;
    }
    steady_clock::time_point start = steady_clock::now();
    int ec;
    if(bfs::is_regular_file(filePath))
        ec = loadFile(to, filePath, palette);
    else if(!bfs::is_directory(filePath))
    {
        LOG.write(_("Could not determine type of path %s\n")) % filePath;
        return false;
    } else
        ec = loadDirectory(to, filePath, palette);
    if(ec)
    {
        LOG.write(_("failed: %1%\n")) % libsiedler2::getErrorString(ec);
        return false;
    }
    LOG.write(_("done in %1%\n")) % duration_cast<milliseconds>(steady_clock::now() - start);
    return true;
}

bool mergeArchives(libsiedler2::Archiv& targetArchive, libsiedler2::Archiv& otherArchive)
{
    if(targetArchive.size() < otherArchive.size())
        targetArchive.alloc_inc(otherArchive.size() - targetArchive.size());
    for(unsigned i = 0; i < otherArchive.size(); i++)
    {
        // Skip empty entries
        if(!otherArchive[i])
            continue;
        // If target entry is empty, just move the new one
        if(!targetArchive[i])
            targetArchive.set(i, otherArchive.release(i));
        else
        {
            libsiedler2::Archiv* subArchiv = dynamic_cast<libsiedler2::Archiv*>(targetArchive[i]);
            if(subArchiv)
            {
                // We have a sub-archiv -> Merge
                libsiedler2::Archiv* otherSubArchiv = dynamic_cast<libsiedler2::Archiv*>(otherArchive[i]);
                if(!otherSubArchiv)
                {
                    LOG.write(_("Failed to merge entry %1%. Archive expected!\n")) % i;
                    return false;
                }
                if(!mergeArchives(*subArchiv, *otherSubArchiv))
                    return false;
            } else
                targetArchive.set(i, otherArchive.release(i)); // Just replace
        }
    }
    return true;
}
} // namespace

std::shared_ptr<libsiedler2::Archiv> ArchiveLoader::load(const ResolvedFile& file) const
{
    std::shared_ptr<libsiedler2::Archiv> archive(new libsiedler2::Archiv);
    for(const bfs::path& curFilepath : file)
    {
        libsiedler2::Archiv newEntries;
        if(!loadFileOrDir(newEntries, curFilepath, palette_))
            return NULL;
        if(!mergeArchives(*archive, newEntries))
            return NULL;
    }
    return archive;
}
