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
#ifndef BulletExplosionH
#define BulletExplosionH
//---------------------------------------------------------------------------
#include "ParticleSystem.h"
#include "LoadDataUnit.h"
//---------------------------------------------------------------------------
class BDBulletExplosion : public BDParticleSystem {
 private:
  float PartSize;
  float Energy;    // Each particles' life length in seconds
  float ColorDiv;
  int Strength; // Maximum value for initial velocity vec length

  void OnAnimate(double deltaTime);
  void OnDraw(BDCamera*);

 public:
  BDBulletExplosion(Vector3, BDFile*, unsigned int);
  ~BDBulletExplosion();

  void Explode();
};
//---------------------------------------------------------------------------
#endif
