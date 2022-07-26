cmake_minimum_required(VERSION 3.16.3)

project("ClientApp" VERSION 1.0.0)


set(CMAKE_CXX_FLAGS "-std=c++17 -lboost_program_options")


set(THREADS_PREFER_PTHREAD_FLAG ON)

set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)




add_executable(ClientApp main.cpp               
                ${MY_INCLUDE_DIR}/client.h client.cpp       
               
               )

target_include_directories(ClientApp PUBLIC ${MY_INCLUDE_DIR})


find_package(Threads REQUIRED)

target_link_libraries(ClientApp PRIVATE Threads::Threads)



find_package(Boost 1.71.0)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(ClientApp PRIVATE ${Boost_LIBRARIES})



