cmake_minimum_required(VERSION 3.20)

project(Denoiser
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(denoiser
    src/main.cpp
    src/FileScanner.cpp
    src/DirectoryNode.cpp
)

target_include_directories(denoiser PRIVATE src)

target_link_libraries(denoiser PRIVATE ${EDIT_LIBRARIES})

target_compile_options(denoiser PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)


