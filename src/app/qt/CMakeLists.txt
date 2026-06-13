cmake_minimum_required(VERSION 3.19)

project(game VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Multimedia)
if (Qt6_FOUND)
    set(QT_PACKAGE_PREFIX "Qt6")
    set(QT_VERSION_MAJOR 6)
else()
    find_package(Qt5 QUIET COMPONENTS Widgets Multimedia)
    if (Qt5_FOUND)
        set(QT_PACKAGE_PREFIX "Qt5")
        set(QT_VERSION_MAJOR 5)
    endif()
endif()

if (DEFINED QT_PACKAGE_PREFIX)
    set(PROJECT_SOURCES
        gameMain.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/mainwindow.h
        mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/game_session.h
        game_session.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/editor_palette.h
        editor_palette.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/music_player.h
        music_player.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/tilemapper.h
        tilemapper.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/save_game_repository.h
        save_game_repository.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/qt/settings_repository.h
        settings_repository.cpp
        ${CMAKE_SOURCE_DIR}/include/game/app/game_settings.h
        ${CMAKE_SOURCE_DIR}/include/game/app/level_selection.h
        ${CMAKE_SOURCE_DIR}/include/game/app/game_controller.h
        ${CMAKE_SOURCE_DIR}/include/game/app/input_mapper.h
        ${CMAKE_SOURCE_DIR}/include/game/app/input_state.h
        ${CMAKE_SOURCE_DIR}/include/game/app/menu_system.h
        ${CMAKE_SOURCE_DIR}/src/app/game_controller.cpp
        ${CMAKE_SOURCE_DIR}/src/app/input_mapper.cpp
        ${CMAKE_SOURCE_DIR}/src/app/menu_system.cpp
        ${CMAKE_SOURCE_DIR}/include/game/renderer/player_animation_state.h
        ${CMAKE_SOURCE_DIR}/include/game/renderer/asset_repository.h
        ${CMAKE_SOURCE_DIR}/include/game/renderer/animation_controller.h
        ${CMAKE_SOURCE_DIR}/include/game/renderer/player_presentation.h
        ${CMAKE_SOURCE_DIR}/include/game/renderer/enemy_presentation.h
        ${CMAKE_SOURCE_DIR}/include/game/renderer/scene_renderer.h
        ${CMAKE_SOURCE_DIR}/src/renderer/asset_repository.cpp
        ${CMAKE_SOURCE_DIR}/src/renderer/animation_controller.cpp
        ${CMAKE_SOURCE_DIR}/src/renderer/player_presentation.cpp
        ${CMAKE_SOURCE_DIR}/src/renderer/enemy_presentation.cpp
        ${CMAKE_SOURCE_DIR}/src/renderer/scene_renderer.cpp
        mainwindow.ui
        ${CMAKE_SOURCE_DIR}/assets/qt/resources.qrc
        ${CMAKE_SOURCE_DIR}/assets/app/BladeOfShadows.icns
    )

    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/assets/app/BladeOfShadows.icns
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
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
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "BladeOfShadows"
        MACOSX_BUNDLE_GUI_IDENTIFIER com.dhunanyan.bladeofshadows
        MACOSX_BUNDLE_BUNDLE_NAME "Blade of Shadows"
        MACOSX_BUNDLE_ICON_FILE "BladeOfShadows.icns"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )

    install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    if (QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(${PROJECT_NAME})
    endif()
else()
    message(STATUS "Qt5/Qt6 not found: skipping 'game' target")
endif()
