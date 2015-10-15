cmake_minimum_required(VERSION 3.3)
project(Hello2SDL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        src/Entity.cpp
        src/Entity.h
        src/Game.cpp
        src/Game.h
        src/LoaderParams.h
        src/main.cpp
        src/TextureManager.cpp
        src/TextureManager.h
        src/Vector2D.h
        src/InputHandler.cpp
        src/InputHandler.h)

add_executable(CLION_PLACEHOLDER ${SOURCE_FILES})

add_custom_target(Hello2SDL
                  COMMAND ${Hello2SDL_SOURCE_DIR}/src/build.sh ${Hello2SDL_SOURCE_DIR}/src)

add_custom_command(TARGET Hello2SDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Hello2SDL_SOURCE_DIR}/assets
        ${Hello2SDL_BINARY_DIR}/assets)