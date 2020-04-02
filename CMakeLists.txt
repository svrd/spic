cmake_minimum_required(VERSION 3.1..3.10.2)

project(spic VERSION 1.0
             DESCRIPTION "A simple process intercommunication library"
             LANGUAGES C CXX)

add_library(spic SHARED
    src/spic.cpp
    src/spic.hpp
    src/spic_fifo_node.cpp
    src/spic_fifo_node.hpp
    src/spic_message.hpp
    src/spic_node.cpp
    src/spic_sys_if.hpp
    include/spic/spic_node.hpp
    include/spic/spic.h)
target_include_directories(spic PUBLIC include)
target_compile_features(spic PUBLIC cxx_std_17)
target_compile_options(spic PRIVATE -Werror -Wall -Wextra)

add_executable(spic-client apps/spic_client.c)
target_link_libraries(spic-client PUBLIC spic)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()
