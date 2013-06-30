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
#ifndef BulletUnitH
#define BulletUnitH
//---------------------------------------------------------------------------
#include "DrawableObject.h"
#include "CollisionObj.h"
#include "Texture.h"
#include "LoadDataUnit.h"
//---------------------------------------------------------------------------
class BDBullet : public DrawableObject {
 protected:
  float speed;
  float distance;
  int numbounces;

  BDFile* File;
  unsigned int texID;
  unsigned int iExplosionTexID;

  virtual void OnAnimate(double deltaTime);
  virtual void OnDraw(BDCamera*);
  virtual bool Collision(BDCollision*);
  virtual void OnPrepare(float deltaTime);

 public:
  BDBullet(Vector3, Vector3, BDFile*, unsigned int, unsigned int);
  ~BDBullet();
};
//---------------------------------------------------------------------------
#endif
