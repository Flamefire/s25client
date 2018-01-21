// Copyright (c) 2016 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#define BOOST_TEST_MODULE RTTR_Test

#include "commonDefines.h" // IWYU pragma: keep
#include "RttrConfig.h"
#include "libutil/LocaleHelper.h"
#include "libutil/Log.h"
#include "libutil/NullWriter.h"
#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

//#include <vld.h>

namespace bfs = boost::filesystem;

struct TestSetup
{
    TestSetup()
    {
        if(!LocaleHelper::init())
            throw std::runtime_error("Could not init locale");
        // Don't write to file
        LOG.setWriter(new NullWriter(), LogTarget::File);
        if(!RTTRCONFIG.Init())
            throw std::runtime_error("Could not init working directory. Misplaced binary?");
        if(!bfs::is_directory(RTTRCONFIG.ExpandPath("<RTTR_RTTR>")))
            throw std::runtime_error(RTTRCONFIG.ExpandPath("<RTTR_RTTR>") + " not found. Binary misplaced or RTTR folder not copied?");
        srand(static_cast<unsigned>(time(NULL)));
    }
    ~TestSetup() {}
};

#if BOOST_VERSION >= 105900
BOOST_GLOBAL_FIXTURE(TestSetup);
#else
// Boost < 1.59 got the semicolon inside the macro causing an "extra ;" warning
BOOST_GLOBAL_FIXTURE(TestSetup)
#endif
