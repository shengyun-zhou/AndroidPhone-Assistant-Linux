cmake_minimum_required(VERSION 2.8.11)

project(AndroidPhone-Assistant-Linux)

set(PROJECT_SOURCE_FILES
        adb-tools/ADBTools.h
        adb-tools/ADBTools.cpp
        tools/PipeTools.h
        tools/PipeTools.cpp
        tools/CommandTools.h
        tools/CommandTools.cpp
        tools/SocketTools.h
        tools/SocketTools.cpp
        tools/SMSAndContactBackup.h
        tools/SMSAndContactBackup.cpp
        info-provider/PhoneInfo.h
        info-provider/ContactInfo.h
        info-provider/SMSInfo.h
        info-provider/AppInfo.h
        #Please put new source and header files here.
        )

set(PROJECT_UI_SOURCE_FILES
        GUI/SMSBackupWindow.h
        GUI/SMSBackupWindow.cpp
        tools/GTKTools.h
        tools/GTKTools.cpp
        GUI/ContactBackupWindow.h
        GUI/ContactBackupWindow.cpp
        GUI/MainWindow.h
        GUI/MainWindow.cpp
        GUI/AppManagementWindow.h
        GUI/AppManagementWindow.cpp
        #Please put new Qt GUI source and header files here.
        )

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ui_header)
include_directories(${CMAKE_BINARY_DIR}/ui_header)
add_custom_target(AndroidPhone-Assistant-UI ALL
        COMMAND sh ${PROJECT_SOURCE_DIR}/GUI/CompileUIFiles.sh ${CMAKE_BINARY_DIR}/ui_header
        COMMAND sh ${PROJECT_SOURCE_DIR}/GUI/res/CompileQRCFiles.sh ${CMAKE_BINARY_DIR}/ui_header
        )

message(STATUS "Copying Android Tools...")
file(COPY ${PROJECT_SOURCE_DIR}/../AndroidTools DESTINATION ${EXECUTABLE_OUTPUT_PATH})

message(STATUS "Try to find GTK+-2.0 modules...")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 gtk+-2.0 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
link_libraries(${GTK2_LIBRARIES})
add_definitions(${GTK2_CFLAGS_OTHER})

message(STATUS "Try to find libgksu2 modules...")
pkg_check_modules(LIBGKSU libgksu2 REQUIRED)
include_directories(${LIBGKSU_INCLUDE_DIRS})
link_directories(${LIBGKSU_LIBRARY_DIRS})
link_libraries(${LIBGKSU_LIBRARIES})

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
        Main.cpp
        )

set(CMAKE_AUTOMOC TRUE)
add_executable(AndroidPhone-Assistant ${PROJECT_ALL_SOURCE_FILES})
target_link_libraries(AndroidPhone-Assistant Qt5::Widgets)
add_dependencies(AndroidPhone-Assistant AndroidPhone-Assistant-UI)

set(GUI_TEST_SOURCE_FILES
        ${PROJECT_SOURCE_FILES}
        ${PROJECT_UI_SOURCE_FILES}
        GUITestMain.cpp
        )
add_executable(GUI-Test ${GUI_TEST_SOURCE_FILES})
target_link_libraries(GUI-Test Qt5::Widgets)
add_dependencies(GUI-Test AndroidPhone-Assistant-UI)

set(TERMINAL_TEST_SOURCE_FILES
        ${PROJECT_SOURCE_FILES}
        TerminalTestMain.cpp
        )
add_executable(Terminal-Test ${TERMINAL_TEST_SOURCE_FILES})
