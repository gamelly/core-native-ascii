cmake_minimum_required(VERSION 3.18)
include(FetchContent)

# Host CPU
if(NOT CMAKE_HOST_SYSTEM_PROCESSOR)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_HOST_SYSTEM_PROCESSOR "${ARCH}")
endif()
if(NOT CMAKE_HOST_SYSTEM_PROCESSOR)
    message(WARNING "host system processor not detected.")
    set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")
endif()
set(CMAKE_HOST_SYSTEM_NAME_LOWER )

# Zig
set(ZIG_VERSION "0.13.0")
set(ZIG_DOWNLOAD "http://ziglang.org/download")
set(ZIG_PATH "${CMAKE_SOURCE_DIR}/vendor/zig/zig")
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-macos-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.tar.xz")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.tar.xz")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(ZIG_DOWNLOAD "${ZIG_DOWNLOAD}/${ZIG_VERSION}/zig-windows-${CMAKE_HOST_SYSTEM_PROCESSOR}-${ZIG_VERSION}.zip")
endif()
if(NOT EXISTS ${ZIG_PATH})
    FetchContent_Populate(
        compiler_zig
        URL "${ZIG_DOWNLOAD}"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/zig"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/zig-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/zig-build"
    )
endif()
find_program(ZIGC zig PATHS "${CMAKE_SOURCE_DIR}/vendor/zig" REQUIRED NO_DEFAULT_PATH)

# Compiler
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain.cmake")

# Project
project(game C)
include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/vendor")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

# Font
set(FONT_DOWNLOAD "https://github.com/gamelly/gly-type/archive/refs/heads/main.zip")
if(NOT EXISTS "${LUA_PATH}/font")
    FetchContent_Populate(
        lib_font
        URL "${FONT_DOWNLOAD}"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/font"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/font-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/font-build"
    )
endif()

# Color
set(COLOR_DOWNLOAD "https://github.com/rodrigodornelles/rgb-to-xterm256/archive/refs/heads/main.zip")
if(NOT EXISTS "${LUA_PATH}/color")
    FetchContent_Populate(
        lib_color
        URL "${COLOR_DOWNLOAD}"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/color"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/rgb_2_xterm256-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/rgb_2_xterm256-build"
    )
endif()

# Lua
set(LUA_VERSION "5.4.7")
set(LUA_DOWNLOAD "https://github.com/lua/lua/archive/refs/tags/v${LUA_VERSION}.tar.gz")
set(LUA_PATH "${CMAKE_SOURCE_DIR}/vendor/lua")
set(LUA_OPTMIZE FALSE)
if(NOT EXISTS "${LUA_PATH}/lua")
    set(LUA_OPTMIZE TRUE)
    string(TOLOWER ${CMAKE_HOST_SYSTEM_NAME} CMAKE_HOST_SYSTEM_NAME_LOWER)
    FetchContent_Populate(
        compiler_lua
        URL "${LUA_DOWNLOAD}"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/lua"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/lua-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/lua-build"
    )
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/lua"
        COMMAND ${ZIGC} cc -DMAKE_LUA onelua.c -o lua
            -target ${CMAKE_HOST_SYSTEM_PROCESSOR}-${CMAKE_HOST_SYSTEM_NAME_LOWER}
    )
endif()
file(GLOB lua_files "${CMAKE_SOURCE_DIR}/vendor/lua/*.c")
list(REMOVE_ITEM lua_files "${CMAKE_SOURCE_DIR}/vendor/lua/lua.c")
list(REMOVE_ITEM lua_files "${CMAKE_SOURCE_DIR}/vendor/lua/onelua.c")
add_library(lua-static STATIC "${lua_files}")
find_program(LUAC lua PATHS "${CMAKE_SOURCE_DIR}/vendor/lua" REQUIRED NO_DEFAULT_PATH)

