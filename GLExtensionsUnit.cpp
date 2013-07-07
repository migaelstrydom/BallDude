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
#include "GLExtensionsUnit.h"
#define NO_SDL_GLEXT
#include <SDL/SDL.h>
//---------------------------------------------------------------------------
// MSLOCKARRAYS_EXTENSIONS
PFNGLLOCKARRAYSEXTPROC              glLockArrays = NULL;
PFNGLUNLOCKARRAYSEXTPROC            glUnlockArrays = NULL;
//#endif // MSLOCKARRAYS_EXTENSIONS

// Shaders
PFNGLCREATEPROGRAMOBJECTARBPROC     glCreateProgramObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC            glDeleteObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC      glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC            glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC           glCompileShaderARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC    glGetObjectParameterivARB = NULL;
PFNGLATTACHOBJECTARBPROC            glAttachObjectARB = NULL;
PFNGLGETINFOLOGARBPROC              glGetInfoLogARB = NULL;
PFNGLLINKPROGRAMARBPROC             glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC        glUseProgramObjectARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC      glGetUniformLocationARB = NULL;
PFNGLUNIFORM1FARBPROC               glUniform1fARB = NULL;
PFNGLUNIFORM1IARBPROC               glUniform1iARB = NULL;
//---------------------------------------------------------------------------
bool MSGLExtensions::InitArrayExt() {
  std::string str = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

  if (str.npos == str.find_first_of("GL_EXT_compiled_vertex_array")) {
    return false;
  }

  glLockArrays = (PFNGLLOCKARRAYSEXTPROC) SDL_GL_GetProcAddress("glLockArraysEXT");
  if (!glLockArrays)
    return false;

  glUnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC) SDL_GL_GetProcAddress("glUnlockArraysEXT");
  if (!glUnlockArrays)
    return false;

  return true;
}
//---------------------------------------------------------------------------
bool MSGLExtensions::InitShaderExt() {
  std::string str = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

  if (str.npos == str.find_first_of("GL_ARB_shader_objects") ||
      str.npos == str.find_first_of("GL_ARB_shading_language_100") ||
      str.npos == str.find_first_of("GL_ARB_vertex_shader") ||
      str.npos == str.find_first_of("GL_ARB_fragment_shader")) {
    return false;
  }

  glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
      SDL_GL_GetProcAddress("glCreateProgramObjectARB");
  glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
      SDL_GL_GetProcAddress("glDeleteObjectARB");
  glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
      SDL_GL_GetProcAddress("glCreateShaderObjectARB");
  glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
      SDL_GL_GetProcAddress("glShaderSourceARB");
  glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
      SDL_GL_GetProcAddress("glCompileShaderARB");
  glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
      SDL_GL_GetProcAddress("glGetObjectParameterivARB");
  glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
      SDL_GL_GetProcAddress("glAttachObjectARB");
  glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)
      SDL_GL_GetProcAddress("glGetInfoLogARB");
  glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
      SDL_GL_GetProcAddress("glLinkProgramARB");
  glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
      SDL_GL_GetProcAddress("glUseProgramObjectARB");
  glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
      SDL_GL_GetProcAddress("glGetUniformLocationARB");
  glUniform1fARB = (PFNGLUNIFORM1FARBPROC)
      SDL_GL_GetProcAddress("glUniform1fARB");
  glUniform1iARB = (PFNGLUNIFORM1IARBPROC)
      SDL_GL_GetProcAddress("glUniform1iARB");

  if (glActiveTextureARB == NULL
      || glMultiTexCoord2fARB == NULL
      || glCreateProgramObjectARB == NULL
      || glDeleteObjectARB == NULL
      || glCreateShaderObjectARB == NULL
      || glShaderSourceARB == NULL
      || glCompileShaderARB == NULL
      || glGetObjectParameterivARB == NULL
      || glAttachObjectARB == NULL
      || glGetInfoLogARB == NULL
      || glLinkProgramARB == NULL
      || glUseProgramObjectARB == NULL
      || glGetUniformLocationARB == NULL
      || glUniform1fARB == NULL
      || glUniform1iARB == NULL)
    return false;

  return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------





