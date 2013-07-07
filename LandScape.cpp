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
#include <cstring>

#include "LandScape.h"

#include "OpenGLUnit.h"
#include "CollisionObj.h"
#include "OctTree.h"
#include "WaterSurface.h"

#define ZEROLENGTH 0.000000000000000001   // if squaredlength of vec is greater than
// ZEROLENGTH then vec has nonzero magnitude
//---------------------------------------------------------------------------
/*BDLandScape::BDLandScape() {

  }
  //---------------------------------------------------------------------------
  BDLandScape::~BDLandScape() {
  }   */
//---------------------------------------------------------------------------
void BDLandScape::OnDraw(BDCamera* camera) {
  // float fullambient[] = {1.0, 1.0, 1.0, 1.0};

  std::vector<int> drawlist;

  if (BDOctTree::Nodes.size() == 1)
    drawlist.push_back(0);
  else
    drawlist = BDOctTree::NodeVisible(&BDOctTree::Nodes[0], camera);

  Triangle *lastTri = &tris[numtris];
  for (Triangle* index = tris; index != lastTri; index++) {
    index->drawn = false;
  }

  unsigned int *currIndex = indices;
  for (std::vector<int>::iterator i = drawlist.begin(); i != drawlist.end(); i++) {
    for (std::vector<Triangle*>::iterator index = BDOctTree::Nodes[*i].tris.begin();
         index != BDOctTree::Nodes[*i].tris.end(); index++) {
      if (!(*index)->drawn) {
        (*index)->drawn = true;
        *currIndex = (*index)->a - verts;
        currIndex++;
        *currIndex = (*index)->b - verts;
        currIndex++;
        *currIndex = (*index)->c - verts;
        currIndex++;
      }
    }
  }

  if (bTexture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texcos);
  }

  glDisable(GL_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  // glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_DOUBLE, 0, verts);
  glColorPointer(3, GL_DOUBLE, 0, cols);
  // glNormalPointer(GL_DOUBLE, 0, norms);
  // glDrawArrays(GL_TRIANGLES, 0, numverts);
  glLockArrays(0, numverts);
  glDrawElements(GL_TRIANGLES, currIndex-indices, GL_UNSIGNED_INT, indices);
  glUnlockArrays();
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  // glDisableClientState(GL_NORMAL_ARRAY);
  glEnable(GL_LIGHTING);

  if (bTexture) {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }


  /*glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, verts);
    glColorPointer(3, GL_DOUBLE, 0, cols);
    glNormalPointer(GL_DOUBLE, 0, norms);
    // MSOpenGL::glLockArrays(0, numdrawn);
    glDrawArrays(GL_TRIANGLES, 0, numdrawn);
    // MSOpenGL::glUnlockArrays();
    glDisableClientState(GL_NORMAL_ARRAY);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexCoordPointer(2, GL_FLOAT, 0, texcos - (textriindex)*2);

    glDrawArrays(GL_TRIANGLES, textriindex, numtexdrawn - textriindex);
    glDisable(GL_TEXTURE_2D);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // for (int i = 0; i < 1000000000; i++);
    // TEMP

    // int d;
    /*if (nl.size()) {
    d = nl[0];
    nl.clear();
    nl.push_back(d);
    for (int i = 0; i < 6; i++)
    nl.push_back(BDOctTree::Nodes[d].neighbour[i]);
    }  */
  // glColor3f(1.0, 1.0, 0.0);
  // BDOctTree::DrawNodes(nl);

  // glColor3f(0.0, 0.0, 1.0);
  // BDOctTree::DrawNodes(drawlist);

  // drawbox.Draw();

  // END TEMP

  // glPopAttrib();
}
//---------------------------------------------------------------------------
void BDLandScape::OnCollision(DrawableObject* Object, float deltaTime) {
  if (Object->velocity.SquaredLength() < ZEROLENGTH)
    return;

  std::vector<Triangle*> pclist; // Potential Collider list
  std::vector<Triangle*>::iterator tidx;

  Vector3 dumdist = Object->velocity * deltaTime * 0.5;

  BDAABB bbox(Object->position + dumdist,
              fabs(dumdist.x) + Object->size,
              fabs(dumdist.y) + Object->size,
              fabs(dumdist.z) + Object->size);

  std::vector<int> nodelist = BDOctTree::BoxInNode(&BDOctTree::Nodes[0], &bbox);

  // TEMP
  // if (Object->size == 0.5) nl = nodelist;
  // END TEMP

  for (std::vector<int>::iterator nidx = nodelist.begin(); nidx != nodelist.end(); nidx++) {
    for (tidx = BDOctTree::Nodes[*nidx].tris.begin(); tidx != BDOctTree::Nodes[*nidx].tris.end(); tidx++) {
      if (!(*tidx)->pclist) {
        if (bbox.Intersect(*(*tidx)))
          pclist.push_back(*tidx);
        (*tidx)->pclist = true;
      }
    }
  }

  // RESET PCLISTS OF TRIS
  for (std::vector<int>::iterator nidx = nodelist.begin(); nidx != nodelist.end(); nidx++) {
    for (tidx = BDOctTree::Nodes[*nidx].tris.begin();
         tidx != BDOctTree::Nodes[*nidx].tris.end(); tidx++) {
      (*tidx)->pclist = false;
    }
  }

  if (pclist.size() == 0)
    return;

  std::vector<BDPlane> cPlanes;
  std::vector<Triangle*> cSPlanes;
  int numcollide = 0;
  ColType col;

  for (tidx = pclist.begin(); tidx != pclist.end(); tidx++) {
    col = (*tidx)->CollidesWithObject(deltaTime, Object);
    if (col == TRIANGLE) {
      Object->velocity += (Object->velocity * (-(*tidx)->cCollision.Plane.N)) *
          (*tidx)->cCollision.Plane.N;
      cPlanes.push_back(BDPlane((*tidx)->cCollision.Plane.N,
                                (*tidx)->cCollision.Plane.D - Object->size));
      numcollide++;
    } else if (col == SPLANE) {
      cSPlanes.push_back(*tidx);
    }
  }

  if (Object->velocity.SquaredLength() > ZEROLENGTH)
    for (tidx = cSPlanes.begin(); tidx != cSPlanes.end(); tidx++) {
      if ((*tidx)->CollidesWithSplane(deltaTime, Object) == SPLANE) {
        Object->velocity += (Object->velocity * (-(*tidx)->cCollision.Plane.N)) *
            (*tidx)->cCollision.Plane.N;
        cPlanes.push_back(BDPlane((*tidx)->cCollision.Plane.N,
                                  (*tidx)->cCollision.Plane.D - Object->size));
        numcollide++;
      }
    }

  if (numcollide == 0)
    return;

  Object->position = cPlanes[0].RayIntersection(Object->position, cPlanes[0].N);

  if (numcollide == 1)
    return;

  numcollide = cPlanes.size();
  if ((cPlanes[0].N - cPlanes[1].N).SquaredLength() > 0.001) {
    Object->position = cPlanes[1].RayIntersection(Object->position,
                                                  cPlanes[0].N ^ (cPlanes[1].N ^ cPlanes[0].N));
  } else
    return;

  if (numcollide == 2)
    return;

  if (((cPlanes[2].N - cPlanes[0].N).SquaredLength() > 0.001) &&
      ((cPlanes[2].N - cPlanes[1].N).SquaredLength() > 0.001)) {
    Object->position = cPlanes[2].RayIntersection(Object->position,
                                                  (cPlanes[1].N ^ cPlanes[0].N));
  }
}
//---------------------------------------------------------------------------
void BDLandScape::LoadLandScape(BDFile* File, unsigned tID) {
  texID = tID;

  unsigned int index, dummy;

  bTexture = false;

  if (!File->GetSizes("VertData")) {
    numverts = File->GetSized("Points") / 3;
    numtris = File->GetSizei("Triangles") / 3;
  } else {
    numverts = File->GetDatai("NumPoints");
    numtris = File->GetDatai("NumTris");
  }

  tris = new Triangle[numtris];

  verts = new Vector3[numverts];
  cols = new Vector3[numverts];
  // norms = new Vector3[numverts];
  indices = new unsigned int[numtris*3];
  texcos = new float[2*numverts];

  if (!File->GetSizes("VertData")) {
    for (index = 0; index < numverts; index++) {
      verts[index].x = File->GetDatad("Points", 3*index);
      verts[index].y = File->GetDatad("Points", 3*index+1);
      verts[index].z = File->GetDatad("Points", 3*index+2);
    }

    if (File->GetSized("Colors") == numverts*3) {
      for (index = 0; index < numverts; index++) {
        cols[index].x = File->GetDatad("Colors", 3*index);
        cols[index].y = File->GetDatad("Colors", 3*index+1);
        cols[index].z = File->GetDatad("Colors", 3*index+2);
      }
    } else {
      if (File->GetSized("Colors") == 0) {
        for (index = 0; index < numverts; index++) {
          cols[index].x = 1.0;
          cols[index].y = 1.0;
          cols[index].z = 1.0;
        }
      } else {
        for (index = 0; index < numverts; index++) {
          cols[index].x = File->GetDatad("Colors", 0);
          cols[index].y = File->GetDatad("Colors", 1);
          cols[index].z = File->GetDatad("Colors", 2);
        }
      }
    }

    Triangle *tridx = tris;
    for (index = 0; index < numtris; index++) {
      *tridx = Triangle(&verts[File->GetDatai("Triangles", 3*index)],
                        &verts[File->GetDatai("Triangles", 3*index+1)],
                        &verts[File->GetDatai("Triangles", 3*index+2)]);

      tridx->friction = File->GetDatad("Friction");

      tridx++;
    }

    memset(texcos, 0, sizeof(float) * 2 * numverts);
    bTexture = (File->GetSizei("TriTexs") > 0);
    for (index = 0; index < File->GetSizei("TriTexs"); index += 4) {
      dummy = 2*(tris[File->GetDatai("TriTexs", index)].a - verts);
      texcos[dummy] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+1));
      texcos[dummy+1] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+1)+1);

      dummy = 2*(tris[File->GetDatai("TriTexs", index)].b - verts);
      texcos[dummy] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+2));
      texcos[dummy+1] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+2)+1);

      dummy = 2*(tris[File->GetDatai("TriTexs", index)].c - verts);
      texcos[dummy] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+3));
      texcos[dummy+1] = File->GetDatad("TexCoords", 2*File->GetDatai("TriTexs", index+3)+1);
    }
  } else {
    File->LoadVertData(File->GetDatas("VertData"), &verts->x, &cols->x, texcos, indices,
                       numverts, numtris);

    bTexture = true;

    for (index = 0; index < numtris; index++) {
      tris[index] = Triangle(&verts[indices[3*index]],
                             &verts[indices[3*index+1]],
                             &verts[indices[3*index+2]]);

      tris[index].friction = File->GetDatad("Friction");
    }
  }

  // SaveVertData();

  BDOctTree::MinTris = File->GetDatai("MinTrisPerNode");

  BDOctTree::MinNodeSize = File->GetDatad("MinNodeSize");

  BDOctTree::CreateOctTree(tris, numtris);

  size = (BDOctTree::Nodes[0].x2 - BDOctTree::Nodes[0].x1) * 1.73205080756887;
  // sqrt(12) * rootsize
}
//---------------------------------------------------------------------------
void BDLandScape::Unload() {
  BDOctTree::DeleteOctTree();

  delete[] tris;

  // VArray
  delete[] verts;
  delete[] cols;
  delete[] texcos;
  // delete[] norms;
  delete[] indices;
}
//---------------------------------------------------------------------------
void BDLandScape::WaterCollision(BDWaterSurface* ws) {
  BDAABB wbox(ws->position, ws->fAmp, 1.0, ws->fAmp);
  std::vector<int> nodelist = BDOctTree::BoxInNode(&BDOctTree::Nodes[0], &wbox);

  std::vector<int>::iterator nidx;
  std::vector<Triangle*>::iterator tidx;
  bool bValid, bAbove;
  Vector3 a, b1, b2, i1, i2;
  BDPlane wp(Vector3(0.0, 1.0, 0.0), -ws->position.y);

  // Check collisions
  for (nidx = nodelist.begin(); nidx != nodelist.end(); nidx++) {
    for (tidx = BDOctTree::Nodes[*nidx].tris.begin();
         tidx != BDOctTree::Nodes[*nidx].tris.end(); tidx++) {
      bValid = false;
      if (!(*tidx)->pclist) {
        bAbove = ((*tidx)->a->y > ws->position.y);

        if (((*tidx)->b->y < ws->position.y) && (bAbove)) {
          a = *(*tidx)->b;
          b1 = *(*tidx)->a;
          b2 = *(*tidx)->c;
          bValid = true;
        }

        if (((*tidx)->c->y < ws->position.y) && (bAbove)) {
          if (bValid) {
            a = *(*tidx)->a;
            b1 = *(*tidx)->b;
            b2 = *(*tidx)->c;
          } else {
            a = *(*tidx)->c;
            b1 = *(*tidx)->a;
            b2 = *(*tidx)->b;
            bValid = true;
          }
        }

        if (((*tidx)->b->y > ws->position.y) && (!bAbove)) {
          a = *(*tidx)->b;
          b1 = *(*tidx)->a;
          b2 = *(*tidx)->c;
          bValid = true;
        }

        if (((*tidx)->c->y > ws->position.y) && (!bAbove)) {
          if (bValid) {
            a = *(*tidx)->a;
            b1 = *(*tidx)->b;
            b2 = *(*tidx)->c;
          } else {
            a = *(*tidx)->c;
            b1 = *(*tidx)->a;
            b2 = *(*tidx)->b;
            bValid = true;
          }
        }

        (*tidx)->pclist = true;
      }

      if (bValid) {
        i1 = wp.RayIntersection(a, b1-a);
        i2 = wp.RayIntersection(a, b2-a);

        int relx1 = int((i1.x - ws->position.x + ws->fAmp) / ws->h);
        int rely1 = int((i1.z - ws->position.z + ws->fAmp) / ws->h);

        int relx2 = int((i2.x - ws->position.x + ws->fAmp) / ws->h);
        int rely2 = int((i2.z - ws->position.z + ws->fAmp) / ws->h);

        CollisionLine(relx1, rely1, relx2, rely2, ws->d, ws->MAXSEG);
      }
    }
  }

  // RESET PCLISTS OF TRIS
  for (nidx = nodelist.begin(); nidx != nodelist.end(); nidx++) {
    for (tidx = BDOctTree::Nodes[*nidx].tris.begin();
         tidx != BDOctTree::Nodes[*nidx].tris.end(); tidx++) {
      (*tidx)->pclist = false;
    }
  }
}
//---------------------------------------------------------------------------
void BDLandScape::CollisionLine(int x1, int y1, int x2, int y2, bool* d,
                                const int ci) {
  int i, sdx, sdy, dxabs, dyabs, x, y, px, py;

  dxabs = abs(x2 - x1);
  dyabs = abs(y2 - y1);
  sdx = (((x2 - x1) < 0) ?- 1 : (((x2 - x1) > 0) ?1 : 0));
  sdy = (((y2 - y1) < 0) ?- 1 : (((y2 - y1) > 0) ?1 : 0));
  x = dyabs >> 1;
  y = dxabs >> 1;
  px = x1;
  py = y1;

  if (dxabs >= dyabs) {
    for (i = 0; i < dxabs; i++) {
      y += dyabs;

      if (y >= dxabs) {
        y -= dxabs;
        py += sdy;
      }

      px += sdx;

      if ((px > 0) && (px < ci-1) && (py > 0) && (py < ci-1))
        d[ci*px + py] = false;
    }

  } else {
    for (i = 0; i < dyabs; i++) {
      x += dxabs;

      if (x >= dyabs) {
        x -= dyabs;
        px += sdx;
      }

      py += sdy;

      if ((px > 0) && (px < ci-1) && (py > 0) && (py < ci-1))
        d[ci*px + py] = false;
    }
  }
}
//---------------------------------------------------------------------------
/*void BDLandScape::SaveVertData() {
  FILE* File;
  if ((File = fopen("Data\\TestingWorld.vd", "wb")) == NULL) {
  throw "Unable to open file: BDFile::LoadVertData";
  }

  fwrite(verts, sizeof(Vector3), numverts, File);

  fwrite(cols, sizeof(Vector3), numverts, File);

  fwrite(texcos, sizeof(float)*2, numverts, File);

  fwrite(indices, sizeof(unsigned int)*3, numtris, File);

  fclose(File);
  }       */
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

