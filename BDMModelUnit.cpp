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
//--------------------------------------------------------------------------
#include "BDMModelUnit.h"
#include "LoadDataUnit.h"
#include "GLExtensionsUnit.h"
#include <fstream>
//--------------------------------------------------------------------------
BDBDMModel::BDBDMModel() {
  bLoaded = false;
  pVerts = NULL;
  pTexCos = NULL;
  pIndices = NULL;
  NumVerts = 0;
  NumTris = 0;
  NumFrames = 0;
}
//---------------------------------------------------------------------------
BDBDMModel::~BDBDMModel() {
  Unload();
}
//---------------------------------------------------------------------------
bool BDBDMModel::Load(std::string mname) {
  if (bLoaded)
    return true;

  std::string filename = std::string("Models/") + mname + std::string(".txt");

  BDFile File;
  File.ReadData(filename.c_str());
  NumVerts = File.GetDatai("NumVerts");
  NumTris = File.GetDatai("NumTris");
  NumFrames = File.GetDatai("NumFrames");
  Dimensions.x = File.GetDatad("Extremities", 0);
  Dimensions.y = File.GetDatad("Extremities", 1);
  Dimensions.z = File.GetDatad("Extremities", 2);

  pVerts = new Vector3[NumVerts*(NumFrames+1)];
  pTexCos = new float[NumVerts*2];
  pIndices = new unsigned int[NumTris*3];

  filename = std::string("Data/") + std::string("Models/") + mname + std::string(".bdm");

  FILE *f = fopen(filename.c_str(), "rb");
  rewind(f);

  fread(&pVerts[NumVerts], sizeof(Vector3), NumVerts*NumFrames, f);
  fread(pTexCos, sizeof(float)*2, NumVerts, f);
  fread(pIndices, sizeof(int)*3, NumTris, f);

  fclose(f);

  bLoaded = true;
  return true;
}
//---------------------------------------------------------------------------
bool BDBDMModel::Unload() {
  if (!bLoaded)
    return true;

  if (pVerts) {
    delete[] pVerts;
    pVerts = NULL;
  }
  if (pTexCos) {
    delete[] pTexCos;
    pTexCos = NULL;
  }
  if (pIndices) {
    delete[] pIndices;
    pIndices = NULL;
  }

  bLoaded = false;
  return true;
}
//---------------------------------------------------------------------------
void BDBDMModel::MakeSphere(int stacks, int slices, double radius) {
  if (bLoaded)
    return;

  NumVerts = slices*(stacks-1) + 2;
  NumTris = 2*slices + (stacks-2)*slices*2;
  NumFrames = 0;
  bLoaded = true;

  double vangle = M_PI/(stacks);
  double hangle = 2*M_PI/slices;

  pVerts = new Vector3[NumVerts];
  pIndices = new unsigned int[NumTris*3];
  pTexCos = new float[NumVerts*2];

  pVerts[0].x = 0.0;
  pVerts[0].y = radius;
  pVerts[0].z = 0.0;

  pTexCos[0] = 0.0;
  pTexCos[1] = 0.0;

  Vector3 *iter = pVerts + 1;
  float *tci = pTexCos+2;
  for (int i = 1; i < stacks; i++) {
    double yval = radius*cos(i*vangle);
    double hrad = radius*sin(i*vangle);
    for (int j = 0; j < slices; j++) {
      iter->x = radius*sin(j*hangle)*hrad;
      iter->y = yval;
      iter->z = radius*cos(j*hangle)*hrad;
      iter++;

      *tci = float(j)/slices;
      tci++;
      *tci = float(i)/stacks;
      tci++;
    }
  }
  iter->x = 0.0;
  iter->y = -radius;
  iter->z = 0.0;

  *tci = 0.0;
  tci++;
  *tci = 1.0;

  unsigned int *iidx = pIndices;
  for (unsigned int i = 0; i < (unsigned int)slices; i++) {
    iidx[0] = 0;
    iidx[1] = 1+i;
    iidx[2] = 1+((i+1)%slices);
    iidx += 3;
  }

  for (int i = 0; i < stacks-2; i++) {
    for (int j = 0; j < slices; j++) {
      iidx[0] = 1+i*slices+j;
      iidx[1] = 1+(i+1)*slices+j;
      iidx[2] = 1+(i+1)*slices+((j+1)%slices);
      iidx += 3;

      iidx[0] = 1+(i+1)*slices+((j+1)%slices);
      iidx[1] = 1+i*slices+((j+1)%slices);
      iidx[2] = 1+i*slices+j;
      iidx += 3;
    }
  }

  for (unsigned int i = 0; i < (unsigned int)slices; i++) {
    iidx[0] = NumVerts-1;
    iidx[1] = NumVerts-2-i;
    iidx[2] = NumVerts-2-((i+1)%slices);
    iidx += 3;
  }
}
//---------------------------------------------------------------------------
void BDBDMModel::StaticDraw() {
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_DOUBLE, 0, pVerts);

  if (pTexCos) {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, pTexCos);
  }

  glLockArrays(0, NumVerts);

  glDrawElements(GL_TRIANGLES, NumTris*3, GL_UNSIGNED_INT, pIndices);

  glUnlockArrays();
  glDisableClientState(GL_VERTEX_ARRAY);
  if (pTexCos) {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
