cmake_minimum_required(VERSION 3.20)

project(Denoiser
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(fmt CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

add_subdirectory("src")

if(BUILD_TESTING)
	FetchContent_MakeAvailable(googletest)
	add_subdirectory("test")
endif()
