

FIND_PACKAGE(BZip2 REQUIRED)
gather_dll(BZIP2)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Gettext REQUIRED)
FIND_PACKAGE(Boost 1.55.0 REQUIRED COMPONENTS filesystem iostreams system program_options locale chrono)

################################################################################

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/driver/include)
INCLUDE_DIRECTORIES(SYSTEM
	${BZIP2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	)

################################################################################

FILE(GLOB RTTR_PO_FILES ../RTTR/languages/*.po)

GETTEXT_CREATE_TRANSLATIONS(../RTTR/languages/rttr.pot ALL
							${RTTR_PO_FILES}
						   )

################################################################################

if(APPLE)
	CORRECT_LIB(OPENGL_gl_LIBRARY OpenGL)
	CORRECT_LIB(SDL_LIBRARY SDL)

	# Add the SDL-include flags to an apple build
	INCLUDE_DIRECTORIES(SYSTEM ${SDL_INCLUDE_DIR})

	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/macos)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")
ENDif()

################################################################################
# OpenGL
################################################################################

find_path(OGLEXT_INCLUDE_DIR NAMES GL/glext.h OpenGL/glext.h)
if(NOT OGLEXT_INCLUDE_DIR)
    set(OGLEXT_INCLUDE_DIR ${RTTR_CONTRIB_DIR}/openGL/include CACHE PATH "" FORCE)
elseif(WIN32)
    # On windows the ARB function signatures might not be defined due to an old glExt version
    # So if they are not we use our new ones
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES ${OGLEXT_INCLUDE_DIR})
    check_cxx_source_compiles("
        #include <windows.h>
        #include <GL/gl.h>
        #include <GL/glext.h>
        PFNGLGENBUFFERSARBPROC test;
        int main(){}" OGL_ARB_FUNCS_EXIST)
    set(CMAKE_REQUIRED_INCLUDES )
    if(NOT OGL_ARB_FUNCS_EXIST)
        set(OGLEXT_INCLUDE_DIR ${RTTR_CONTRIB_DIR}/openGL/include CACHE PATH "" FORCE)
    endif()
endif()
include_directories(BEFORE SYSTEM ${OGLEXT_INCLUDE_DIR})

################################################################################

add_subdirectory(libGamedata)
include(s25Main.cmake)
add_subdirectory(s25client)
add_subdirectory(test)

# Copy the RTTR folder with the updated translations to the directory of the client.
configure_file(${CMAKE_SOURCE_DIR}/copyDepsToBuildDir.cmake . @ONLY)
ADD_CUSTOM_COMMAND(TARGET translations POST_BUILD COMMAND ${CMAKE_COMMAND} -D CMAKE_BUILD_TYPE=$<CONFIGURATION> -P ${CMAKE_CURRENT_BINARY_DIR}/copyDepsToBuildDir.cmake)
