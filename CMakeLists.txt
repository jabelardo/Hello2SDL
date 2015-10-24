cmake_minimum_required(VERSION 3.3)
project(Hello2SDL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -I/usr/include/libxml2")

set(SOURCE_FILES
        src/AnimatedGraphic.cpp
        src/AnimatedGraphic.h
        src/Bitmap.h
        src/Entity.h
        src/GameContext.h
        src/Game.cpp
        src/Game.h
        src/main.cpp
        src/MenuButton.cpp
        src/MenuButton.h
        src/RenderUtils.cpp
        src/RenderUtils.h
        src/TileMap.cpp
        src/TileMap.h
        src/UserInput.h
        src/V2D.h)

add_executable(CLION_PLACEHOLDER ${SOURCE_FILES})

target_compile_definitions(CLION_PLACEHOLDER PUBLIC "TMXEXPORT=")

add_custom_target(Hello2SDL
        COMMAND ${Hello2SDL_SOURCE_DIR}/src/build.sh ${Hello2SDL_SOURCE_DIR}/src)

add_custom_command(TARGET Hello2SDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Hello2SDL_SOURCE_DIR}/assets
        ${Hello2SDL_BINARY_DIR}/assets)