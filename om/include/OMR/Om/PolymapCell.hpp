
#if !defined(OMR_OM_POLYMAPBASE_HPP_)
#define OMR_OM_POLYMAPBASE_HPP_

#include <OMR/Om/Cell.hpp>
#include <OMR/Om/CellHeader.hpp>
#include <OMR/Om/Id.hpp>
#include <OMR/Om/Value.hpp>

#include <OMR/SizeP2.hpp>

#include <cstdint>
#include <cstdlib>

namespace OMR {
namespace Om {

class PolymapData;
class Polymap;

using PolyVal = Value;

struct PolymapNode {
	Id id;
	PolyVal value;
};

inline void swap(PolymapNode& a, PolymapNode& b) noexcept {
	PolymapNode tmp = a;
	a = b;
	b = tmp;
}

/// A special constant in the polymap indicating a free slot.
constexpr PolymapNode POLYMAP_HOLE_NODE = {HOLE_ID, GARBAGE_VALUE};

/// A PolymapNode array. The backing storage for a Polymap.
/// This structure is only the header, the array of PolymapNodes is allocated past the end.
class PolymapDataCell {
public:
	static constexpr std::size_t allocSize(std::size_t length) {
		return sizeof(PolymapDataCell) + (length * sizeof(PolymapNode));
	}

	CellHeader& header() noexcept { return header_; }

	const CellHeader& header() const noexcept { return header_; }

	std::size_t cellSize() const noexcept { return allocSize(length_); }

	PolymapNode* nodes() noexcept { return (PolymapNode*)(this + 1); }

	const PolymapNode* nodes() const noexcept { return (const PolymapNode*)(this + 1); }

	/// Get the node at index.
	PolymapNode& at(std::size_t index) noexcept { return nodes()[index]; }

	const PolymapNode& at(std::size_t index) const noexcept { return nodes()[index]; }

	/// The total number of nodes we've allocated for.
	std::size_t length() const noexcept { return length_; }

private:
	friend class PolymapDataInit;
	friend class PolymapData;
	friend class PolymapDataScanner;

	CellHeader header_;
	std::size_t length_;
};

static_assert(std::is_standard_layout<PolymapDataCell>::value, 
	"The PolymapDataCell must be a standard layout type");

/// A map of Id->PolyVal
/// An open-addressed power-of-two table, with bounded probecount + robinhood hashing
/// idea
class PolymapCell {
public:

	static std::size_t allocSize() { return sizeof(PolymapCell); }

	CellHeader& header() noexcept { return header_; }

	const CellHeader& header() const noexcept { return header_; }

	std::size_t cellSize() const noexcept { return allocSize(); }

protected:
	friend class PolymapInit;
	friend class PolymapScanner;

	PolymapCell() : header_(), capacity_(0) {}

	CellHeader header_;

	/// The maximum index is a power of two. This is critical for fast id->index calculations.
	/// An ID is translated to an index in the 
	/// We actually overallocate
	SizeP2 capacity_;

	/// The number of elements actually stored in the table.
	std::size_t nodeCount_;

	/// The data of the map is actually stored in a side-table, allocated on the heap.
	PolymapData* data_;
};

static_assert(std::is_standard_layout<PolymapCell>::value,
	"Polymap must be a standard layout type");

}  // namespace Om
}  // namespace OMR

#endif // OMR_OM_POLYMAPBASE_HPP_
