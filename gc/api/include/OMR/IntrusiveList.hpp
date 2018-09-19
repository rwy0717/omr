
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

#if !defined(OMR_INTRUSIVELIST_HPP_)
#define OMR_INTRUSIVELIST_HPP_

#include <type_traits>
#include <utility>
#include <cassert>

namespace OMR
{

template <typename T>
class IntrusiveListNode;

template <typename T>
struct IntrusiveListTraits;

template<typename T, typename Accessor>
class IntrusiveList;

template<typename T, typename Accessor>
class IntrusiveListIterator;

template<typename T, typename Accessor>
class IntrusiveListConstIterator;

/// Stores the next and previous pointers for elements in an IntrusiveList.
/// Each element in an intrusive list must contain an IntrusiveListNode.
/// The IntrusiveListTraits class is used to obtain the node from an element.
template<typename T>
class IntrusiveListNode
{
public:
	constexpr IntrusiveListNode() noexcept : prev(nullptr), next(nullptr) {}

	constexpr IntrusiveListNode(T* p, T* n) noexcept : prev(p), next(n) {}

	/// assign previous, next to node.
	void assign(T* p, T* n) noexcept
	{
		prev = p;
		next = n;
	}

	// deactivate the node, and clear the next/prev pointers.
	void clear() noexcept
	{
		prev = nullptr;
		next = nullptr;
	}

	T* prev;
	T* next;
};

/// The IntrusiveListAccessor defines how to obtain an InstrusiveListNode from a list element. This template can be
/// specialized to set the default accessor for a type.
template <typename T>
struct IntrusiveListNodeAccessor {

	using Node = IntrusiveListNode<T>;

	/// Obtain the IntrusiveListNode from an element. By default, calls element.node().
	static constexpr Node& node(T& element) noexcept { return element.node(); }

	/// Obtain a constant node from an element.
	static constexpr const Node& node(const T& element) noexcept { return element.node(); }
};

/// Simple bidirectional iterator for the elements in an intrusive list.
template<typename T, typename Accessor = IntrusiveListNodeAccessor<T>>
class IntrusiveListIterator
{
public:
	IntrusiveListIterator() noexcept : IntrusiveListIterator(nullptr) {}

	explicit constexpr IntrusiveListIterator(T* root) noexcept : _current(root) {}

	constexpr IntrusiveListIterator(const IntrusiveListIterator<T, Accessor>&) noexcept = default;

	constexpr T& operator*() const noexcept { return *_current; }

	constexpr T* operator->() const noexcept { return _current; }

	IntrusiveListIterator<T, Accessor>& operator++() noexcept
	{
		_current = Accessor::node(*_current).next;
		return *this;
	}

	IntrusiveListIterator<T, Accessor> operator++(int) noexcept {
		IntrusiveListIterator<T, Accessor> copy = *this;
		++this;
		return copy;
	}

	IntrusiveListIterator<T, Accessor>& operator--() noexcept
	{
		_current = Accessor::node(*_current).prev;
		return *this;
	}

	IntrusiveListIterator<T, Accessor>& operator--(int) noexcept
	{
		IntrusiveListIterator<T, Accessor> copy = *this;
		_current = Accessor::node(*_current).prev;
		return copy;
	}

	constexpr bool operator==(const IntrusiveListIterator<T, Accessor>& rhs) const noexcept
	{
		return _current == rhs._current;
	}

	constexpr bool operator!=(const IntrusiveListIterator<T, Accessor>& rhs) const noexcept
	{
		return _current != rhs._current;
	}

	IntrusiveListIterator<T, Accessor>&
	operator=(const IntrusiveListIterator<T, Accessor>& rhs) noexcept
	{
		_current = rhs._current;
		return *this;
	}

	constexpr T* current() const noexcept { return _current; }

private:
	T* _current;
};

/// Simple bidirectional iterator for the elements in a constant intrusive list.
template<typename T, typename Accessor = IntrusiveListNodeAccessor<T>>
class IntrusiveListConstIterator
{
public:
	IntrusiveListConstIterator() noexcept : IntrusiveListConstIterator(nullptr) {}

	explicit constexpr IntrusiveListConstIterator(T* root) noexcept : _current(root) {}

	constexpr IntrusiveListConstIterator(const IntrusiveListConstIterator<T, Accessor>&) noexcept = default;

