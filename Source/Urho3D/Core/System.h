//
// Copyright (c) 2008-2013 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
// Platform & Architecture
/////////////////////////////////////////////////////////////////////////////////////////

// Operating system (U_OS_*) macros:
// - U_OS_PC: PC operating system (any)
//   - U_OS_WIN: Windows (any architecture/version)
//     - U_OS_WIN32: 32-bit Windows
//     - U_OS_WIN64: 64-bit Windows

// CPU architecture (U_CPU_*) macros:
// - U_CPU_X86: Intel x86-based (any architecture)
//   - U_CPU_X86_32: 32-bit Intel x86
//   - U_CPU_X86_64: 64-bit Intel x86

// Compiler (U_CC_*) macros:
// - U_CC_CL: Microsoft Visual C++
// - U_CC_GCC: GCC

#if defined( _WIN64 )
# define U_OS_WIN 1
# define U_OS_WIN64 1
# define U_CPU_X86 1
# define U_CPU_X86_64 1
#elif defined( _WIN32 )
# define U_OS_WIN 1
# define U_OS_WIN32 1
# define U_CPU_X86 1
# define U_CPU_X86_32 1
#elif defined( __APPLE__ )
# define U_OS_MAC 1
# if defined( __x86_64__ )
#  define U_OS_MAC64 1
#  define U_CPU_X86 1
#  define U_CPU_X86_64 1
# else
#  define U_OS_MAC32 1
#  define U_CPU_X86 1
#  define U_CPU_X86_32 1
# endif
#elif defined( __gnu_linux__ )
# define U_OS_LINUX 1
# if defined( __x86_64__ )
#  define U_CPU_X86 1
#  define U_CPU_X86_64 1
# else
#  define U_CPU_X86 1
#  define U_CPU_X86_32 1
# endif
#else
# error Unsupported platform.
#endif


/////////////////////////////////////////////////////////////////////////////////////////
// Compiler
/////////////////////////////////////////////////////////////////////////////////////////
# include "../Core/Compiler.h"

// #if U_CPU_X86
// # include "../Core/CpuX86.h"
// #endif
// #if U_OS_WIN
// # include "../Core/SystemWin.h"
// #endif
// 
// #if U_OS_MAC
// # include "../Core/SystemMac.h"
// #endif
// 
// #if U_OS_LINUX
// # include "../Core/SystemLinux.h"
// #endif
