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
#include "Interpolate.h"
//---------------------------------------------------------------------------
BDInterpolate::BDInterpolate(std::vector<Vector3> vp, double t) {
  InitBezier(vp, t);
}
//---------------------------------------------------------------------------
void BDInterpolate::InitBezier(std::vector<Vector3> vp, double t) {
  timeTaken = 0;
  totalTime = t;
  interval = 0;
  bDone = false;
  coeffs.clear();

  // Fill the vector to get right number of points
  while((vp.size() - 1) % 3 != 0)
    vp.push_back(vp[vp.size()-1]);

  unsigned int i, j = 0;
  for (i = 0; i < vp.size(); i += 3, j += 4) {
    coeffs.resize(coeffs.size() + 4);
    coeffs[j+3] = vp[i];
    coeffs[j+2] = 3*(vp[i+1] - vp[i]);
    coeffs[j+1] = 3*(vp[i+2] - vp[i+1]) - coeffs[j+2];
    coeffs[j] = vp[i+3] - vp[i] - coeffs[j+2] - coeffs[j+1];

    if (i == vp.size() - 4)
      i++;
  }

  maxintervals = coeffs.size() / 4;
  timePerInterval = totalTime / maxintervals;
}
//---------------------------------------------------------------------------
void BDInterpolate::InitBezier(double t) {
  InitBezier(points, t);
  points.clear();
}
//---------------------------------------------------------------------------
Vector3 BDInterpolate::Update(double deltaTime) {
  float t = timeTaken/timePerInterval;
  float t2 = t*t;
  float t3 = t2*t;
  int i4 = interval*4;

  timeTaken += deltaTime;

  if (timeTaken >= timePerInterval) {
    if (++interval >= maxintervals)
      bDone = true;

    while(timeTaken > timePerInterval)
      timeTaken -= timePerInterval;
  }

  return coeffs[i4]*t3 + coeffs[i4+1]*t2 + coeffs[i4+2]*t + coeffs[i4+3];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
