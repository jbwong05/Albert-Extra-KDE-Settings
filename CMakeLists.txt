cmake_minimum_required(VERSION 3.1.3)

project(extrakdesettings)

option(BUILD_SEPARATELY "Build separately from Albert" OFF)

file(GLOB_RECURSE SRC src/* metadata.json)

find_package(Qt5 5.9.5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(KF5Service REQUIRED COMPONENTS Service)

if(BUILD_SEPARATELY)
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    
    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    # Set standard
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    
    # Set additional compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") #-Wconversion -Wunused
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_QML_DEBUG")
    
    # Color make output
    SET(CMAKE_COLOR_MAKEFILE ON)
    
    # Set the RPATH for the library lookup
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../")

    set(ALBERT_PATH /usr/lib/x86_64-linux-gnu/albert/)

    find_library(ALBERT NAMES albert libalbert PATHS ${ALBERT_PATH})
    if(NOT ALBERT)
        message(FATAL_ERROR "albert library not found")
    endif()
    message(STATUS "albert library - ${ALBERT}")

    find_library(XDG NAMES xdg libxdg PATHS ${ALBERT_PATH})
    if(NOT XDG)
        message(FATAL_ERROR "xdg library not found")
    endif()
    message(STATUS "xdg library - ${XDG}")

    set(INCLUDE src/ include/)
    set(LINK_LIBRARIES KF5::Service Qt5::Widgets Qt5::Concurrent ${ALBERT} ${XDG})

else()

    set(INCLUDE src/)
    set(LINK_LIBRARIES KF5::Service Qt5::Widgets Qt5::Concurrent albert::lib xdg)

endif()

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LINK_LIBRARIES})

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib/albert/plugins)
