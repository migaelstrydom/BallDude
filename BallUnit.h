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
#ifndef BallUnitH
#define BallUnitH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
//---------------------------------------------------------------------------
class BDBall : public DrawableObject
{
 private:
  // static loading stuff common to all balls
  static unsigned int iDList;
  static bool bLoaded;

  bool bFriction;  // true when ball is touching landscape

  unsigned int texID;

  // ROTATION
  float rotmat[9];    // rotation matrix
  Vector3 colN;      // normal of triangle collided with last.
  // Used to determine rotation axis
  Vector3 rotvel;    // velocity used when determining rotation

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnCollision(DrawableObject*, float);
  virtual void OnPrepare(float);

 public:
  BDBall(Vector3, unsigned int);
  ~BDBall();

  virtual bool Collision(BDCollision*);
  static void Load();
  static void Unload();
};
//---------------------------------------------------------------------------
#endif
