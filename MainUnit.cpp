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
#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include "OpenGLUnit.h"
#include "MainEngine.h"
//---------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  bool Done = false;
  BDMainEngine *Engine;

  try {
    SDL_Event event;
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      throw BDError(std::string("Unable to init SDL: ") + SDL_GetError());
    }

    MSOpenGL::Init();
    if (argc > 1) {
      if (argv[1][0] == 'f') {
        MSOpenGL::MakeFullScreen(1280, 1024, 32);
      } else {
        MSOpenGL::MakeWindow(800, 600, 32);
      }
    } else {
      MSOpenGL::MakeWindow(800, 600, 32);
    }

    SDL_ShowCursor(false);

    // std::cout << glGetString(GL_VERSION) << std::endl;

    Engine = new BDMainEngine();
    Engine->Load();

    while (!Done) {
      while (SDL_PollEvent(&event)) {
        // check for messages
        switch (event.type) {
          // exit if the window is closed
          case SDL_QUIT:
            Done = true;
            break;

            // check for keypresses
          case SDL_KEYDOWN:
            // exit if ESCAPE is pressed
            if (event.key.keysym.sym == SDLK_ESCAPE)
              Done = true;
            break;
        }
      }

      Engine->GameCycle();
    }
  } catch (BDError error) {
    std::cerr << error.GetMsg() << std::endl;
  }

  delete Engine;
  MSOpenGL::Shutdown();

  return 0;
}
//---------------------------------------------------------------------------