	constexpr IntrusiveListConstIterator(const IntrusiveListIterator<T, Accessor>& rhs) noexcept
		: _current(rhs.current()) {}

	constexpr const T& operator*() const noexcept { return *_current; }

	constexpr const T* operator->() const noexcept { return _current; }

	IntrusiveListConstIterator<T, Accessor>& operator++() noexcept
	{
		_current = Accessor::node(*_current).next;
		return *this;
	}

	IntrusiveListConstIterator<T, Accessor> operator++(int) noexcept {
		IntrusiveListConstIterator<T, Accessor> copy = *this;
		++this;
		return copy;
	}

	IntrusiveListConstIterator<T, Accessor>& operator--() noexcept
	{
		_current = Accessor::node(*_current).prev;
		return *this;
	}

	IntrusiveListConstIterator<T, Accessor>& operator--(int) noexcept
	{
		IntrusiveListConstIterator<T, Accessor> copy = *this;
		_current = Accessor::node(*_current).prev;
		return copy;
	}

	constexpr bool operator==(const IntrusiveListConstIterator<T, Accessor>& rhs) const noexcept
	{
		return _current == rhs._current;
	}

	constexpr bool operator!=(const IntrusiveListConstIterator<T, Accessor>& rhs) const noexcept
	{
		return _current != rhs._current;
	}

	IntrusiveListConstIterator<T, Accessor>&
	operator=(const IntrusiveListConstIterator<T, Accessor>& rhs) noexcept
	{
		_current = rhs._current;
		return *this;
	}

	constexpr const T* current() const noexcept { return _current; }

private:
	const T* _current;
};

template <typename T, typename A>
constexpr bool
operator==(const IntrusiveListIterator<T, A>& lhs, const IntrusiveListConstIterator<T, A>& rhs) {
	return rhs == lhs;
}

template <typename T, typename A>
constexpr bool
operator!=(const IntrusiveListIterator<T, A>& lhs, const IntrusiveListConstIterator<T, A>& rhs) {
	return rhs != lhs;
}

/// A doubly linked linear list, where the list node is embedded in the element.
///
/// To use an intrusive list, the element type T must store an IntrusiveListNode<T>. The default accessor will use T's
/// node() member-function to access the list node.
///
/// The Intrusive list provides two mechanisms for overriding the default node accessor:
///  1. Specialize the IntrusiveListNodeAccessor<T> template.
///  2. Override the Accessor template parameter in the list.
///
/// The IntrusiveList is a doubly-linked, linear list.
template<typename T, typename Accessor = IntrusiveListNodeAccessor<T>>
class IntrusiveList
{
public:
	using Node = IntrusiveListNode<T>;

	using Iterator = IntrusiveListIterator<T, Accessor>;

	using ConstIterator = IntrusiveListConstIterator<T, Accessor>;

	constexpr IntrusiveList() noexcept : _root(nullptr) {}

	/// Add element to the head of the list. Constant time.
	void add(T* element) noexcept
	{
		assert(Accessor::node(*element).next == nullptr);
		assert(Accessor::node(*element).prev == nullptr);
	
		Accessor::node(*element).assign(nullptr, _root);
		if (_root) {
			Accessor::node(*_root).prev = element;
		}
		_root = element;
	}

	/// Remove element from the list. Removing an element invalidates any iterators. Constant time.
	void remove(T* element) noexcept
	{
		assert(element != nullptr);
		Node& node = Accessor::node(*element);
		if (element == _root) {
			assert(node.prev == nullptr);
			if (node.next != nullptr) {
				Accessor::node(*node.next).prev = nullptr;
				_root = node.next;
			}
			else {
				_root = nullptr;
			}
		} else {
			assert(node.prev != nullptr);
			Accessor::node(*node.prev).next = node.next;
			if (node.next != nullptr) {
				Accessor::node(*node.next).prev = node.prev;
			}
		}
		node.clear();
	}

	constexpr Iterator begin() const noexcept { return Iterator(_root); }

	constexpr Iterator end() const noexcept { return Iterator(nullptr); }

	constexpr ConstIterator cbegin() const noexcept { return ConstIterator(_root); }

	constexpr ConstIterator cend() const noexcept { return ConstIterator(nullptr); }

	constexpr bool empty() const noexcept { return _root == nullptr; }

private:
	T* _root;
};

} // namespace OMR

#endif // OMR_INTRUSIVELIST_HPP_
