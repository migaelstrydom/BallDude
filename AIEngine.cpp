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
#include "AIEngine.h"
//---------------------------------------------------------------------------
void BDAIEngine::Update() {
  if (firstAI == NULL)
    return;

  bool bDone = false;

  // check for dead AI units
  if (firstAI->isDead()) {
    if (firstAI == lastAI) {
      firstAI = lastAI = NULL;
      return;
    } else {
      firstAI = (BDAIObject*)firstAI->previous;
    }
  }

  if (lastAI->isDead()) {
    if (lastAI == firstAI) {
      firstAI = lastAI = NULL;
      return;
    } else {
      lastAI = (BDAIObject*)lastAI->next;
    }
  }

  BDAIObject *idx = firstAI;

  while(!bDone) {
    idx->SetPlayerPos(*playerPos);

    if (idx == lastAI) {
      bDone = true;
    } else {
      idx = (BDAIObject*)idx->previous;
    }
  }
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
//---------------------------------------------------------------------------
