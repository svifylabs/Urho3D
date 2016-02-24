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
/// REF: 
/// https://github.com/HeliumProject/Platform
/// http://www.codeitive.com/0xxjkXUUPg/macro-substitution-fail-in-gcc-47-clang-32.html
/// https://molecularmusings.wordpress.com/2011/07/12/a-plethora-of-macros/

#include <intrin.h>


#define U_COMPILER_CL 1

#define U_FUNCTION __FUNCTION__
#define U_FUNCTION_SIG __FUNCSIG__

/////////////////////////////////////////////////////////////////////////////////////////
// Macros for some C++11 features, which are not available on every compiler
/////////////////////////////////////////////////////////////////////////////////////////

#if (__cplusplus == 201103L) ||  (defined(_MSC_VER) && _MSC_VER >= 1900)
#  define U_CXX11 1
#  define U_CXX11_OR_GREATER 1
#elif (__cplusplus >= 201103L)
#  define U_CXX11_OR_GREATER 1
#endif

#if (_MSC_VER <= 1800)
#   define U_CONSTEXPR
#   define U_CONSTEXPR_OR_CONST const
#else
#   define U_CONSTEXPR constexpr
#   define U_CONSTEXPR_OR_CONST constexpr
#endif

#if (_MSC_VER <= 1800)
#   define U_NOEXCEPT
#   define U_NOEXCEPT_OR_NOTHROW throw()
#else
#   define U_NOEXCEPT noexcept
#   define U_NOEXCEPT_OR_NOTHROW noexcept
#endif

#if (_MSC_VER >= 1900 )
#   define U_NULLPTR nullptr
#else
#   define U_NULLPTR 0
#endif

#if (__cplusplus >= 201103L) ||  (defined(_MSC_VER) && _MSC_VER >= 1900)
#  define U_FKT_DELETE = delete;
#  define U_FKT_DEFAULD = default;
#else
#  define U_FKT_DELETE ;
#  define U_FKT_DEFAULD ;
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// Nonstandard C++ extensions provided by MSVC
/////////////////////////////////////////////////////////////////////////////////////////

///
#define U_STATIC_CONSTEXPR static U_CONSTEXPR_OR_CONST
///
#define U_BREAKPOINT __debugbreak()
/// allows to emit pragmas from within macros.Attribute for explicitly defining a pointer or reference as not being externally aliased.
#define U_PRAGMA(pragma)    __pragma(pragma)
/// Declare a class method as overriding a virtual method of a parent class.
#define U_OVERRIDE override
/// Declare a class as an abstract base class.
#define U_ABSTRACT abstract
/// allows classes and member functions to be made sealed. 
#define U_FINAL       final
/// function to be inlined
#define U_INLINE    inline
/// Attribute for forcing the compiler to inline a function.
#define U_FORCEINLINE __forceinline
/// tells the compiler to never inline a particular function
#define U_NO_INLINE    __declspec(noinline)
/// Attribute for explicitly defining a pointer or reference as not being externally aliased.
#define U_RESTRICT __restrict
/// tells the compiler that the return value (RV) of a function is an object that will not be aliased with any other pointers
#define U_RESTRICT_RV    __declspec(restrict)
/// tells the compiler that a function call does not modify or reference visible global state and only modifies the memory pointed to directly by pointer parameters
#define U_NO_ALIAS    __declspec(noalias)
/// passes optimization hints to the compiler
#define U_HINT(hint)    __assume(hint)
/// used in switch-statements whose default-case can never be reached, resulting in more optimal code
#define U_NO_SWITCH_DEFAULT    U_HINT(0)
/// Prefix macro for declaring type or variable alignment.
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define U_ALIGN_PRE( ALIGNMENT ) __declspec( align( ALIGNMENT ) )
/// Suffix macro for declaring type or variable alignment.
/// @param[in] ALIGNMENT  Byte alignment (must be a power of two).
#define U_ALIGN_POST( ALIGNMENT )

/// DLL export API declaration.
#define U_API_EXPORT __declspec( dllexport )
/// DLL import API declaration.
#define U_API_IMPORT __declspec( dllimport )

/// Mark variable as actually used (omit unused variable warning)
#define U_UNUSED(expr) (void)(sizeof((x), 0))

/// support for C99 restrict keyword
#define U_RESTRICT    __restrict


/////////////////////////////////////////////////////////////////////////////////////////
// Disable warnings
/////////////////////////////////////////////////////////////////////////////////////////

// Save warnings state
#   pragma warning (push)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4786)

// Turn off warnings generated by long std templates
// This warns about truncation to 255 characters in debug/browse info
#   pragma warning (disable : 4503)

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
// Template classes shouldn't be DLL exported, but the compiler warns us by default.
// 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

// disable: "C++ Exception Specification ignored"
// This is because MSVC 6 did not implement all the C++ exception
// specifications in the ANSI C++ draft.
// Visual C++ does not support exception specifications at this time, but we still want to retain them for compilers
// that do support them.  This is harmless to ignore.
// C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#   pragma warning( disable : 4290 )

// This spuriously comes up on occasion with certain template class methods.
// 'function' : unreferenced local function has been removed
#pragma warning( disable : 4505 ) 

// Visual C++ specific keywords such as "override" and "abstract" are used, but are abstracted via "U_OVERRIDE" and
// "U_ABSTRACT" macros.
#   pragma warning( disable : 4481 ) // nonstandard extension used: override specifier 'keyword'

// disable: "no suitable definition provided for explicit template
// instantiation request" Occurs in VC7 for no justifiable reason on all
// #includes of Singleton
#   pragma warning( disable: 4661)

// disable: deprecation warnings when using CRT calls in VC8
// These show up on all C runtime lib code in VC8, disable since they clutter
// the warnings with things we may not be able to do anything about (e.g.
// generated code from nvparse etc). I doubt very much that these calls
// will ever be actually removed from VC anyway, it would break too much code.
#   pragma warning( disable: 4996)

// disable: "unreferenced formal parameter"
// Many versions of VC have bugs which generate this error in cases where they shouldn't
#   pragma warning (disable : 4100)

#if _MSC_VER == 1500
#pragma warning( disable : 4985 ) // ceil() flawed in vs2008 headers
#endif
