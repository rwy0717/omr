
#if !defined(OMR_GC_DYNAMICROOTHEAP_HPP_)
#define OMR_GC_DYNAMICROOTHEAP_HPP_

#include <OMR/GC/Handle.hpp>
#include <OMR/GC/PointerOperators.hpp>

#include <unordered_set>
#include <AtomicSupport.hpp>

namespace OMR {
namespace GC {

struct DynamicRootNode {
	void* ref;
};

class DynamicRootFlatHeap {
	allocate()

	free()
};

class DynamicRoot;

class DynamicRootSet {
public:

	DynamicRootSet(std::size_t capacity)
		: _capacity(capacity) {
		_storage = new DynamicRoot*[_capacity];
	}

	~DynamicRootSet() noexcept {
		delete _storage;
	}

	/// insert an element into the set. Return the unique id.
	std::size_t insert(DynamicRoot* element) noexcept {
		std::size_t start = index(element);

		for (std::size_t i = 0; i < _capacity; ++i) {
			std::size_t idx = 
		}
	}

	std::size_t remove(std::size_t index) {
		_storage[index] = 0;
	}

	template <typename VisitorT>
	visit(VisitorT&& visitor) {
		for (std::size_t i = 0; i < )
		for (auto nptr : _set) {
			visitor.edge(nullptr, RefSlotHandle(&node->ref));
		}
	}

private:

	/// Hash the node's address
	std::size_t hash(DynamicRootNode* node) {
		return hash(std::size_t(node));
	}

	/// Stolen from preshing.com
	std::size_t hash(std::size_t data) {
		data ^= data >> 16;
		data *= 0x85ebca6b;
		data ^= data >> 13;
		data *= 0xc2b2ae35;
		data ^= data >> 16;
		return data;
	}

	std::size_t index(DynamicRootNode* node) {
		return index(hash(node));
	}

	std::size_t index(std::size_t hash) {
		return hash & (_capacity - 1);
	}

	std::size_t _capacity;
	DynamicRootNode* _storage;
};

template<typename T>
class DynamicRootBase
{
public:

	DynamicRootBase(DynamicRootNode* node, DynamicRootHeap* heap)
		: _node(node)
		, _heap(heap) {}

	T* get() const noexcept { return reinterpret_cast<T*>(_node->ref); }

protected:
	DynamicRootNode* _node;
	DynamicRootHeap* _heap;
};

/// This mixin adds Pointer-style operations to the StackRoot.
template<typename T, bool DEFINE_OPERATORS = CanDereference<T*>::value>
class DynamicRootOps : public StackRootBase<T>
{
public:
	using DynamicRootBase<T>::DynamicRootBase;

	T* operator->() const noexcept { return this->get(); }

	T& operator*() const noexcept { return *this->get(); }
};

/// This specialization removes pointer operations for void referents.
/// You cannot dereference a StackRoot<void>.
template<typename T>
class DynamicRootOps<T, false> : public DynamicRootBase<T>
{
public:
	using DynamicRootBase<T>::DynamicRootBase;
};

template <typename T>
class DynamicRoot : public DynamicRootOps<T> {
public:
	DynamicRoot(Context& cx, void* ref = nullptr)
		: DynamicRootOps(&cx.dynamicRootHeap(), cx.dynamicRootHeap().createNode(ref))
	{}

	DynamicRoot(const DynamicRoot& rhs)
		: DynamicRootOps(rhs.heap, rhs.heap->createNode(rhs.node->ref))
	{}

	DynamicRoot(DynamicRoot&& rhs)
		: DynamicRootOps(rhs.heap, rhs.node)
	{
		rhs.node = nullptr;
	}

	~DynamicRoot() noexcept {
		if (node) {
			_heap->deleteNode(node);
		}
	}
};

} // namespace GC
} // namespace OMR

#endif // OMR_GC_DYNAMICROOTHEAP_HPP_
