cmake_minimum_required(VERSION 3.0)

project(GamePlay-deps)

# set cmake default output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
if(WINDOWS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endif()

# set output directories

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
    SET(LINUX 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT ANDROID)
if(NOT ANDROID AND NOT IOS)
    include(${PROJECT_SOURCE_DIR}/cmake/TargetArch.cmake)
    set(ARCH "unknown")
    target_architecture(ARCH)
endif(NOT ANDROID AND NOT IOS)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/out/external-deps")
set(OUT_DIR_INCLUDE "${PROJECT_SOURCE_DIR}/out/external-deps/include")
if(NOT ANDROID AND NOT IOS)
    IF(APPLE)
        set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/out/external-deps/lib/macos/${ARCH}")
    ELSE()
        string(TOLOWER ${CMAKE_SYSTEM_NAME} LOWER_SYSTEM_NAME)
        set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/out/external-deps/lib/${LOWER_SYSTEM_NAME}/${ARCH}")
    ENDIF(APPLE)
endif(NOT ANDROID AND NOT IOS)
IF(NOT WIN32)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT WIN32)
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})
message(STATUS "Directory: ${LIBRARY_OUTPUT_PATH}")


# bgfx options
set( BGFX_BUILD_TOOLS OFF CACHE BOOL "Build bgfx tools." FORCE )
set( BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build bgfx examples." FORCE )
set( BGFX_INSTALL OFF CACHE BOOL "Create installation target." FORCE )
set( BGFX_INSTALL_EXAMPLES OFF CACHE BOOL "Install examples and their runtimes." FORCE )
set( BGFX_CUSTOM_TARGETS OFF CACHE BOOL "Include convenience custom targets." FORCE )
set( BGFX_USE_OVR OFF CACHE BOOL "Build with OVR support." FORCE )
set( BGFX_AMALGAMATED OFF CACHE BOOL "Amalgamated bgfx build for faster compilation" FORCE )
set( BX_AMALGAMATED OFF CACHE BOOL "Amalgamated bx build for faster compilation" FORCE )
set( BGFX_CONFIG_DEBUG ON CACHE BOOL "Enables debug configuration on all builds" FORCE )
set( BGFX_USE_DEBUG_SUFFIX OFF CACHE BOOL "Add 'd' suffix to debug output targets" FORCE )


# add subdirectory

if(LINUX)
    # openal as static
    set(LIBTYPE STATIC)
endif()

if(LINUX OR WINDOWS)    
    set( SDL_SHARED_ENABLED_BY_DEFAULT OFF )    # SDL2
    add_subdirectory(sdl-2.0.8)
    add_subdirectory(openal-1.18.2)
    # add_subdirectory(freetype-2.7.1)
endif()

add_subdirectory(bgfx-1.0.0)
add_subdirectory(json-7.6.1)
# add_subdirectory(tinyxml2-4.0.1)
add_subdirectory(lua-5.3.4)
add_subdirectory(zlib-1.2.8)
# add_subdirectory(png-1.6.15)
add_subdirectory(ogg-1.3.2)
add_subdirectory(vorbis-1.3.5)
add_subdirectory(spark-2.0)

set(BUILD_TARGETS
    json
    vorbisfile
    vorbisenc
    vorbis
    ogg
    # png
    zlib
    spark
    bgfx
    bimg
    bx
    brtshaderc
    # lua
    # tinyxml2
)

# Emscripten ships with its own implementation of the OpenAL, sdl2, freetype
if(NOT EMSCRIPTEN)
    list(APPEND BUILD_TARGETS SDL2-static)
    list(APPEND BUILD_TARGETS SDL2main)
    # list(APPEND BUILD_TARGETS freetype)
    list(APPEND BUILD_TARGETS OpenAL)
    # list(APPEND BUILD_TARGETS png_static)
endif()



# amalgamate libraries

if(LINUX)
    set(GAMEPLAY_DEPS libgameplay-deps.a)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh
        ${CMAKE_AR}
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${LIBRARY_OUTPUT_PATH}
    )
