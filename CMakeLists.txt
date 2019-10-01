cmake_minimum_required(VERSION 3.14)
project(CooperativeEditor)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if(CMAKE_VERSION VERSION_LESS 3.0.0)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-std=c++11 COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag(-std=c++0x COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()
else()
    SET(CMAKE_CXX_STANDARD 17)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

include_directories(src)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

#Set the client executable
add_executable(CooperativeClient src/mainclient.cpp src/client/controller/Controller.cpp src/client/model/Model.cpp src/client/view/View.cpp
        src/components/Symbol.cpp src/components/Identifier.cpp src/components/Message.cpp src/utility/CrdtAlgorithm.cpp)

#Set the server executable
add_executable(CooperativeServer src/mainserver.cpp src/server/controller/Controller.cpp src/server/model/Model.cpp
        src/components/Symbol.cpp src/components/Identifier.cpp src/components/Message.cpp src/utility/CrdtAlgorithm.cpp)

#Add libraries to compile the client
target_link_libraries(CooperativeClient ${CONAN_LIBS})
#Add libraries to compile the server
target_link_libraries(CooperativeServer ${CONAN_LIBS})