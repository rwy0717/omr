

#if !defined OMR_GC_POINTEROPERATIONS_HPP_
#define OMR_GC_POINTEROPERATIONS_HPP_

#include <OMR/GC/CRTP.hpp>

namespace OMR {
namespace GC {

/// Define accessor operations if T is not void
template<typename T, typename BaseT>
class PointerOperations : public CRTP<BaseT> {
public:
	constexpr T* operator->() const noexcept { return this->base()->get(); }

	constexpr T& operator*() const noexcept { return *this->base()->get(); }

	constexpr T
};

template<typename BaseT>
class PointerOperations<void, BaseT> : public CRTP<BaseT> {};

} // namespace GC
} // namespace OMR

#endif // OMR_GC_POINTEROPERATIONS_HPP_
