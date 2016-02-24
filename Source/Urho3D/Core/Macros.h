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

//------------------------------------------------------------
/// Memory macros
//------------------------------------------------------------
#define U_SAFEDELETE(ptr)        {if(ptr) delete(ptr); ptr = 0;}
#define U_SAFEDELETE_ARRAY(ptr)  {if(ptr) delete[] (ptr); ptr = 0;}
#define SAFE_DELETE(ptr)         {if(ptr) delete(ptr); ptr = 0;}
#define SAFE_DELETE_ARRAY(ptr)   {if(ptr) delete[] (ptr); ptr = 0;}

/// stringizes a string, even macros
#define U_STRINGIZE_HELPER(token)    #token
#define U_STRINGIZE(str)             U_STRINGIZE_HELPER(str)

/// concatenates two strings, even when the strings are macros themselves
#define U_JOIN_HELPER_HELPER(x, y)      x##y
#define U_JOIN_HELPER(x, y)             U_JOIN_HELPER_HELPER(x, y)
#define U_JOIN(x, y)                    U_JOIN_HELPER(x, y)

#define U_JOIN_2(_0, _1)                                  U_JOIN(_0, _1)
#define U_JOIN_3(_0, _1, _2)                                U_JOIN_2(U_JOIN_2(_0, _1), _2)
#define U_JOIN_4(_0, _1, _2, _3)                              U_JOIN_2(U_JOIN_3(_0, _1, _2), _3)
#define U_JOIN_5(_0, _1, _2, _3, _4)                            U_JOIN_2(U_JOIN_4(_0, _1, _2, _3), _4)
#define U_JOIN_6(_0, _1, _2, _3, _4, _5)                          U_JOIN_2(U_JOIN_5(_0, _1, _2, _3, _4), _5)
#define U_JOIN_7(_0, _1, _2, _3, _4, _5, _6)                        U_JOIN_2(U_JOIN_6(_0, _1, _2, _3, _4, _5), _6)
#define U_JOIN_8(_0, _1, _2, _3, _4, _5, _6, _7)                      U_JOIN_2(U_JOIN_7(_0, _1, _2, _3, _4, _5, _6), _7)
#define U_JOIN_9(_0, _1, _2, _3, _4, _5, _6, _7, _8)                    U_JOIN_2(U_JOIN_8(_0, _1, _2, _3, _4, _5, _6, _7), _8)
#define U_JOIN_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9)                 U_JOIN_2(U_JOIN_9(_0, _1, _2, _3, _4, _5, _6, _7, _8), _9)
#define U_JOIN_11(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)                U_JOIN_2(U_JOIN_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9), _10)
#define U_JOIN_12(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)             U_JOIN_2(U_JOIN_11(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10), _11)
#define U_JOIN_13(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)          U_JOIN_2(U_JOIN_12(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11), _12)
#define U_JOIN_14(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)       U_JOIN_2(U_JOIN_13(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12), _13)
#define U_JOIN_15(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14)      U_JOIN_2(U_JOIN_14(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13), _14)
#define U_JOIN_16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15)   U_JOIN_2(U_JOIN_15(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14), _15)
/// chooses a value based on a condition
#define U_IF_0(t, f)      f
#define U_IF_1(t, f)      t
#define U_IF(cond, t, f)    U_JOIN_2(U_IF_, U_TO_BOOL(cond))(t, f)

/// converts a condition into a boolean 0 (=false) or 1 (=true)
#define U_TO_BOOL_0 0
#define U_TO_BOOL_1 1
#define U_TO_BOOL_2 1
#define U_TO_BOOL_3 1
#define U_TO_BOOL_4 1
#define U_TO_BOOL_5 1
#define U_TO_BOOL_6 1
#define U_TO_BOOL_7 1
#define U_TO_BOOL_8 1
#define U_TO_BOOL_9 1
#define U_TO_BOOL_10 1
#define U_TO_BOOL_11 1
#define U_TO_BOOL_12 1
#define U_TO_BOOL_13 1
#define U_TO_BOOL_14 1
#define U_TO_BOOL_15 1
#define U_TO_BOOL_16 1

#define U_TO_BOOL(x)    U_JOIN_2(U_TO_BOOL_, x)

