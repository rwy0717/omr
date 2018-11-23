#if !defined(OMR_OM_POLYMAP_HPP_)
#define OMR_OM_POLYMAP_HPP_

#include <OMR/Om/Context.hpp>
#include <OMR/Om/PolymapCell.hpp>
#include <OMR/Om/PolymapScanner.hpp>

#include <OMR/GC/Allocator.hpp>
#include <OMR/GC/Handle.hpp>

#include <iostream>

namespace OMR {
namespace Om {

/// The result of an id lookup operation.
struct PolymapLookup {
	/// A failed lookup--the id was not found. this->found is false.
	PolymapLookup() : value(GARBAGE_VALUE), found(false) {}

	/// A successful lookup. this->found is true.
	PolymapLookup(PolyVal value, bool found = true) : value(value), found(found) {}

	PolymapLookup(const PolymapLookup&) = default;

	PolyVal value;
	bool found;
};

class PolymapData : public PolymapDataCell {
public:
	static PolymapData* allocate(Context& cx, std::size_t length) {
		return OMR::GC::allocateNonZero<PolymapData>(
		        cx.gc(), allocSize(length), Init(length));
	}

private:
	class Init {
	public:
		Init(std::size_t length) : length_(length) {}

		/// Set the length, initialize all nodes to holes.
		void operator()(PolymapData* target) const {
			target->header_ = CellHeader(CellKind::POLYMAP_DATA);
			target->length_ = length_;
			for (std::size_t i = 0; i < length_; ++i) {
				target->at(i) = POLYMAP_HOLE_NODE;
			}
		}

	private:
		std::size_t length_;
	};
};

static_assert(std::is_standard_layout<PolymapData>::value,
              "PolymapData must be a standard layout type");

static_assert(sizeof(PolymapData) == sizeof(PolymapDataCell),
              "The PolymapDataCell must contain all members.");

class Polymap : public PolymapCell {
public:
	/// Allocate a new Polymap.
	static Polymap* allocate(Context& cx, SizeP2 capacity = SIZEP2_16) {
		Polymap* result = nullptr;
		GC::StackRoot<PolymapData> data(cx.gc());
		data = PolymapData::allocate(cx, requiredDataLength(capacity));
		if (data != nullptr) {
			result = GC::allocate<Polymap>(cx.gc(), allocSize(), Init(data, capacity));
		}
		return result;
	}

	/// Lookup the PolyVal corresponding to ID.
	static PolymapLookup lookup(Context& cx, const Polymap* map, Id id) {
		const PolymapNode* node = map->lookupNode(id);
		if (node != nullptr) {
			return PolymapLookup(node->value);
		}
		return PolymapLookup();
	}

	/// Is id present in map?
	static bool defined(Context& cx, const Polymap* map, Id id) {
		return lookup(cx, map, id).found;
	}

	/// Remove the definition of id.
	static void erase(Context& cx, Polymap* map, Id id) {
		auto node = map->lookupNode(id);
		if (node != nullptr) {
			*node = POLYMAP_HOLE_NODE;
			--map->nodeCount_;
		}
	}

	/// Double the size of the map.
	static bool grow(Context& cx, GC::Handle<Polymap> map) {
		std::cerr << "> size: " << map->capacity_.dbl().get() << "\n";
		return resize(cx, map, map->capacity_.dbl());
	}

	/// Try assigning to an ID, if that ID has already been defined. Returns false if the ID
	/// isn't present in the table.
	static bool assign(Context& cx, Polymap* map, Id id, PolyVal value) noexcept {
		PolymapNode* node = map->lookupNode(id);
		if (node) {
			node->value = value;
			return true;
		}
		return false;
	}

	/// Insert the value into the map, or assign if the id already exists. Will resize the map.
	static bool insert(Context& cx, GC::Handle<Polymap> map, Id id, PolyVal value) {

		std::cerr << ">start insert" << id.raw() << "\n";
	
		// fast-path success: redefining an id

		if (assign(cx, map, id, value)) {
			return true;
		}

		// adding a new node

		if (map->shouldGrow()) {
			std::cerr << "> should grow\n";
			grow(cx, map);
		}

		return doInsert(cx, map, id, value);
	}

	static std::size_t size(Context& cx, Polymap* map) {
		return map->nodeCount_;
	}

private:
	class Init {
	public:
		Init(GC::Handle<PolymapData> data, SizeP2 capacity)
		        : data_(data), capacity_(capacity) {}

		void operator()(Polymap* target) const noexcept {
			target->header_ = CellHeader(CellKind::POLYMAP);
			target->capacity_ = capacity_;
			target->nodeCount_ = 0;
			target->data_ = data_;
		}

	private:
		GC::Handle<PolymapData> data_;
		SizeP2 capacity_;
	};

	/// The ideal index is computed by translating the id directly to an index.
	static std::size_t idealIndex(SizeP2 capacity, Id id) { return id.raw() % capacity; }

	/// Find the node corresponding to id. Returns null if the id wasn't matched.
	PolymapNode* lookupNode(Id id) noexcept {
		assert(id != HOLE_ID);
		const std::size_t ideal = idealIndex(id);
		const std::size_t max   = maxDistance();
		for (std::size_t dist = 0; dist < max; ++dist) {
			PolymapNode& node = nodeAt(ideal + dist);
			if (node.id == id) {
				return &node;
			}
		}
		return nullptr;
	}

