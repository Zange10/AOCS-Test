cmake_minimum_required(VERSION 3.24)
project(AOCS_Test C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(AOCS_Test
        main.c movement.c geometry.c drawing.c connector.c connector.h)
target_link_libraries(AOCS_Test ${GTK3_LIBRARIES} m)
