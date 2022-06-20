cmake_minimum_required(VERSION 3.5)

project(PresenterSender VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml REQUIRED)

set(PROJECT_FILES
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        control.cpp
        control.h

        logindialog.cpp
        logindialog.h
        logindialog.ui

        network.cpp
        network.h

        homewindow.cpp
        homewindow.h
        homewindow.ui

        Filemanager/filemanager.h
        Filemanager/filemanager.cpp
        Filemanager/filemanager.ui

        settingsdialog.h
        settingsdialog.cpp
        settingsdialog.ui

        Admin/usermanager.h
        Admin/usermanager.cpp
        Admin/usermanager.ui

        Admin/edituserdialog.h
        Admin/edituserdialog.cpp
        Admin/edituserdialog.ui

        Admin/resetpassworddialog.h
        Admin/resetpassworddialog.cpp
        Admin/resetpassworddialog.ui

        screenshareclient.h
        screenshareclient.cpp
        screenshareclient.ui

        ui/clickablelabel.h
        ui/loadinganimation.h
        ui/loadinganimation.cpp

        Filemanager/uploaddialog.h
        Filemanager/uploaddialog.cpp
        Filemanager/uploaddialog.ui

        Filemanager/fileitem.h
        Filemanager/fileitem.cpp

        Admin/checksysdialog.h
        Admin/checksysdialog.cpp
        Admin/checksysdialog.ui

        Admin/checkitem.h
)

set(PRESENTATION_SOURCES
        Presentation/paintwidget.cpp
        Presentation/paintwidget.h

        Presentation/presentation.cpp
        Presentation/presentation.h
        Presentation/presentation.ui

        Presentation/presentationhandler.cpp
        Presentation/presentationhandler.h

        Presentation/presentationscreen.cpp
        Presentation/presentationscreen.h
        Presentation/presentationscreen.ui

        Presentation/undoredo.cpp
        Presentation/undoredo.h

        Presentation/stroke.h
        Presentation/stroke.cpp
)

add_executable(PresenterSender
    ${PROJECT_FILES}
    ${PRESENTATION_SOURCES}
    main.cpp
)

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
 #   qt_add_executable(PresenterSender
  #      MANUAL_FINALIZATION
   #     ${PROJECT_SOURCES}
    #)

target_link_libraries(PresenterSender PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PresenterSender PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(PresenterSender PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

set_target_properties(PresenterSender PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

#if(QT_VERSION_MAJOR EQUAL 6)
 #   qt_finalize_executable(PresenterSender)
#endif()
