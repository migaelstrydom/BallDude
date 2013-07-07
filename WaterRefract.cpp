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
#include "WaterRefract.h"
#include "AABB.h"
#include "OpenGLUnit.h"
//#define DEPTH 10.0
//#define WALLINVDEPTH TEXWALL/DEPTH
#define TEXFLOOR 0.5
#define TEXWALL 0.25
//---------------------------------------------------------------------------
BDWaterRefract::BDWaterRefract(Vector3 pos, const float xamp, const float deep,
                               int t1, int t2, const int ms)
    : BDWaterSurface(pos, xamp, t1, ms), DEPTH(deep), WALLINVDEPTH(TEXWALL/deep), texID2(t2) {
  tc = new TexCoord[MAXSEG*MAXSEG];

  fPlane = BDPlane(Vector3(0.0, 1.0, 0.0), -position.y + DEPTH);
  lPlane = BDPlane(Vector3(1.0, 0.0, 0.0), position + Vector3(-fAmp, 0.0, 0.0));
  rPlane = BDPlane(Vector3(-1.0, 0.0, 0.0), position + Vector3(fAmp, 0.0, 0.0));
  tPlane = BDPlane(Vector3(0.0, 0.0, 1.0), position + Vector3(0.0, 0.0, -fAmp));
  bPlane = BDPlane(Vector3(0.0, 0.0, -1.0), position + Vector3(0.0, 0.0, fAmp));

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, t1);

  glActiveTextureARB(GL_TEXTURE0_ARB);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
  // glBindTexture(GL_TEXTURE_2D, t2);
}
//---------------------------------------------------------------------------
BDWaterRefract::~BDWaterRefract() {
  delete[] tc;
}
//---------------------------------------------------------------------------
void BDWaterRefract::OnDraw(BDCamera* Cam) {
  BDAABB wbox(position, fAmp, 1.0, fAmp);
  if (!Cam->BoxInFrustum(wbox))
    return;

  // unit vector of incidence dot surface normal (0,1,0)
  // float fFresnel = -Cam->GetDir().y;//(Cam->Position - position).y/Cam->GetDist();
  // fFresnel = 1 - fFresnel*fFresnel;
  glColor4f(0.5, 0.5, 0.5, 0.8);// fFresnel);//
  glDisable(GL_LIGHTING);

  // If far, lower detail
  if (Cam->GetDist() - size > 75.0) {
    glTranslatef(position.x - fAmp, position.y, position.z - fAmp);

    DrawLowDetail();

    glEnable(GL_LIGHTING);

    return;
  }

  // Initialise variables for Refraction
  Vector3 tl = position;
  tl.x -= fAmp;
  tl.z -= fAmp;
  tl.y -= DEPTH;

  // Used for calculations
  Vector3 in, ray, vertex;
  float dot, tmp;
  Vector3 *pz = &z[MAXSEG*MAXSEG];
  Vector3 *pn = &n[MAXSEG*MAXSEG];
  TexCoord *ptc = &tc[MAXSEG*MAXSEG];
  const float tdiv(0.5/fAmp);
  bool bMiddle;

  // Calculate refraction   // FPS = 210
  while(pz != z) {
    pz--; pn--; ptc--;
    vertex = *pz + tl;
    vertex.y += DEPTH;
    ray = (Cam->Position - vertex).WithLength(1.0);

    dot = ray * (*pn);
    tmp = 1.0f - ((0.75f*0.75f) * (1.0f-(dot*dot)));
    tmp = (float)sqrt(tmp);

    tmp = (0.75f*dot) - tmp;

    ray = (tmp*(*pn)) - (0.75f*ray);

    in = fPlane.RayIntersection(vertex, ray);
    bMiddle = true;

    if (in.z < tl.z) {
      in = tPlane.RayIntersection(vertex, ray);

      if ((in.x > tl.x) && (in.x < position.x + fAmp)) {
        in -= tl;
        in.y -= DEPTH;
        in.z = -in.y * WALLINVDEPTH;
        in.x *= tdiv;
        in.x *= -2.0*in.z + 1.0;
        in.x += in.z;
        bMiddle = false;
      }
    } else if (in.z > position.z + fAmp) {
      in = bPlane.RayIntersection(vertex, ray);

      if ((in.x > tl.x) && (in.x < position.x + fAmp)) {
        in -= position;
        in.x += fAmp;
        in.z -= fAmp;

        in.z = 1.0 + in.y*WALLINVDEPTH;
        in.x *= tdiv;
        in.x *= 2.0*in.z - 1.0;
        in.x += 1.0 - in.z;
        bMiddle = false;
      }
    }

    if ((in.x < tl.x) && bMiddle) {
      in = lPlane.RayIntersection(vertex, ray) - tl;
      in.y -= DEPTH;

      in.x = -in.y*WALLINVDEPTH;
      in.z *= tdiv;
      in.z *= -2.0*in.x + 1.0;
      in.z += in.x;
      bMiddle = false;
    } else if ((in.x > position.x + fAmp) && bMiddle) {
      in = rPlane.RayIntersection(vertex, ray) - position;
      in.x -= fAmp;
      in.z += fAmp;

      in.x = 1.0 + in.y*WALLINVDEPTH;
      in.z *= tdiv;
      in.z *= 2.0*in.x - 1.0;
      in.z += 1.0 - in.x;
      bMiddle = false;
    }

    if (bMiddle) {
      in -= tl;
      in *= tdiv;
      in *= TEXFLOOR;
      in.x += TEXWALL;
      in.z += TEXWALL;
    }

    ptc->x = in.x;
    ptc->y = in.z;
  }

  glEnable(GL_BLEND);
  glTranslatef(position.x - fAmp, position.y, position.z - fAmp);
  glDepthMask(GL_FALSE);

  // Texture reflection
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, texID2);
  glEnable(GL_TEXTURE_2D);
  MSOpenGL::SetTextureMatrixReflection(0.9f);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PRIMARY_COLOR_EXT);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f);

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR_EXT);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1.0f);


  // Texture refraction
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glClientActiveTextureARB(GL_TEXTURE1_ARB);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, tc);

  /*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1.0f);

    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_ONE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1.0f); */

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glVertexPointer(3, GL_DOUBLE, 0, z);
  glNormalPointer(GL_DOUBLE, 0, n);


  // glLockArrays(0, MAXSEG*MAXSEG);
  // glDrawElements(GL_TRIANGLES, (MAXSEG-1)*(MAXSEG-1)*6, GL_UNSIGNED_INT, indices);
  for (int i = 0; i < MAXSEG - 1; i++)
    glDrawElements(GL_TRIANGLE_STRIP, MAXSEG*2, GL_UNSIGNED_INT, indices + MAXSEG*i*2);
  // glUnlockArrays();

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);


  glActiveTextureARB(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);

  glActiveTextureARB(GL_TEXTURE0_ARB);
  // Clear texture matrix
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);

  glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
  bDrawn = true;
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
//---------------------------------------------------------------------------
