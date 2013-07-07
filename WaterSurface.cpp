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
#include "WaterSurface.h"
#include "AABB.h"
#include "OpenGLUnit.h"
//---------------------------------------------------------------------------
BDWaterSurface::BDWaterSurface(Vector3 pos, const float xamp, int t, const int ms)
    : fAmp(xamp), h(2*xamp/(ms - 1)), MAXSEG(ms), fPosDiv(ms / (2*xamp)), texID(t) {
  position = pos;
  size = sqrt(xamp*xamp*2);

  ObjType = OBJ_WATER;

  z = new Vector3[MAXSEG*MAXSEG];
  z1 = new Vector3[MAXSEG*MAXSEG];
  d = new bool[MAXSEG*MAXSEG];
  n = new Vector3[MAXSEG*MAXSEG];
  indices = new int[MAXSEG*(MAXSEG-1)*2];

  int ci, cij;
  int i, j;
  for (i = 0; i < MAXSEG; i++) {
    ci = i * MAXSEG;
    for (j = 0; j < MAXSEG; j++) {
      cij = ci + j;
      z[cij].x = i * h;
      z[cij].y = 0.0;
      z[cij].z = j * h;
      z1[cij].x = i * h;
      z1[cij].y = 0.0;
      z1[cij].z = j * h;
      d[cij] = true;
      n[cij] = Vector3(0.0, 1.0, 0.0);
    }
  }

  for (i = 0; i < MAXSEG - 1; i++) {
    ci = i * MAXSEG;
    for (j = 0; j < MAXSEG; j++) {
      cij = ci + j;
      indices[cij*2 + 1] = cij;

      indices[cij*2] = cij + MAXSEG;
    }
  }

  if (!(MAXSEG % 2))
    z[(MAXSEG*MAXSEG + MAXSEG)/2].y = 0.5;
  else
    z[(MAXSEG*MAXSEG - 1)/2].y = 0.5;
}
//---------------------------------------------------------------------------
BDWaterSurface::~BDWaterSurface() {
  delete[] z;
  delete[] z1;
  delete[] d;
  delete[] n;
  delete[] indices;
}
//---------------------------------------------------------------------------
void BDWaterSurface::OnAnimate(double deltaTime) {
  if (!bDrawn)
    return;

  bDrawn = false;
  static float dt = 99*deltaTime;
  static int numframes = 99;
  dt += deltaTime;
  numframes++;
  deltaTime = dt / numframes;

  if (numframes == 200) {
    numframes = 100;
    dt *= 0.5;
  }

  const float cc = 130;  // cc = c^2
  float A = (cc*deltaTime*deltaTime)/(h*h);
  if (A > 0.5) A = 0.5;
  const float B = 2 - 4*A;

  int i, j, ci, cij;
  //1 FPS can be gained here by converting to pointers and while loop
  for (i = 1; i < MAXSEG - 1; i++) {
    ci = i * MAXSEG;
    for (j = 1; j < MAXSEG - 1; j++) {
      cij = ci + j;
      if (!d[cij])
        z1[cij].y = 0.0;
      else
        z1[cij].y = A * (z[cij-MAXSEG].y + z[cij+MAXSEG].y + z[cij-1].y + z[cij+1].y) +
            B * z[cij].y - z1[cij].y;

      if (z1[cij].y > 1.0)
        z1[cij].y = 1.0;
      else if (z1[cij].y < -1.0)
        z1[cij].y = -1.0;
      else
        if (fabs(z1[cij].y) > 0.07)
          z1[cij].y *= (1 - 0.5*deltaTime);

      n[cij].x = z1[cij-MAXSEG].y - z[cij+MAXSEG].y;
      n[cij].y = 2*h;
      n[cij].z = z1[cij-1].y - z[cij+1].y;
      n[cij].SetLength(1.0);
    }
  }

  // Swap buffers
  Vector3 *dz = z;
  z = z1;
  z1 = dz;
}
//---------------------------------------------------------------------------
void BDWaterSurface::OnDraw(BDCamera* Cam) {
  BDAABB wbox(position, fAmp, 1.0, fAmp);
  if (!Cam->BoxInFrustum(wbox))
    return;

  glTranslatef(position.x - fAmp, position.y, position.z - fAmp);

  // glDisable(GL_LIGHTING);
  // glDepthMask(GL_FALSE);

  glColor4f(1.0, 1.0, 1.0, 0.7);

  // If far, lower detail
  if (Cam->GetDist() - size > 50.0) {
    glEnable(GL_BLEND);
    DrawLowDetail();
    glDisable(GL_BLEND);
    return;
  }

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texID);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);

  MSOpenGL::SetTextureMatrixReflection(0.9f);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glDepthMask(GL_FALSE);

  // glPolygonMode(GL_FRONT, GL_LINE);
  // glDisable(GL_CULL_FACE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glVertexPointer(3, GL_DOUBLE, 0, z);
  glNormalPointer(GL_DOUBLE, 0, n);

  // MSOpenGL::glLockArrays(0, MAXSEG*MAXSEG);
  // glDrawElements(GL_TRIANGLES, (MAXSEG-1)*(MAXSEG-1)*6, GL_UNSIGNED_INT, indices);
  for (int i = 0; i < MAXSEG - 1; i++)
    glDrawElements(GL_TRIANGLE_STRIP, MAXSEG*2, GL_UNSIGNED_INT, indices + MAXSEG*i*2);
  // MSOpenGL::glUnlockArrays();

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  // glPolygonMode(GL_FRONT, GL_FILL);

  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDepthMask(GL_TRUE);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  bDrawn = true;
}
//---------------------------------------------------------------------------
void BDWaterSurface::OnCollision(DrawableObject* Object, float deltaTime) {
  if ((fabs(Object->position.x - position.x) < fAmp) &&
      (fabs(Object->position.z - position.z) < fAmp)) {
    int relx = int((Object->position.x - position.x + fAmp) * fPosDiv);
    int rely = int((Object->position.z - position.z + fAmp) * fPosDiv);
    int crelx = relx * MAXSEG + rely;

    if (Object->position.y - Object->size < position.y + z[crelx].y) {
      // Notify object of collision
      BDCollision col(NULL,
                      BDPlane(n[crelx], -position.y),
                      Vector3(Object->position.x, position.y, Object->position.z));
      col.ColType = ObjType;
      if (!Object->Collision(&col))
        return;

      // Splash size, returned by object
      // float splash = col.Size; // Object->velocity.y * 0.2;

      if (Object->velocity.y <= 0.0) {
        // && (Object->position.y  - Object->size <= position.y)) {
        // Slow down downward movement
        Object->velocity.y *= (1 - 5*deltaTime);
        // Nullify gravity
        Object->acceleration.y = 0.0;

        // Move upwards
        // The higher the coefficient of deltaTime, the faster
        // upward movement
        Object->position.y = position.y + z[crelx].y -
            (position.y + z[crelx].y - Object->position.y)/(1 + 3*deltaTime);
      }

      // Create splash if not on edge
      if ((relx != 0) && (rely != 0) && (relx != MAXSEG - 1) &&
          (rely != MAXSEG - 1) && (fabs(col.Size) > 0.2)) {
        z[crelx].y = col.Size;
      }
    }
  }
}
//---------------------------------------------------------------------------
void BDWaterSurface::DrawLowDetail() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texID);
  glBegin(GL_TRIANGLE_STRIP);

  glNormal3f(0.0, 1.0, 0.0);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);

  glTexCoord2f(0.0, 1.0);
  glVertex3f(0.0, 0.0, 2*fAmp);

  glTexCoord2f(1.0, 0.0);
  glVertex3f(2*fAmp, 0.0, 0.0);

  glTexCoord2f(1.0, 1.0);
  glVertex3f(2*fAmp, 0.0, 2*fAmp);

  glEnd();
  glDisable(GL_TEXTURE_2D);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