	const PolymapNode* lookupNode(Id id) const noexcept {
		assert(id != HOLE_ID);
		const std::size_t ideal = idealIndex(id);
		const std::size_t max   = maxDistance();
		for (std::size_t dist = 0; dist < max; ++dist) {
			const PolymapNode& node = nodeAt(ideal + dist);
			if (node.id == id) {
				return &node;
			}
		}
		return nullptr;
	}

	/// Find an appropriate hole for ID. Will shuffle the elements with robin-hood style
	/// shuffling. Will potentially grow the data.
	static bool doInsert(Context& cx, GC::Handle<Polymap> map, Id id, PolyVal value) {
		PolymapNode node  = {id, value};
		while (true) {
			const std::size_t max = map->maxDistance();
			std::size_t ideal = map->idealIndex(id);
			std::size_t dist  = 0;
			do {
				std::cerr << "> inserting: " << node.id.raw() << "\n";
				const std::size_t index = ideal + dist;
				PolymapNode& candidate = map->nodeAt(index);

				if (candidate.id == HOLE_ID) {
					// store to the slot
					std::cerr << "> place: " << node.id.raw() << " @ " << index << "\n";
					candidate = node;
					++map->nodeCount_;
					return true;
				} else if (dist > (index - map->idealIndex(candidate.id))) {
					// Steal the slot, and try to place the candidate
					std::cerr << "> steal-slot: " << candidate.id.raw() << " @ " << index << "\n";
					swap(node, candidate);
					dist = 0;
					ideal = map->idealIndex(node.id);
				}
				++dist;
			} while (dist < max);

			// hit max probe distance, grow and try again
			bool ok = grow(cx, map);
			assert(ok);
			if (!ok) {
				return false;
			}
		}
	}

	/// Resize the map to a new capacity.
	static bool resize(Context& cx, GC::Handle<Polymap> map, SizeP2 capacity) noexcept {
		bool success = false;
		PolymapData* data = PolymapData::allocate(cx, requiredDataLength(capacity));
		if (data != nullptr) {
			if (copyDataOver(cx, map->data(), data, capacity)) {
				map->data_ = data;
				map->capacity_ = capacity;
				success = true;
			}
		}
		return success;
	}

	/// There must be enough room in the map for the new element.
	/// If the id / value pair cannot be placed within the max-distance from their ideal index, crash.
	static bool insertIntoData(PolymapData* data, SizeP2 capacity, Id id, PolyVal value) {
		const std::size_t max = maxDistance(capacity);

		PolymapNode node  = {id, value};
		std::size_t ideal = idealIndex(capacity, id);
		std::size_t dist  = 0;

		do {
			const std::size_t index = ideal + dist;
			PolymapNode& candidate = data->at(index);

			if (candidate.id == HOLE_ID) {
				// store to the slot
				candidate = node;
				return true;
			} else if (dist > (index - idealIndex(capacity, candidate.id))) {
				// Steal the slot, and try to place the candidate
				swap(node, candidate);
				dist = 0;
				ideal = idealIndex(capacity, node.id);
			}

		} while (dist < max);

		// Hit the distance limit--should never happen
		assert(0);
		return false;
	}

	/// Copy nodes between data buffers.
	static bool copyDataOver(Context& cx, PolymapData* src, PolymapData* dst, SizeP2 capacity) {
		for (std::size_t i = 0; i < src->length(); ++i) {
			PolymapNode& node = src->at(i);
			if (node.id != HOLE_ID) {
				if (!insertIntoData(dst, capacity, node.id, node.value)) {
					return false;
				}
			}
		}
		return true;
	}

	/// The maximum distance between a node and it's ideal location is relative to the capacity
	/// of the map.
	static std::size_t maxDistance(SizeP2 capacity) { return capacity.log2(); }

	/// The data is overallocated by a map's maximum distance between it's nodes and their ideal
	/// locations. This way, we can probe maxDistance times without bounds-checking the data.
	static std::size_t requiredDataLength(SizeP2 capacity) {
		return capacity.get() + maxDistance(capacity);
	}

	PolymapData* data() noexcept { return data_; }

	const PolymapData* data() const noexcept { return data_; }

	PolymapNode& nodeAt(std::size_t index) noexcept { return data()->at(index); }

	const PolymapNode& nodeAt(std::size_t index) const noexcept { return data()->at(index); }

	std::size_t capacity() const noexcept { return capacity_.get(); }

	bool shouldGrow() const noexcept {
		constexpr double LOAD_FACTOR = 0.5;
		return (nodeCount_ + 1) > (capacity() * LOAD_FACTOR);
	}

	/// Initial index is computed directly from the id. Equal to id % max_index.
	std::size_t idealIndex(Id id) const noexcept { return idealIndex(capacity_, id); }

	/// The maximum probe is log2(capacity)
	std::size_t maxDistance() const noexcept { return maxDistance(capacity_); }
};

static_assert(std::is_standard_layout<Polymap>::value, "Polymap must be a standard layout type");

static_assert(sizeof(Polymap) == sizeof(PolymapCell), "Polymap must be the same size as it's cell");

} // namespace Om
} // namespace OMR

#endif // OMR_OM_POLYMAP_HPP_
