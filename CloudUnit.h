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
#ifndef CloudUnitH
#define CloudUnitH
//---------------------------------------------------------------------------
#include "ParticleSystem.h"
//---------------------------------------------------------------------------
class BDCloud : public BDParticleSystem {
 private:
  BDParticle* Drops;
  float Height;
  Vector3* playerPos;
  float RescueTime;   // the longer this ticks, the faster the cloud goes
  // to save space, acceleration, which is not used, will store the player's
  // last position before leaving the root OctNode
  bool bCaught;
  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnCollision(DrawableObject*, float);
  virtual void OnPrepare(float deltaTime);

  void Emit(BDParticle* d) {
    d->position.x = position.x + double(rand() % 10000 - 5000)/1500.0;
    d->position.y = position.y; // double(rand() % 10000 - 5000)/20000.0;
    d->position.z = position.z + double(rand() % 10000 - 5000)/1500.0;
    d->energy = Height;     // distance still to fall
  }

 public:
  BDCloud(Vector3, unsigned int, Vector3*);
  ~BDCloud();

  virtual bool Collision(BDCollision*);
};
//---------------------------------------------------------------------------
#endif
