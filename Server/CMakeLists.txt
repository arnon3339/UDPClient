#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(testUDP)

#----------------------------------------------------------------------------
file(GLOB headers ${PROJECT_SOURCE_DIR}/../config.h)

#----------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
#find_package(Boost REQUIRED COMPONENTS system asio filesystem array bind) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(server server.cpp ${headers}) 
    target_link_libraries(server ${Boost_LIBRARIES})
endif()

install(server testUDP DESTINATION bin)
