
/// Access used by read/write mutators.
class BasicAccess {};

/// Exclusive acess used when transforming an object's representation.
class TransformAccess {};

enum class Access {
	/// Access required when reading out of a slot.
	READ,
	
	/// Access required when storing to a slot. Assign to slot.
	WRITE,
	
	/// Change the layout of a cell, mutating it's representation.
	/// Needed for operations like adding new fields.
	TRANSFORM
};

class AccessGuard {

};

TransformLock {

};

class ReadAccess {
public:
	void begin(Cell* c) {

	}

	void end(Cell* c) {

	}
};

class WriteAccess {

};

class TransformGuard {
public:
	TransformGuard(Context& cx, Cell* cell) {
		TransformAccess::begin(cx);
	}

	~TransformGuard() {
		TransformAccess::end();
	}
};

DynObject::takeExistingTransition(Context& cx, Handle<DynObject> object) {
	TransformAccessGuard(cx, object.get());


}

DynObject::readPolyVal(DynObject* object, SlotIndex index) {
	ReadAccessGuard(cx, object.get());


}

class LockTable {
public:
	bool lock(Context& cx, Cell* cell) {

	}

	void unlock(Context& cx, Cell* cell) {

	}

	bool isLocked(Cell* cell) {

	}

	Context* owner(Cell* cell) {

	}

	bool owns()
};


LockGuard guard(Cell* cell)

class SymbolTable {
	SymbolTable() {

	};
};



SlotIndex [] {
};


class SlotTable {

};

constexpr SlotDescriptorTable CONS_SLOTS {
	{ Id(0), }
};

Om::Struct* makeCons(Om::Context& cx) {
	Om::allocate()
}


struct LockTableEntry {
	Address address;
	Lock 
};

class LockTable {
	bool lock(Context& cx, Address addr) {

	}

	void unlock(Context& cx, Address addr) {

	}

	std::vector<LockTableEntry> entry;

}

bool Context::lock() {
	return system().locktable.lock(*this, addr);
}

void Context::unlock() {

}

class ObjectLockGuard {
	ObjectLockGuard(Cell* cell) {
	}

	~ObjectLockGuard() {

	}
};

class {

	Id allocate() {

	}

	void release(Id id) {

	}

	std::vector<Id> freeIds_;
	Id next_;
};