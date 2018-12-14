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

#include <OMR/GC/RefSlotHandle.hpp>
#include <OMR/GC/CompressedRefSlotHandle.hpp>
#include <gtest/gtest.h>

namespace OMR
{
namespace GC
{

TEST(TestRefSlotHandle, ReadFromRefSlot)
{
	omrobjectptr_t slot = (omrobjectptr_t)42;
	RefSlotHandle handle(&slot);
	EXPECT_EQ(handle.toAddress(), &slot);
	EXPECT_EQ(handle.readReference(), slot);
}

TEST(TestRefSlotHandle, WriteToRefSlot)
{
	const omrobjectptr_t DATA = (omrobjectptr_t)42;

	omrobjectptr_t slot = (omrobjectptr_t)0;
	RefSlotHandle handle(&slot);
	handle.writeReference(DATA);
	EXPECT_EQ(handle.readReference(), DATA);
	EXPECT_EQ(slot, DATA);
}

#if defined(OMR_ENV_DATA64)

TEST(TestCompressedRefSlotHandle, ReadFromCompressedRefSlot)
{
	const size_t SHIFT = 2;
	const uint32_t DATA = 42 << SHIFT;

	uint32_t slot = DATA >> SHIFT;
	CompressedRefSlotHandle handle(&slot, SHIFT);
	EXPECT_EQ(handle.toAddress(), &slot);
	EXPECT_EQ(handle.readReference(), (omrobjectptr_t)DATA);
}

TEST(TestCompressedRefSlotHandle, WriteToCompressedRefSlot)
{
	const size_t SHIFT = 2;
	const omrobjectptr_t DATA = (omrobjectptr_t)(42 << SHIFT);

	uint32_t slot = 0;
	CompressedRefSlotHandle handle(&slot, SHIFT);
	handle.writeReference(DATA);
	EXPECT_EQ(handle.readReference(), DATA);
	EXPECT_EQ(slot, (uint32_t)(((uint64_t)DATA) >> SHIFT));
}

#endif  /* OMR_ENV_DATA64 */

}  /* namespace GC */
}  /* namespace OMR */
