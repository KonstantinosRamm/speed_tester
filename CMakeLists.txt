cmake_minimum_required(VERSION 3.10)
project(speedtester)

set(CMAKE_CXX_STANDARD 17)

# Βρες όλα τα cpp αρχεία μέσα στο src
file(GLOB SOURCES "src/*.cpp")

add_executable(speedtester ${SOURCES})

target_include_directories(speedtester PRIVATE include)

target_link_libraries(speedtester ixwebsocket curl ssl crypto z pthread)

