#ifndef OMR_OM_BASESCANNER_HPP_
#define OMR_OM_BASESCANNER_HPP_

#include <OMR/Om/Any.hpp>
#include <OMR/Om/BaseScanner.hpp>

namespace OMR {
namespace Om {

/// Base scanner class. First element is a Ref to T.
template<typename T>
class BaseScanner {
public:
	BaseScanner() noexcept = default;

	constexpr BaseScanner(T* target) noexcept : target_(target) {}

	constexpr T* target() const noexcept { return target_; }

protected:
	/// Set the target.
	void target(T* target) noexcept { target_ = target; }

	T* target_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_BASESCANNER_HPP_
