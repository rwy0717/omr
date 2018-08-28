#if !defined(OMR_GC_REF_HPP_)
#define OMR_GC_REF_HPP_

#include <OMR/GC/Address.hpp>

#include <cstdint>
#include <type_traits>

namespace OMR {
namespace GC {

template<typename T>
class Ref;

/// Define accessor operations if T is not void
template<typename T>
class RefOperations {
public:
	constexpr T* operator->() const { return static_cast<const Ref<T>*>(this)->get(); }

	constexpr T& operator*() const { return *static_cast<const Ref<T>*>(this)->get(); }
};

/// Empty operation set for T
template<>
class RefOperations<void> {};

template<>
class RefOperations<const void> {};

/// A pointer to managed memory. It is GC-Unsafe. This is a very simple
/// pointer-wrapper.
template<typename T>
class Ref : public RefOperations<T> {
public:
	Ref() = default;

	constexpr Ref(std::nullptr_t) : value_(nullptr) {}

	template<typename U>
	constexpr Ref(U* ptr) : value_(ptr) {}

	template<typename U>
	constexpr Ref(Ref<U> other) : value_(other.get()) {
		static_assert(std::is_convertible<U*, T*>::value,
		              "U* must be convertible to T* "
		              "If not, use castRef<U> to explicitly cast.");
	}

	template<typename U>
	Ref<T>& operator=(U* rhs) noexcept {
		value_ = rhs;
	}

	template<typename U>
	Ref<T> operator=(const Ref<U>& rhs) noexcept {
		value_ = rhs.get();
	}

	/// Obtain the raw pointer
	constexpr T* get() const noexcept { return value_; }

	/// The address of the referrent.
	Address toAddress() const { return reinterpret_cast<Address>(value_); }

	/// @group Comparisons
	/// @{

	template<typename U>
	constexpr bool operator==(const Ref<U>& rhs) const noexcept {
		return value_ == rhs.get();
	}

	template<typename U>
	constexpr bool operator==(U* rhs) const noexcept {
		return value_ == rhs;
	}

	constexpr bool operator==(std::nullptr_t) const noexcept { return value_ == nullptr; }

	template<typename U>
	constexpr bool operator!=(const Ref<U>& rhs) const noexcept {
		return value_ != rhs.get();
	}

	template<typename U>
	constexpr bool operator!=(U* rhs) const noexcept {
		return value_ != rhs;
	}

	constexpr bool operator!=(std::nullptr_t) const noexcept { return value_ != nullptr; }

	constexpr bool operator<(const Ref<T>& rhs) const noexcept { return value_ != rhs.get(); }

	/// @todo: need more comparison operators.

	/// @}

	/// @group Type conversion
	/// @{

	/// Ref<T> can be cast to U*.
	explicit constexpr operator T*() const noexcept { return value_; }

	/// Ref<T> is convertible to Ref<U>.
	template<typename U>
	explicit operator Ref<U>() const noexcept {
		static_assert(std::is_convertible<U*, T*>::value,
		              "T* must be convertible to U*. "
		              "If not, use castRef<U> to explicitly cast.");
		return Ref<U>(value_);
	}

	/// Ref<T> is implicitly convertible to Ref<const T>.
	operator Ref<const T>() const noexcept {
		return Ref<const T>(const_cast<const T*>(value_));
	}

	/// @}

private:
	T* value_;
};

/// Make a Ref<T>
template<typename T>
constexpr Ref<T> makeRef(T* ref) noexcept {
	return Ref<T>(ref);
}

template<typename T>
constexpr Ref<T> makeRef(Address address) noexcept {
	return Ref<T>(reinterpret_cast<T*>(address));
}

/// Cast the argument from Ref<U> to Ref<T>
template<typename T, typename U>
constexpr Ref<T> reinterpretCastRef(Ref<U> ref) {
	return Ref<T>(reinterpret_cast<T*>(ref.get()));
}

/// C-style cast the Ref from Ref<U> to Ref<T>
template<typename T, typename U>
constexpr Ref<T> castRef(Ref<U> ref) {
	return Ref<T>((T*)ref.get());
}

template<typename T>
constexpr bool operator==(std::nullptr_t, Ref<T> ref) {
	return ref == nullptr;
}

template<typename T>
constexpr bool operator!=(std::nullptr_t, Ref<T> ref) {
	return ref != nullptr;
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_REF_HPP_
