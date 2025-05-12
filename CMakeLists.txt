cmake_minimum_required(VERSION 3.28.3)

project(speedtester)

file(GLOB_RECURSE SOURCES src/*.cpp)

include_directories(include)

# Find and link the CURL library
find_package(CURL REQUIRED)

add_executable(speedtester ${SOURCES})

# Link the CURL library
target_link_libraries(speedtester curl)

# Alternatively, if find_package doesn't work, you can explicitly specify the library path
# target_link_libraries(speedtester ${CURL_LIBRARIES})
