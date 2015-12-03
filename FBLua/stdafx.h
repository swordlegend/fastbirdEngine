/*
 -----------------------------------------------------------------------------
 This source file is part of fastbird engine
 For the latest info, see http://www.jungwan.net/
 
 Copyright (c) 2013-2015 Jungwan Byun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
*/

#include "FBCommonHeaders/platform.h"
#if defined(_PLATFORM_WINDOWS_)
#define FB_DLL_LUA __declspec(dllexport)
#define FB_DLL_FILESYSTEM __declspec(dllimport)
#else
#define FB_DLL_LUA
#endif
// If you are linking against Lua compiled in C++, define LUAW_NO_EXTERN_C
#ifndef LUAW_NO_EXTERN_C
extern "C"
{
#endif // LUAW_NO_EXTERN_C

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#ifndef LUAW_NO_EXTERN_C
}
#endif // LUAW_NO_EXTERN_C

#include "FBCommonHeaders/platform.h"
#if defined(_PLATFORM_WINDOWS_)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#endif
#include <regex>

#include "FBMemoryManagerLib/MemoryManager.h"