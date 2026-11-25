cmake_minimum_required(VERSION 3.20)

project(Denoiser
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(PkgConfig REQUIRED)
find_package(fmt CONFIG REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(denoiser
    src/main.cpp
    src/FileScanner.cpp
    src/DirectoryNode.cpp
)

include(CTest)

include(FetchContent)
FetchContent_Declare(
	googletest
  	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

target_include_directories(denoiser PRIVATE src)

if(BUILD_TESTING)
	find_package(GTest REQUIRED)
	add_subdirectory("test")
endif()

target_link_libraries(denoiser PRIVATE ${EDIT_LIBRARIES} fmt::fmt)

target_compile_options(denoiser PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)


