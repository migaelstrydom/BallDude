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
#ifndef ParticleSystemH
#define ParticleSystemH
//---------------------------------------------------------------------------
#include "Vector.h"
#include "DrawableObject.h"
#include "Particle.h"
#include "Texture.h"
#include "CollisionObj.h"
//---------------------------------------------------------------------------
class BDParticleSystem : public DrawableObject
{
protected:
  int MaxNum;  // Max num of particles
  int NumParticles;  // Number of particles currently active
  BDParticle *Particles;    // particles

  unsigned int texID;   // Texture ID for each particle
  float AccumTime;   // Amount of time since last particle emitted

  Vector3 Force;    // Force acting on system

  virtual void OnAnimate(double deltaTime) {}
  virtual void OnDraw(BDCamera*) {}

public:
  BDParticleSystem() {}
  ~BDParticleSystem() {}
};
//---------------------------------------------------------------------------
#endif
