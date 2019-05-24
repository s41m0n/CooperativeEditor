cmake_minimum_required(VERSION 3.14)
project(CooperativeEditor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)
set (Boost_USE_STATIC_LIBS TRUE) #disable dynamic linking

set(Boost_INCLUDE_DIR /usr/include)
set(Boost_LIBRARY_DIR /usr/lib)

find_package(Boost COMPONENTS REQUIRED serialization system iostreams thread)
include_directories(${Boost_INCLUDE_DIR})

set(SPDLOG_DIR ${CMAKE_SOURCE_DIR})
include_directories(${SPDLOG_DIR})

add_executable(CooperativeEditor main.cpp client/SharedEditor.cpp client/SharedEditor.h components/Symbol.cpp components/Symbol.h server/NetworkServer.cpp server/NetworkServer.h components/Message.cpp components/Message.h components/Identifier.cpp components/Identifier.h network/Socket.h network/Socket.cpp utility/Utility.h)

#link Boost libraries to the new target
target_link_libraries (CooperativeEditor ${Boost_LIBRARIES})