# Engine
set(ENGINE_HEADER "${CMAKE_SOURCE_DIR}/vendor/engine/bytecode.h")
if(NOT EXISTS ${ENGINE_HEADER})
    FetchContent_Declare(
        lib_engine
        GIT_REPOSITORY https://github.com/gamelly/gly-engine
        GIT_TAG develop
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/engine"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/engine-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/engine-build"
    )
    FetchContent_MakeAvailable(lib_engine)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine/dist")
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine"
        COMMAND ${LUAC} src/cli/main.lua build --core native --bundler
    )
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine"
        COMMAND ${LUAC} src/cli/main.lua fs-xxd-i dist/main.lua bytecode.h --const --name engine_bytecode_lua
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# Game
string(LENGTH "${GAME}" GAME_LENGTH)
set(GAME_HEADER "${CMAKE_SOURCE_DIR}/vendor/game/bytecode.h")
if ((NOT DEFINED GAME AND NOT EXISTS ${GAME_HEADER}) OR (DEFINED GAME AND NOT GAME_LENGTH))
    file(WRITE ${GAME_HEADER} "const unsigned char* game_bytecode_lua = NULL;\nconst unsigned int game_bytecode_lua_len = 0;")
endif()
if(DEFINED GAME AND GAME_LENGTH)
    if(NOT EXISTS ${GAME})
        message(FATAL_ERROR "game not found: ${GAME}")
    endif()
    get_filename_component(GAME "${GAME}" ABSOLUTE)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/game")
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine"
        COMMAND ${LUAC} src/cli/main.lua fs-xxd-i ${GAME} ${GAME_HEADER} --const --name game_bytecode_lua
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# Lua CJson
set(LUACJSON_FILE "${CMAKE_SOURCE_DIR}/vendor/lua_cjson/lua_cjson.c")
if(NOT EXISTS "${LUACJSON_FILE}")
    FetchContent_Declare(
        lib_lua_cjson
        GIT_REPOSITORY https://github.com/openresty/lua-cjson
        GIT_TAG 2.1.0.9
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/lua_cjson"
        SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/lua_cjson-subbuild"
        BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/lua_cjson-build"
    )
    FetchContent_Populate(lib_lua_cjson)
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine"
        COMMAND ${LUAC} src/cli/main.lua fs-replace ${LUACJSON_FILE} ${LUACJSON_FILE}
            --format "<lua.h>" --replace "\"lua/lua.h\""
    )
    execute_process(
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/engine"
        COMMAND ${LUAC} src/cli/main.lua fs-replace ${LUACJSON_FILE} ${LUACJSON_FILE}
            --format "<lauxlib.h>" --replace "\"lua/lauxlib.h\""
    )
endif()
add_library(lua_cjson-static STATIC
    "${CMAKE_SOURCE_DIR}/vendor/lua_cjson/lua_cjson.c"
    "${CMAKE_SOURCE_DIR}/vendor/lua_cjson/strbuf.c"
    "${CMAKE_SOURCE_DIR}/vendor/lua_cjson/fpconv.c"
)

# Bear SSL
set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
set(BEARSSL_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/vendor/bearssl/inc")
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/bearssl/build;${CMAKE_PREFIX_PATH}")
FetchContent_Declare(
    lib_bearssl
    GIT_REPOSITORY https://www.bearssl.org/git/BearSSL
    GIT_TAG v0.6
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/bearssl"
    SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/bearssl-subbuild"
    BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/bearssl-build"
)
FetchContent_MakeAvailable(lib_bearssl)
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/vendor/bearssl/build/libbearssl.a")
    execute_process(
        COMMAND make build/libbearssl.a
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/vendor/bearssl"
    )
endif()

# Curl
set(HTTP_ONLY ON CACHE BOOL "" FORCE)
set(PICKY_COMPILER OFF CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_STATIC_CURL OFF CACHE BOOL "" FORCE)
set(ENABLE_ARES OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_BEARSSL ON CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    lib_curl
    GIT_REPOSITORY https://github.com/curl/curl
    GIT_TAG curl-8_9_1
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/curl"
    SUBBUILD_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/curl-subbuild"
    BINARY_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/_deps/curl-build"
)
FetchContent_MakeAvailable(lib_curl)

target_link_libraries(${PROJECT_NAME} CURL::libcurl)
target_link_libraries(${PROJECT_NAME} lua-static)
target_link_libraries(${PROJECT_NAME} lua_cjson-static)
target_link_libraries(${PROJECT_NAME} m)
