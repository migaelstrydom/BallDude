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
#include "BallUnit.h"
#include "OctTree.h"  // to see when ball falls off level
#include "BDShadow.h"

#define BALLELASTICITY 0.2  //1.0 if completely inelastic, 0.0 completely elastic
#define BALLFRICTION 0.4
#define BALLSIZE 1.0
//---------------------------------------------------------------------------
unsigned int BDBall::iDList;
bool BDBall::bLoaded = false;
//---------------------------------------------------------------------------
BDBall::BDBall(Vector3 pos, unsigned int tID) {
  position = pos;
  size = BALLSIZE;
  bFriction = false;
  texID = tID;

  colN = Vector3(0.0, 1.0, 0.0);
  rotmat[0] = 1.0f;
  rotmat[1] = 0.0f;
  rotmat[2] = 0.0f;
  rotmat[3] = 0.0f;
  rotmat[4] = 1.0f;
  rotmat[5] = 0.0f;
  rotmat[6] = 0.0f;
  rotmat[7] = 0.0f;
  rotmat[8] = 1.0f;

  ObjType = OBJ_OBJECT;

  // SHADOW
  shadow = new BDShadow();
  shadow->InitSphere();
  shadow->SphereShadow(size+0.01, Vector3(329.983, 942.809, 47.1405));
}
//---------------------------------------------------------------------------
BDBall::~BDBall() {
  delete shadow;
}
//---------------------------------------------------------------------------
void BDBall::OnAnimate(double deltaTime) {
  // POSITION
  position += velocity * deltaTime;
  velocity += acceleration * deltaTime;

  if ((BDOctTree::Nodes[0].x1 > position.x) || (BDOctTree::Nodes[0].x2 < position.x) ||
      (BDOctTree::Nodes[0].y1 > position.y) || (BDOctTree::Nodes[0].y2 < position.y) ||
      (BDOctTree::Nodes[0].z1 > position.z) || (BDOctTree::Nodes[0].z2 < position.z))
    bDelete = true;

  // GRAVITY
  acceleration.y = -10.0;

  // FRICTION
  if (bFriction) {
    acceleration = -velocity * BALLFRICTION;
    // GRAVITY
    acceleration.y -= 10.0;
    rotvel = velocity;
    bFriction = false;
  }

  // ROTATION
  double len = rotvel.length();

  if (len == 0.0)
    return;

  float theta = ((180/3.141592653589793) / size) * len;

  Vector3 axis = (rotvel / len) ^ colN;
  if (axis == Vector3(0.0, 0.0, 0.0))
    return;

  float mat[16] = {rotmat[0], rotmat[1], rotmat[2], 0.0f,
                   rotmat[3], rotmat[4], rotmat[5], 0.0f,
                   rotmat[6], rotmat[7], rotmat[8], 0.0f,
                   0.0f, 0.0f, 0.0f, 1.0f};
  glPushMatrix();

  glRotatef(deltaTime * -theta, axis.x, axis.y, axis.z);
  glMultMatrixf(mat);

  glGetFloatv(GL_MODELVIEW_MATRIX, mat);

  glPopMatrix();

  rotmat[0] = mat[0];
  rotmat[1] = mat[1];
  rotmat[2] = mat[2];
  rotmat[3] = mat[4];
  rotmat[4] = mat[5];
  rotmat[5] = mat[6];
  rotmat[6] = mat[8];
  rotmat[7] = mat[9];
  rotmat[8] = mat[10];
}
//---------------------------------------------------------------------------
void BDBall::OnDraw(BDCamera* Cam) {
  shadow->position = position;

  if (!Cam->SphereInFrustum(position, size))
    return;

  glTranslated(position.x, position.y, position.z);

  float m[16] = {rotmat[0], rotmat[1], rotmat[2], 0.0f,
                 rotmat[3], rotmat[4], rotmat[5], 0.0f,
                 rotmat[6], rotmat[7], rotmat[8], 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f};

  glMultMatrixf(m);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glCallList(iDList);
  glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------
void BDBall::OnCollision(DrawableObject* Object, float deltaTime) {
  BDCollision col;
  col.Position = position;
  col.Size = size;
  col.ColType = ObjType;
  if (!Object->Collision(&col))
    return;
  // if (col.ColType != BDCollision::OBJECT)
  //    return;

  Vector3 R = (position - Object->position);
  double len = R.length();

  if (len == 0.0)
    return;

  R /= len;
  colN = -R;

  // position = Object->position + R*(size + Object->size);

  Vector3 I = Object->velocity - velocity;

  double speed = I.length();

  if (speed == 0.0)
    return;

  Vector3 rot = I; // For rotation so no mult. by speed is needed

  I /= speed;

  double cosa = (I * R);

  if (cosa < 0.0)
    return;

  R = R * cosa;

  R *= speed * Object->size / size;

  Vector3 V = rot - R;

  Object->velocity = V + velocity;
  velocity += R;

  rotvel += (rot - rotvel) * Object->size * cosa;

  // acceleration -= rotvel;
}
//---------------------------------------------------------------------------
bool BDBall::Collision(BDCollision* Col) {
  if (Col->ColType == OBJ_WATER) {
    Col->Size = velocity.y * 0.2;

    if (fabs(Col->Size) < 0.2) {
      Col->Size = 0.0;
      if (velocity.x*velocity.x + velocity.z*velocity.z > 1.0) {
        Col->Size = -0.4;
      }
    }
    bFriction = true;
  } else if (Col->ColType == OBJ_TRIANGLE) {
    /*if (Col->Triangle->friction < Col->Plane.N.y) {
      acceleration.y = 0.0;
      } */

    bFriction = true;
    colN = Col->Plane.N;
    Vector3 projN = (velocity * Col->Plane.N) * Col->Plane.N;
    if (projN.SquaredLength() > 1.0) {
      position = Col->pInt + (Col->Plane.N * size);
      velocity = velocity.Reflection(Col->Plane.N) + projN * BALLELASTICITY;
      return false;
    }
  } else if (Col->ColType == OBJ_EXPLOSION) {
    if (acceleration.SquaredLength() <= 10000.0)
      acceleration += (position - Col->Position).WithLength(300.0);
  }

  return true;
}
//---------------------------------------------------------------------------
void BDBall::Load() {
  if (bLoaded)
    return;

  // BDBDMModel *sphere = new BDBDMModel();
  // sphere->MakeSphere(20, 14, BALLSIZE);
  GLUquadricObj *Sphere = gluNewQuadric();

  gluQuadricTexture(Sphere, GL_TRUE);
  iDList = glGenLists(1);
  glNewList(iDList, GL_COMPILE);
  glColor3f(1.0, 1.0, 1.0);
  gluSphere(Sphere, BALLSIZE, 20, 14); //20, 14
  // sphere->StaticDraw();
  glEndList();

  gluDeleteQuadric(Sphere);
  // delete sphere;

  /*pbuffer = MSOpenGL::CreatePBuffer(256, 256);
    glBindTexture(GL_TEXTURE_2D, pbuffer->texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    pbuffer->Bind();
    pbuffer->MakeCurrent();
    glClearColor(1.0, 0.0, 1.0, 0.025);
    glClear(GL_COLOR_BUFFER_BIT);

    MSOpenGL::MakeViewport(pbuffer->Width, pbuffer->Height);
    wglMakeCurrent(MSOpenGL::hDC, MSOpenGL::hRC);   */

  bLoaded = true;
}
//---------------------------------------------------------------------------
void BDBall::OnPrepare(float deltaTime) {
  ((DrawableObject*)next)->PerformCollisionDetection(this, deltaTime);
}
//---------------------------------------------------------------------------
void BDBall::Unload() {
  if (!bLoaded)
    return;

  glDeleteLists(iDList, 1);

  bLoaded = false;
}
//---------------------------------------------------------------------------
/*void BDBall::DrawToPBuffer(float deltaTime) {
  static float fAngle = 0.0f;
  fAngle += 360*deltaTime;

  pbuffer->MakeCurrent();
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -8.0);
  glRotatef(fAngle, 0.0, 0.0, 1.0);
  glBegin( GL_TRIANGLES );
  glColor3f( 1.0f, 0.0f, 0.0f ); glVertex2f( 0.0f, 1.0f );
  glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( 0.87f, -0.5f );
  glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.87f, -0.5f );
  glEnd();
  wglMakeCurrent(MSOpenGL::hDC, MSOpenGL::hRC);
  }    */
//---------------------------------------------------------------------------
