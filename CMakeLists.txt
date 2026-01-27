cmake_minimum_required(VERSION 3.16)
project(KIPFIN_LABWORK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS
    projects/cpp/src/*.cpp
)

foreach(source ${PROJECT_SOURCES})
    get_filename_component(name ${source} NAME_WE)
    add_executable(${name} ${source})

    if(name STREQUAL "lvl3proj1")
        target_link_libraries(${name} PRIVATE Qt5::Widgets)
    endif()
endforeach()
