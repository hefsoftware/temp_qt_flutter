# https://github.com/flutter/engine/blob/main/docs/Custom-Flutter-Engine-Embedders.md
cmake_minimum_required(VERSION 3.16)

project(flutter_embedding VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL)

#set(FLUTTER_ENGINE_SHA 949fb0fb4cbe4bf4753d68264b9d9cc966ba7058)
set(FLUTTER_ENGINE_SHA a18df97ca57a249df5d8d68cd0820600223ce262)


#set(FLUTTER_EMBEDDER_ARTIFACTS_ZIP ${CMAKE_BINARY_DIR}/flutter_embedder_${FLUTTER_ENGINE_SHA}.zip)
#set(FLUTTER_ARTIFACTS_ZIP          ${CMAKE_BINARY_DIR}/flutter_artifact_${FLUTTER_ENGINE_SHA}.zip)
set(FLUTTER_EMBEDDER_ARTIFACTS_ZIP ${CMAKE_CURRENT_LIST_DIR}/download/flutter_embedder_${FLUTTER_ENGINE_SHA}.zip)
set(FLUTTER_ARTIFACTS_ZIP          ${CMAKE_CURRENT_LIST_DIR}/download/flutter_artifact_${FLUTTER_ENGINE_SHA}.zip)
#set(FLUTTER_BUCKET_BASE "https://storage.googleapis.com/flutter_infra/flutter")
set(FLUTTER_BUCKET_BASE "https://storage.googleapis.com/flutter_infra_release/flutter")

# Download and setup the Flutter Engine.
if(NOT EXISTS ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP})
  file(DOWNLOAD
    ${FLUTTER_BUCKET_BASE}/${FLUTTER_ENGINE_SHA}/windows-x64/windows-x64-embedder.zip
    ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP}
    SHOW_PROGRESS
  )
#https://storage.googleapis.com/flutter_infra_release/flutter/FLUTTER_ENGINE/windows-x64/windows-x64-embedder.zip
  #execute_process(
  #  COMMAND ${CMAKE_COMMAND} -E tar xzf ${FLUTTER_EMBEDDER_ARTIFACTS_ZIP}
  #  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  #)
endif()

if(NOT EXISTS ${FLUTTER_ARTIFACTS_ZIP})
  file(DOWNLOAD
    ${FLUTTER_BUCKET_BASE}/${FLUTTER_ENGINE_SHA}/windows-x64/artifacts.zip
    ${FLUTTER_ARTIFACTS_ZIP}
    SHOW_PROGRESS
  )
  #execute_process(
  #  COMMAND ${CMAKE_COMMAND} -E tar xzf ${FLUTTER_ARTIFACTS_ZIP}
  #  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  #)
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(flutter_embedding
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        flutterapp.h flutterapp.cpp
        flutter_engine/flutter_embedder.h
        flutter_engine/embedder.h
        flutterappsoftware.h flutterappsoftware.cpp
        flutterwidgetsoftware.h flutterwidgetsoftware.cpp
        flutterappopengl.h flutterappopengl.cpp
        flutterwidgetopengl.h flutterwidgetopengl.cpp
        FlutterAppOpenGl2.h
        FlutterAppOpenGl2.cpp
        README

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET flutter_embedding APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(flutter_embedding SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(flutter_embedding
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(flutter_embedding PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_directories(flutter_embedding PRIVATE flutter_engine)
target_link_libraries(flutter_embedding PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGL flutter_engine.dll)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.flutter_embedding)
endif()
set_target_properties(flutter_embedding PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS flutter_embedding
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(flutter_embedding)
endif()
