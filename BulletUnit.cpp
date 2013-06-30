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
#include "BulletUnit.h"
#include "BulletExplosion.h"
//---------------------------------------------------------------------------
BDBullet::BDBullet(Vector3 pos, Vector3 v, BDFile* f,
                   unsigned int tex, unsigned int etex)
  : File(f), texID(tex), iExplosionTexID(etex) {
  position = pos;
  speed = File->GetDatad("Speed");
  distance = File->GetDatad("MaxDist");
  size = File->GetDatad("Size");

  v.x += ((rand() % 100) - 50) * File->GetDatad("Accuracy");
  v.y += ((rand() % 100) - 50) * File->GetDatad("Accuracy");
  v.z += ((rand() % 100) - 50) * File->GetDatad("Accuracy");

  velocity = v.WithLength(speed);

  numbounces = f->GetDatai("NumBounces");
  ObjType = OBJ_BULLET;
}
//---------------------------------------------------------------------------
BDBullet::~BDBullet() {
}
//---------------------------------------------------------------------------
void BDBullet::OnAnimate(double deltaTime) {
  velocity += acceleration * deltaTime;
  position += velocity * deltaTime;

  distance -= speed * deltaTime;
  if (distance <= 0) {
    BDBulletExplosion *Explode = new BDBulletExplosion(position, File, iExplosionTexID);
    parent->AttachInFront(Explode);
    Explode->Explode();

    bDelete = true;
  }
}
//---------------------------------------------------------------------------
void BDBullet::OnDraw(BDCamera* Cam) {
  if (!Cam->SphereInFrustum(position, size))
    return;

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);
  // glDepthMask(GL_FALSE);
  // CLOUD BLENDFUNC
  // glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA_SATURATE);

  // The modelview matrix
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  // Get the correct vectors for billboarding
  Vector3 right(matrix[0], matrix[4], matrix[8]);
  Vector3 up(matrix[1], matrix[5], matrix[9]);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTranslated(position.x, position.y, position.z);
  // Draw bullet
  glBegin(GL_QUADS);
  Vector3 Calc = ((right + up) * -size);
  glTexCoord2f(0.0, 0.0);
  glVertex3d(Calc.x, Calc.y, Calc.z);

  Calc = ((right - up) * size);
  glTexCoord2f(1.0, 0.0);
  glVertex3d(Calc.x, Calc.y, Calc.z);

  Calc = ((right + up) * size);
  glTexCoord2f(1.0, 1.0);
  glVertex3d(Calc.x, Calc.y, Calc.z);

  Calc = ((up - right) * size);
  glTexCoord2f(0.0, 1.0);
  glVertex3d(Calc.x, Calc.y, Calc.z);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // glDepthMask(GL_TRUE);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
//---------------------------------------------------------------------------
bool BDBullet::Collision(BDCollision* Col) {
  if (Col->ColType == OBJ_WATER) {
    numbounces = 0;
    Col->Size = 1.0;
  }

  if ((Col->ColType == OBJ_FIRE) || (Col->ColType == OBJ_EXPLOSION))
    return false;

  if ((Col->ColType == OBJ_OBJECT) || (Col->ColType == OBJ_ENEMY)) {
    numbounces = 0;
  }

  if (Col->ColType == OBJ_FARTCLOUD) {
    return false;
  }

  if (numbounces > 0) {
    numbounces--;
    position = Col->pInt + (Col->Plane.N * size);

    // Reflection
    const Vector3 vec = velocity / speed;
    const float dot = (vec * Col->Plane.N);
    if (fabs(dot) < 0.2)
      numbounces++;

    velocity = (vec - Col->Plane.N * 2.0 * dot) * speed;
    return false;
  }

  BDBulletExplosion *Explode = new BDBulletExplosion(position, File, iExplosionTexID);
  parent->AttachInFront(Explode);
  Explode->Explode();
  bDelete = true;
  return (Col->ColType != OBJ_TRIANGLE);
}
//---------------------------------------------------------------------------
void BDBullet::OnPrepare(float deltaTime) {
  ((DrawableObject*)(parent->child->next))->PerformCollisionDetection(this, deltaTime);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
