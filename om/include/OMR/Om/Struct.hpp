#if !defined(OMR_OM_STRUCT_HPP_)
#define OMR_OM_STRUCT_HPP_

#include <OMR/Om/StructCell.hpp>

namespace OMR {
namespace Om {

struct SlotLookup {
	SlotDescriptor descriptor;
	bool found = false;
};

class Struct {
public:
	static SlotLookup lookup(Context& cx, Struct* s, Id id) {
	}

	static void setPolyval(Context& cx, Struct* s, SlotIndex index) {

	}

	static void getPolyval(Context& cx, Struct* s, SlotIndex index) {}

	static void setInt8(Context& cx, Struct* s, SlotIndex index) {

	}

	static void setRef(Context& cx, Struct* s, SlotIndex index) {

	}

	static std::uint8_t* slotAddress()

private:
	StructCell cell_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_STRUCT_HPP_
