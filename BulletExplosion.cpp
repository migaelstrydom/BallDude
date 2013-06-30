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
#include "BulletExplosion.h"
#include "LoadDataUnit.h"
//---------------------------------------------------------------------------
BDBulletExplosion::BDBulletExplosion(Vector3 pos, BDFile* File, unsigned int Tex) {
  bAlpha = true;
  position = pos;
  texID = Tex;
  MaxNum = File->GetDatai("MaxNum");

  Force.x = File->GetDatad("Force", 0);
  Force.y = File->GetDatad("Force", 1);
  Force.z = File->GetDatad("Force", 2);

  PartSize = File->GetDatad("ParticleSize");

  Energy = File->GetDatad("Energy");

  Strength = int(File->GetDatad("BlastStrength"));

  size = File->GetDatad("Size");

  ColorDiv = 1/Energy;

  Particles = new BDParticle[MaxNum];
}
//---------------------------------------------------------------------------
BDBulletExplosion::~BDBulletExplosion() {
  delete[] Particles;
}
//---------------------------------------------------------------------------
void BDBulletExplosion::OnAnimate(double deltaTime) {
  Energy -= deltaTime;
  if (Energy <= 0.0)
    bDelete = true;

  velocity += acceleration * deltaTime;
  position += velocity * deltaTime;

  for (int index = 0; index < MaxNum; index++) {
    Particles[index].velocity += Particles[index].acceleration * deltaTime;
    Particles[index].position += Particles[index].velocity * deltaTime;
  }
}
//---------------------------------------------------------------------------
void BDBulletExplosion::OnDraw(BDCamera* Cam) {                                                                                                                                  //60 degrees
  // all alpha objects will be tested automatically for visibility

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);
  glDepthMask(GL_FALSE);
  glDisable(GL_LIGHTING);

  // The modelview matrix
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  // Get the correct vectors for billboarding
  Vector3 right(matrix[0], matrix[4], matrix[8]);
  Vector3 up(matrix[1], matrix[5], matrix[9]);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTranslated(position.x, position.y, position.z);
  float col = Energy*ColorDiv;
  glColor4f(col+0.5, 0.8*(col+0.5), 0.5*(col+0.5), col+0.5);

  // Draw particles
  glBegin(GL_QUADS);
  for (int index = 0; index < MaxNum; index++) {
    Vector3 Calc = Particles[index].position + ((right + up) * -PartSize);
    glTexCoord2f(0.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((right - up) * PartSize);
    glTexCoord2f(1.0, 0.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((right + up) * PartSize);
    glTexCoord2f(1.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);

    Calc = Particles[index].position + ((up - right) * PartSize);
    glTexCoord2f(0.0, 1.0);
    glVertex3d(Calc.x, Calc.y, Calc.z);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glDepthMask(GL_TRUE);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
//---------------------------------------------------------------------------
void BDBulletExplosion::Explode() {
  for (int index = 0; index < MaxNum; index++) {
    // Particles[index].position = position;
    Particles[index].acceleration = Force;
    Particles[index].velocity =
        (Vector3((rand() % 100) - 50,
                 (rand() % 100) - 50,
                 (rand() % 100) - 50).WithLength((rand() % int(Strength))));
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
