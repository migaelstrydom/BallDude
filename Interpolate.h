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
#ifndef InterpolateH
#define InterpolateH
//---------------------------------------------------------------------------
#include <vector>
#include "Vector.h"
//---------------------------------------------------------------------------
class BDInterpolate {
 private:
  double totalTime, timeTaken, timePerInterval;
  int interval, maxintervals;     // for determining which set of intervals to use
  std::vector<Vector3> coeffs, points; // points is for one by one insertion

  bool bDone;

 public:
  BDInterpolate() {}
  BDInterpolate(std::vector<Vector3>, double);

  void InitBezier(std::vector<Vector3>, double);
  void InitBezier(double);    // Call after one by one insertion
  void AddPoint(Vector3 vec) {points.push_back(vec);}
  Vector3 Update(double);

  bool IsDone() {return bDone;}
};
//---------------------------------------------------------------------------
#endif
