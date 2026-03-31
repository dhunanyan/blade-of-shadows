cmake_minimum_required(VERSION 3.19)

project(game VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Multimedia MultimediaWidgets)
if (Qt6_FOUND)
    set(QT_PACKAGE_PREFIX "Qt6")
    set(QT_VERSION_MAJOR 6)
else()
    find_package(Qt5 QUIET COMPONENTS Widgets Multimedia MultimediaWidgets)
    if (Qt5_FOUND)
        set(QT_PACKAGE_PREFIX "Qt5")
        set(QT_VERSION_MAJOR 5)
    endif()
endif()

if (DEFINED QT_PACKAGE_PREFIX)
    set(PROJECT_SOURCES
        gameMain.cpp
        mainwindow.h
        mainwindow.cpp
        tilemapper.h
        tilemapper.cpp
        mainwindow.ui
        ${CMAKE_SOURCE_DIR}/assets/qt/resources.qrc
    )

    if (QT_VERSION_MAJOR GREATER_EQUAL 6)
        qt_add_executable(${PROJECT_NAME}
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
    elseif(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    else()
        add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE
        game_core
        ${QT_PACKAGE_PREFIX}::Widgets
        ${QT_PACKAGE_PREFIX}::Multimedia
        ${QT_PACKAGE_PREFIX}::MultimediaWidgets
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )

    install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    if (QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(${PROJECT_NAME})
    endif()
else()
    message(STATUS "Qt5/Qt6 not found: skipping 'game' target")
endif()
