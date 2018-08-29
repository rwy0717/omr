#if !defined(OMR_GC_HANDLE_HPP_)
#define OMR_GC_HANDLE_HPP_

#include <cstddef>

namespace OMR {
namespace GC {

template <typename T>
class HandlePtrOps {};

template <>
class HandlePtrOps<void> {};

/// The Handle is an indirect reference to an object through some kind of root object.
/// The classic way to construct a Handle is via a RootRef. The RootRef contains a bare pointer to a
/// Cell. This reference slot will be kept up-to-date through GC safepoints. A Handle can be
/// constructed to indirect through the RootRef. Other rooting mechanisms exist as well.
template<typename T>
class Handle : public HandlePtrOps<T> {
public:
	constexpr Handle() : root_(nullptr) {}

	constexpr Handle(std::nullptr_t) : root_(nullptr) {}

	template<typename U>
	explicit constexpr Handle(U* const& root) : root_(&root) {}

	template<typename U>
	constexpr Handle(U* const* rootAddr) : root_(rootAddr) {}

	template<typename U>
	constexpr Handle(Handle<U> other) : root_(reinterpret_cast<T* const*>(other.raw())) {}

#if 0
  template <typename U>
  constexpr Handle(const RootRef<U>& root)
      : root_(reinterpret_cast<T* const*>(root.address())) {}
#endif

	T* get() const noexcept { return *root_; }

	T* const* raw() const { return root_; }

	T& operator*() const { return *ptr(); }

	T* operator->() const noexcept { return ptr(); }

	template<typename U>
	U& operator->*(U T::*mptr) const noexcept {
		return ptr()->*mptr;
	}

	template<typename U>
	Handle<U> reinterpret() const {
		return Handle<U>(reinterpret_cast<U* const*>(root_));
	}

	template<typename U = T>
	constexpr U* ptr() const {
		static_assert(sizeof(U**) == sizeof(T**), "Pointers must be compatible.");
		static_assert(sizeof(U*) == sizeof(T*), "Pointers must be compatible.");
		return *reinterpret_cast<U* const*>(root_);
	}

	template<typename U = T>
	constexpr operator U*() const {
		return ptr<U>();
	}

private:
	T* const* root_;
};

} // namespace GC
} // namespace OMR

#endif // OMR_GC_HANDLE_HPP_
