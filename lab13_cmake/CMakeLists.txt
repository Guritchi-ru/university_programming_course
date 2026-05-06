cmake_minimum_required(VERSION 3.10)

project(lab13_cmake)

option(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compgame.c)

if(BUILD_SHARED_LIBS)
    add_library(compgame SHARED ${SOURCES})
else()
    add_library(compgame STATIC ${SOURCES})
endif()

add_executable(lab13 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

target_link_libraries(lab13 compgame)