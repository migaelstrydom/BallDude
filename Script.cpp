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
#include "LoadDataUnit.h"
#include "Script.h"
#include "BDError.h"
#include "World.h"
#include <GL/gl.h>
//---------------------------------------------------------------------------
using std::string;
//---------------------------------------------------------------------------
BDScript::BDScript(BDWorld* p)
  : World(p) {
  // Font initialisation
  MSFont::InitSystem();
  Font = new MSFont("Comfortaa-Regular.ttf");
  Delay = 0.0f;
  currText.Width = -1;    // If the width is -1, it is the first time and
  // no previous texture needs to be deleted.
}
//---------------------------------------------------------------------------
BDScript::~BDScript() {
  delete Font;
  MSFont::DeInitSystem();

  Unload();
}
//---------------------------------------------------------------------------
void BDScript::Load(string Filename) {
  // LOAD WORLD
  World->LoadWorld((Filename + string("World.txt")).c_str());

  bDispText = false;
  File = new BDFile();

  File->ReadData((Filename + string("Script.txt")).c_str());

  string sEventString;
  for (unsigned int idx = 0; idx < File->GetSizes("Events"); idx++) {
    Events.push_back(BDSEvent());
    Events[idx].CurrInstruct = 0;

    sEventString = File->GetDatas("Events", idx);

    AddInstructions(&Events[idx].Instruct,
                    sEventString.substr(0, sEventString.find_first_of(':')));

    sEventString.erase(0, sEventString.find_first_of(':')+1);
    if (sEventString != string("start")) {
      Events[idx].TriggerPos.x = File->GetDatad(sEventString);
      Events[idx].TriggerPos.y = File->GetDatad(sEventString, 1);
      Events[idx].TriggerPos.z = File->GetDatad(sEventString, 2);
      Events[idx].TriggerRad = File->GetDatad(sEventString, 3);
    } else {
      Events[idx].TriggerPos = World->GetPlayer()->position;
      Events[idx].TriggerRad = 1.0;
    }
  }

  /*if (File->GetSizes("Global")) {
    AddInstructions(&GlobalInstruct, "Global");
    }          */

  delete File;
  File = NULL;
}
//---------------------------------------------------------------------------
void BDScript::Unload() {
  Events.clear();
  sData.clear();
  vecData.clear();
  dData.clear();
  iData.clear();
  bDispText = false;
  World->UnloadWorld();
}
//---------------------------------------------------------------------------
void BDScript::TextBox() {
  if (!bDispText)
    return;

  Font->DisplayTextBox(currText);
}
//---------------------------------------------------------------------------
void BDScript::Update(double& deltaTime, BDInput *Input) {
  bool isDisplayingText = false, cancelText = false;  // Don't cancel text if this is true.
  bool Next = true;  // true if has to go to next instruction
  for (unsigned int idx = 0; idx < Events.size(); idx++) {
    if (((World->GetPlayer()->position - Events[idx].TriggerPos).SquaredLength() <
         (Events[idx].TriggerRad * Events[idx].TriggerRad))) {
      switch(Events[idx].Instruct[Events[idx].CurrInstruct].type) {
        case MSGBOX:
          AssignText(sData[Events[idx].Instruct[Events[idx].CurrInstruct].id]);
          bDispText = true;
          isDisplayingText = true;
          break;
        case VELOCITY:
          World->GetPlayer()->velocity =
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case POSITION:
          World->GetPlayer()->position =
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case ADDPOSITION:
          World->GetPlayer()->position +=
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case ACCELERATION:
          World->GetPlayer()->acceleration =
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case CAMPOSITION:
          World->GetCamera()->Position =
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case CAMVELOCITY:
          World->GetCamera()->Velocity =
              vecData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          break;
        case CAMPOSBEZIER:
          for (int i = Events[idx].Instruct[Events[idx].CurrInstruct].id + 1;
               i < (Events[idx].Instruct[Events[idx].CurrInstruct].id +
                    Events[idx].Instruct[Events[idx].CurrInstruct].numdata);
               i += 3)
            World->GetCamera()->Movement.AddPoint(Vector3(dData[i], dData[i+1], dData[i+2]));

          World->GetCamera()->Movement.InitBezier(dData[Events[idx].Instruct[Events[idx].CurrInstruct].id]);

          World->GetCamera()->MoveBezier();
          break;
        case WAITFORENTER:
          if (!(Input->Key(SDLK_RETURN) || Input->MouseLeftDown()))
            Next = false;

          // Input->Clear();  // TEMP UNCOMMENT
          World->GetPlayer()->velocity = Vector3(0.0, 0.0, 0.0);
          break;
        case DELAY:
          if ((Delay += deltaTime) <= dData[Events[idx].Instruct[Events[idx].CurrInstruct].id]) {
            Next = false;
            // Input->Clear();    // TEMP UNCOMMENT
            World->GetPlayer()->velocity = Vector3(0.0, 0.0, 0.0);
          }
          break;
        case DELETETHIS:
          for (unsigned int i = idx; i < Events.size()-1; i++) {
            Events[i] = Events[i+1];
          }
          Events.pop_back();
          break;
        case NOMSGBOX:
          cancelText = true;
          break;
        case FADETOMUSIC:
          Next = World->FadeToMusic(iData[Events[idx].Instruct[Events[idx].CurrInstruct].id]);
          break;
        case LOADLEVEL:
          string levelname = sData[Events[idx].Instruct[Events[idx].CurrInstruct].id];
          Unload();
          Load(levelname);
          return;
      };

      if (Next) {
        if (++Events[idx].CurrInstruct >= int(Events[idx].Instruct.size()))
          Events[idx].CurrInstruct = 0;

        Delay = 0.0f;
      } else Next = true;
    } else Events[idx].CurrInstruct = 0;
  }

  if (cancelText && !isDisplayingText) {
    bDispText = false;
  }
}
//---------------------------------------------------------------------------
void BDScript::AssignText(string txt) {
  if (bDispText)
    return;

  if (currText.Width > 0) {
    glDeleteTextures(1, &currText.texID);
  }

  currText = Font->CreateTextBoxImage(txt, 55);
}
//---------------------------------------------------------------------------
/*void BDScript::ProcessMSG(String sEventString, int idx) {
  Events[idx].Type = MSGBOX;
  sEventString.Delete(1, 4);

  Events[idx].iArraySize =
  File->GetSizes(sEventString.SubString(1, sEventString.Pos(":") - 1));

  Events[idx].sText = new AnsiString[Events[idx].iArraySize];

  for (int idx2 = 0; idx2 < Events[idx].iArraySize; idx2++) {
  Events[idx].sText[idx2] =
  File->GetDatas(sEventString.SubString(1, sEventString.Pos(":") - 1), idx2);
  }
  sEventString.Delete(1, sEventString.Pos(":"));
  Events[idx].TriggerPos.x =
  File->GetDatad(sEventString.SubString(1, sEventString.Pos(":") - 1));
  Events[idx].TriggerPos.y =
  File->GetDatad(sEventString.SubString(1, sEventString.Pos(":") - 1), 1);
  Events[idx].TriggerPos.z =
  File->GetDatad(sEventString.SubString(1, sEventString.Pos(":") - 1), 2);
  Events[idx].TriggerRad =
  File->GetDatad(sEventString.SubString(1, sEventString.Pos(":") - 1), 3);

  sEventString.Delete(1, sEventString.Pos(":"));
  if (sEventString == "t")
  Events[idx].bButton = true;
  else
  Events[idx].bButton = false;
  }      */
