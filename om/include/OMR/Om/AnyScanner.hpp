#ifndef OMR_OM_ANYSCANNER_HPP_
#define OMR_OM_ANYSCANNER_HPP_

#include <OMR/Om/Any.hpp>
#include <OMR/Om/ArrayScanner.hpp>
#include <OMR/Om/ObjectScanner.hpp>
#include <OMR/Om/ShapeScanner.hpp>

namespace OMR {
namespace Om {

class AnyScanner {
public:
	AnyScanner() = default;

	/// Start scanning a cell. Returns true if there are more slots to scan.
	template<typename VisitorT>
	bool scan(VisitorT&& visitor, Any* tgt) {
		switch (tgt->kind()) {
		case CellKind::OBJECT:
			new (&subscanner_.as.object) ObjectScanner();
			return subscanner_.as.object.scan(visitor, &tgt->as.object);
		case CellKind::ARRAY:
			new (&subscanner_.as.array) ArrayScanner();
			return subscanner_.as.array.scan(visitor, &tgt->as.array);
		case CellKind::SHAPE:
			new (&subscanner_.as.shape) ShapeScanner();
			return subscanner_.as.shape.scan(visitor, &tgt->as.shape);
		default:
			assert(0); // unreachable.
		}
	}

	/// Continue scanning an existing cell. Returns true if there are more slots to
	/// scan.
	template<typename VisitorT>
	bool scan(VisitorT&& visitor) {
		assert(target() != nullptr);
		switch (target()->kind()) {
		case CellKind::OBJECT: return subscanner_.as.object.scan(visitor);
		case CellKind::ARRAY: return subscanner_.as.array.scan(visitor);
		case CellKind::SHAPE: return subscanner_.as.shape.scan(visitor);
		}
	}

	constexpr Any* target() const { return subscanner_.as.base.target(); }

private:
	union SubscannerUnion {
		SubscannerUnion() : base() {}

		SubscannerUnion(Any* target) : base(target) {}

		BaseScanner<Any> base;
		ObjectScanner object;
		ArrayScanner array;
		ShapeScanner shape;
	};

	struct Subscanner {
		SubscannerUnion as;
	};

	Subscanner subscanner_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ANYSCANNER_HPP_
