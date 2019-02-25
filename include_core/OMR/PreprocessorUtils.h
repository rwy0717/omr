/*******************************************************************************
 * Copyright (c) 2019, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#if !defined(OMR_PREPROCESSORUTILS_H_)
#define OMR_PREPROCESSORUTILS_H_

/**
 * Paste two tokens together. Arguments will NOT be macro-expanded. Use OMR_TOKEN_PASTE instead.
 */
#define OMR_TOKEN_PASTE_NOEXPAND(a, b) a##b

/**
 * Paste two tokens together, forming a single identifier. Arguments will be macro-expanded once.
 */
#define OMR_TOKEN_PASTE(a, b) OMR_TOKEN_PASTE_NOEXPAND(a, b)

/**
 * Transform x to a literal string. x will NOT be expanded by the preprocessor. Use OMR_STRINGIFY instead.
 */
#define OMR_STRINGIFY_NOEXPAND(x) #x

/**
 * Transform x to a literal string. x will be macro-expanded once.
 */
#define OMR_STRINGIFY(x) OMR_STRINGIFY_NOEXPAND(x)

/**
 * A literal string describing the current position in the source code.
 * The format is:
 *    FILE:LINE
 */
#define OMR_FILEPOS __FILE_ ":" OMR_STRINGIFY(__LINE__)

/**
 * @def OMR_FUNC A macro that expands to the base name of the current function.
 * A portable wrapper around __func__.
 */
#if defined(__func__)
/* c99 and c++11: standard function name macro. */
#define OMR_FUNC __func__ 
#elif defined(__FUNCTION__)
/* pre-c99 and xlc: nonstandard function name. Available on all compilers. */
#define OMR_FUNC __FUNCTION__ 
#else
#warning "OMR_FUNC is unimplemented"
#define OMR_FUNC "UNKNOWN"
#endif

/**
 * @def OMR_FUNCSIG A macro that expands to the signature of the current function.
 * On unsupported compilers, this macro falls back to the behaviour of OMR_FUNC.
 * Result: "int OMR::MyClass::MyMember(int, int)"
 */
#if defined(__PRETTY_FUNCTION__)
/* gcc and clang: name with signature.*/
#define OMR_FUNCSIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
/* msvc: name with signature. */
#define OMR_FUNCSIG __FUNCSIG__
#else
/* fallback to just the base-name of the function */
#define OMR_FUNCSIG OMR_FUNC
#endif

/**
 * A literal string that expands to the current location in source code.
 * This macro is intended to provide as much context as possible.
 * The format is:
 *    FILE:LINE: FUNCTION
 */
#define OMR_LOCATION OMR_FILEPOS ": " OMR_FUNCSIG

#endif /* OMR_PREPROCESSORUTILS_H_ */
