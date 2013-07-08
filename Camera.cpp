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
#include "Camera.h"
#include "Player.h"
#include "AABB.h"

#include "LoadDataUnit.h"

#include <cmath>
//TEMP:
#include <cassert>
#include <iostream>
using namespace std;

#define CAM_ACC     50.0
#define CAM_DEC     7.0
#define INPUT_RESPONSE_TIME     0.1
//---------------------------------------------------------------------------
BDCamera::BDCamera(BDPlayer *p)
    : Player(p), bDrawAlpha(false) {
  BDFile File;
  File.ReadData("Camera.txt");

  MinZVal = File.GetDatad("MinZVal");
  MaxZVal = File.GetDatad("MaxZVal");

  DistFromPlayer = File.GetDatad("DefaultZVal");

  Speed.x = File.GetDatad("Speed", 0);
  Speed.y = File.GetDatad("Speed", 1);
  Speed.z = File.GetDatad("Speed", 2);

  ZoomOutGrad = File.GetDatad("ZoomOutGrad");

  FollowSpeed = File.GetDatad("FollowSpeed");

  DispFactor = File.GetDatad("DispFactor");

  Position.x = File.GetDatad("StartingPosition", 0);
  Position.y = File.GetDatad("StartingPosition", 1);
  Position.z = File.GetDatad("StartingPosition", 2);

  CamObject = new DrawableObject;
  CamObject->size = 3.0;

  IndependentMovement = false;
  xmovement = ymovement = 0;
}
//---------------------------------------------------------------------------
BDCamera::~BDCamera() {
  delete CamObject;
}
//---------------------------------------------------------------------------
void BDCamera::Animate(double deltaTime) {
  LookAt = Player->position - (Player->position - LookAt)/(1 + 10*deltaTime);

  if (IndependentMovement) {
    AnimateMovement(deltaTime);
    return;
  }

  Vector3 relpos = Position - LookAt;

  float rellength = relpos.length();

  float newlength = DistFromPlayer - (DistFromPlayer - rellength)/(1 + 8*deltaTime);
  relpos = (relpos / rellength) * newlength;
  rellength = newlength;
  Position = relpos + LookAt;
  Velocity = Vector3(0.0, 0.0, 0.0);

  Velocity.y = ((LookAt.y + rellength * ZoomOutGrad) - Position.y) * DispFactor;

  // FOR MOUSE MOVEMENT
  int mx, my;
  Input->MousePosition(mx, my);

  static float tint = 0.0;
  static float xmovesum = 0.0, ymovesum = 0.0;
  static float xmovetarget = 0.0, ymovetarget = 0.0;
  tint += deltaTime;
  xmovesum += mx;
  ymovesum += my;
  if (tint > 0.1) {
    xmovetarget = xmovesum;
    ymovetarget = ymovesum;
    tint = 0.0;
    xmovesum = 0.0;
    ymovesum = 0.0;
  }
  xmovement = xmovetarget - (xmovetarget - xmovement)/(1 + 5*deltaTime);
  ymovement = ymovetarget - (ymovetarget - ymovement)/(1 + 5*deltaTime);

  // GO UP AND DOWN
  ZoomOutGrad += Speed.y * ymovement;

  // Restrictions
  if (ZoomOutGrad < 0.2)
    ZoomOutGrad = 0.2;

  if (ZoomOutGrad > 0.7)
    ZoomOutGrad = 0.7;

  // ROTATE CAMERA AROUND PLAYER
  Velocity += (relpos ^ Vector3(0.0, 1.0, 0.0)).WithLength(
      Speed.x * DistFromPlayer * xmovement);

  // MOVE CAMERA DISTANCE FROM PLAYER
  if (Input->Key('\'') && !Input->Key('.')) {
    DistFromPlayer -= Speed.z * deltaTime;
    if (DistFromPlayer < MinZVal)
      DistFromPlayer = MinZVal;
  } else if (Input->Key('.') && !Input->Key('\'')) {
    DistFromPlayer += Speed.z * deltaTime;
    if (DistFromPlayer > MaxZVal)
      DistFromPlayer = MaxZVal;
  }

  // update position
  Position += Velocity * deltaTime;
}
//---------------------------------------------------------------------------
void BDCamera::Draw() {
  gluLookAt(Position.x, Position.y, Position.z,
            LookAt.x, LookAt.y, LookAt.z,
            0.0, 1.0, 0.0);

  if (Velocity != Vector3(0.0, 0.0, 0.0)) {
    CalcFrustum();
  }
  /*glPopMatrix();

 /// BEGIN TEMP

 glMatrixMode(GL_PROJECTION);    // prepare for and then
 glPopMatrix();

 glMatrixMode(GL_MODELVIEW);
 gluLookAt(50.0, 50.0, 32.0,
 Position.x, Position.y, Position.z,
 0.0, 1.0, 0.0);

 // Draw frustum
 glLineWidth(1.0);
 glBegin(GL_LINE_LOOP);
 Vector3 t = fFar->RayIntersection(Position, fNear->N);
 t = fTop->RayIntersection(t, fFar->N ^ (fFar->N ^ fTop->N));
 t = fRight->RayIntersection(t, (fFar->N ^ fTop->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 fur = t;

 t = fLeft->RayIntersection(t, (fFar->N ^ fTop->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 ful = t;

 t = fBottom->RayIntersection(t, (fFar->N ^ fLeft->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 fbl = t;

 t = fRight->RayIntersection(t, (fFar->N ^ fBottom->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 fbr = t;
 glEnd();

 glBegin(GL_LINE_LOOP);
 t = fNear->RayIntersection(fur, (fRight->N ^ fTop->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 nur = t;

 t = fNear->RayIntersection(ful, (fLeft->N ^ fTop->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 nul = t;

 t = fNear->RayIntersection(fbl, (fBottom->N ^ fLeft->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 nbl = t;

 t = fNear->RayIntersection(fbr, (fRight->N ^ fBottom->N));
 glVertex3f(t.x, t.y, t.z);
 Vector3 nbr = t;
 glEnd();

 glBegin(GL_LINES);
 glVertex3f(fur.x, fur.y, fur.z);
 glVertex3f(nur.x, nur.y, nur.z);

 glVertex3f(ful.x, ful.y, ful.z);
 glVertex3f(nul.x, nul.y, nul.z);

 glVertex3f(fbr.x, fbr.y, fbr.z);
 glVertex3f(nbr.x, nbr.y, nbr.z);

 glVertex3f(fbl.x, fbl.y, fbl.z);
 glVertex3f(nbl.x, nbl.y, nbl.z);
 glEnd();

 // End draw frustum

 // Draw camera
 glPushMatrix();
 glTranslatef(Position.x, Position.y, Position.z);
 glScalef(2.0, 2.0, 2.0);
 glCallList(97);
 glPopMatrix();
 // END TEMP    */
}
//---------------------------------------------------------------------------
void BDCamera::CalcFrustum() {
  float   proj[16];
  float   modl[16];
  float   clip[16];

  // Get the current PROJECTION matrix from OpenGL
  glGetFloatv(GL_PROJECTION_MATRIX, proj);

  // Get the current MODELVIEW matrix from OpenGL
  glGetFloatv(GL_MODELVIEW_MATRIX, modl);

  // multiply projection by modelview
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 4; r++) {
      clip[4*c+r] = 0.0;
      for (int i = 0; i < 4; i++) {
        clip[4*c+r] += modl[i + 4*c] * proj[4*i + r];
      }
    }
  }

  float normalisation;

  // Extract the numbers for the RIGHT plane
  fRight.N.x = clip[3] - clip[0];
  fRight.N.y = clip[7] - clip[4];
  fRight.N.z = clip[11] - clip[8];
  fRight.D = clip[15] - clip[12];

  normalisation = 1 / fRight.N.length();
  fRight.N *= normalisation;
  fRight.D *= normalisation;


  fLeft.N.x = clip[3] + clip[0];
  fLeft.N.y = clip[7] + clip[4];
  fLeft.N.z = clip[11] + clip[8];
  fLeft.D = clip[15] + clip[12];
  normalisation = 1 / fLeft.N.length();
  fLeft.N *= normalisation;
  fLeft.D *= normalisation;


  fBottom.N.x = clip[3] + clip[1];
  fBottom.N.y = clip[7] + clip[5];
  fBottom.N.z = clip[11] + clip[9];
  fBottom.D = clip[15] + clip[13];

  normalisation = 1 / fBottom.N.length();
  fBottom.N *= normalisation;
  fBottom.D *= normalisation;


  fTop.N.x = clip[3] - clip[1];
  fTop.N.y = clip[7] - clip[5];
  fTop.N.z = clip[11] - clip[9];
  fTop.D = clip[15] - clip[13];

  normalisation = 1 / fTop.N.length();
  fTop.N *= normalisation;
  fTop.D *= normalisation;


  fFar.N.x = clip[3] - clip[2];
  fFar.N.y = clip[7] - clip[6];
  fFar.N.z = clip[11] - clip[10];
  fFar.D = clip[15] - clip[14];

  normalisation = 1 / fFar.N.length();
  fFar.N *= normalisation;
  fFar.D *= normalisation;


  fNear.N.x = clip[3] + clip[2];
  fNear.N.y = clip[7] + clip[6];
  fNear.N.z = clip[11] + clip[10];
  fNear.D = clip[15] + clip[14];

  normalisation = 1 / fNear.N.length();
  fNear.N *= normalisation;
  fNear.D *= normalisation;
}
//---------------------------------------------------------------------------
bool BDCamera::SphereInFrustum(Vector3 pos, float sz) {    // Returns distance to cam
  dist = fNear.DistanceToPlane(pos);
  if (dist <= -sz)
    return false;

  if (fTop.DistanceToPlane(pos) <= -sz)
    return false;

  if (fLeft.DistanceToPlane(pos) <= -sz)
    return false;

  if (fRight.DistanceToPlane(pos) <= -sz)
    return false;

  if (fBottom.DistanceToPlane(pos) <= -sz)
    return false;

  if (fFar.DistanceToPlane(pos) <= -sz)
    return false;

  return true;
}
//---------------------------------------------------------------------------
BDCamera::InFrustum BDCamera::BoxInFrustum(BDAABB box) {
  InFrustum vis = IFALL;
  // Far plane
  dist = fFar.DistanceToPlane(box.center);
  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fFar.N.x) + fabs(box.extent[1]*fFar.N.y) +
        fabs(box.extent[2]*fFar.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fFar.N.x) + fabs(box.extent[1]*fFar.N.y) +
        fabs(box.extent[2]*fFar.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  // Top plane
  dist = fTop.DistanceToPlane(box.center);
  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fTop.N.x) + fabs(box.extent[1]*fTop.N.y) +
        fabs(box.extent[2]*fTop.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fTop.N.x) + fabs(box.extent[1]*fTop.N.y) +
        fabs(box.extent[2]*fTop.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  // Left plane
  dist = fLeft.DistanceToPlane(box.center);
  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fLeft.N.x) + fabs(box.extent[1]*fLeft.N.y) +
        fabs(box.extent[2]*fLeft.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fLeft.N.x) + fabs(box.extent[1]*fLeft.N.y) +
        fabs(box.extent[2]*fLeft.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  // Right plane
  dist = fRight.DistanceToPlane(box.center);
  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fRight.N.x) + fabs(box.extent[1]*fRight.N.y) +
        fabs(box.extent[2]*fRight.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fRight.N.x) + fabs(box.extent[1]*fRight.N.y) +
        fabs(box.extent[2]*fRight.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  // Bottom plane
  dist = fBottom.DistanceToPlane(box.center);
  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fBottom.N.x) + fabs(box.extent[1]*fBottom.N.y) +
        fabs(box.extent[2]*fBottom.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fBottom.N.x) + fabs(box.extent[1]*fBottom.N.y) +
        fabs(box.extent[2]*fBottom.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  // Near plane (last so that dist is stored as dist from camera)
  dist = fNear.DistanceToPlane(box.center);

  if (dist >= 0) {
    if (dist <
       (fabs(box.extent[0]*fNear.N.x) + fabs(box.extent[1]*fNear.N.y) +
        fabs(box.extent[2]*fNear.N.z))) {
      vis = IFSOME;
    }
  } else {
    if (fabs(dist) >=
       (fabs(box.extent[0]*fNear.N.x) + fabs(box.extent[1]*fNear.N.y) +
        fabs(box.extent[2]*fNear.N.z))) {
      return IFNONE;
    } else vis = IFSOME;
  }

  return vis;
}
//---------------------------------------------------------------------------
void BDCamera::SetPlayer(BDPlayer* p) {
  Player = p;
  LookAt = p->position;
}
//---------------------------------------------------------------------------
void BDCamera::AnimateMovement(double deltaTime) {
  Position = Movement.Update(deltaTime);

  if (Movement.IsDone())
    IndependentMovement = false;
}
//---------------------------------------------------------------------------
void BDCamera::MoveBezier() {
  IndependentMovement = true;

  Velocity.x = Velocity.y = Velocity.z = 0.00001;   // so it will calcfrustum
}
//---------------------------------------------------------------------------
void BDCamera::DrawAlphaObjects(std::vector<BDShadow*> &shadows) {
  bDrawAlpha = true;

  while(!alphas.empty()) {
    alphas.top().ptr->Draw(this, shadows);
    alphas.pop();
  }

  bDrawAlpha = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
