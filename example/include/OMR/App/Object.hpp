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

#if !defined(OMR_APP_OBJECT_HPP_)
#define OMR_APP_OBJECT_HPP_

#include <OMR/App/SlotHandle.hpp>

#include <omrcfg.h>
#include <cstdlib>
#include <stdint.h>

namespace OMR
{
namespace App
{

typedef uint8_t ObjectFlags;

/**
 * A header containing basic information about an object. Contains the object's size and an 8 bit object flag.
 * The size and flags are masked together into a single fomrobjectptr_t.
 */
class ObjectHeader
{
public:

	ObjectHeader() {}

	explicit ObjectHeader(size_t sizeInBytes, ObjectFlags flags = 0) { assign(sizeInBytes, flags); }

	size_t sizeInBytes() const { return _value >> SIZE_SHIFT; }

	void sizeInBytes(size_t value) { assign(value, flags()); }

	ObjectFlags flags() const { return (ObjectFlags)_value; }

	void flags(ObjectFlags value) { assign(sizeInBytes(), value); }

	void assign(size_t sizeInBytes, ObjectFlags flags) { _value = (sizeInBytes << SIZE_SHIFT) | flags; }

	Slot raw() const { return _value; }

	void raw(Slot raw) { _value = raw; }

private:
	static const size_t SIZE_SHIFT = sizeof(ObjectFlags)*8;

	Slot _value;
};

class Object
{
public:
	ObjectHeader header;

	explicit Object(size_t sizeInBytes, ObjectFlags flags = 0) : header(sizeInBytes, flags) {}

	size_t sizeOfSlotsInBytes() const { return header.sizeInBytes() - sizeof(ObjectHeader); }

	size_t slotCount() const { return sizeOfSlotsInBytes() / sizeof(Slot); }

	Slot* slots() { return (Slot*)(this + 1); }

	const Slot* slots() const { return (Slot*)(this + 1); }

	Slot* begin() { return slots(); }

	Slot* end() { return begin() + slotCount(); }
};

} /* namespace App */
} /* namespace OMR */

#endif /* OMR_APP_OBJECT_HPP_ */
