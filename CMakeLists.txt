cmake_minimum_required(VERSION 3.16)

project(server-core
        VERSION 0.0.1
        DESCRIPTION "Boost::Asio server static library"
        LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

#settings
include(ProjectSettings)
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

#warnings
include(CompilerWarnings)
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

#libs path
if(EXISTS ${CMAKE_MODULE_PATH}/LibsPath.cmake)
    include(LibsPath)
endif()

#boost settings
include(BoostSettings)

#versioning
configure_file(src/servercore_version.cpp.in src/servercore_version.cpp @ONLY)
add_library(servercore_version STATIC src/servercore_version.h src/servercore_version.cpp)
set_target_properties(servercore_version PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(servercore_version PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCE_FILES
        src/connection.cpp
        src/connectionmanager.cpp
        src/coreutility.cpp
        src/server.cpp
        src/worker.cpp
)
set(PRIVATE_HEADER_FILES
        src/connection.h
        src/connectionmanager.h
        src/coreutility.h
        src/macros.h
        src/worker.h
)
set(PUBLIC_HEADER_FILES
        include/servercore/server.h
)
add_library(servercore STATIC ${SOURCE_FILES} ${PRIVATE_HEADER_FILES} ${PUBLIC_HEADER_FILES})
add_library(server::core ALIAS servercore)
target_include_directories(servercore
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#precompiled headers
target_precompile_headers(servercore PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/pch.h)

# set MT / MTd static linking flags
set_property(TARGET servercore PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time)

target_link_libraries(servercore
        PRIVATE
        servercore_version
        project_options
        project_warnings
        fmt::fmt
        spdlog::spdlog
        Boost::boost
        Boost::date_time
)