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
#ifndef ScriptEventH
#define ScriptEventH
//---------------------------------------------------------------------------
#include "Vector.h"
#include <vector>
//---------------------------------------------------------------------------
enum BDInstructionType {MSGBOX, VELOCITY, POSITION, ADDPOSITION,
                        ACCELERATION, CAMPOSITION, CAMVELOCITY, WAITFORENTER, DELAY,
                        DELETETHIS, NOMSGBOX, LOADLEVEL, CAMPOSBEZIER, FADETOMUSIC};
//---------------------------------------------------------------------------
struct BDSInstruction {
  BDInstructionType type;
  int id;                  // WHERE IN ARRAY TO FIND DATA
  int numdata;             // NUMBER OF PIECES OF DATA
};
//---------------------------------------------------------------------------
struct BDSEvent {
  Vector3 TriggerPos;
  float TriggerRad;

  int CurrInstruct;

  std::vector<BDSInstruction> Instruct;
};
//---------------------------------------------------------------------------
#endif
