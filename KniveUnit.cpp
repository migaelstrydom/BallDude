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
#include "KniveUnit.h"
#include "OpenGLUnit.h"
#include "LoadDataUnit.h"
#include "Triangle.h"
#include "OctTree.h"  // to see boundaries of level
#include <cstdlib>
#define VELANIMRAT 0.42  // ratio between walk speed and animation speed
#define MAXWALKSPEED 12.0
#define CYCLETIME 5.0    // cycle independent of walkspeed and animation
#define KNIVE_MAXHEALTH 0.1
//---------------------------------------------------------------------------
BDBDMModel *BDKnive::Model = NULL;
//---------------------------------------------------------------------------
BDKnive::BDKnive() {
  size = Model->Dimensions.y;
  fFT = 0.0f;
  fCycle = 0.0f;
  currFrame = rand() % Model->NumFrames;
  nextFrame = (currFrame+1) % Model->NumFrames;
  // dir = BDQuaternion(Vector3(0.0, 1.0, 0.0), 0.0);
  walkspeed = MAXWALKSPEED + float(rand() % 11 - 5)/2.0f ;
  fAngle = 0.0;
  dest.y = 0.0;
  SetDest();
  bAlpha = true;
  fDieingAnim = 2.0f;
  bDieingAnim = false;

  // AI values
  fAwarenessRadius = 2500.0f;    // remember: squared distance

  ObjType = OBJ_ENEMY;
  fHealth = KNIVE_MAXHEALTH;
  eState = AI_IDLE;

  shadow = new BDShadow();
  shadow->MakeGraph(Model);
}
//---------------------------------------------------------------------------
BDKnive::~BDKnive() {
  delete shadow;
}
//---------------------------------------------------------------------------
void BDKnive::OnAnimate(double deltaTime) {
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

  dir.x = -sa;
  dir.z = -ca;

  // DIE!!!
  if (bDieingAnim)
    fDieingAnim -= deltaTime;
}
//---------------------------------------------------------------------------
void BDKnive::OnDraw(BDCamera* Cam) {                                                                                                                                  //60 degrees
  if (!Cam->SphereInFrustum(position, size))
    return;

  /*MSOpenGL::msg = dir.v.x;
    MSOpenGL::msg += "\n";
    MSOpenGL::msg += dir.v.y;
    MSOpenGL::msg += "\n";
    MSOpenGL::msg += dir.v.z;
    MSOpenGL::msg += "\n";
    MSOpenGL::msg += dir.s;  */

  glDisable(GL_LIGHTING);
  glTranslated(position.x, position.y, position.z);

  glRotatef(RAD2DEG(fAngle), 0.0, 1.0, 0.0);

  // glBindTexture(GL_TEXTURE_2D, texID);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_VERTEX_ARRAY);
  // glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3, GL_DOUBLE, 0, Model->GetVerts(currFrame, nextFrame, fFT));
  // glTexCoordPointer(2, GL_FLOAT, 0, pTexCos);

  // calculate shadow, after the vertices have been updated
  shadow->CalcShadow(Vector3(7.0, 20.0, 0.0));
  shadow->position = position;

  glLockArrays(0, Model->NumVerts);
  // glDrawBuffer(GL_NONE);
  // glDrawBuffer(GL_BACK);
  // Outline
  /*if (bDieingAnim) {
    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(10.0f - 5.0f*fDieingAnim);
    glDrawBuffer(GL_NONE);
    glDrawElements(GL_TRIANGLES, Model->NumTris*3, GL_UNSIGNED_INT, Model->GetIndices());
    // reset to defaults
    glDrawBuffer(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    } else  */
  if (!bDieingAnim) {
    glFrontFace(GL_CW);
    glPolygonMode(GL_FRONT, GL_LINE);
    glLineWidth(3.0);
    glDepthMask(GL_FALSE);
    glColor3f(0.85, 1.0, 0.4);
    glDrawElements(GL_TRIANGLES, Model->NumTris*3, GL_UNSIGNED_INT, Model->GetIndices());
    // reset to defaults
    glDepthMask(GL_TRUE);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT, GL_FILL);
    glColor3f(0.0, 0.0, 0.0);
    glDrawElements(GL_TRIANGLES, Model->NumTris*3, GL_UNSIGNED_INT, Model->GetIndices());
  } else {
    glEnable(GL_BLEND);
    glColor4f(1.0, 0.4, 0.8, fDieingAnim*0.5);
    glDrawElements(GL_TRIANGLES, Model->NumTris*3, GL_UNSIGNED_INT, Model->GetIndices());
    glDisable(GL_BLEND);
  }
  glUnlockArrays();

  glDisableClientState(GL_VERTEX_ARRAY);
  // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  // glDisable(GL_TEXTURE_2D);

  glEnable(GL_LIGHTING);

  // TEMP: DRAW DEST
  /*glPopMatrix();
    glPushMatrix();
    glTranslated(dest.x, position.y, dest.z);
    glScalef(8.0, 8.0, 8.0);
    glCallList(97);
    glPopMatrix();
    glPushMatrix(); */
}
//---------------------------------------------------------------------------
void BDKnive::OnCollision(DrawableObject* Object, float deltaTime) {
  if (bDieingAnim)
    return;

  BDCollision col;

  if (Object->ObjType == OBJ_ENEMY) {
    Vector3 rel = (position - Object->position);
    double rad = rel.length();
    if (rad == 0.0)
      return;

    rel /= rad;

    BDPlane div(rel, 0.0);
    double twidimz = Model->Dimensions.z*2;
    if (rad < twidimz)    // if colliding
    {
      double intru = twidimz - rad;
      rel *= intru*0.5;
      position += rel;
      Object->position -= rel;
    }

    if (div.DistanceToPlane(velocity) < 0.0) {
      dest = position + div.RayIntersection(velocity, div.N)*size*10;
      dest.y = 0.0;
    }
    col.Plane = div;
  } else {
    BDQuaternion orient(Vector3(0.0, 1.0, 0.0), -fAngle);
    BDQuaternion opos;
    opos.v = Object->position - position;
    opos = orient*opos*orient.Conjugate();

    // check if sphere in box
    if (fabs(opos.v.x) > Model->Dimensions.x + Object->size)
      return;
    if (fabs(opos.v.y) > Model->Dimensions.y + Object->size)
      return;
    if (fabs(opos.v.z) > Model->Dimensions.z + Object->size)
      return;

    if (Object->ObjType == OBJ_BULLET) {
      fHealth -= 1.0;            //1 once off
    } else if ((Object->ObjType == OBJ_FIRE) || (Object->ObjType == OBJ_EXPLOSION)) {
      fHealth -= 5.0*deltaTime;  //5 per second
    }
  }

  col.Position = position;
  col.Size = size;
  col.ColType = ObjType;
  Object->Collision(&col);
}
//---------------------------------------------------------------------------
bool BDKnive::Collision(BDCollision* Col) {
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
  } else if (Col->ColType == OBJ_ENEMY) {
    // make dest directly away from other enemy
    if (Col->Plane.DistanceToPlane(velocity) > 0.0) {
      dest = position + Col->Plane.RayIntersection(velocity, Col->Plane.N)*size*10;
      dest.y = 0.0;
    }
  }

  return true;
}
//---------------------------------------------------------------------------
void BDKnive::OnPrepare(float deltaTime) {
  // perform AI
  if ((fHealth < 0.0) && (!bDieingAnim))  // die!!!!
  {
    bDieingAnim = true;
  }

  if (fDieingAnim < 0.0f)
    bDelete = true;

  if (bPlayerSighted) {
    if (eState == AI_IDLE)
      eState = AI_CURIOUS;


    dest = playerPos;
  }

  bPlayerSighted = false;
  bFriendSighted = false;

  ((DrawableObject*)(parent->child->next))->PerformCollisionDetection(this, deltaTime);
}
//---------------------------------------------------------------------------
void BDKnive::Load() {
  if (Model != NULL)
    return;

  Model = new BDBDMModel();
  Model->Load("Knive");
}
//---------------------------------------------------------------------------
void BDKnive::Unload() {
  if (Model == NULL)
    return;

  delete Model;
  Model = NULL;
}
//---------------------------------------------------------------------------
void BDKnive::SetDest() {
  dest.x = double(rand() % 101)/100.0 * (BDOctTree::Nodes[0].x2 - BDOctTree::Nodes[0].x1) +
      BDOctTree::Nodes[0].x1;
  dest.z = double(rand() % 101)/100.0 * (BDOctTree::Nodes[0].z2 - BDOctTree::Nodes[0].z1) +
      BDOctTree::Nodes[0].z1;
}
//---------------------------------------------------------------------------
void BDKnive::CalcDestAngle() {
  Vector3 pd = dest - position;
  pd.y = 0.0;
  pd.SetLength(1.0);

  fDestAngle = pd * Vector3(0.0, 0.0, -1.0); // optimise here
  fDestAngle = acos(fDestAngle);
  if (pd.x > 0.0)
    fDestAngle = 2*M_PI - fDestAngle;
}
//---------------------------------------------------------------------------
void BDKnive::TestReachability() {
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
void BDKnive::ChangeState(AI_STATES) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
