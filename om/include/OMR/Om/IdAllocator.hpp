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

#if !defined(OMR_OM_IDALLOCATOR_HPP_)
#define OMR_OM_IDALLOCATOR_HPP_

#include <OMR/Om/Id.hpp>

namespace OMR {
namespace Om {

constexpr RawId MIN_USER_ID = 32;
constexpr RawId MAX_USER_ID = 0xFFFF'FFFE;
constexpr RawId INVALID = 0xFFFF'FFFF;

/// [start, end)
class RawIdBlock {
public:
	constexpr RawIdBlock(RawId start, std::size_t length) : _start(start), _length(length) {}

	RawId start() { return _start; }

	RawId end() const noexcept { return _start + _length; }

	std::size_t length() const noexcept { return _length; }

	RawIdBlock* next() const noexcept { return _next; }

	bool empty() const noexcept { return length() == 0; }

	/// Peel an ID off the start of the block.
	RawId take() {
		if (empty()) {
			return INVALID;
		}
		RawId id = _start;
		_start += 1;
		_length -= 1;
		return id;
	}

	void absorbNext() {
		auto n = next();
		assert(end() == n->start());
		_length += n->length();
		_next = n->next();
		delete n;
	}

private:
	RawId _start;
	std::size_t _length;
	RawIdBlock* _next;
};

class RawIdHeap {
public:
	RawIdHeap(RawId min, RawId max) {
		block_ = new RawIdBlock(min, max);
	}

	bool available() const noexcept { return !empty(); }

	bool empty() const noexcept { return block_->empty(); }

	RawId allocate() noexcept {
		if (block_->empty()) {
			return INVALID;
		}
		RawId id = block_->take();
		if (block_->empty()) {
			auto next = block_->next();
			delete block_;
			block_ = next;
		}
		return id;
	}

	void free(RawId id) noexcept {

		auto block = block_;
		while (block->next() && (id > block->next()->start())) {
			block = block->next();
		}
	
		// block is the block _before_ our free ID.

		if(id)
		auto node = new HeapNode(id, weight());

		node** target = &_root;

		while (target != nullptr) {

		}

		target = node;
		if ()
	}

private:

	RawIdBlock* block_;
	RawId max_;
};

class IdAllocator {
public:
	IdAllocator() : IdAllocator(MIN_USER_ID, MAX_USER_ID) {}

	IdAllocator(RawId min, RawId max) : heap_(min, max) {}

	/// Allocate a free ID
	Id allocate() {
		return Id(heap_.allocate());
	}

	void free(Id id) {
		heap_.free(id.raw());
	}

private:
	RawIdHeap heap_;
	Id limit_;
};


class SymbolTable {

	Symbol* intern()
};

}
}

#endif // OMR_OM_IDALLOCATOR_HPP_