elseif(EMSCRIPTEN)
    set(GAMEPLAY_DEPS libgameplay-deps.a)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gnu-amalgamate.sh
        $(EMSCRIPTEN)/emar
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${LIBRARY_OUTPUT_PATH}
    )
elseif(WINDOWS)
    set(GAMEPLAY_DEPS gameplay-deps.lib)
    set(TARGET_CMD ${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc-amalgamate.bat
        ${CMAKE_AR}
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${LIBRARY_OUTPUT_PATH}
    )
endif()

add_custom_command(
    OUTPUT ${GAMEPLAY_DEPS}
    COMMAND ${TARGET_CMD}
    #DEPENDS ${BUILD_TARGETS}
    COMMENT "Amalgamate external dependencies..."
)

add_custom_target(gameplay-deps ALL DEPENDS ${GAMEPLAY_DEPS})
add_dependencies(gameplay-deps ${BUILD_TARGETS})

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${LIBRARY_OUTPUT_PATH}/${GAMEPLAY_DEPS})





# helper to copy include files to output directory
FUNCTION(COPY_HEADERS src dst)
    file(COPY ${src}
        DESTINATION ${dst} FILES_MATCHING REGEX "^.*.(h|hpp|inl)$"
        PATTERN ".svn" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE)
ENDFUNCTION(COPY_HEADERS)


# copy thirdparty headers to output directory

COPY_HEADERS(json-7.6.1/ ${OUT_DIR_INCLUDE}/libjson/)
COPY_HEADERS(ogg-1.3.2/include/ogg/ ${OUT_DIR_INCLUDE}/ogg/)
COPY_HEADERS(openal-1.17.2/include/AL/ ${OUT_DIR_INCLUDE}/AL/)
COPY_HEADERS(sdl-2.0.8/include/ ${OUT_DIR_INCLUDE}/SDL2/)
COPY_HEADERS(spark-2.0/spark/include/ ${OUT_DIR_INCLUDE}/spark/)
COPY_HEADERS(vorbis-1.3.5/include/vorbis/ ${OUT_DIR_INCLUDE}/vorbis/)
COPY_HEADERS(zlib-1.2.8/zlib.h ${OUT_DIR_INCLUDE}/zlib/)
COPY_HEADERS(png-1.6.15/ ${OUT_DIR_INCLUDE}/png/)
# COPY_HEADERS(freetype-2.7.1/include/freetype/ ${OUT_DIR_INCLUDE}/freetype/)
# COPY_HEADERS(freetype-2.7.1/include/ft2build.h ${OUT_DIR_INCLUDE}/)

# copy thirdparty bgfx stuff to output directory

COPY_HEADERS(bgfx-1.0.0/bgfx/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-1.0.0/bimg/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-1.0.0/bx/include/ ${OUT_DIR_INCLUDE}/)
COPY_HEADERS(bgfx-1.0.0/brtshaderc/brtshaderc.h ${OUT_DIR_INCLUDE}/brtshaderc/)
COPY_HEADERS(bgfx-1.0.0/bgfx/3rdparty/dear-imgui/ ${OUT_DIR_INCLUDE}/dear-imgui/)
COPY_HEADERS(bgfx-1.0.0/bgfx/3rdparty/iconfontheaders/ ${OUT_DIR_INCLUDE}/iconfontheaders/)
COPY_HEADERS(bgfx-1.0.0/bgfxcommon/ ${OUT_DIR_INCLUDE}/bgfxcommon/)
COPY_HEADERS(bgfx-1.0.0/bgfx/3rdparty/stb/ ${OUT_DIR_INCLUDE}/stb/)

if(WINDOWS)
    COPY_HEADERS(bgfx-1.0.0/bx/include/compat/ ${OUT_DIR_INCLUDE}/bx/compat/)
endif()