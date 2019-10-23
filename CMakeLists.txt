cmake_minimum_required(VERSION 3.14)
project(CooperativeEditor)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_POSITION_INDEPENDENT_CODE True)

if (CMAKE_VERSION VERSION_LESS 3.0.0)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag(-std=c++0x COMPILER_SUPPORTS_CXX0X)
    if (COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif (COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif ()
else ()
    SET(CMAKE_CXX_STANDARD 17)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

include_directories(src)
include_directories(src/include/lib/)

find_package(Threads)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

#Set the client executable
add_executable(CooperativeClient src/mainclient.cpp src/client/controller/Controller.cpp src/client/model/Model.cpp
        src/components/Symbol.cpp src/components/Identifier.cpp src/components/messages/BasicMessage.cpp
        src/components/messages/FileMessage.cpp src/components/messages/RequestMessage.cpp
        src/components/messages/CrdtMessage.cpp src/components/messages/FileListingMessage.cpp src/components/messages/UserMessage.cpp
        src/common/CrdtAlgorithm.cpp src/client/view/View.cpp src/client/view/windows/Login.cpp
        src/client/view/windows/FileVisualizer.cpp src/client/view/windows/Editor.cpp
        src/common/TcpSocket.cpp src/client/view/windows/EditUserProfile.cpp src/client/view/windows/SignUp.cpp
        src/common/User.cpp src/common/File.cpp)

#Set the server executable
add_executable(CooperativeServer src/mainserver.cpp src/server/controller/Controller.cpp src/server/model/Model.cpp
        src/components/Symbol.cpp src/components/Identifier.cpp src/components/messages/BasicMessage.cpp
        src/components/messages/FileMessage.cpp src/components/messages/RequestMessage.cpp src/components/messages/UserMessage.cpp
        src/components/messages/CrdtMessage.cpp src/components/messages/FileListingMessage.cpp src/common/CrdtAlgorithm.cpp
        src/common/TcpSocket.cpp src/common/User.cpp src/common/TcpSocket.cpp src/common/File.cpp src/server/model/ServerFile.cpp)

#Add libraries to compile the client
if (CMAKE_G++_COMPILER_VERSION VERSION_LESS 9.0.0)
    target_link_libraries(CooperativeServer "-lstdc++fs")
endif ()

target_link_libraries(CooperativeClient Qt5::Core Qt5::Network Qt5::Widgets)
target_link_libraries(CooperativeServer Qt5::Core Qt5::Network Qt5::Widgets)
