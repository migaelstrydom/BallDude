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
#ifndef FontUnitH
#define FontUnitH
//---------------------------------------------------------------------------
#include <SDL/SDL_ttf.h>
//---------------------------------------------------------------------------
struct MSTextTexture {
  unsigned int texID;
  int Width, Height;
  float tW, tH;       // width & height of the text
};
//---------------------------------------------------------------------------
class MSFont {
 private:
  bool bInited;
  TTF_Font *font;

  SDL_Surface* CreateSurface(std::string);

 public:
  MSFont();
  MSFont(const char *);
  ~MSFont();
  void Init(const char *);
  void UnInit();

  void DisplayTextBox(MSTextTexture);
  MSTextTexture CreateTextImage(std::string);
  MSTextTexture CreateTextBoxImage(std::string, int);

  static void InitSystem();
  static void DeInitSystem();
  static void DisplayTextBox(unsigned int texID);
};
//---------------------------------------------------------------------------
#endif
