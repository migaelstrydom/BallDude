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
#include <fstream>
#include "LoadDataUnit.h"
#include <assert.h>
#include <GL/gl.h>
#include "BDError.h"
#include <cstdlib> // atof
// temp
#include <iostream>
using namespace std;
//---------------------------------------------------------------------------
BDFile::BDFile() {
  // Create();
}
//---------------------------------------------------------------------------
BDFile::~BDFile() {
  // Destroy();
}
//---------------------------------------------------------------------------
void BDFile::Create() {
}
//---------------------------------------------------------------------------
void BDFile::Destroy() {
}
//---------------------------------------------------------------------------
// ReadData reads data from file filename
void BDFile::ReadData(const char* filename) {
  string f = "Data/" + string(filename);

  FILE* File;
  if ((File = fopen(f.c_str(), "r")) == 0) {
    throw BDError(string("Unable to open file: BDFile::ReadInputData : ")+f);
  }

  fseek(File, 0, SEEK_END);
  const long length = ftell(File);

  if (length == -1L)
    throw BDError("Unable to get length of file!");

  char *filedata = new char[length + 1];

  rewind(File);

  int pindex = 0;
  while(!feof(File)) {
    fread(&filedata[pindex++], 1, 1, File);
  }

  fclose(File);

  filedata[length] = '\n';

  // TEMP
  /*for (int i = 0; i < length; i++) {
    cout << filedata[i];
    }*/
  // END TEMP

  pindex = 0;
  std::string line = "";
  bool bQuote = false;  // Spaces should not be removed when in a quote.

  while(pindex < length) {
    // skip over comments
    if (((filedata[pindex] == '/') && (filedata[pindex + 1] == '/')) ||
        (filedata[pindex] == '#')) {
      while(filedata[pindex] != '\n')
        pindex++;

      pindex++;
    } else {
      if (filedata[pindex] == ';') {
        CheckLine(line);
        line = "";
      } else {
        if ((line[0] == 's') && (filedata[pindex] == '"'))
          bQuote = !bQuote;

        if (bQuote)
          line += filedata[pindex];
        else
          // if ((filedata[pindex] != ' ') && (filedata[pindex] != '\n') &&
          //   (filedata[pindex] != '\t'))
          //    line += filedata[pindex];
          if (!isspace(filedata[pindex]))
            line += filedata[pindex];
      }
      pindex++;
    }
  }

  delete[] filedata;

  /*string s = "";
    while(s != "x") {
    cout << "Choose: ";
    cin >> s;

    cout << s << ": " << dData[s].size() << endl;
    }*/
}
//---------------------------------------------------------------------------
void BDFile::CheckLine(std::string line) {
  if (line.empty())
    return;

  unsigned int pindex = 0;

  bool HasData = false;    // DOES THE LINE HAVE PROPER DATA?

  // DOUBLE CHECK TO MAKE SURE THAT THIS LINE HAS PROPER INFO
  while(pindex < line.size()) {
    if (line[pindex] == '=') {
      HasData = true;
      break;
    }
    pindex++;
  }
  if (!HasData) {
    return;
  }

  string Name = "";

  char type = line[0];
  pindex = 1;

  while(line[pindex] != '=') {
    Name += line[pindex];
    pindex++;
  }
  if (Name == "PointColors") {
    int index = 0;
    index++;
  }

  pindex++;

  if (type == 'd')
    while(pindex < line.size()) {
      dData[Name.c_str()].push_back(atof(line.substr(pindex, line.size() - pindex).c_str()));
      for (;;) {
        pindex++;
        if (line[pindex] == ',') {
          pindex++;

          break;
        }
        if (pindex >= (line.size() - 1)) {
          return;
        }
      }
    } else if (type == 'i')
    while(pindex < line.size()) {
      iData[Name.c_str()].push_back(atoi(line.substr(pindex, line.size() - pindex).c_str()));
      for (;;) {
        pindex++;
        if (line[pindex] == ',') {
          pindex++;

          break;
        }
        if (pindex >= (line.size() - 1)) {
          return;
        }
      }
    } else if (type == 's') {
    std::string val = "";
    while(pindex < line.size()) {
      pindex++;
      while(line[pindex] != '\"') {
        val += line[pindex];
        pindex++;
      }

      sData[Name.c_str()].push_back(val.c_str());
      val = "";
      for (;;) {
        pindex++;
        if (line[pindex] == ',') {
          pindex++;

          break;
        }
        if (pindex >= (line.size() - 1)) {
          return;
        }
      }
    }
  } else if (type == 'b')
    while(pindex < line.size()) {
      if ((line[pindex] == 't') || (line[pindex] == 'T'))
        bData[Name.c_str()].push_back(true);
      else
        bData[Name.c_str()].push_back(false);

      for (;;) {
        pindex++;
        if (line[pindex] == ',') {
          pindex++;

          break;
        }
        if (pindex >= (line.size() - 1)) {
          return;
        }
      }
    }
}
//---------------------------------------------------------------------------
/*void BDFile::LoadMesh(char* filename, int listnum) {
  char filedata[1500];
  char tempdata[2] = {'\0', '\0'};

  string f = "Data\\" + string(filename);

  FILE* File;
  if ((File = fopen(f.c_str(), "r")) == 0) {
  assert(!"Error opening file in BDFile::LoadMesh");
  }

  while(!feof(File)) {
  fread(tempdata, 1, 1, File);
  strcat(filedata, tempdata);
  }

  fclose(File);

  vertices = ReadMeshVertices("Vertices", filedata);
  facedata = ReadMeshFaceData("FaceData", filedata);
  CreateMeshAsDisList(listnum);

  delete vertices;
  delete facedata;
  }
  //---------------------------------------------------------------------------
  BDFile::Vertex* BDFile::ReadMeshVertices(char* whichdat, char* src) {
  int pointerindex;
  int NumItems = atoi(&src[strlen(src) - strlen(strstr(src, whichdat)) + strlen(whichdat) + 1]);

  Vertex* data = new Vertex[NumItems];

  pointerindex = strlen(src) - strlen(strstr((&src[strlen(src) - strlen(strstr(src, whichdat) + strlen(whichdat) + 1)]), "\n")) + 1;
  for (int index = 0; index < NumItems; index++) {
  for (short i = 0; i < 3; i++) {
  if (i == 0) data[index].X = atof(&src[pointerindex]);
  if (i == 1) data[index].Y = atof(&src[pointerindex]);
  if (i == 2) data[index].Z = atof(&src[pointerindex]);

  while(1) {
  pointerindex++;
  if (src[pointerindex] == ',') {
  pointerindex += 2;
  break;
  }
  }
  }
  }

  return data;
  }
  //---------------------------------------------------------------------------
  int* BDFile::ReadMeshFaceData(char* whichdat, char* src) {
  int index;
  int pointerindex;
  int NumItems = atoi(&src[strlen(src) - strlen(strstr(src, whichdat)) + strlen(whichdat) + 1]);

  int* data = new int[NumItems];

  pointerindex = strlen(src) - strlen(strstr((&src[strlen(src) - strlen(strstr(src, whichdat) + strlen(whichdat) + 1)]), "\n")) + 1;
  for (index = 0; index < NumItems; index++) {
  data[index] = atoi(&src[pointerindex]);

  while(1) {
  pointerindex++;
  if (src[pointerindex] == ',') {
  pointerindex += 2;
  break;
  }
  }
  }

  return data;
  }
  //---------------------------------------------------------------------------
  /*void BDFile::CreateMeshAsDisList(int listnum) {
  int faceindex = 0;
  int index;

  glNewList(listnum, GL_COMPILE);
  do
  {
  glNormal3dv(CalcNormal(vertices[facedata[faceindex + 1]],
  vertices[facedata[faceindex + 2]],
  vertices[facedata[faceindex + 3]]));

  glBegin(GL_POLYGON);
  for (index = 0; index < facedata[faceindex]; index++) {
  glVertex3f(vertices[facedata[faceindex + 1 + index]].X,
  vertices[facedata[faceindex + 1 + index]].Y,
  vertices[facedata[faceindex + 1 + index]].Z);
  }
  glEnd();
  faceindex += facedata[faceindex] + 1;
  }while(facedata[faceindex] != 0);
  glEndList();
  }      */
