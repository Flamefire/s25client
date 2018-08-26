// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchiveLocator.h"
#include "RttrConfig.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>

void ArchiveLocator::addStandardFolders()
{
    addFolder("<RTTR_GAME>/GFX/PALETTE");
    addFolder("<RTTR_GAME>/GFX/TEXTURES");
    addFolder("<RTTR_GAME>/GFX/PICS");
    addFolder("<RTTR_GAME>/GFX/PICS/MISSION");
    addFolder("<RTTR_GAME>/DATA");
    addFolder("<RTTR_GAME>/DATA/IO");
    addFolder("<RTTR_GAME>/DATA/CBOB");
    addFolder("<RTTR_GAME>/DATA/MBOB");
    addFolder("<RTTR_GAME>/DATA/BOBS");
    addFolder("<RTTR_GAME>/DATA/SOUNDDAT");
    addFolder("<RTTR_GAME>/DATA/BOBS");
    addFolder("<RTTR_GAME>/DATA/SOUNDDAT/SNG");
    addFolder("<RTTR_RTTR>");
    addFolder("<RTTR_RTTR>/LSTS");
    addFolder("<RTTR_USERDATA>/LSTS");
}

void ArchiveLocator::addFolder(std::string path, bool atBack)
{
    path = RTTRCONFIG.ExpandPath(path);
    if(!bfs::exists(path))
        throw std::runtime_error(std::string("Path ") + path + " does not exist");
    // Don't add folders twice although it is not an error
    for(const FolderData& cur : folders_)
    {
        if(bfs::equivalent(cur.path, path))
            return;
    }
    FolderData folder;
    folder.path = path;
    for(const auto& it: bfs::directory_iterator(path))
        folder.files.push_back(it.path().filename().string());
    // Sort so we can use binary_search later
    std::sort(folder.files.begin(), folder.files.end());

    if(atBack)
        folders_.push_back(folder);
    else
        folders_.insert(folders_.begin(), folder);
}

void ArchiveLocator::addAddonFolder(AddonId id)
{
    std::stringstream s;
    s << "<RTTR_RTTR>/LSTS/GAME/Addon_0x" << std::setw(8) << std::setfill('0') << std::hex << static_cast<unsigned>(id);
    std::string path = s.str();
    if(bfs::exists(path))
        addFolder(path);
}

void ArchiveLocator::clear()
{
    folders_.clear();
}

ResolvedFile ArchiveLocator::resolveFile(const std::string& possibleFilename)
{
    ResolvedFile result;
    std::string filename;
    // We should have a filename but old code might still pass full paths
    if(possibleFilename.find_first_of("\\/") != std::string::npos)
        filename = bfs::path(filename).filename().string();
    else
        filename = possibleFilename;
    for(const FolderData& folder : folders_)
    {
        if(std::binary_search(folder.files.begin(), folder.files.end(), filename))
        {
            const bfs::path filepath = bfs::path(folder.path) / filename;
            result.push_back(filepath);
        }
    }
    return result;
}
