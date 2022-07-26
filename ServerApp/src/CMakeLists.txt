cmake_minimum_required(VERSION 3.16.3)

project("ServerApp" VERSION 1.0.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lboost_program_options")

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)


add_executable(ServerApp main.cpp               
                ${MY_INCLUDE_DIR}/server.h server.cpp
                ${MY_INCLUDE_DIR}/background.h background.cpp      
               
               )

target_include_directories(ServerApp PUBLIC ${MY_INCLUDE_DIR})

find_package(Threads REQUIRED)

target_link_libraries(ServerApp PRIVATE Threads::Threads)


find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(ServerApp PRIVATE ${Boost_LIBRARIES})
