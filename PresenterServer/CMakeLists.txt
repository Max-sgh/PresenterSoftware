cmake_minimum_required(VERSION 3.14)

project(PresenterServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)
find_package(Qt5 COMPONENTS Pdf REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)

find_package(Qt5Test REQUIRED)

set(PROJECT_FILES
    tcpserver.h
    tcpserver.cpp
    room.h
    room.cpp
    Database/CDB.cpp
    Database/CDB.h
    Database/CDBFileHandler.cpp
    Database/CDBFileHandler.h
    Database/CDBRow.cpp
    Database/CDBRow.h
    strokehandler.h
    strokehandler.cpp
    filehandler.h
    filehandler.cpp
    client.h
    client.cpp
    MobileServer/mobileserver.h
    MobileServer/mobileserver.cpp
    clienthandler.h clienthandler.cpp

    slidemanager.h slidemanager.cpp
    checksystem.h checksystem.cpp
    checkUserClass.h
    checkPresClass.h
    checkFileClass.h

    dbmanager.h
    dbmanager.cpp
)

add_executable(PresenterServer
  ${PROJECT_FILES}
  main.cpp
)

#add_executable(testTcpServer
 #   ${PROJECT_FILES}
 #   testTcpServer.cpp
#)
add_test(testTcpServer testTcpServer)

## Links for the real Application
target_link_libraries(PresenterServer Qt5::Core)
target_link_libraries(PresenterServer Qt5::Network)
target_link_libraries(PresenterServer Qt5::Multimedia)
target_link_libraries(PresenterServer Qt5::PrintSupport)
target_link_libraries(PresenterServer Qt5::Pdf)
target_link_libraries(PresenterServer Qt5::Sql)

## Links for the Tests
#target_link_libraries(testTcpServer Qt5::Core)
#target_link_libraries(testTcpServer Qt5::Network)
#target_link_libraries(testTcpServer Qt5::Multimedia)
#target_link_libraries(testTcpServer Qt5::Test)
#target_link_libraries(testTcpServer Qt5::PrintSupport)
#target_link_libraries(testTcpServer Qt5::Pdf)
#target_link_libraries(testTcpServer Qt5::Sql)
