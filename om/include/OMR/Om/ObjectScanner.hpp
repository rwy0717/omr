#ifndef OMR_OM_OBJECTSCANNER_HPP_
#define OMR_OM_OBJECTSCANNER_HPP_

#include <OMR/Om/BaseScanner.hpp>
#include <OMR/Om/Object.hpp>

#include <iostream>

namespace OMR {
namespace Om {

class ObjectScanner : public BaseScanner<Object> {
public:
	ObjectScanner() noexcept : BaseScanner() {}

	/// Scan the target. Target returns true if complete.
	template<typename VisitorT>
	bool scan(VisitorT& visitor, Object* target) {
		target_ = target;
		layout_ = target_->layout();
		iter_ = layout_->instanceSlots().begin();

		bool cont = visitHeader(target_->header(), visitor);
		if (!cont) {
			return true;
		}
	
		return scan(visitor);
	}

	/// Continue scanning. Returns true if there is more to do, false if more to do.
	template<typename VisitorT>
	bool scan(VisitorT& visitor) {

		if (layout_ == nullptr) {
			return false;
		}

		// continue scanning the current shape.
		// finish off the current iteration.
		while (iter_ != layout_->instanceSlots().end()) {
			bool cont = target_->visitSlot(*iter_, visitor);
			++iter_;
			if (!cont) {
				return true; // more slots
			}
		}

		// scan the rest of the slots.

		layout_ = layout_->parentLayout();

		while (layout_ != nullptr) {
			iter_ = layout_->instanceSlots().begin();
			while (iter_ != layout_->instanceSlots().end()) {
				bool cont = target_->visitSlot(*iter_, visitor);
				++iter_;
				if (!cont) {
					return true; // more slots
				}
			}
			layout_ = layout_->parentLayout();
		}

		// complete--return "no more slots"
		return false;
	}

private:
	Shape* layout_;
	SlotDescriptorRangeIterator iter_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_OBJECTSCANNER_HPP_