//---------------------------------------------------------------------------
void BDFile::LoadOBJ(const char* filename) {
  std::string f = "Data\\" + std::string(filename) + std::string(".obj");

  FILE* File;
  if ((File = fopen(f.c_str(), "r")) == 0) {
    assert(!"Unable to open file: BDFile::LoadOBJ");
  }

  rewind(File);

  std::string line = "";
  char c;

  while(!feof(File)) {
    fread(&c, 1, 1, File);
    if (c == '\n') {
      OBJCheckLine(line);
      line = "";
    } else {
      line += c;
    }
  }
  fclose(File);
}
//---------------------------------------------------------------------------
void BDFile::OBJCheckLine(std::string line) {
  if ((line.empty()) || ((line[0] != 'v') && (line[0] != 'f')))
    return;

  if (line[0] == 'v') {
    if (line[1] == ' ') {
      line.erase(0, 2);
      for (int x = 0; x < 2; x++) {
        dData["Points"].push_back(atof(line.c_str()));
        line.erase(0, line.find_first_of(' '));
      }
      dData["Points"].push_back(atof(line.c_str()));
    } else if (line[2] == 't') {
      line.erase(0, 3);
      dData["TexCoords"].push_back(atof(line.c_str()));
      line.erase(0, line.find_first_of(' '));
      dData["TexCoords"].push_back(atof(line.c_str()));
    }
  } else {
    line.erase(0, 2);
    if (dData["TexCoords"].size() > 0) {
      iData["TriTexs"].push_back(iData["Triangles"].size() / 3);
      // iData["TriTexs"].push_back(0);
    }

    for (int x = 0; x < 2; x++) {
      iData["Triangles"].push_back(atoi(line.c_str()) - 1);
      if (dData["TexCoords"].size() > 0) {
        line.erase(0, line.find_first_of('/'));
        iData["TriTexs"].push_back(atoi(line.c_str()) - 1);
      }

      line.erase(0, line.find_first_of(' '));
    }

    iData["Triangles"].push_back(atoi(line.c_str()) - 1);
    line.erase(0, line.find_first_of("/"));

    if (dData["TexCoords"].size() > 0)
      iData["TriTexs"].push_back(atoi(line.c_str()) - 1);
  }
}
//---------------------------------------------------------------------------
void BDFile::LoadVertData(std::string filename, double *verts, double *cols, float *texcos,
                          unsigned int* indices, unsigned int numverts, unsigned int numtris) {
  filename = "Data/" + filename;

  FILE* File;
  if ((File = fopen(filename.c_str(), "rb")) == 0) {
    throw BDError("Unable to open file: BDFile::LoadVertData");
  }

  /*fseek(File, 0, SEEK_END);
    const int length = ftell(File);

    if (length == -1L)
    throw "Unable to get length of file! : BDFile::LoadVertData";

    const int numverts = (length / sizeof(float)) / 14; */
  rewind(File);
  fread(verts, sizeof(double)*3, numverts, File);

  fread(cols, sizeof(double)*3, numverts, File);

  fread(texcos, sizeof(float)*2, numverts, File);

  fread(indices, sizeof(unsigned int)*3, numtris, File);

  fclose(File);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