//---------------------------------------------------------------------------
// First parameter is pointer to instructions in event to fill, second is event name
void BDScript::AddInstructions(std::vector<BDSInstruction> *eventinstr, string ins) {
  string command;
  string data;
  BDSInstruction instr;
  enum {NONE, STRING, BDVECTOR, DOUBLE, INTEGER} dataType;

  for (unsigned int idx = 0; idx < File->GetSizes(ins); idx++) {
    command = File->GetDatas(ins, idx);
    if (command.find_first_of('(') != 0) {
      data = command.substr(command.find_first_of('(') + 1,
                            command.length() - command.find_first_of('(') - 2);
      command = command.substr(0, command.find_first_of('('));
    } else {
      data = "";
      instr.id = -1;
    }

    dataType = NONE;

    if (command == "msg") {
      instr.type = MSGBOX;
      dataType = STRING;
    } else if (command == "velocity") {
      instr.type = VELOCITY;
      dataType = BDVECTOR;
    } else if (command == "position") {
      instr.type = POSITION;
      dataType = BDVECTOR;
    } else if (command == "addposition") {
      instr.type = ADDPOSITION;
      dataType = BDVECTOR;
    } else if (command == "acceleration") {
      instr.type = ACCELERATION;
      dataType = BDVECTOR;
    } else if (command == "camposition") {
      instr.type = CAMPOSITION;
      dataType = BDVECTOR;
    } else if (command == "camvelocity") {
      instr.type = CAMVELOCITY;
      dataType = BDVECTOR;
    } else if (command == "waitforenter")
      instr.type = WAITFORENTER;
    else if (command == "delay") {
      instr.type = DELAY;
      dataType = DOUBLE;
    } else if (command == "delete")
      instr.type = DELETETHIS;
    else if (command == "nomsg")
      instr.type = NOMSGBOX;
    else if (command == "loadlevel") {
      instr.type = LOADLEVEL;
      dataType = STRING;
    } else if (command == "camposbezier") {
      instr.type = CAMPOSBEZIER;
      dataType = DOUBLE;
    } else if (command == "fadetomusic") {
      instr.type = FADETOMUSIC;
      dataType = INTEGER;
    }

    switch(dataType) {
      case STRING :
        instr.id = sData.size();
        if (data[0] != '\'')
          sData.push_back(File->GetDatas(data));
        else
          sData.push_back(data.substr(1, data.length() - 2));
        break;
      case BDVECTOR :
        instr.id = vecData.size();
        instr.numdata = File->GetSized(data) / 3;
        for (unsigned int i = 0; i < File->GetSized(data); i += 3)
          vecData.push_back(Vector3(File->GetDatad(data, i),
                                    File->GetDatad(data, i+1),
                                    File->GetDatad(data, i+2)));
        break;
      case DOUBLE :
        instr.id = dData.size();
        instr.numdata = File->GetSized(data);
        for (unsigned int i = 0; i < File->GetSized(data); i++)
          dData.push_back(File->GetDatad(data, i));
        break;
      case INTEGER :
        instr.id = iData.size();
        instr.numdata = File->GetSizei(data);
        for (unsigned int i = 0; i < File->GetSizei(data); i++)
          iData.push_back(File->GetDatai(data, i));
        break;
    };

    eventinstr->push_back(instr);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

