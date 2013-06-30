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
#include "FartCloudUnit.h"
//---------------------------------------------------------------------------
BDFartCloud::BDFartCloud(Vector3 pos, Vector3 vel, BDFartCloudParam* p) {
  bAlpha = true;
  position = pos;
  velocity = -vel*3;

  texID = p->bBubble ? p->BubtexID : p->texID;
  ECtexID = p->ECtexID;
  MaxNum = p->MaxNum;

  Force = p->Force;

  PartSize = p->PartSize;

  Strength = p->Strength;

  ECStrength = p->ECStrength;

  EmitInterval = p->EmitInterval;

  LifeTime = p->bBubble ? 0.2 : p->LifeTime;

  size = Strength*EmitInterval*MaxNum;

  Particles = new BDParticle[MaxNum];
  AccumTime = 0.0;
  LastEmitted = 0;
  Exploded = p->bBubble;
  colDiv = 1/(EmitInterval*MaxNum*1.75);

  ObjType = OBJ_FARTCLOUD;
}
//---------------------------------------------------------------------------
BDFartCloud::~BDFartCloud() {
  delete[] Particles;
}
//---------------------------------------------------------------------------
void BDFartCloud::OnAnimate(double deltaTime) {
  velocity *= 1-deltaTime;//0.997;
  position += velocity * deltaTime;

  LifeTime -= deltaTime;

  if (MaxNum <= 0)
    bDelete = true;

  AccumTime += deltaTime;

  while(AccumTime >= EmitInterval) {
    if (LifeTime < 0.0)
      MaxNum--;

    LastEmitted++;
    if (LastEmitted >= MaxNum)
      LastEmitted = 0;
    Emit(&Particles[LastEmitted]);
    AccumTime -= EmitInterval;
  }

  BDParticle *idx, *last = &Particles[MaxNum];
  for (idx = Particles; idx < last; idx++) {
    idx->velocity += idx->acceleration * deltaTime;
    idx->position += idx->velocity * deltaTime;
    idx->energy -= deltaTime * colDiv;
  }
}
//---------------------------------------------------------------------------
void BDFartCloud::OnDraw(BDCamera* Cam) {                                                                                                                                  //60 degrees
  // all alpha objects will be tested automatically for visibility
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  if (texID == ECtexID)
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
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

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  if (Exploded)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTranslated(position.x, position.y, position.z);

  // Draw particles
  glBegin(GL_QUADS);
  for (int index = 0; index < MaxNum; index++) {
    if (!Exploded)
      glColor4f(0.0, Particles[index].energy+0.25, 0.0, Particles[index].energy);
    Vector3 Calc = Particles[index].position - ((right + up) * PartSize);
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
  if (texID == ECtexID)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
//---------------------------------------------------------------------------
void BDFartCloud::Explode() {
  BDParticle *index, *last = &Particles[MaxNum];
  for (index = Particles; index != last; index++) {
    index->acceleration = Force;
    Emit(index);
  }
}
//---------------------------------------------------------------------------
void BDFartCloud::Emit(BDParticle* n) {
  n->velocity = Vector3((rand() % 100) - 50, (rand() % 100) - 15, (rand() % 100) - 50);
  if (n->velocity != Vector3(0.0))
    n->velocity.SetLength((rand() % Strength) + Strength/2);
  n->position.x = n->position.y = n->position.z = 0.0;
  n->energy = 1.0;
}
//---------------------------------------------------------------------------
void BDFartCloud::OnCollision(DrawableObject* Object, float) {
  if ((!Exploded) &&
      ((Object->ObjType == OBJ_FIRE) || (Object->ObjType == OBJ_EXPLOSION) ||
       (Object->ObjType == OBJ_BULLET))) {
    Burn();
  }

  BDCollision col;
  col.Position = position;
  col.ColType = ObjType;
  Object->Collision(&col);
}
//---------------------------------------------------------------------------
bool BDFartCloud::Collision(BDCollision* col) {
  if ((!Exploded) &&
      ((col->ColType == OBJ_FIRE) || (col->ColType == OBJ_EXPLOSION) ||
       (col->ColType == OBJ_BULLET))) {
    Burn();
  }

  col->ColType = ObjType;

  return false;
}
//---------------------------------------------------------------------------
void BDFartCloud::Burn() {
  texID = ECtexID;
  Force.x = Force.y = Force.z = 0.0;
  LifeTime = 0.1;
  Strength = ECStrength;
  Explode();
  Exploded = true;
  ObjType = OBJ_EXPLOSION;
}
//---------------------------------------------------------------------------
void BDFartCloud::OnPrepare(float deltaTime) {
  // Collisions
  Vector3 dummy = velocity;   // avoid landscape collisions
  velocity = Vector3(0.0, 0.0, 0.0);
  ((DrawableObject*)(parent->child->next))->PerformCollisionDetection(this, deltaTime);
  velocity = dummy;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
