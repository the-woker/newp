cmake_minimum_required(VERSION 3.10)
project(newp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(newp src/main.cpp)

target_compile_features(newp PRIVATE cxx_std_20)

target_include_directories(newp
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS newp DESTINATION bin)
