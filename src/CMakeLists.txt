add_library(libdns DirectoryNode.cpp FileScanner.cpp)
add_library(dns::libdns ALIAS libdns)
target_link_libraries(libdns PRIVATE fmt::fmt)

set_target_properties(
	libdns
	PROPERTIES OUTPUT_NAME dns
)

target_compile_features(libdns PUBLIC cxx_std_20)

target_compile_options(libdns PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(libdns
	PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

add_executable(denoiser main.cpp)
target_link_libraries(denoiser PRIVATE dns::libdns)
target_compile_options(denoiser PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)

find_path(EDITLINE_INCLUDE_DIR editline/readline.h)
find_library(EDITLINE_LIBRARY edit)
if(EDITLINE_INCLUDE_DIR
	AND EXISTS "${EDITLINE_INCLUDE_DIR}/editline/readline.h"
	AND EDITLINE_LIBRARY
	AND EXISTS "${EDITLINE_LIBRARY}")
	target_include_directories(denoiser PRIVATE ${EDITLINE_INCLUDE_DIR})
	target_link_libraries(denoiser PRIVATE ${EDITLINE_LIBRARY})
else()
	find_path(READLINE_INCLUDE_DIR readline/readline.h REQUIRED)
	find_library(READLINE_LIBRARY readline REQUIRED)
	target_include_directories(denoiser PRIVATE ${READLINE_INCLUDE_DIR})
	target_compile_definitions(denoiser PRIVATE DNS_USE_READLINE)
	target_link_libraries(denoiser PRIVATE ${READLINE_LIBRARY})
endif()

include(GNUInstallDirs)
install(TARGETS libdns
	EXPORT dns-targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	EXPORT dns-targets
	FILE dns-config.cmake
	NAMESPACE dns::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dns
)
