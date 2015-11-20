cmake_minimum_required (VERSION 2.8.11)

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

set(PROJECT_UI_SOURCE_FILES 
	PC-Client/GUI/ContactBackupWindow.h
	PC-Client/GUI/ContactBackupWindow.cpp
	PC-Client/GUI/MainWindow.h
	PC-Client/GUI/MainWindow.cpp
	PC-Client/GUI/AppManagementWindow.h
	PC-Client/GUI/AppManagementWindow.cpp
	#Please put new Qt GUI source and header files here.
	)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ui_header)
include_directories(${CMAKE_BINARY_DIR}/ui_header)
add_custom_target(AndroidPhone-Assistant-UI ALL
				  COMMAND sh ${PROJECT_SOURCE_DIR}/PC-Client/GUI/CompileUIFiles.sh ${CMAKE_BINARY_DIR}/ui_header
				  )

message(STATUS "Copying Android Tools...")
file(COPY ${PROJECT_SOURCE_DIR}/AndroidTools DESTINATION ${EXECUTABLE_OUTPUT_PATH})

message(STATUS "Try to find GLib-2.0 modules...")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
link_libraries(${GLIB_LIBRARIES})

message(STATUS "Try to find Sqlite3 modules...")
pkg_check_modules(SQLITE3 sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIRS})
link_directories(${SQLITE3_LIBRARY_DIRS})
link_libraries(${SQLITE3_LIBRARIES})

message(STATUS "Try to find Qt5Widgets modules...")
find_package(Qt5Widgets REQUIRED)

set(PROJECT_ALL_SOURCE_FILES
	${PROJECT_SOURCE_FILES}
	${PROJECT_UI_SOURCE_FILES}
	PC-Client/Main.cpp
	)

set(CMAKE_AUTOMOC TRUE)
add_executable(AndroidPhone-Assistant ${PROJECT_ALL_SOURCE_FILES})
target_link_libraries(AndroidPhone-Assistant Qt5::Widgets)
add_dependencies(AndroidPhone-Assistant AndroidPhone-Assistant-UI)

set(GUI_TEST_SOURCE_FILES
	${PROJECT_SOURCE_FILES}
	${PROJECT_UI_SOURCE_FILES}
	PC-Client/GUITestMain.cpp
	)
add_executable(GUI-Test ${GUI_TEST_SOURCE_FILES})
target_link_libraries(GUI-Test Qt5::Widgets)
add_dependencies(GUI-Test AndroidPhone-Assistant-UI)

set(TERMINAL_TEST_SOURCE_FILES
	${PROJECT_SOURCE_FILES}
	PC-Client/TerminalTestMain.cpp
	)
add_executable(Terminal-Test ${TERMINAL_TEST_SOURCE_FILES})
