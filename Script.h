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
#ifndef ScriptH
#define ScriptH
//---------------------------------------------------------------------------
#include "Vector.h"
#include "ScriptEvent.h"
#include "FontUnit.h"
//---------------------------------------------------------------------------
class BDWorld;
class MSFont;
class BDInput;
//---------------------------------------------------------------------------
class BDScript {
 private:
  BDFile *File;
  BDWorld *World;

  // std::vector<std::string> sTextBox;
  MSTextTexture currText;
  bool bDispText;
  MSFont *Font;   // For text

  std::vector<BDSEvent> Events;
  std::vector<BDSInstruction> GlobalInstruct;

  // Delay
  float Delay;

  // Data to be used by scripts
  std::vector<std::string> sData;
  std::vector<Vector3> vecData;
  std::vector<double> dData;
  std::vector<int> iData;

  void AssignText(std::string);
  void AddInstructions(std::vector<BDSInstruction> *, std::string);
 public:
  BDScript(BDWorld*);
  ~BDScript();

  void Load(std::string);
  void Unload();

  void Update(double&, BDInput*);
  void TextBox();
};
//---------------------------------------------------------------------------
#endif
