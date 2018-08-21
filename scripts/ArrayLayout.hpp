#if !defined OMR_OM_ARRAY_HPP_
#defined OMR_OM_ARRAY_HPP_

#include <OMR/Om/SlotIndex.hpp>
#include <OMR/Om/SlotAttr.hpp>
#include <OMR/Om/Shape.hpp>

namespace OMR {
namespace Om {

class ArrayShape {

	SlotAttr instanceSlotAttr_;
	std::size_t length_;
};

class ArrayDescription {
	std::size_t elements;
};

/// Fixed length array.
class Array {
public:

	static std::size_t cellSize(const SlotAttr& attr, std::size_t n) {
		return sizeof Array + (attr.width() * n);
	}

	CellHeader& header() { return header_; }

	const CellHeader& header() const { return header_; }

	Shape* layout() const { return header().layout(); }

	const SlotAttr& instanceSlotAttr() const noexcept { return instanceSlotAttr_; }

	SlotHandle
	Byte data_[0];
};

class ArrayInitializer : public Initializer {

};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAY_HPP_
