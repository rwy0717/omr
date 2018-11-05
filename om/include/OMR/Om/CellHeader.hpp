/*******************************************************************************
 *  Copyright (c) 2018, 2018 IBM and others
 *
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License 2.0 which accompanies this
 *  distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 *  or the Apache License, Version 2.0 which accompanies this distribution and
 *  is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  This Source Code may also be made available under the following
 *  Secondary Licenses when the conditions for such availability set
 *  forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 *  General Public License, version 2 with the GNU Classpath
 *  Exception [1] and GNU General Public License, version 2 with the
 *  OpenJDK Assembly Exception [2].
 *
 *  [1] https://www.gnu.org/software/classpath/license.html
 *  [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#if !defined(OMR_OM_CELLHEADER_HPP_)
#define OMR_OM_CELLHEADER_HPP_

#include <OMR/Om/CellKind.hpp>

#include <cassert> // for broken atomicWriteReference
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <type_traits>

namespace OMR {
namespace Om {

class Cell;
class Shape;
class Any;

using LayoutId = std::uint64_t;

/// Encodes a pointer to the shape that lays out this cell, and GC flags.
/// 
/// Encoding:
///
/// | Range   | Size | Property    |
/// |---------|------|-------------|
/// | 15 - 47 |   48 | layout ptr  |
/// | 12 - 14 |   03 | layout xtra |
/// | 08 - 11 |   04 | cell kind   |
/// | 00 - 07 |   08 | gc flags    |
///
/// | Layout ID                   |
/// |-----------------------------|
/// | Layout      | Xtra | Kind   |
/// |-------------|---------------|
class CellHeader {
public:
	static constexpr std::size_t LAYOUT_SHIFT = 15;
	static constexpr std::size_t LAYOUT_MASK = 0x00;  // 0x0000'0000'7FFF'FFFF'FFFF'FFFF'FFFF'FFFF;
	static constexpr std::size_t KIND_SHIFT = 8;
	static constexpr std::size_t KIND_MASK = 0xF;
	static constexpr std::size_t FLAGS_MASK = 0xFF;

	static std::uintptr_t encode(CellKind kind, Shape* layout, std::uint8_t flags = 0) {
		return std::uintptr_t(layout) << LAYOUT_SHIFT | std::uintptr_t(kind) << KIND_SHIFT | std::uintptr_t(flags);
	}

	CellHeader() = default;

	explicit CellHeader(CellKind kind, Shape* layout = nullptr, std::uint8_t flags = 0)
	        : data_(encode(kind, layout, flags)) {}

	/// Set the shape and the flags.
	CellHeader& set( CellKind kind, Shape* layout, std::uint8_t flags) noexcept {
		data_ = encode(kind, layout, flags);
		return *this;
	}

	CellKind kind() const noexcept { return CellKind((data_ >> KIND_SHIFT) & KIND_MASK); }

	CellHeader& kind(CellKind& kind) noexcept { return set(kind, layout(), flags()); }

	/// Get the shape that describes this cell's layout.
	Shape* layout() const noexcept {
		return reinterpret_cast<Shape*>(data_ >> LAYOUT_SHIFT);
	}

	/// Set the shape reference. No write barrier.
	CellHeader& layout(Shape* shape) noexcept { return set(kind(), shape, flags()); }

	/// The flags are a byte of reserved data for the OMR GC. Undefined value.
	std::uint8_t flags() const noexcept { return std::uint8_t(data_ & FLAGS_MASK); }

	CellHeader& flags(std::uint8_t flags) noexcept { return set(kind(), layout(), flags); }

protected:
	friend struct CellHeaderOffsets;

	std::uintptr_t data_;
};

static_assert(std::is_standard_layout<CellHeader>::value,
              "CellHeader must be a standard layout type");

struct CellHeaderOffsets {
	static constexpr std::size_t DATA = offsetof(CellHeader, data_);
};

/// A Handle to the layout ref in a Cell's header. This is a special kind of SlotHandle used to
/// read/write a cell header's layout reference from the header. Used by the GC to fixup the layout
/// pointer.
class CellHeaderLayoutHandle {
public:
	constexpr explicit CellHeaderLayoutHandle(CellHeader* header) noexcept : header_(header) {}

	void writeReference(void* ref) const noexcept { header_->layout((Shape*)ref); }

	/// TODO: Implement atomics
	void atomicWriteReference(void* ref) const noexcept { assert(0); }

	Any* readReference() const noexcept { return reinterpret_cast<Any*>(header_->layout()); }

private:
	CellHeader* header_;
};

template<typename VisitorT>
bool visitHeader(CellHeader& header, VisitorT& visitor) {
	return visitor.edge(&header, CellHeaderLayoutHandle(&header));
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CELLHEADER_HPP_
