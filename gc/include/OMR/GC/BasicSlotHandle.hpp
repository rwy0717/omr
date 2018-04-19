/*******************************************************************************
 * Copyright (c) 2018, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH
 *Assembly-exception
 *******************************************************************************/

#if !defined(OMR_GC_BASICSLOTHANDLE_HPP_)
#define OMR_GC_BASICSLOTHANDLE_HPP_

#include "AtomicOperations.hpp"
#include "objectdescription.h"
#include "omr.h"
#include "omrcfg.h"
#include "omrgcconsts.h"

#include <stdint.h>

namespace OMR
{
namespace GC
{

/**
 * The BasicSlot is a full-width reference slot, with integer semantics.
 */
typedef uintptr_t BasicSlot;

/**
 * A basic implementation of an App::SlotHandle. The BasicSlotHandle provides a read/write interface, where no special
 * slot encoding is taking place.
 */
class BasicSlotHandle
{
public:
	BasicSlotHandle(volatile BasicSlot* slot) : _slot(slot) {}

	/**
	 * Read reference from slot
	 * @return address of object slot reference to.
	 */
	MMINLINE omrobjectptr_t readReferenceFromSlot() const { return (omrobjectptr_t)*_slot; }

	/**
	 * Write reference to slot if it was changed only.
	 * @param reference address of object should be written to slot
	 */
	MMINLINE void writeReferenceToSlot(omrobjectptr_t reference) const
	{
		BasicSlot value = (BasicSlot)reference;
		if (value != *_slot) {
			*_slot = value;
		}
	}

	/**
	 * Atomically replace heap reference. It is accepted to fail - some other thread
	 * might have raced us and put a more up to date value.
	 * @return true if write succeeded
	 */
	MMINLINE bool atomicWriteReferenceToSlot(omrobjectptr_t oldReference, omrobjectptr_t newReference) const
	{
		return ((uintptr_t)oldReference ==
			MM_AtomicOperations::lockCompareExchange(
				(uintptr_t*)_slot, (uintptr_t)oldReference, (uintptr_t)newReference));
	}

	/**
	 * Returns true if the referrent is a leaf object. Used as a hint to speed up some operations.
	 */
	bool isReferentLeafObject() const { return false; }

	/**
	 * Return slot address. This address must be used as read only
	 * Created for compatibility with existing code
	 * @return slot address
	 */
	MMINLINE BasicSlot* readAddressFromSlot() const { return (BasicSlot*)_slot; }

	/**
	 *	Update of slot address.
	 *	Must be used by friends only for fast address replacement
	 *	@param slot slot address
	 */
	MMINLINE void writeAddressToSlot(BasicSlot* slot) { _slot = slot; }

protected:
	volatile BasicSlot* _slot;
};

} /* namespace GC */
} /* namespace OMR */

#endif /* OMR_GC_BASICSLOTHANDLE_HPP_ */
