//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_USERINPUT_H
#define HELLO2SDL_USERINPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ButtonState {
  int halfTransitionCount;
  bool endedDown;
} ButtonState;

typedef struct UserInput {
  bool shouldQuit;

  int mousePositionX;
  int mousePositionY;
  int mouseWheelY;
  ButtonState mouseButtonLeft;
  ButtonState mouseButtonMiddle;
  ButtonState mouseButtonRight;

  ButtonState moveUp;
  ButtonState moveDown;
  ButtonState moveLeft;
  ButtonState moveRight;

  ButtonState actionUp;
  ButtonState actionDown;
  ButtonState actionLeft;
  ButtonState actionRight;

  ButtonState leftShoulder;
  ButtonState rightShoulder;

  ButtonState back;
  ButtonState start;
} UserInput;

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_USERINPUT_H
