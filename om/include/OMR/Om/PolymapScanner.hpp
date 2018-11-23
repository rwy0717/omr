#if !defined(OMR_OM_POLYMAPSCANNER_HPP_)
#define OMR_OM_POLYMAPSCANNER_HPP_

#include <OMR/Om/PolymapCell.hpp>
#include <OMR/Om/BaseScanner.hpp>

#include <OMR/GC/RefSlotHandle.hpp>

namespace OMR {
namespace Om {

/// A scanner for PolymapDataCells.
class PolymapDataScanner : public BaseScanner<PolymapDataCell> {
public:
	PolymapDataScanner() : BaseScanner(nullptr), index_(0) {}

	template <typename VisitorT>
	GC::ScanResult start(VisitorT&& visitor, PolymapDataCell* tgt, std::size_t todo = SIZE_MAX) {
		target(tgt);
		index_ = 0;
		return resume(visitor, todo);
	}

	template <typename VisitorT>
	GC::ScanResult resume(VisitorT&& visitor, std::size_t todo = SIZE_MAX) {
		bool cont = true;
		std::size_t nbytes = 0;
		std::size_t length = target()->length();

		while (true) {
			if (index_ == length) {
				return {nbytes, true};
			}
			if (!cont || nbytes >= todo) {
				return {nbytes, false};
			}

			PolymapNode& node = target()->at(index_);

			if (node.id != HOLE_ID && node.value.isRef()) {
				cont = visitor.edge(this, ValueSlotHandle(&node.value));
			}

			++index_;
			nbytes += sizeof(PolymapNode);
		}
	}

private:
	std::size_t index_;
};

class PolymapScanner : public BaseScanner<PolymapCell> {
public:
	PolymapScanner() : BaseScanner() {}

	template <typename VisitorT>
	GC::ScanResult start(VisitorT&& visitor, PolymapCell* tgt, std::size_t todo) {
		target(tgt);
		visitor.edge(target(), GC::RefSlotHandle((Any**)&target()->data_));
		return {sizeof(PolymapCell), true};
	}

	template <typename VisitorT>
	GC::ScanResult resume(VisitorT&& visitor, std::size_t todo) {
		assert(0);
		return {0, true};
	}
};

}  // namespace Om
}  // namespace OMR

#endif // OMR_OM_POLYMAPSCANNER_HPP_
