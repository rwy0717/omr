/*******************************************************************************
 *  Copyright (c) 2018, 2018 IBM and others
 *
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License 2.0 which accompanies this
 *  distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 *  or the Apache License, Version 2.0 which accompanies this distribution and
 *  is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  This Source Code may also be made available under the following
 *  Secondary Licenses when the conditions for such availability set
 *  forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 *  General Public License, version 2 with the GNU Classpath
 *  Exception [1] and GNU General Public License, version 2 with the
 *  OpenJDK Assembly Exception [2].
 *
 *  [1] https://www.gnu.org/software/classpath/license.html
 *  [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#if !defined(OMR_TYPETRAITS_HPP_)
#define OMR_TYPETRAITS_HPP_

namespace OMR
{

///
/// Basic constants
///

/// Define an integral constant.
template <typename T, T value>
struct IntegralConstant
{
	typedef T ValueType;

	static const ValueType VALUE = value;

	ValueType operator()() const { return VALUE; }
};

template <typename T, T value>
const T IntegralConstant<T, value>::VALUE;

template <bool value>
struct BoolConstant : IntegralConstant<bool, value> {};

struct FalseConstant : BoolConstant<false> {};

struct TrueConstant : BoolConstant<true> {};

template <typename T>
struct TypeAlias
{
	typedef T Type;
};

///
/// Type modifications: add or remove qualifiers or type modifiers
///

/// RemoveConst<T> is T without any const qualifiers.
template <typename T>
struct RemoveConst : TypeAlias<T> {};

template <typename T>
struct RemoveConst<const T> : RemoveConst<T> {};

/// RemoveVolatile<T>::Type is T without volatile qualifiers.
template <typename T>
struct RemoveVolatile : TypeAlias<T> {};

template <typename T>
struct RemoveVolatile<volatile T> : RemoveVolatile<T> {};

/// RemoveCv<T>::Type is T without const/volatile qualifiers.
template <typename T>
struct RemoveCv : TypeAlias<T> {};

template <typename T>
struct RemoveCv<const T> : RemoveCv<T> {};

template <typename T>
struct RemoveCv<volatile T> : RemoveCv<T> {};

template <typename T>
struct RemoveCv<const volatile T> : RemoveCv<T> {};

/// RemoveReference<T>::Type is T as a non-reference.
template <typename T>
struct RemoveReference : TypeAlias<T> {};

template <typename T>
struct RemoveReference<T&> : RemoveReference<T> {};

// TODO: Handle RValue references in typetraits
// template <typename T>
// struct RemoveReference<T&&> : RemoveReference<T> {};

/// Remove cv-qualifiers and one layer of references.
template <typename T>
struct RemoveCvRef : TypeAlias<T> {};

template <typename T>
struct RemoveCvRef<const T> : RemoveCvRef<T> {};

template <typename T>
struct RemoveCvRef<volatile T> : RemoveCvRef<T> {};

template <typename T>
struct RemoveCvRef<const volatile T> : RemoveCvRef<T> {};

template <typename T>
struct RemoveCvRef<T&> : RemoveCvRef<T> {};

// TODO: Handle rvalue references in type traits
// template <typename T>
// struct RemoveCvRef<T&&> : RemoveCvRef<T> {};

///
/// Type reflection: statically query types
///

/// True if T and U are the exact same type. Does not remove qualifiers.
template <typename T, typename U>
struct IsSame : FalseConstant {};

template <typename T>
struct IsSame<T, T> : TrueConstant {};

/// true if T is a reference type.
template <typename T>
struct IsReference : FalseConstant {};

template <typename T>
struct IsReference<T&> : TrueConstant {};

// TODO: Handle rvalue references in type traits
// template <typename T>
// struct IsReference<T&&> : TrueConstant {};

/// Helper for IsPointer.
template <typename T>
struct IsPointerBase : FalseConstant {};

template <typename T>
struct IsPointerBase<T*> : TrueConstant {};

/// IsPointer<T>::VALUE is true if T is a (possibly cv qualified) primitive pointer type.
template <typename T>
struct IsPointer : IsPointerBase<typename RemoveCvRef<T>::Type> {};

/// IsVoid<T>::VALUE is true if T is (possibly cv qualified) void.
template <typename T>
struct IsVoid : IsSame<typename RemoveCv<T>::Type, void> {};

}  // namespace OMR

#endif // OMR_TYPETRAITS_HPP_
