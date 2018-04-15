FIND_PACKAGE(Boost 1.55.0 REQUIRED COMPONENTS filesystem iostreams system regex unit_test_framework)
if(NOT Boost_USE_STATIC_LIBS)
	ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
endif()
if(MSVC)
    # Use full paths for defines (makes Boost.Test with the VS addon work better)
    add_definitions(/FC)
endif()

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(.)

SET(SOURCES_SUBDIRS )
MACRO(AddDirectory dir)
	FILE(GLOB SUB_FILES ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp ${dir}/*.tpp)
	SET(SOURCES_SUBDIRS ${SOURCES_SUBDIRS} ${SUB_FILES})
	SOURCE_GROUP(${dir} FILES ${SUB_FILES})
ENDMACRO()

FILE(GLOB SOURCES_OTHER *.cpp *.h)
SOURCE_GROUP(other FILES ${SOURCES_OTHER})

AddDirectory(gameData)
AddDirectory(gameTypes)

add_library(gdCreator STATIC addNewData.h addNewData.cpp simpleLuaParser.h simpleLuaParser.cpp GameDataFile.h GameDataFile.cpp  ${SOURCES_SUBDIRS})
add_executable(gameDataCreator gameDataCreator.cpp)
add_executable(gameDataTest gameDataTest.cpp testGameDataCreate.cpp testSimpleLuaData.cpp)
target_link_libraries(gdCreator
	PUBLIC s25util
	PUBLIC mygettext
	PUBLIC s25Common
	PUBLIC rttrConfig
	PUBLIC gamedata
 ${Boost_LIBRARIES})

target_link_libraries(gameDataCreator gdCreator)
target_link_libraries(gameDataTest gdCreator)

add_test(NAME GameDataTest COMMAND gameDataTest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if(ClangFormat_FOUND)
    add_clangFormat_files(${SOURCES_OTHER} ${SOURCES_SUBDIRS})
endif()
