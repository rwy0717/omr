/*******************************************************************************
 *  Copyright (c) 2018, 2019 IBM and others
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

#ifndef OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_
#define OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_

#include <objectdescription.h>
#include <omrcfg.h>
#include <stdint.h>

namespace OMR
{
namespace GC
{

#if defined(OMR_ENV_DATA64)

class CompressedRefSlotHandle
{
public:
	CompressedRefSlotHandle(uint32_t* slot, uint32_t shift) : _slot(slot), _shift(shift) {}

	void* toAddress() const { return _slot; }

	omrobjectptr_t readReference() const { return decode(*_slot, _shift); }

	void writeReference(omrobjectptr_t value) const { *_slot = encode(value, _shift); }

private:
	static omrobjectptr_t
	decode(uint32_t data, uint32_t shift)
	{
		return (omrobjectptr_t)(((uint64_t)data) << shift);
	}

	static uint32_t
	encode(omrobjectptr_t data, uint32_t shift)
	{
		return (uint32_t)(((uint64_t)(data)) >> shift);
	}

	uint32_t* _slot;
	uint32_t _shift;
};

#endif /* defined(OMR_ENV_DATA64) */

} /* namespace GC */
} /* namespace OMR */

#endif /* OMR_GC_COMPRESSEDREFSLOTHANDLE_HPP_ */
