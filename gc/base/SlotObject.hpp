/*******************************************************************************
 * Copyright (c) 2018, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH
 *Assembly-exception
 *******************************************************************************/

#if !defined(OMR_GC_SLOTOBJECT_HPP_)
#define OMR_GC_SLOTOBJECT_HPP_

#include <OMR/GC/BasicSlotHandle.hpp>
#include <OMR/GC/CompressedSlotHandle.hpp>
#include <objectdescription.h>
#include <omr.h>
#include <omrcfg.h>

#if defined(OMR_GC_COMPRESSED_POINTERS)

class GC_SlotObject : public OMR::GC::CompressedSlotHandle
{
public:
	GC_SlotObject(void* vm, OMR::GC::CompressedSlot* slot) : OMR::GC::CompressedSlotHandle(slot, (OMR_VM *)vm)->_compressedPointersShift)
	{
	}
};

#else /* OMR_GC_COMPRESSED_POINTERS */

class GC_SlotObject : public OMR::GC::BasicSlotHandle
{
public:
	GC_SlotObject(void* vm, OMR::GC::BasicSlot* slot) : OMR::GC::BasicSlotHandle(slot) {}
};

#endif /* OMR_GC_COMPRESSED_POINTERS */

#endif /* OMR_GC_SLOTOBJECT_HPP_ */
