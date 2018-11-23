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

#ifndef OMR_OM_DYNOBJECT_HPP_
#define OMR_OM_DYNOBJECT_HPP_

#include <OMR/Om/Allocator.hpp>
#include <OMR/Om/Array.hpp>
#include <OMR/Om/Barrier.hpp>
#include <OMR/Om/BasicSlotHandle.hpp>
#include <OMR/Om/CellHeader.hpp>
#include <OMR/Om/DynObjectCell.hpp>
#include <OMR/Om/LayoutTree.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/ShapeOperations.hpp>
#include <OMR/Om/SlotIndex.hpp>
#include <OMR/Om/Value.hpp>

#include <OMR/GC/Allocator.hpp>
#include <OMR/GC/StackRoot.hpp>
#include <OMR/GC/Handle.hpp>
#include <type_traits>

namespace OMR {
namespace Om {

class Context;

/// Dynamic DynObject. A struct-like object with growable storage and fast reflection.
class DynObject : public DynObjectCell {
public:
	/// Allocate object with corresponding slot shape;
	static DynObject* allocate(Context& cx, GC::Handle<Shape> layout) {
		Init init;
		init.layout = layout;

		return GC::allocate<DynObject>(
		        cx.gc(), DynObject::cellSize(layout->instanceInlineSlotSize()), init);
	}

	/// Allocate an object with no slots.
	static DynObject*
	allocateEmpty(Context& cx, std::size_t inlineSlotsSize = 32 * sizeof(void*)) {
		GC::StackRoot<Shape> shape(cx.gc());
		shape = allocateRootObjectLayout(cx, {}, inlineSlotsSize);
		return allocate(cx, shape);
	}

	/// TODO: Implement cloneObject
	static DynObject* cloneObject(Context& cx, GC::Handle<DynObject> base) { assert(0); }

	template<typename T = std::uint8_t*>
	static T slotAddress(Context& cx, DynObject* object, SlotIndex index) noexcept {
		return reinterpret_cast<T>(object->DynObjectCell::slotAddress(index));
	}

	/// Slot lookup by Id. The result is a Descriptor, which describes the slot's
	/// location and type.
	static bool
	lookupSlot(Context& cx, const DynObject* object, Id id, SlotDescriptor& result) noexcept {
		for (const Shape& shape : object->layoutTree()) {
			for (const SlotDescriptor descriptor :
			     shape.instanceSlotDescriptorRange()) {
				if (descriptor.attr().id() == id) {
					result = descriptor;
					return true;
				}
			}
		}
		return false;
	}

	/// Read a value from an object's slot.
	static Value getValue(Context& cx, DynObject* object, SlotIndex index) noexcept {
		return *slotAddress<Value*>(cx, object, index);
	}

	/// Store a value into an object's slot. Barriered.
	static void
	setValue(Context& cx, DynObject* object, SlotIndex index, Value value) noexcept {
		auto slotAddr = slotAddress<Value*>(cx, object, index);
		if (value.isRef()) {
			Barrier<ValueSlotHandle>::writeReference(
			        cx, object, ValueSlotHandle(slotAddr), value.getRef());
		} else {
			*slotAddr = value;
		}
	}

	/// Store a value into an object's inline slot. Barriered.
	static void setValueToInlineSlot(Context& cx, DynObject* self, SlotIndex index) noexcept {
		assert(0);
	}

	/// Look up a layout, using a precomputed hash of the slots. Does not GC.
	static Shape* lookUpTransition(Context& cx,
	                               const DynObject* object,
	                               Span<const SlotAttr> attributes,
	                               std::size_t hash) noexcept {
		return Om::lookUpTransition(cx, object->layout(), attributes, hash);
	}

	/// Take an existing transition, without allocating. Barriered.
	static Shape*
	takeExistingTransition(Context& cx, DynObject* object, Span<const SlotAttr> attributes) {
		return takeExistingTransition(cx, object, attributes, hash(attributes));
	}

	/// Look up a layout from the slots to append. Does not GC.
	static Shape* lookUpTransition(Context& cx,
	                               const DynObject* object,
	                               Span<const SlotAttr> attributes) noexcept {
		return lookUpTransition(cx, object, attributes, hash(attributes));
	}

	/// Force set the layout of an object. Barriered.
	static void setLayout(Context& cx, DynObject* object, Shape* layout) noexcept {
		Barrier<CellHeaderLayoutHandle>::writeReference(
		        cx, object, CellHeaderLayoutHandle(&object->header()), layout);
	}

	/// Take an existing transition, without allocating. Barriered.
	static Shape* takeExistingTransition(Context& cx,
	                                     DynObject* object,
	                                     Span<const SlotAttr> attributes,
	                                     std::size_t hash) {
		Shape* derivation = lookUpTransition(cx, object, attributes, hash);
		if (derivation != nullptr) {
			setLayout(cx, object, derivation);
		}
		return derivation;
	}

	// Take a layout/shape transition that hasn't been cached. Before taking a new
	// transition, users should use `lookupTransition` to ensure the transition
	// hasn't been taken before. See also `transition`, a higher level call for
	// transitioning across object layouts. Barriered.
	static Shape* takeNewTransition(Context& cx,
	                                GC::Handle<DynObject> object,
	                                Span<const SlotAttr> attributes,
	                                std::size_t hash) {
		GC::StackRoot<Shape> base(cx.gc(), object->layout());
		Shape* derivation = deriveObjectLayout(cx, base, attributes, hash);
		setLayout(cx, object, derivation); // barrier
		return derivation;
	}

	/// Transition the object's shape by adding a set of new slots
	/// This function will reuse cached transitions. Uses a precomputed hash for the attributes.
	static Shape* transitionLayout(Context& cx,
	                               GC::Handle<DynObject> object,
	                               Span<const SlotAttr> attributes,
	                               std::size_t hash) {
		Shape* derivation = takeExistingTransition(cx, object, attributes, hash);
		if (!derivation) {
			derivation = takeNewTransition(cx, object, attributes, hash);
		}
		return derivation;
	}

	/// Transition the object's shape by adding a set of new slots.
	/// This function will reuse cached transitions.
	static Shape* transitionLayout(Context& cx,
	                               GC::Handle<DynObject> object,
	                               Span<const SlotAttr> attributes) {
		return transitionLayout(cx, object, attributes, hash(attributes));
	}

	/// Transition an object to a new layout. Barriered. Can GC.
	/// TODO: allocate overflow slot storage?
	static Shape* transitionLayout(Context& cx,
	                               GC::Handle<DynObject> object,
	                               std::initializer_list<SlotAttr> list) {
		return transitionLayout(
		        cx, object, Span<const SlotAttr>(list.begin(), list.size()));
	}

private:
	struct Init {
		void operator()(DynObject* target) const {
			target->header_.set(CellKind::OBJECT, layout, 0);
		}

		GC::Handle<Shape> layout;
	};
};

static_assert(std::is_standard_layout<DynObject>::value, "DynObject must be a StandardLayoutType.");

} // namespace Om
} // namespace OMR

#endif // OMR_OM_DYNOBJECT_HPP_
