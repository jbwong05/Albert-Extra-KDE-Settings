cmake_minimum_required(VERSION 3.1.3)

project(extrakdesettings)

file(GLOB_RECURSE SRC src/* metadata.json)

find_package(Qt5 5.9.5 REQUIRED COMPONENTS Widgets)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE src/)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Widgets
        albert::lib
        xdg
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib/albert/plugins)
