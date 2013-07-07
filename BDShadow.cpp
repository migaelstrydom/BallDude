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
#include "BDShadow.h"
#include <map>
#include <iostream>
#include "Plane.h"
const int s_segs = 20;
const double s_length = 20.0;
const double hangle = 2*M_PI/s_segs;
//--------------------------------------------------------------------------
BDShadow::BDShadow()
    : shadow(NULL) {
}
//---------------------------------------------------------------------------
BDShadow::~BDShadow() {
  if (shadow) {
    delete shadow;
    shadow = NULL;
  }
}
//---------------------------------------------------------------------------
struct CEdgeIdx {
  int a, b;

  CEdgeIdx(int ba, int bb) {
    if (ba < bb) {
      a = ba;
      b = bb;
    } else {
      a = bb;
      b = ba;
    }
  }

  bool operator<(const CEdgeIdx& e) const {
    if (e.a == a)
      return b < e.b;

    return a < e.a;
  }
};
//---------------------------------------------------------------------------
void BDShadow::MakeGraph(BDBDMModel* m) {
  using namespace std;

  caster = m;

  map<CEdgeIdx, int> emap;
  // cout << m->NumTris << endl;
  // cout << m->GetIndices()+m->NumTris/3 << endl;
  for (unsigned *idx = m->GetIndices(); idx != m->GetIndices()+m->NumTris*3; idx += 3) {
    // cout << idx << endl;
    for (int i = 0; i < 3; i++) {
      CEdgeIdx ce(idx[i], idx[(i+1)%3]);

      if (emap[ce] == 0) {
        edges.push_back(CEdge(ce.a, ce.b));
        edges.back().triangles.push_back(idx-m->GetIndices());
        emap[ce] = edges.size();
      } else {
        edges[emap[ce]-1].triangles.push_back(idx-m->GetIndices());
      }
    }
  }

  shadow = new BDBDMModel();

  shadow->SetVerts(new Vector3[edges.size()*4], edges.size()*4);

  unsigned *indices = new unsigned[edges.size()*6];
  shadow->SetIndices(indices, edges.size()*2);

  for (unsigned i = 0;  i < edges.size(); i++) {
    indices[0] = 4*i;
    indices[1] = 4*i+1;
    indices[2] = 4*i+2;
    indices[3] = 4*i;
    indices[4] = 4*i+2;
    indices[5] = 4*i+3;
    indices += 6;
  }
}
//---------------------------------------------------------------------------
void BDShadow::CalcShadow(Vector3 lightpos) {
  Vector3 *vidx = shadow->GetVerts();

  for (int i = 0; i < (int)edges.size(); i++) {
    // if (edges[i].triangles.size() == 1)
    {
      int idx = edges[i].triangles[0];
      unsigned *indices = caster->GetIndices();
      Vector3 *verts = caster->GetVerts();

      // true if the triangle is facing the light
      bool cast = BDPlane::ccw(verts[indices[idx]],
                               verts[indices[idx+1]],
                               verts[indices[idx+2]], lightpos);
      if (edges[i].triangles.size() > 1) {
        int idx2 = edges[i].triangles[1];
        unsigned *indices2 = caster->GetIndices();
        Vector3 *verts2 = caster->GetVerts();

        cast ^= BDPlane::ccw(verts2[indices2[idx2]],
                             verts2[indices2[idx2+1]],
                             verts2[indices2[idx2+2]], lightpos);
      }

      if (cast) {
        int other = 0;
        for (; other < 3; other++) {
          if (indices[idx+other] != edges[i].a && indices[idx+other] != edges[i].b) {
            break;
          }
        }

        other = idx + other;
        int a, b;

        if (BDPlane::ccw(verts[edges[i].a], verts[edges[i].b], verts[indices[other]], lightpos)) {
          a = edges[i].a;
          b = edges[i].b;
        } else {
          a = edges[i].b;
          b = edges[i].a;
        }

        *vidx = verts[b];
        vidx++;
        *vidx = verts[a];
        vidx++;
        *vidx = verts[a]-lightpos;
        vidx->SetLength(s_length);
        *vidx += verts[a];
        vidx++;
        *vidx = verts[b]-lightpos;
        vidx->SetLength(s_length);
        *vidx += verts[b];
        vidx++;
      }
    }
    // else  // more than one (two) triangles
    //{

    //}
  }

  shadow->NumTris = (vidx - shadow->GetVerts())/4*2;
}
//---------------------------------------------------------------------------
void BDShadow::InitSphere() {
  shadow = new BDBDMModel();
  Vector3 *verts = new Vector3[s_segs*2];

  shadow->SetVerts(verts, s_segs*2);

  unsigned* indices = new unsigned[s_segs*6];
  shadow->SetIndices(indices, s_segs*2);
}
//---------------------------------------------------------------------------
void BDShadow::SphereShadow(double radius, Vector3 lightpos) {
  Vector3 d = -lightpos;
  double dlen = d.length();
  double q = sqrt(dlen*dlen - radius*radius);
  double cosalpha = q/dlen;
  double c = radius*cosalpha;
  double b = radius*radius/dlen;
  d *= (dlen-b)/dlen;
  dlen = dlen-b;

  /*using namespace std;
    cout << "d: " << dlen << endl;
    cout << "c: " << c << endl;
    cout << "q: " << q << endl;
    cout << "b: " << b << endl;*/

  Vector3 xdir = d ^ Vector3(1.0, 0.0, 0.0);
  if (xdir.SquaredLength() < 10e-4) {
    xdir = d ^ Vector3(0.0, 0.0, 1.0);
  }
  xdir.SetLength(c);
  Vector3 ydir = (d ^ xdir) / (dlen);
  d = lightpos + d;

  /*cout << "d.x = " << d.x << endl;
    cout << "d.y = " << d.y << endl;
    cout << "d.z = " << d.z << endl;

    cout << "xdir.x = " << xdir.x << endl;
    cout << "xdir.y = " << xdir.y << endl;
    cout << "xdir.z = " << xdir.z << endl;

    cout << "ydir.x = " << ydir.x << endl;
    cout << "ydir.y = " << ydir.y << endl;
    cout << "ydir.z = " << ydir.z << endl;*/

  Vector3 *vidx = shadow->GetVerts();
  for (int i = 0; i < s_segs; i++) {
    *vidx = d + (cos(hangle*i)*xdir + sin(hangle*i)*ydir);
    vidx++;
  }

  Vector3 *vprev = shadow->GetVerts();
  for (int i = 0; i < s_segs; i++) {
    *vidx = (*vprev - lightpos)*(s_length/q) + *vprev;
    vidx++;
    vprev++;
  }

  unsigned *iidx = shadow->GetIndices();

  for (int i = 0; i < s_segs; i++) {
    iidx[0] = i;
    iidx[2] = s_segs+i;
    iidx[1] = (i+1)%s_segs;
    iidx += 3;
  }

  for (int i = 0; i < s_segs; i++) {
    iidx[0] = s_segs+i;
    iidx[2] = s_segs+((i+1)%s_segs);
    iidx[1] = (i+1)%s_segs;
    iidx += 3;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
