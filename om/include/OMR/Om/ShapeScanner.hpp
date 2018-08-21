#ifndef OMR_OM_SHAPESCANNER_HPP_
#define OMR_OM_SHAPESCANNER_HPP_

#include <OMR/Om/BaseScanner.hpp>
#include <OMR/Om/Shape.hpp>

namespace OMR {
namespace Om {

class ShapeScanner : public BaseScanner<Shape> {
public:
	ShapeScanner() noexcept : BaseScanner() {}

	constexpr ShapeScanner(Shape* target) : BaseScanner(target) {}

	template<typename VisitorT>
	bool scan(VisitorT& visitor, Shape* target) {
		initialize(target);
		return scan(visitor);
	}

	/// Visit the references from the shape. For simplicity, scanning is non-interruptible for
	/// shapes.
	template<typename VisitorT>
	bool scan(VisitorT& visitor) {
		visitHeader(target_->header(), visitor);
		if (target_->parentLayout_ != nullptr) {
			visitor.edge(this, BasicSlotHandle(&target_->parentLayout_));
		}
		target_->transitions_.visit(visitor);
		// visitor.edge(this, BasicSlotHandle(&shapeTreeData_));
		return false;
	}

private:
	void initialize(Shape* target) { target_ = target; }
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SHAPESCANNER_HPP_