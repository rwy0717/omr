#if !defined(OMR_OM_BASICSLOTHANDLE_HPP_)
#define OMR_OM_BASICSLOTHANDLE_HPP_

#include <cassert>

namespace OMR {
namespace Om {

class Any;

/// A pointer into a object's slot.
class BasicSlotHandle {
public:
	BasicSlotHandle() noexcept = default;

	constexpr BasicSlotHandle(const BasicSlotHandle&) = default;

	constexpr BasicSlotHandle(void* slot) noexcept : slot_((Any**)slot) {}

	constexpr BasicSlotHandle(Any** slot) noexcept : slot_(slot) {}

	void writeReference(Any* ref) const noexcept { *slot_ = ref; }

	void atomicWriteReference(Any* ref) const noexcept { assert(0); }

	constexpr Any* readReference() const noexcept { return *slot_; }

private:
	Any** slot_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_BASICSLOTHANDLE_HPP_
