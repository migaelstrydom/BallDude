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
#ifndef MainEngineH
#define MainEngineH
//---------------------------------------------------------------------------
#include <GL/gl.h>
//---------------------------------------------------------------------------
#include "World.h"
#include "InputUnit.h"
#include "OpenGLUnit.h"
#include "Script.h"
// TEMP
#include "FontUnit.h"
//---------------------------------------------------------------------------
class BDMainEngine
{
 private:
  BDWorld *World;
  BDInput Input;
  BDScript *Script;

  // TEMP
  //MSFont *debugOutput; // For text output

 public:
  BDMainEngine();
  ~BDMainEngine();

  void Load();

  void GameCycle(void);
};
//---------------------------------------------------------------------------
#endif
