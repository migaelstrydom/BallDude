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
#ifndef OpenGLUnitH
#define OpenGLUnitH

#include <GL/gl.h>
#include "GLExtensionsUnit.h"
#include "BDError.h"
#include <string>
//#include "PBuffer.h"
//---------------------------------------------------------------------------
namespace MSOpenGL {
  // TEMP
  extern std::string msg;
  // END TEMP

  // Window creation
  extern int Width;
  extern int Height;
  extern int ColorDepth;
  extern bool FullScreen;

  void Init();               // Init
  void MakeWindow(int pwidth, int pheight, int bpp);

  void MakeFullScreen(int pwidth, int pheight, int bpp);
  void GetCurrentScreenMode();
  void ChangeScreenMode();         // Changes back to original screen mode at the end
  void MakeViewport(int x, int y);
  void Shutdown();
  // End Window creation

  // Saving a screenshot
  void SaveScreen(char*);
  void SaveImage(char*);
  // end saving a screenshot

  // misc
  void SetTextureMatrixReflection(float);
  // PBUFFER* CreatePBuffer(unsigned int, unsigned int);
};
//---------------------------------------------------------------------------
#endif