/// Returns 1 if the arguments to the variadic macro are separated by a comma, 0 otherwise.
#define U_HAS_COMMA(...)              U_HAS_COMMA_EVAL(U_HAS_COMMA_ARGS(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))
#define U_HAS_COMMA_EVAL(...)           __VA_ARGS__
#define U_HAS_COMMA_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, ...) _16

/// Returns 1 if the argument list to the variadic macro is empty, 0 otherwise.
#define U_IS_EMPTY(...)                           \
  U_HAS_COMMA                               \
  (                                     \
    U_JOIN_5                              \
    (                                   \
      U_IS_EMPTY_CASE_,                       \
      U_HAS_COMMA(__VA_ARGS__),                   \
      U_HAS_COMMA(U_IS_EMPTY_BRACKET_TEST __VA_ARGS__),     \
      U_HAS_COMMA(__VA_ARGS__ (~)),                 \
      U_HAS_COMMA(U_IS_EMPTY_BRACKET_TEST __VA_ARGS__ (~))    \
    )                                   \
  )

#define U_IS_EMPTY_CASE_0001      ,
#define U_IS_EMPTY_BRACKET_TEST(...)  ,

// U_VA_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro.
// unfortunately, VS 2010 still has this preprocessor bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
# define U_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...)  N
# define U_VA_NUM_ARGS_REVERSE_SEQUENCE     16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
# define U_VA_NUM_ARGS_LEFT (
# define U_VA_NUM_ARGS_RIGHT )
# define U_VA_NUM_ARGS(...)           U_VA_NUM_ARGS_HELPER U_VA_NUM_ARGS_LEFT __VA_ARGS__, U_VA_NUM_ARGS_REVERSE_SEQUENCE U_VA_NUM_ARGS_RIGHT
#else
# define U_VA_NUM_ARGS(args...)           U_VA_NUM_ARGS_HELPER(args, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
# define U_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...)  N
#endif

// U_NUM_ARGS correctly handles the case of 0 arguments
#define U_NUM_ARGS(...)               U_IF(U_IS_EMPTY(__VA_ARGS__), 0, U_VA_NUM_ARGS(__VA_ARGS__))

// U_PASS_ARGS passes __VA_ARGS__ as multiple parameters to another macro, working around the following bug:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
# define U_PASS_ARGS_LEFT (
# define U_PASS_ARGS_RIGHT )
# define U_PASS_ARGS(...)             U_PASS_ARGS_LEFT __VA_ARGS__ U_PASS_ARGS_RIGHT
#else
# define U_PASS_ARGS(...)             __VA_ARGS__
#endif

/// Expand any number of arguments into a list of operations called with those arguments
#define U_EXPAND_ARGS_0(op)
#define U_EXPAND_ARGS_1(op, a1)                                     op(a1)
#define U_EXPAND_ARGS_2(op, a1, a2)                                   op(a1) op(a2)
#define U_EXPAND_ARGS_3(op, a1, a2, a3)                                 op(a1) op(a2) op(a3)
#define U_EXPAND_ARGS_4(op, a1, a2, a3, a4)                               op(a1) op(a2) op(a3) op(a4)
#define U_EXPAND_ARGS_5(op, a1, a2, a3, a4, a5)                             op(a1) op(a2) op(a3) op(a4) op(a5)
#define U_EXPAND_ARGS_6(op, a1, a2, a3, a4, a5, a6)                           op(a1) op(a2) op(a3) op(a4) op(a5) op(a6)
#define U_EXPAND_ARGS_7(op, a1, a2, a3, a4, a5, a6, a7)                         op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7)
#define U_EXPAND_ARGS_8(op, a1, a2, a3, a4, a5, a6, a7, a8)                       op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8)
#define U_EXPAND_ARGS_9(op, a1, a2, a3, a4, a5, a6, a7, a8, a9)                     op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9)
#define U_EXPAND_ARGS_10(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)                 op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10)
#define U_EXPAND_ARGS_11(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)                op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11)
#define U_EXPAND_ARGS_12(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)             op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11) op(a12)
#define U_EXPAND_ARGS_13(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)          op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11) op(a12) op(a13)
#define U_EXPAND_ARGS_14(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)       op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11) op(a12) op(a13) op(a14)
#define U_EXPAND_ARGS_15(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)      op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11) op(a12) op(a13) op(a14) op(a15)
#define U_EXPAND_ARGS_16(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)   op(a1) op(a2) op(a3) op(a4) op(a5) op(a6) op(a7) op(a8) op(a9) op(a10) op(a11) op(a12) op(a13) op(a14) op(a15) op(a16)
#if defined( _MSC_VER )
#define U_EXPAND_ARGS(op, ...)    U_JOIN_2(U_EXPAND_ARGS_, U_NUM_ARGS(__VA_ARGS__)) U_PASS_ARGS(op, __VA_ARGS__)
#else // works for android build on windows
#define U_EXPAND_ARGS(op, ...)    U_JOIN_2(U_EXPAND_ARGS_, U_NUM_ARGS(__VA_ARGS__)) (op, __VA_ARGS__)
#endif

