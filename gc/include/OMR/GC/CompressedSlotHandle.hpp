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

#if !defined(OMR_GC_COMPRESSEDSLOTHANDLE_HPP_)
#define OMR_GC_COMPRESSEDSLOTHANDLE_HPP_

#include <AtomicOperations.hpp>
#include <omrcfg.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(OMR_ENV_DATA64)

namespace OMR
{
namespace GC
{

typedef uint32_t CompressedSlot;

/**
 * An implementation of OMR::App::SlotHandle, where GC references are compressed to 32 bits. CompressedSlotHandles are
 * only supported on 64 bit platforms.
 */
class CompressedSlotHandle
{
public:
	explicit CompressedSlotHandle(CompressedSlot* slot, size_t shift) : _slot(slot), _shift(shift) {}

	/**
	 * Read reference from slot
	 * @return address of object slot reference to.
	 */
	MMINLINE omrobjectptr_t readReferenceFromSlot() { return convertPointerFromToken(*_slot); }

	/**
	 * Write reference to slot if it was changed only.
	 * @param reference address of object should be written to slot
	 */
	MMINLINE void writeReferenceToSlot(omrobjectptr_t reference)
	{
		CompressedSlot compressed = convertTokenFromPointer(reference);
		if (compressed != *_slot) {
			*_slot = compressed;
		}
	}

	/**
	 * Return slot address. This address must be used as read only
	 * Created for compatibility with existing code
	 * @return slot address
	 */
	MMINLINE CompressedSlot* readAddressFromSlot() { return (CompressedSlot*)_slot; }

	/**
	 * Atomically replace heap reference. It is accepted to fail - some other thread
	 * might have raced us and put a more up to date value.
	 * @return true if write succeeded
	 */
	MMINLINE bool atomicWriteReferenceToSlot(omrobjectptr_t oldReference, omrobjectptr_t newReference)
	{
		/* Caller should ensure oldReference != newReference */
		CompressedSlot compressedOld = convertTokenFromPointer(oldReference);
		CompressedSlot compressedNew = convertTokenFromPointer(newReference);
		bool swapResult =
			((uint32_t)(uintptr_t)compressedOld ==
			 MM_AtomicOperations::lockCompareExchangeU32(
				 (uint32_t*)_slot, (uint32_t)(uintptr_t)compressedOld,
				 (uint32_t)(uintptr_t)compressedNew));
		return swapResult;
	}

	/**
	 *	Update of slot address.
	 *	Must be used by friends only for fast address replacement
	 *	@param slot slot address
	 */
	MMINLINE void writeAddressToSlot(CompressedSlot* slot) { _slot = slot; }

protected:
	MMINLINE omrobjectptr_t convertPointerFromToken(CompressedSlot token)
	{
		return (omrobjectptr_t)((uintptr_t)token << _shift);
	}

	MMINLINE CompressedSlot convertTokenFromPointer(omrobjectptr_t pointer)
	{
		return (CompressedSlot)((uintptr_t)pointer >> _shift);
	}

	/** Stored slot address. Volatile, because in concurrent GC the mutator can change the value in _slot. */
	volatile CompressedSlot* _slot;
	/** the number of bits to shift by when converting between the compressed pointers heap and real heap */
	uintptr_t _shift;
};

} /* namespace GC */
} /* namespace OMR */

#endif /* OMR_ENV_DATA64 */

#endif /* OMR_GC_COMPRESSEDSLOTHANDLE_HPP_ */
