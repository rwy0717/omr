#if !defined(OMR_GC_STACKROOT_HPP_)
#define OMR_GC_STACKROOT_HPP_

#include <OMR/GC/Handle.hpp>
#include <OMR/GC/StackRootList.hpp>
#include <OMR/GC/System.hpp>

namespace OMR {
namespace GC {

template<typename T>
class StackRoot;

/// This mixin adds Pointer-style operations to the StackRoot.
template<typename T>
class StackRootPtrOps {
public:
	T* operator->() const noexcept { return self()->get(); }

	T& operator*() const noexcept { return *self()->get(); }

private:
	friend class StackRoot<T>;

	StackRootPtrOps() = default;

	StackRoot<T>* self() { return static_cast<StackRoot<T>*>(this); }

	const StackRoot<T>* self() const { return static_cast<const StackRoot<T>*>(this); }
};

/// This specialization removes pointer operations for void referents.
/// You cannot dereference a StackRoot<void>.
template<>
class StackRootPtrOps<void> {};

/// A rooted GC pointer. T must be a managed allocation. This class must be
/// stack allocated. StackRoots push themselves onto the linked list of stack
/// roots at construction. When A GC occurs, the sequence of StackRoots will be
/// walked, the referants marked, and the references fixed up. StackRoots have a
/// strict LIFO lifetime, when a StackRoot is destroyed, it must be the head of
/// the list.
///
/// For users of Om: this is your primary mechanism for rooting references across
/// GC boundaries.
template<typename T>
class StackRoot : public StackRootPtrOps<T> {
public:
	/// StackRoots must be stack allocated, so the new operator is explicitly removed.
	void* operator new(std::size_t) = delete;

	StackRoot(StackRootList& list, T* ref) noexcept : list_(list), node_(ref, list.head()) {
		list_.head(&node_);
	}

	/// Attach a new StackRoot to the context's stackRoots.
	explicit StackRoot(Context& cx, T* ref = nullptr) noexcept
	        : StackRoot(cx.stackRoots(), ref) {}

	/// Create a new StackRoot that refers to the same object as an existing StackRoot. The new
	/// StackRoot is attached to the head of the list.
	template<typename U>
	explicit StackRoot(const StackRoot<U>& other) noexcept
	        : StackRoot(other.list(), other.ref()) {}

	/// Move from other StackRoot. The StackRoot we are moving from must be the head
	/// of the List. The moved-from StackRoot is invalidated and cleared. This new StackRoot
	/// replaces the old StackRoot in the RootList.
	template<typename U>
	explicit StackRoot(StackRoot<U>&& other) noexcept
	        : list_(other.list()), node_(other.node()) {
		other.clear();
	}

	~StackRoot() noexcept {
		assert(isHead());
		list_.head(node_.tail);
	}

	/// Obtain the underlying pointer.
	template<typename U = T>
	U* get() const noexcept {
		return reinterpret_cast<U*>(node_.ref);
	}

	StackRoot& operator=(T* p) noexcept {
		node_.ref = p;
		return *this;
	}

	/// Comparison of the referent.
	bool operator==(const StackRoot& rhs) const noexcept { return get() == rhs.get(); }

	bool operator==(T* ptr) const noexcept { return get() == ptr; }

	/// The address of the reference slot in this StackRoot.
	template<typename U = T>
	U** address() noexcept {
		return reinterpret_cast<U**>(&node_.ref);
	}

	template<typename U = T>
	U* const* address() const noexcept {
		return reinterpret_cast<U* const*>(&node_.ref);
	}

	bool isHead() const noexcept { return list_.head() == &node_; }

	StackRootList& list() const noexcept { return list_; }

	StackRootListNode& node() noexcept { return node_; }

	const StackRootListNode& node() const noexcept { return node_; }

	StackRootListNode* tail() const noexcept { return node_.tail; }

	/// The StackRoot freely degrades into a Handle.
	operator Handle<T>() const { return Handle<T>(address()); }

	/// The StackRoot degrades into a raw pointer.
	operator T*() const { return get(); }

private:
	void clear() noexcept { node_.clear(); }

	StackRootList& list_;
	StackRootListNode node_;
};

template<typename T, typename U>
T get(StackRoot<U>& root) {
	return root.template get<T>();
}

} // namespace GC
} // namespace OMR

#endif // OMR_OM_StackRoot_HPP_
