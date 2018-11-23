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
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR
 *LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef OMR_OM_ANYSCANNER_HPP_
#define OMR_OM_ANYSCANNER_HPP_

#include <OMR/Om/Any.hpp>
#include <OMR/Om/ArrayScanner.hpp>
#include <OMR/Om/ObjectScanner.hpp>
#include <OMR/Om/PolymapScanner.hpp>
#include <OMR/Om/ShapeScanner.hpp>

#include <OMR/GC/ScanResult.hpp>

namespace OMR {
namespace Om {

union SubscannerUnion {

	SubscannerUnion() : base() {}

	BaseScanner<Any> base;
	ObjectScanner objectScanner;
	ArrayScanner arrayScanner;
	ShapeScanner shapeScanner;
	PolymapScanner polymapScanner;
	PolymapDataScanner polymapDataScanner;
};

struct Subscanner {
public:
	Any* target() const noexcept { return as.base.target(); }

	template<typename VisitorT>
	GC::ScanResult startObject(VisitorT& visitor, DynObjectCell* target, std::size_t todo) {
		new (&as.objectScanner) ObjectScanner();
		return as.objectScanner.start(visitor, target, todo);
	}

	template<typename VisitorT>
	GC::ScanResult resumeObject(VisitorT& visitor, std::size_t todo) {
		return as.objectScanner.resume(visitor);
	}

	template<typename VisitorT>
	GC::ScanResult startArray(VisitorT& visitor, Array* tgt, std::size_t todo) {
		new (&as.arrayScanner) ArrayScanner();
		return as.arrayScanner.start(visitor, tgt, todo);
	}

	template<typename VisitorT>
	GC::ScanResult resumeArray(VisitorT& visitor, std::size_t todo) {
		return as.arrayScanner.resume(visitor, todo);
	}

	template<typename VisitorT>
	GC::ScanResult startShape(VisitorT& visitor, Shape* tgt, std::size_t todo) {
		new (&as.shapeScanner) ShapeScanner();
		return as.shapeScanner.start(visitor, tgt, todo);
	}

	template<typename VisitorT>
	GC::ScanResult resumeShape(VisitorT& visitor, std::size_t todo) {
		return as.shapeScanner.resume(visitor, todo);
	}

	template<typename VisitorT>
	GC::ScanResult startPolymap(VisitorT& visitor, PolymapCell* tgt, std::size_t todo) {
		new (&as.polymapScanner) PolymapScanner();
		return as.polymapScanner.start(visitor, tgt, todo);
	}

	template<typename VisitorT>
	GC::ScanResult resumePolymap(VisitorT& visitor, std::size_t todo) {
		return as.polymapScanner.resume(visitor, todo);
	}

	template<typename VisitorT>
	GC::ScanResult startPolymapData(VisitorT& visitor, PolymapDataCell* tgt, std::size_t todo) {
		new (&as.polymapDataScanner) PolymapDataScanner();
		return as.polymapDataScanner.start(visitor, tgt, todo);
	}

	template<typename VisitorT>
	GC::ScanResult resumePolymapData(VisitorT& visitor, std::size_t todo) {
		return as.polymapDataScanner.resume(visitor, todo);
	}

private:
	SubscannerUnion as;
};

/// A scannner capable of scanning any cell type.
/// Dispatches on the base type
class AnyScanner {
public:
	AnyScanner() = default;

	/// Start scanning a cell. Returns true if there are more slots to scan.
	template<typename VisitorT>
	OMR::GC::ScanResult start(VisitorT&& visitor, Any* tgt, std::size_t todo = SIZE_MAX) {
		switch (tgt->kind()) {
		case CellKind::OBJECT: return sub_.startObject(visitor, &tgt->as.object, todo);
		case CellKind::ARRAY: return sub_.startArray(visitor, &tgt->as.array, todo);
		case CellKind::SHAPE: return sub_.startShape(visitor, &tgt->as.shape, todo);
		case CellKind::POLYMAP: return sub_.startPolymap(visitor, &tgt->as.polymap, todo);
		case CellKind::POLYMAP_DATA: return sub_.startPolymapData(visitor, &tgt->as.polymapData, todo);
		default:
			assert(0); // unreachable
			return {0, true};
		}
	}

	/// Continue scanning an existing cell.
	template<typename VisitorT>
	OMR::GC::ScanResult resume(VisitorT&& visitor, std::size_t todo = SIZE_MAX) {
		assert(target() != nullptr);
		switch (target()->kind()) {
		case CellKind::OBJECT: return sub_.resumeObject(visitor, todo);
		case CellKind::ARRAY: return sub_.resumeArray(visitor, todo);
		case CellKind::SHAPE: return sub_.resumeShape(visitor, todo);
		case CellKind::POLYMAP: return sub_.resumePolymap(visitor, todo);
		case CellKind::POLYMAP_DATA: return sub_.resumePolymapData(visitor, todo);
		}
	}

	constexpr Any* target() const { return sub_.target(); }

private:
	Subscanner sub_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ANYSCANNER_HPP_
