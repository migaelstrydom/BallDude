/************************************************************************
 * Copyright 2013 Migael Strydom
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/
//---------------------------------------------------------------------------
#ifndef InputUnitH
#define InputUnitH
//---------------------------------------------------------------------------
#include <SDL/SDL.h>
//---------------------------------------------------------------------------
class BDInput {
 private:
  int cursorX, cursorY;
  bool mouseLeft, mouseRight;
  unsigned char *keys;

 public:
  BDInput() {
    cursorX = 0;
    cursorY = 0;
    mouseLeft = false;
    mouseRight = false;
  }

  void Update() {
    int m = SDL_GetRelativeMouseState(&cursorX, &cursorY);
    mouseLeft = (m & SDL_BUTTON(SDL_BUTTON_LEFT));
    mouseRight = (m & SDL_BUTTON(SDL_BUTTON_RIGHT));
    keys = SDL_GetKeyState(NULL);
  }

  bool Key(int key) {
    return keys[key] != 0;
  }

  // Mouse
  void MousePosition(int& x, int& y) {
    x = cursorX;
    y = cursorY;
  }

  bool MouseLeftDown() {
    return mouseLeft;
  }

  bool MouseRightDown() {
    return mouseRight;
  }
};
//---------------------------------------------------------------------------
#endif
