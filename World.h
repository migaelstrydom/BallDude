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
#ifndef WorldH
#define WorldH
//---------------------------------------------------------------------------
#include "LandScape.h"
#include "Camera.h"
#include "Player.h"
#include "ResourceControlUnit.h"
#include "AIEngine.h"
//---------------------------------------------------------------------------
class BDWorld {
 private:
  BDCamera *Camera;
  BDLandScape *LandScape;
  BDPlayer *Player;
  BDResourceControl *ResCon;
  BDAIEngine *AIEngine;

  std::vector<float> Lights;

  bool bLoaded;

  // Music
  bool bPlayMusic;
  // Channel 0 is reserved for streaming music
  // Fade time is 3 seconds
  float fFadeTime;
  int iCurrMusic, iFadeToMusic;
  void UpdateFadeToMusic(float);  // takes deltaTime

  // TEMP BALL GAME
  // bool bgInLevel;
  // bool bgInGame;
  // float bgTimeLeft;
  // int bgScore;
  // std::vector<BDBall*> bgBallPointer;
  // END TEMP BALL GAME
 public:
  BDWorld();
  ~BDWorld();

  void LoadWorld(std::string);
  void UnloadWorld();

  void Animate(double deltaTime);

  void Draw();

  bool FadeToMusic(int);  // takes music. returns true when done fading

  BDPlayer* GetPlayer() {return Player;}
  BDCamera* GetCamera() {return Camera;}
  DrawableObject* GetTopObject() {return LandScape;};
};
//---------------------------------------------------------------------------
#endif
