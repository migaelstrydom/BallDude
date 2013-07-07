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
#ifndef GLExtensionsUnitH
#define GLExtensionsUnitH
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <string>
//---------------------------------------------------------------------------
extern PFNGLLOCKARRAYSEXTPROC glLockArrays;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArrays;

// shaders
extern PFNGLCREATEPROGRAMOBJECTARBPROC     glCreateProgramObjectARB;
extern PFNGLDELETEOBJECTARBPROC            glDeleteObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC      glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC            glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC           glCompileShaderARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC    glGetObjectParameterivARB;
extern PFNGLATTACHOBJECTARBPROC            glAttachObjectARB;
extern PFNGLGETINFOLOGARBPROC              glGetInfoLogARB;
extern PFNGLLINKPROGRAMARBPROC             glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC        glUseProgramObjectARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC      glGetUniformLocationARB;
extern PFNGLUNIFORM1FARBPROC               glUniform1fARB;
extern PFNGLUNIFORM1IARBPROC               glUniform1iARB;

namespace MSGLExtensions {
  bool InitArrayExt();

  bool InitShaderExt();
};

//---------------------------------------------------------------------------
#endif
