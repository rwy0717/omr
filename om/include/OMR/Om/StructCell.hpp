#if !defined(OMR_OM_STRUCTCELL_HPP_)
#define OMR_OM_STRUCTCELL_HPP_

#include <OMR/Om/CellHeader.hpp>
#include <OMR/Om/CellKind.hpp>
#include <OMR/Om/Shape.hpp>

#include <type_traits>
#include <cstdint>
#include <cstddef>

namespace OMR {
namespace Om {

class Shape;

/// The basic layout of a Struct object. Higher level operations are defined by the Struct class.
class StructCell {
private:
	friend class Struct;
	friend class StructScanner;
	friend class StructOffsetTable;

	static constexpr std::size_t allocSize(std::size_t dataSize) {
		return dataSize + sizeof(StructCell);
	}

	StructCell(Shape* shape)
		: header_(CellKind::STRUCT, shape) {}

	CellHeader& header() noexcept { return header_; }
	
	const CellHeader& header() const noexcept { return header_; }

	Shape* layout() const noexcept { return header().layout(); }

	std::size_t cellSize() const noexcept { return allocSize(layout()->instanceInlineSlotSize()); }

	std::uint8_t* data() noexcept { return (this + 1); }

	const std::uint8_t* data() const { return reinterpret_cast<const std::uint8_t*>(this + 1); }

	CellHeader header_;
};

static_assert(std::is_standard_layout<StructCell>::value,
	"StructCell must be a standard layout type");

class StructOffsetTable {
	static constexpr std::size_t HEADER = offsetof(StructCell, header_);
};


class StructArrayCell {
private:
	friend class StructArray;
	friend class StructArrayScanner;
	friend class StructArrayOffsetTable;

	static constexpr std::size_t allocSize(std::size_t dataSize, std::size_t length) {
		return sizeof(StructArrayCell) + (dataSize * length);
	}

	StructArrayCell(Shape* shape)
		: header_(CellKind::STRUCT_ARRAY, shape) {}

	CellHeader& header() noexcept { return header_; }
	
	const CellHeader& header() const noexcept { return header_; }

	Shape* layout() const noexcept { return header().layout(); }

	std::size_t elementSize() const noexcept { return allocSize(layout()->instanceInlineSlotSize()); }

	std::uint8_t* data() noexcept { return (this + 1); }

	const std::uint8_t* data() const { return reinterpret_cast<const std::uint8_t*>(this + 1); }

	std::uint8_t* at(std::size_t index) noexcept { return data() + (elementSize() * index); }

	const std::uint8_t* at(std::size_t index) const noexcept { return data() + (elementSize() * index); }

	CellHeader header_;
	std::size_t length_; // in elements
};

}  // namespace Om
}  // namespace OMR

#endif // OMR_OM_STRUCTCELL_HPP_
