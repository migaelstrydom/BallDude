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
#ifndef FartCloudUnitH
#define FartCloudUnitH
//---------------------------------------------------------------------------
#include "ParticleSystem.h"
//---------------------------------------------------------------------------
struct BDFartCloudParam {
  unsigned int texID, ECtexID, BubtexID; // normal texID, exploding cloud texID, bubble texID
  int MaxNum;

  Vector3 Force;
  float PartSize, BubPartSize;
  int Strength, ECStrength;
  float EmitInterval;
  float LifeTime;
  bool bBubble;
};
//---------------------------------------------------------------------------
class BDFartCloud : public BDParticleSystem {
 private:
  float PartSize;
  int Strength, ECStrength; // Maximum value for initial velocity vec length
  int LastEmitted;
  float EmitInterval, LifeTime;
  bool Exploded;
  float colDiv;

  unsigned int ECtexID;

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual void OnCollision(DrawableObject*, float);
  virtual void OnPrepare(float deltaTime);

  void Emit(BDParticle*);

 public:
  BDFartCloud(Vector3, Vector3, BDFartCloudParam*);
  ~BDFartCloud();

  void Explode();
  inline void Burn();
  virtual bool Collision(BDCollision*);
};
//---------------------------------------------------------------------------
#endif
