/*******************************************************************************
 *  Copyright (c) 2018, 2019 IBM and others
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

#if !defined(OMR_GC_HANDLE_HPP_)
#define OMR_GC_HANDLE_HPP_

#include <OMR/TypeTraits.hpp>
#include <cstddef>
#include <omrcfg.h>

namespace OMR
{
namespace GC
{

template <typename T>
class HandleBase
{
public:
	HandleBase() {}

	HandleBase(T* const volatile* root) : _root(root) {}

	/**
	 * Convert this Handle to a raw indirect pointer. The address of the rooting slot.
	 */
	T* const volatile* root() const { return this->_root; }

	/**
	 * Convert this Handle to a raw pointer.
	 */
	T* get() const { return *_root; }

protected:
	T* const volatile* _root;
};

template <typename T, bool DEFINE_OPERATORS = !IsVoid<T>::VALUE>
class HandlePtrOps;

template <typename T>
class HandlePtrOps<T, true> : public HandleBase<T>
{
public:
	HandlePtrOps() : HandleBase<T>() {}

	HandlePtrOps(T* const volatile* root) : HandleBase<T>(root) {}

	T& operator*() const { return *this->get(); }

	T* operator->() const { return this->get(); }
};

template <typename T>
class HandlePtrOps<T, false> : public HandleBase<T>
{
public:
	HandlePtrOps() : HandleBase<T>() {}

	HandlePtrOps(T* const volatile* root) : HandleBase<T>(root) {}
};

/**
 * The Handle<T> provides pointer semantics by indirecting through roots.
 *
 * A Handle<T> is associated with a single root slot, such as an instance of StackRoot<T>. When a Handle<T> is
 * dereferenced, it will access the target indirectly through the associated root. Because Handles always indirect
 * through roots, they are safe to hold across a collection. A Handle is invalid when it's root is destroyed. Handles
 * are fast and flexible to use, but care must be taken so a Handle does not outlive it's root.
 */
template <typename T>
class Handle : public HandlePtrOps<T>
{
public:
	/**
	 * Uninitialized handle object. Very dangerous.
	 */
	Handle() : HandlePtrOps<T>() {}

	template <typename U>
	explicit Handle(U* const volatile* root) : HandlePtrOps<T>(root)
	{}

	/**
	 * Disabled when T = U, where we rely on the implicit copy constructor.
	 */
	template <typename U, typename = typename EnableIf<!IsSame<T, U>::VALUE>::Type>
	Handle(const Handle<U>& other) : HandlePtrOps<T>(other.root())
	{}
};

/**
 * Compare the address of the referents. Note that two Handles with different roots, but pointing at the same object,
 * are considered equal.
 */
template <typename T, typename U>
bool
operator==(Handle<T> lhs, Handle<U> rhs)
{
	return lhs.get() == rhs.get();
}

/**
 * Compare the address of the referents.
 */
template <typename T, typename U>
bool
operator!=(Handle<T> lhs, Handle<U> rhs)
{
	return lhs.get() != rhs.get();
}

/**
 * Compare the address of the referent to a pointer.
 */
template <typename T, typename U>
bool
operator==(Handle<T> lhs, U* rhs)
{
	return lhs.get() == rhs;
}

/**
 * Compare the address of the referent to a pointer.
 */
template <typename T, typename U>
bool
operator!=(Handle<T> lhs, U* rhs)
{
	return lhs.get() != rhs;
}

/**
 * Compare the address of the referent.
 */
template <typename T, typename U>
bool
operator==(T* lhs, Handle<U> rhs)
{
	return lhs == rhs.get();
}

template <typename T, typename U>
bool
operator!=(T* lhs, Handle<U> rhs)
{
	return lhs != rhs.get();
}

} /* namespace GC */
} /* namespace OMR */

#endif /* OMR_GC_HANDLE_HPP_ */
