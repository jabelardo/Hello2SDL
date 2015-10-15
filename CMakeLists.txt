cmake_minimum_required(VERSION 3.3)
project(Hello2SDL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_custom_target(Hello2SDL
                  COMMAND ${Hello2SDL_SOURCE_DIR}/src/build.sh ${Hello2SDL_SOURCE_DIR}/src)

add_custom_command(TARGET Hello2SDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Hello2SDL_SOURCE_DIR}/assets
        ${Hello2SDL_BINARY_DIR}/assets)