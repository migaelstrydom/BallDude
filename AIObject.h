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
#ifndef AIObjectH
#define AIObjectH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
//---------------------------------------------------------------------------
enum AI_STATES {AI_IDLE, AI_CURIOUS, AI_ANGRY, AI_WOUNDED, AI_SCARED, AI_SEEKFRIENDS};
//---------------------------------------------------------------------------
class BDAIObject : public DrawableObject
{
 protected:
  AI_STATES eState;
  float fAwarenessRadius;        // aware of everything closer than this
  float fFOVCos;                 // cosine of FOV angle
  Vector3 dir;                  // direction unit is facing
  bool bPlayerSighted;
  Vector3 playerPos; // not pointer so that it equals position where last seen
  bool bFriendSighted;
  Vector3 friendPos;
  float fHealth;

  bool IsVisible(Vector3 p) {
    return ((p - position).SquaredLength() < fAwarenessRadius);
  }

  virtual void ChangeState(AI_STATES nstate) {eState = nstate;}

 public:
  void SetPlayerPos(Vector3 p) {
    if (IsVisible(p)) {
      playerPos = p;
      bPlayerSighted = true;
    }
  }
  void SetFriendPos(Vector3 p) {
    if (IsVisible(p)) {
      friendPos = p;
      bFriendSighted = true;
    }
  }
  bool isDead() {
    return bDelete;
  }
};
//---------------------------------------------------------------------------
#endif
