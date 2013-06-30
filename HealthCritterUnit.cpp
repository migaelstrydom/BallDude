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
#include "HealthCritterUnit.h"
#include "OpenGLUnit.h"
#include "LoadDataUnit.h"
#include "Triangle.h"
#include "OctTree.h"  // to see boundaries of level
#include <cstdlib>
#define VELANIMRAT 0.42  // ratio between walk speed and animation speed
#define MAXWALKSPEED 20.0   //
#define CYCLETIME 5.0    // cycle independent of walkspeed and animation
//---------------------------------------------------------------------------
BDBDMModel *BDHealthCritter::Model = NULL;
//---------------------------------------------------------------------------
BDHealthCritter::BDHealthCritter(unsigned tID)
    : texID(tID) {
  size = Model->Dimensions.y;
  fFT = 0.0f;
  fCycle = 0.0f;
  currFrame = rand() % Model->NumFrames;
  nextFrame = (currFrame+1) % Model->NumFrames;
  walkspeed = MAXWALKSPEED + float(rand() % 11 - 5)/2.0f ;
  fAngle = 0.0;
  dest.y = 0.0;
  SetDest();

  ObjType = OBJ_ENEMY;

  // Shadow
  shadow = new BDShadow();
  shadow->MakeGraph(Model);
  // shadow->CalcShadow(Vector3(329.983, 942.809, 47.1405));
}
//---------------------------------------------------------------------------
BDHealthCritter::~BDHealthCritter() {
  delete shadow;
}
//---------------------------------------------------------------------------
void BDHealthCritter::OnAnimate(double deltaTime) {
  velocity += acceleration * deltaTime;
  position += velocity * deltaTime;
  acceleration.y = -10.0;
  fFT += deltaTime*walkspeed;
  if (fFT > 1.0f) {
    currFrame = nextFrame;
    nextFrame = (nextFrame+1) % Model->NumFrames;
    fFT = 0.0f;
  }

  fCycle += deltaTime;
  if (fCycle > CYCLETIME) {
    fCycle = 0.0f;
    if ((position - distTraveled).SquaredLength() < walkspeed*VELANIMRAT*CYCLETIME*0.5)
      SetDest();

    distTraveled = position;
    TestReachability();
  }

  double relx = dest.x-position.x, relz = dest.z-position.z;
  relx = relx*relx + relz*relz;   // relx is relative horiz distance squared

  if (relx < 16.0f)    // if at destination, set new destination
  {
    SetDest();
    TestReachability();
  }

  CalcDestAngle();

  if (fabs(fAngle - fDestAngle) < M_PI) {
    if (fAngle > fDestAngle)
      fAngle -= deltaTime;
    else
      fAngle += deltaTime;
  } else {
    if (fAngle > fDestAngle)
      fAngle += deltaTime;
    else
      fAngle -= deltaTime;
  }
  if (fAngle > 2*M_PI)
    fAngle -= 2*M_PI;
  else if (fAngle < 0.0)
    fAngle += 2*M_PI;

  double sa, ca;
  // sincos(fAngle, &sa, &ca);
  sa = sin(fAngle);
  ca = cos(fAngle);
  double velx = -sa*walkspeed*VELANIMRAT;
  double velz = -ca*walkspeed*VELANIMRAT;

  acceleration.x = (velx - velocity.x) * walkspeed;
  acceleration.z = (velz - velocity.z) * walkspeed;

  // update shadow
  // shadow->MakeGraph(Model);
  shadow->CalcShadow(Vector3(329.983, 942.809, 47.1405));
}
//---------------------------------------------------------------------------
void BDHealthCritter::OnDraw(BDCamera* Cam) {
  shadow->position = position;
  // update shadow
  // shadow->MakeGraph(Model);
  // shadow->CalcShadow(Vector3(1329.983, 942.809, 747.1405));
  //60 degrees
  if (!Cam->SphereInFrustum(position, size))
    return;

  glTranslated(position.x, position.y, position.z);
  // glScalef(5,5,5);

  glRotatef(RAD2DEG(fAngle), 0.0, 1.0, 0.0);

  glBindTexture(GL_TEXTURE_2D, texID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_DOUBLE, 0, Model->GetVerts(currFrame, nextFrame, fFT));
  glTexCoordPointer(2, GL_FLOAT, 0, Model->GetTexCos());

  glLockArrays(0, Model->NumVerts);
  glColor3f(1.0, 1.0, 1.0);
  glDrawElements(GL_TRIANGLES, Model->NumTris*3, GL_UNSIGNED_INT, Model->GetIndices());

  glUnlockArrays();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------
void BDHealthCritter::OnCollision(DrawableObject* Object, float deltaTime) {
  BDCollision col;
  col.Position = position;
  col.Size = size;
  col.ColType = ObjType;
  Object->Collision(&col);
}
//---------------------------------------------------------------------------
bool BDHealthCritter::Collision(BDCollision* Col) {
  if (Col->ColType == OBJ_WATER) {
    if (fFT < 0.1)
      Col->Size = -0.4;
    else
      Col->Size = velocity.y * 0.2;

    if (fabs(Col->Size) < 0.2) {
      Col->Size = 0.0;
      if (velocity.x*velocity.x + velocity.z*velocity.z > 1.0) {
        Col->Size = -0.4;
      }
    }
  } else if (Col->ColType == OBJ_TRIANGLE) {
    if (Col->triangle->friction < Col->Plane.N.y) {
      // if (velocity.y < 0.0)
      acceleration.y = 0.0;
      velocity.y = 0.0;
    }
  } else if (Col->ColType == OBJ_EXPLOSION) {
    if (acceleration.SquaredLength() <= 4000.0)
      acceleration += (position - Col->Position).WithLength(100.0);
  }
  /*else if (Col->ColType == BDCollision::ENEMY) {
    dest.x = 2 * position.x - dest.x;
    dest.z = 2 * position.z - dest.z;
    } */

  return true;
}
//---------------------------------------------------------------------------
void BDHealthCritter::OnPrepare(float deltaTime) {
  ((DrawableObject*)next)->PerformCollisionDetection(this, deltaTime);
}
//---------------------------------------------------------------------------
void BDHealthCritter::Load() {
  if (Model != NULL)
    return;

  Model = new BDBDMModel();
  Model->Load("HealthCritter");
}
//---------------------------------------------------------------------------
void BDHealthCritter::Unload() {
  if (Model == NULL)
    return;

  delete Model;
  Model = NULL;
}
//---------------------------------------------------------------------------
void BDHealthCritter::SetDest() {
  dest.x = double(rand() % 101)/100.0 * (BDOctTree::Nodes[0].x2 - BDOctTree::Nodes[0].x1) +
      BDOctTree::Nodes[0].x1;
  dest.z = double(rand() % 101)/100.0 * (BDOctTree::Nodes[0].z2 - BDOctTree::Nodes[0].z1) +
      BDOctTree::Nodes[0].z1;
}
//---------------------------------------------------------------------------
void BDHealthCritter::CalcDestAngle() {
  Vector3 pd = dest - position;
  pd.y = 0.0;
  pd.SetLength(1.0);

  fDestAngle = pd * Vector3(0.0, 0.0, -1.0); // optimise here
  fDestAngle = acos(fDestAngle);
  if (pd.x > 0.0)
    fDestAngle = 2*M_PI - fDestAngle;
}
//---------------------------------------------------------------------------
void BDHealthCritter::TestReachability() {
  dest.y = position.y;
  Vector3 colp = BDOctTree::RayTrace(position, (dest-position).WithLength(1.0));

  if (BDOctTree::TracedTri != NULL)
    if ((colp-position).SquaredLength() < (dest-position).SquaredLength()) {
      if (BDOctTree::TracedTri->Plane.N.y < BDOctTree::TracedTri->friction)
        dest = colp;
    }

  dest.y = 0.0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
