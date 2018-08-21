#if !defined OMR_OM_ARRAY_HPP_
#defined OMR_OM_ARRAY_HPP_

#include <OMR/Om/SlotIndex.hpp>

namespace OMR {
namespace Om {

class ArrayShape {

};

class ArrayDescription {
	std::size_t elements;
};

/// Fixed length array.
class Array {
public:

	static std::size_t cellSize(std::size_t inlineSlotSize);

	CellHeader& header() { return header_; }

	const CellHeader& header() const { return header_; }

	Shape* layout() const { return header().layout(); }

	SlotHandle
	Byte data_[0];

};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAY_HPP_
