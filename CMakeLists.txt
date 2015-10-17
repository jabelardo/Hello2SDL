cmake_minimum_required(VERSION 3.3)
project(Hello2SDL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        src/AnimatedGraphic.cpp
        src/AnimatedGraphic.h
        src/Entity.cpp
        src/Entity.h
        src/Game.cpp
        src/Game.h
        src/GameContext.h
        src/GameOverState.cpp
        src/GameOverState.h
        src/GameUpdateAndRender.cpp
        src/GameUpdateAndRender.h
        src/GameState.h
        src/GameStateId.h
        src/GameStateMachine.cpp
        src/GameStateMachine.h
        src/LoaderParams.h
        src/main.cpp
        src/MenuButton.cpp
        src/MenuButton.h
        src/MenuState.cpp
        src/MenuState.h
        src/PauseState.cpp
        src/PauseState.h
        src/Player.cpp
        src/Player.h
        src/PlayState.cpp
        src/PlayState.h
        src/Sprite.cpp
        src/Sprite.h
        src/TextureId.h
        src/TextureManager.cpp
        src/TextureManager.h
        src/UserInput.h
        src/Vector2D.h)

add_executable(CLION_PLACEHOLDER ${SOURCE_FILES})

add_custom_target(Hello2SDL
        COMMAND ${Hello2SDL_SOURCE_DIR}/src/build.sh ${Hello2SDL_SOURCE_DIR}/src)

add_custom_command(TARGET Hello2SDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${Hello2SDL_SOURCE_DIR}/assets
        ${Hello2SDL_BINARY_DIR}/assets)