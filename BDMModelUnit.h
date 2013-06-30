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
#ifndef BDMModelUnitH
#define BDMModelUnitH
//---------------------------------------------------------------------------
#include "Vector.h"
#include <string>
//---------------------------------------------------------------------------
class BDBDMModel {
 private:
  bool bLoaded;
  Vector3 *pVerts;
  float *pTexCos;
  unsigned int *pIndices;

 public:
  Vector3 Dimensions;
  unsigned int NumVerts;
  unsigned int NumTris;
  unsigned int NumFrames;
  BDBDMModel();
  ~BDBDMModel();

  bool Load(std::string);
  bool Unload();

  void MakeSphere(int, int, double);

  void StaticDraw();

  inline Vector3* GetVerts(unsigned, unsigned, float);
  Vector3* GetVerts() {return pVerts; }
  void SetVerts(Vector3 *v, int nv) {pVerts = v; NumVerts = nv;}
  float *GetTexCos() { return pTexCos; }
  unsigned *GetIndices() { return pIndices; }
  void SetIndices(unsigned* i, int nt) {pIndices = i; NumTris = nt; bLoaded = true;}
};
//---------------------------------------------------------------------------
Vector3* BDBDMModel::GetVerts(unsigned cframe, unsigned nframe, float ft) {
  Vector3 *cf = &pVerts[(cframe+1)*NumVerts];
  Vector3 *nf = &pVerts[(nframe+1)*NumVerts];

  // idx
  Vector3 *idx = pVerts;
  Vector3 *lastFrame = &pVerts[NumVerts];

  while(idx != lastFrame) {
    (*idx) = (*cf) + ((*nf) - (*cf))*ft;
    idx++; cf++; nf++;
  }

  return pVerts;
}
//---------------------------------------------------------------------------
#endif
