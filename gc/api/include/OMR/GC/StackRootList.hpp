#if !defined(OMR_GC_STACKROOTLIST_HPP_)
#define OMR_GC_STACKROOTLIST_HPP_

namespace OMR {
namespace GC {

/// A node in a StackRootList.
struct StackRootListNode {

	StackRootListNode(void* ref, StackRootListNode* tail) : ref(ref), tail(tail) {}

	void clear() noexcept {
		ref = nullptr;
		tail = nullptr;
	}

	void* ref;
	StackRootListNode* tail;
};

/// A foward iterator through a StackRootList.
/// @see-also: StackRootList::begin()
class StackRootListIterator {
public:
	constexpr StackRootListIterator(const StackRootListIterator& other) noexcept
	        : current_(other.current_) {}

	constexpr const StackRootListNode& operator*() const noexcept { return *current_; }

	constexpr const StackRootListNode* operator->() const noexcept { return current_; }

	StackRootListIterator& operator++() noexcept {
		current_ = current_->tail;
		return *this;
	}

	StackRootListIterator operator++(int) noexcept {
		StackRootListIterator tmp(*this);
		current_ = current_->tail;
		return tmp;
	}

	constexpr bool operator==(const StackRootListIterator& rhs) const noexcept {
		return current_ == rhs.current_;
	}

	constexpr bool operator!=(const StackRootListIterator& rhs) const noexcept {
		return current_ != rhs.current_;
	}

	StackRootListIterator& operator=(const StackRootListIterator& rhs) noexcept {
		current_ = rhs.current_;
		return *this;
	}

protected:
	friend class StackRootList;
	friend class ConstStackRootListIterator;

	constexpr StackRootListIterator() noexcept : current_(nullptr) {}

	constexpr StackRootListIterator(const StackRootListNode* start) noexcept
	        : current_(start) {}

private:
	const StackRootListNode* current_;
};

/// A forward iterator through a const StackRootList.
/// @see-also: StackRootList::cbegin()
class ConstStackRootListIterator {
public:
	constexpr ConstStackRootListIterator(const ConstStackRootListIterator& other) noexcept
	        : current_(other.current_) {}

	constexpr ConstStackRootListIterator(const StackRootListIterator& other) noexcept
	        : current_(other.current_) {}

	constexpr const StackRootListNode& operator*() const noexcept { return *current_; }

	constexpr const StackRootListNode* operator->() const noexcept { return current_; }

	ConstStackRootListIterator& operator++() noexcept {
		current_ = current_->tail;
		return *this;
	}

	ConstStackRootListIterator operator++(int) noexcept {
		ConstStackRootListIterator tmp(*this);
		current_ = current_->tail;
		return tmp;
	}

	constexpr bool operator==(const ConstStackRootListIterator& rhs) const noexcept {
		return current_ == rhs.current_;
	}

	constexpr bool operator!=(const ConstStackRootListIterator& rhs) const noexcept {
		return current_ != rhs.current_;
	}

	constexpr bool operator==(const StackRootListIterator& rhs) const noexcept {
		return current_ == rhs.current_;
	}

	constexpr bool operator!=(const StackRootListIterator& rhs) const noexcept {
		return current_ != rhs.current_;
	}

	ConstStackRootListIterator& operator=(const ConstStackRootListIterator& rhs) noexcept {
		current_ = rhs.current_;
		return *this;
	}

	ConstStackRootListIterator& operator=(const StackRootListIterator& rhs) noexcept {
		current_ = rhs.current_;
		return *this;
	}

protected:
	friend class StackRootList;

	constexpr ConstStackRootListIterator() noexcept : current_(nullptr) {}

	constexpr ConstStackRootListIterator(const StackRootListNode* start) noexcept
	        : current_(start) {}

private:
	const StackRootListNode* current_;
};

/// The StackRootList is an untyped linked list of rooted, untagged pointers.
/// Nodes are manually added and removed from the list.
/// The StackRootList explicitly tracks untyped node elements.
/// Users should not use StackRootList directly, instead root objects using the automatic RootRef
/// type, or register a RootFn callback.
class StackRootList {
public:
	constexpr StackRootList() noexcept : head_(nullptr) {}

	StackRootListIterator begin() noexcept { return StackRootListIterator(head()); }

	StackRootListIterator end() noexcept { return StackRootListIterator(nullptr); }

	ConstStackRootListIterator begin() const noexcept { return cbegin(); }

	ConstStackRootListIterator end() const noexcept { return cend(); }

	ConstStackRootListIterator cbegin() const noexcept {
		return ConstStackRootListIterator(head());
	}

	ConstStackRootListIterator cend() const noexcept {
		return ConstStackRootListIterator(nullptr);
	}

	StackRootListNode* head() const noexcept { return head_; }

	StackRootList& head(StackRootListNode* head) noexcept {
		head_ = head;
		return *this;
	}

	StackRootList& push(StackRootListNode* node) noexcept {
		node->tail = head_;
		head_ = node;
		return *this;
	}

private:
	StackRootListNode* head_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ROOTLIST_HPP_
