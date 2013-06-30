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
#include "FireUnit.h"
//---------------------------------------------------------------------------
BDFire::BDFire(Vector3 pos, unsigned int tID) {
  position = pos;

  texID = tID;
  MaxNum = 20;

  Force.x = Force.z = 0.0;

  PartSize = 0.5;

  Strength = 5;

  EmitInterval = 0.02;

  size = 0.5*Strength*EmitInterval*MaxNum;
  position.y += size;

  Particles = new BDParticle[MaxNum];
  AccumTime = 0.0;
  LastEmitted = 0;
  ObjType = OBJ_FIRE;
}
//---------------------------------------------------------------------------
BDFire::~BDFire() {
  delete[] Particles;
}
//---------------------------------------------------------------------------
void BDFire::OnAnimate(double deltaTime) {
  AccumTime += deltaTime;

  while(AccumTime >= EmitInterval) {
    LastEmitted++;
    if (LastEmitted >= MaxNum)
      LastEmitted = 0;
    Emit(LastEmitted);
    AccumTime -= EmitInterval;
  }

  for (int index = 0; index < MaxNum; index++) {
    Particles[index].velocity += Particles[index].acceleration * deltaTime;
    Particles[index].position += Particles[index].velocity * deltaTime;
  }
}
//---------------------------------------------------------------------------
void BDFire::OnDraw(BDCamera* Cam) {                                                                                                                                  //60 degrees
  if (!Cam->SphereInFrustum(position, size))
    return;

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);

  // The modelview matrix
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  // Get the correct vectors for billboarding
  Vector3 right(matrix[0], matrix[4], matrix[8]);
  Vector3 up(matrix[1], matrix[5], matrix[9]);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTranslated(position.x, position.y - size, position.z);

  // Draw particles
  glBegin(GL_QUADS);
  Vector3 Calc;
  for (int index = 0; index < MaxNum; index++) {
    Calc = -right * PartSize;
    glTexCoord2f(0.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = right * PartSize;
    glTexCoord2f(1.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + (right * PartSize);
    glTexCoord2f(1.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position - (right * PartSize);
    glTexCoord2f(0.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
//---------------------------------------------------------------------------
void BDFire::Explode() {
  for (int index = 0; index < MaxNum; index++) {
    Particles[index].acceleration = Force;
    Emit(index);
  }
}
//---------------------------------------------------------------------------
void BDFire::Emit(int n) {
  Particles[n].velocity = (Vector3((rand() % 70) - 35,
                                   (rand() % 100) + 30,
                                   (rand() % 70) - 35).WithLength(
                                       (rand() % Strength) + Strength/2));
  Particles[n].position.x = Particles[n].position.z = 0.0;
  Particles[n].position.y = PartSize;
}
//---------------------------------------------------------------------------
void BDFire::OnCollision(DrawableObject* Object, float) {
  BDCollision col;
  col.ColType = ObjType;
  Object->Collision(&col);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