/// Expand any number of arguments into a list of operations called with those arguments
/// with a index count in the argument list
#define U_EXPAND_ARGS_C_0(op)
#define U_EXPAND_ARGS_C_1(op, a1)                                     op(a1, 0)
#define U_EXPAND_ARGS_C_2(op, a1, a2)                                   op(a1, 0) op(a2, 1)
#define U_EXPAND_ARGS_C_3(op, a1, a2, a3)                                 op(a1, 0) op(a2, 1) op(a3, 2)
#define U_EXPAND_ARGS_C_4(op, a1, a2, a3, a4)                               op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3)
#define U_EXPAND_ARGS_C_5(op, a1, a2, a3, a4, a5)                             op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4)
#define U_EXPAND_ARGS_C_6(op, a1, a2, a3, a4, a5, a6)                           op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5)
#define U_EXPAND_ARGS_C_7(op, a1, a2, a3, a4, a5, a6, a7)                         op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6)
#define U_EXPAND_ARGS_C_8(op, a1, a2, a3, a4, a5, a6, a7, a8)                       op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7)
#define U_EXPAND_ARGS_C_9(op, a1, a2, a3, a4, a5, a6, a7, a8, a9)                     op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8)
#define U_EXPAND_ARGS_C_10(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)                 op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9)
#define U_EXPAND_ARGS_C_11(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)                op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10)
#define U_EXPAND_ARGS_C_12(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)             op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10) op(a12, 11)
#define U_EXPAND_ARGS_C_13(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)          op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10) op(a12, 11) op(a13, 12)
#define U_EXPAND_ARGS_C_14(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)       op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10) op(a12, 11) op(a13, 12) op(a14, 13)
#define U_EXPAND_ARGS_C_15(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)      op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10) op(a12, 11) op(a13, 12) op(a14, 13) op(a15, 14)
#define U_EXPAND_ARGS_C_16(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)   op(a1, 0) op(a2, 1) op(a3, 2) op(a4, 3) op(a5, 4) op(a6, 5) op(a7, 6) op(a8, 7) op(a9, 8) op(a10, 9) op(a11, 10) op(a12, 11) op(a13, 12) op(a14, 13) op(a15, 14) op(a16, 15)

#if defined( _MSC_VER )
#define U_EXPAND_ARGS_C(op, ...)    U_JOIN_2(U_EXPAND_ARGS_C_, U_NUM_ARGS(__VA_ARGS__)) U_PASS_ARGS(op, __VA_ARGS__)
#else // works for android build on windows
#define U_EXPAND_ARGS_C(op, ...)    U_JOIN_2(U_EXPAND_ARGS_C_, U_NUM_ARGS(__VA_ARGS__)) (op, __VA_ARGS__)
#endif
/// http://stackoverflow.com/questions/4851075/universally-compiler-independent-way-of-implementing-an-unused-macro-in-c-c#
/// http://stackoverflow.com/questions/4030959/will-a-variablename-c-statement-be-a-no-op-at-all-times/4030983#4030983
/// Turn any legal C++ expression into nothing
#define UNUSED_IMPL(symExpr, n) , (void)sizeof(symExpr)

#if defined( _MSC_VER )
#define UNUSED(...) (void)sizeof(true) U_EXPAND_ARGS_C U_PASS_ARGS(UNUSED_IMPL, __VA_ARGS__)
#else // works for android build on windows
#define UNUSED(...) (void)sizeof(true) U_EXPAND_ARGS_C (UNUSED_IMPL, __VA_ARGS__)
#endif

#define TEST_CONDITION(condition, a, ...) (condition) ? UNUSED(a) : UNUSED(__VA_ARGS__)
