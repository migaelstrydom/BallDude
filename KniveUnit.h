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
#ifndef KniveUnitH
#define KniveUnitH
//---------------------------------------------------------------------------
#include "AIObject.h"
#include "Quaternion.h"
#include "Plane.h"
#include "BDMModelUnit.h"
//---------------------------------------------------------------------------
class BDKnive : public BDAIObject {
 private:
  /*static bool bLoaded;
    static unsigned int NumVerts;
    static unsigned int NumTris;
    static unsigned int NumFrames;
    static Vector3 *pVerts;
    static float *pTexCos;
    static unsigned int *pIndices;
    static Vector3 Dimensions; */
  static BDBDMModel *Model;

  float fFT;
  float walkspeed; // speed of walking, sync with animation
  unsigned currFrame, nextFrame;
  // BDQuaternion dir;
  float fAngle, fDestAngle;
  Vector3 dest;
  float fCycle;            // every couple of seconds, something needs to be done
  Vector3 distTraveled;   //1ce per cycle position is saved here. if found
  // that knive not moving far, change dest

  // DIE!!!
  float fDieingAnim;
  bool bDieingAnim;

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnCollision(DrawableObject*, float);
  virtual void OnPrepare(float);

  inline void SetDest();
  inline void CalcDestAngle();
  inline void TestReachability();  // Test if the dest can be reached

  void ChangeState(AI_STATES);

 public:
  // static unsigned int texID;
  BDKnive();
  ~BDKnive();

  virtual bool Collision(BDCollision*);
  static void Load();
  static void Unload();
};
//---------------------------------------------------------------------------
#endif
