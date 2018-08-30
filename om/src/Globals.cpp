#include <OMR/Om/Globals.hpp>
#include <OMR/Om/ArrayOperations.hpp>
#include <OMR/Om/ShapeOperations.hpp>

namespace OMR {
namespace Om {

bool Globals::init(StartupContext& cx) {
	// The order of allocation matters. The MetaShape must be allocated before any
	// other shape.
	metaShape_ = allocateShapeLayout(cx);
	if (metaShape_ == nullptr) {
		return false;
	}

	// Tricky note: The object shape's transition table is backed by an array
	// buffer. If the ArrayBufferShape isn't brought up first, than the first
	// empty object shape allocated will not be able to allocate it's transition
	// table.
	arrayBufferShape_ = allocateArrayLayout(cx);
	if (arrayBufferShape_ == nullptr) {
		return false;
	}

	return true;
}

} // namespace Om
} // namespace OMR
