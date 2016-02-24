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

#if __clang_major__ < 3
#error Clang 3 required.
#endif

#define U_COMPILER_CLANG 1
#define U_COMPILER_CLANG_VERSION ( __clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__ )

#define U_FUNCTION __FUNCTION__
#define U_FUNCTION_SIG __PRETTY_FUNCTION__

#if (__cplusplus == 201103L) )
#  define U_CXX11 1
#  define U_CXX11_OR_GREATER 1
#elif (__cplusplus >= 201103L)
#  define U_CXX11_OR_GREATER 1
#endif


#if __has_feature(cxx_constexpr)
#	define U_CONSTEXPR    constexpr
#else
#	define U_CONSTEXPR
#endif

/// DLL export API declaration.
#define U_API_EXPORT
/// DLL import API declaration.
#define U_API_IMPORT

/// allows member functions to be made abstract. uses nonstandard C++ extensions provided by MSVC
#define U_ABSTRACT
/// marks member functions as being an override of a base class virtual function. uses nonstandard C++ extensions provided by MSVC
#define U_OVERRIDE
/// allows classes and member functions to be made sealed. uses nonstandard C++ extensions provided by MSVC
#define U_FINAL
/// allows to emit pragmas from within macros.Attribute for explicitly defining a pointer or reference as not being externally aliased.
#define U_PRAGMA(pragma)
/// support for C99 restrict keyword
#define U_RESTRICT    __restrict
/// tells the compiler that the return value (RV) of a function is an object that will not be aliased with any other pointers
#define U_RESTRICT_RV
/// tells the compiler that a function call does not modify or reference visible global state and only modifies the memory pointed to directly by pointer parameters
#define U_NO_ALIAS
/// forces a function to be inlined
#define U_INLINE    inline
/// Attribute for forcing the compiler to inline a function.
#define U_FORCEINLINE inline __attribute__( ( always_inline ) )
/// tells the compiler to never inline a particular function
#define U_NO_INLINE
/// passes optimization hints to the compiler
#define U_HINT(hint)
/// used in switch-statements whose default-case can never be reached, resulting in more optimal code
#define U_NO_SWITCH_DEFAULT    U_HINT(0)
/// Prefix macro for declaring type or variable alignment.
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define U_ALIGN_PRE( ALIGNMENT )
/// Suffix macro for declaring type or variable alignment.
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define U_ALIGN_POST( ALIGNMENT ) __attribute__( ( aligned( ALIGNMENT ) ) )


/// Mark variable as actually used (omit unused variable warning)
#define U_UNUSED(expr) (void)(expr)