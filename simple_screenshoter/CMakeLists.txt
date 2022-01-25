cmake_minimum_required(VERSION 3.5)

project(simple_screenshoter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

file(GLOB headers "*.h")
source_group("Headers" FILES ${headers})

set(sources
    bitmap_from_image.cpp
    main.cpp
)

add_executable(simple_screenshoter ${headers} ${sources})

target_link_libraries(simple_screenshoter ${X11_LIBRARIES})
