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
#include "MainEngine.h"
#include <SDL/SDL.h>
#include "SoundUnit.h"
//---------------------------------------------------------------------------
BDMainEngine::BDMainEngine() {
}
//---------------------------------------------------------------------------
BDMainEngine::~BDMainEngine() {
  delete Script;
  delete World;
  // TEMP
  //delete debugOutput;

  BDSound::Deinitialise();
}
//---------------------------------------------------------------------------
void BDMainEngine::Load() {
  // if (!MSGLExtensions::InitMultitextureExt())
  //    throw BDError("Your stupid system does not support multitexturing!");
  if (!MSGLExtensions::InitArrayExt())
    throw BDError("Your stupid system does not support array extensions!");
  // if (!MSGLExtensions::CheckReflectionSupport())
  //    throw BDError("The piece of junk you call your computer does not support reflection texture coordinates!");
  if (!MSGLExtensions::InitShaderExt())
    throw BDError("Your computer sucks. It doesn't support shaders.");

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);

  glFrontFace(GL_CCW);
  glEnable(GL_LIGHTING);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);

  BDSound::Initialise();

  World = new BDWorld();

  World->GetCamera()->SetInput(&Input);

  Script = new BDScript(World);
  Script->Load("Testing");

  // TEMP : JUST TO DISPLAY TEXT
  // debugOutput = new MSFont();
  // END
}
//---------------------------------------------------------------------------
void BDMainEngine::GameCycle() {
  static unsigned int last_game_cycle_time = SDL_GetTicks();

  unsigned int current_game_cycle_time = SDL_GetTicks();

  double deltaTime = (current_game_cycle_time - last_game_cycle_time)/1000.0;
  last_game_cycle_time = current_game_cycle_time;

  // deltaTime *= 0.0125;
  // for (int i = 0; i <= 10000000; i++);

  // Min frame rate is 10 fps
  if (deltaTime > 0.1)
    deltaTime = 0.1;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  Input.Update();

  // INPUT

  Script->Update(deltaTime, &Input);

  World->GetPlayer()->CheckInput(&Input);

  // COLLISIONS
  World->GetTopObject()->Prepare(deltaTime);

  // ANIMATE + DRAW
  World->Animate(deltaTime);

  World->Draw();

  World->GetPlayer()->HealthBar.Draw();

  Script->TextBox();

  // BDSound::SoundSystem->update();

  // TEMP
  /*if (MSOpenGL::msg.size() != 0) {
    debugOutput->DrawStr(MSOpenGL::msg.c_str());

    MSOpenGL::msg = "";
    }

  // FPS counter
  double now = Timer->GetCurrentTime();
  static double FPSTime = now;
  static int FrameCount = 0;
  static char fpsstr[32];

  if (now >= (FPSTime + 1)) {
    FPSTime = now;
    sprintf(fpsstr, "FPS = %i", FrameCount);
    FrameCount = 0;
  }
  FrameCount++;

  debugOutput->DrawStr(fpsstr);
  */
  // END TEMP

  glFlush();

  SDL_GL_SwapBuffers();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

