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
#ifndef FileUnitH
#define FileUnitH
//---------------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
//---------------------------------------------------------------------------
class BDFile {
 private:
  struct Vertex {
    float X, Y, Z;
  };
  Vertex* vertices;
  int* facedata;

  std::map<std::string, std::vector<double> > dData;
  std::map<std::string, std::vector<int> > iData;
  std::map<std::string, std::vector<std::string> > sData;
  std::map<std::string, std::vector<bool> > bData;

  void CheckLine(std::string);    // Checks the line and adds info (if any) to Data

  void OBJCheckLine(std::string);

  // MESH FUNCTIONS
  // Vertex* ReadMeshVertices(char* whichdat, char* src);
  // int* ReadMeshFaceData(char* whichdat, char* src);
  // void CreateMeshAsDisList(int listnum);
  // double* CalcNormal(Vertex V1, Vertex V2, Vertex V3);

 public:
  BDFile();
  ~BDFile();

  void Create();
  void Destroy();

  void ReadData(const char* filename);

  inline double GetDatad(std::string dat, int num = 0) {return dData[dat][num];}
  inline int GetDatai(std::string dat, int num = 0) {return iData[dat][num];}
  inline std::string GetDatas(std::string dat, int num = 0) {return sData[dat][num];}
  inline bool GetDatab(std::string dat, int num = 0) {return bData[dat][num];}

  inline unsigned int GetSized(std::string dat) {return dData[dat].size();}
  inline unsigned int GetSizei(std::string dat) {return iData[dat].size();}
  inline unsigned int GetSizes(std::string dat) {return sData[dat].size();}
  inline unsigned int GetSizeb(std::string dat) {return bData[dat].size();}

  void LoadMesh(char* filename, int listnum);

  void LoadOBJ(const char* filename);

  static void LoadVertData(std::string, double*, double*, float*,
                           unsigned int*, unsigned int, unsigned int);
};
//---------------------------------------------------------------------------
#endif
