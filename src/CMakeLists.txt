cmake_minimum_required (VERSION 2.6)

project (AndroidPhone-Assistant-Linux)

set(PROJECT_SOURCE_FILES
	PC-Client/adb-tools/ADBTools.h
	PC-Client/adb-tools/ADBTools.cpp
	PC-Client/tools/PipeTools.h
	PC-Client/tools/PipeTools.cpp
	PC-Client/tools/CommandTools.h
	PC-Client/tools/CommandTools.cpp
	PC-Client/tools/SocketTools.h
	PC-Client/tools/SocketTools.cpp
	PC-Client/info-provider/ContactInfo.h
	PC-Client/info-provider/SMSInfo.h
	PC-Client/info-provider/AppInfo.h
	#Please put new source and header files here.
	)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Copying Android Tools...")
file(COPY ${PROJECT_SOURCE_DIR}/AndroidTools DESTINATION ${EXECUTABLE_OUTPUT_PATH})

message(STATUS "Try to find GTK+3 modules...")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 gtk+-3.0 REQUIRED)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
link_libraries(${GTK3_LIBRARIES})

message(STATUS "Try to find Sqlite3 modules...")
pkg_check_modules(SQLITE3 sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIRS})
link_directories(${SQLITE3_LIBRARY_DIRS})
link_libraries(${SQLITE3_LIBRARIES})

add_executable(AndroidPhone-Assistant ${PROJECT_SOURCE_FILES})

set(TERMINAL_TEST_SOURCE_FILES
	${PROJECT_SOURCE_FILES}
	PC-Client/TerminalTestMain.cpp
	)
add_executable(Terminal-Test ${TERMINAL_TEST_SOURCE_FILES})


