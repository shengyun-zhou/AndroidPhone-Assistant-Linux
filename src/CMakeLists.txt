cmake_minimum_required (VERSION 2.6)

project (AndroidPhone-Assistant-Linux)

set(PROJECT_SOURCE_FILES
	adb-tools/ADBTools.h
	adb-tools/ADBTools.cpp
	tools/PipeTools.h
	tools/PipeTools.cpp
	tools/CommandTools.h
	tools/CommandTools.cpp
	)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Copying Android Tools...")
file(COPY ${PROJECT_SOURCE_DIR}/AndroidTools DESTINATION ${EXECUTABLE_OUTPUT_PATH})

message(STATUS "Try to find Glib modules...")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
link_libraries(${GLIB_LIBRARIES})

add_executable(AndroidPhone-Assistant ${PROJECT_SOURCE_FILES})

set(TERMINAL_TEST_SOURCE_FILES
	${PROJECT_SOURCE_FILES}
	TerminalTestMain.cpp
	)
add_executable(Terminal-Test ${TERMINAL_TEST_SOURCE_FILES})

