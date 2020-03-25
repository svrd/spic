cmake_minimum_required(VERSION 3.1..3.10.2)

project(spic VERSION 1.0
             DESCRIPTION "A simple process intercommunication library"
             LANGUAGES CXX)

add_library(spic SHARED src/spic.cpp include/spic/spic.h)
target_include_directories(spic PUBLIC include)
target_compile_features(spic PUBLIC cxx_std_11)

add_executable(spic-client apps/spic_client.c)
target_link_libraries(spic-client PUBLIC spic)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()
