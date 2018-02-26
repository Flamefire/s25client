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

#include "commonDefines.h" // IWYU pragma: keep
#include "RttrConfig.h"
#include "addNewData.h"
#include "libutil/LocaleHelper.h"
#include "libutil/Log.h"
#include "libutil/NullWriter.h"
#include <boost/filesystem.hpp>

void copyDir(const bfs::path& source, const bfs::path& destination)
{
    if(!bfs::is_directory(source))
        throw std::runtime_error("Source does not exist");
    bfs::create_directory(destination);
    // Iterate through the source directory
    for(bfs::directory_iterator file(source); file != bfs::directory_iterator(); ++file)
    {
        const bfs::path& current = file->path();
        if(bfs::is_directory(current))
            copyDir(current, destination / current.filename());
        else
            bfs::copy_file(current, destination / current.filename());
    }
}

int main(int argc, char** argv)
{
    if(!LocaleHelper::init())
        return 1;
    if(!RTTRCONFIG.Init())
        return 1;
    LOG.setWriter(new NullWriter, LogTarget::File);

    std::string newPath = "newGameData/nations";
    if(bfs::exists(newPath))
        bfs::remove_all(newPath);
    copyDir(RTTRCONFIG.ExpandPath("<RTTR_RTTR>/gamedata/nations"), newPath);
    addNewData(newPath);
}
