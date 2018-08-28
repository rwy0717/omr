#ifndef OMR_OM_REFSLOTHANDLE_HPP_
#define OMR_OM_REFSLOTHANDLE_HPP_

#include <OMR/GC/Ref.hpp>

namespace OMR {
namespace Om {

class RefSlotHandle {
public:
	template<typename T>
	RefSlotHandle(Ref<T>* slot) : slot_(reinterpret_cast<Ref<void>> * (slot)) {}

	void* readReference() const noexcept { return slot->get(); }

	void writeReference(void* value) const noexcept { *slot = value; }

	void atomicWriteReference(void* value) const noexcept {
		assert(0);
		// slot->atomicAssign(value);
	}

private:
	Ref<void>* slot_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_REFSLOTHANDLE_HPP_
