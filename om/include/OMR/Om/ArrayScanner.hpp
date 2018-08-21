#ifndef OMR_OM_ARRAYSCANNER_HPP_
#define OMR_OM_ARRAYSCANNER_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/BaseScanner.hpp>

#include <cassert>

namespace OMR {
namespace Om {

class ArrayScanner : public BaseScanner<Array> {
public:
	/// Construct without a target.
	ArrayScanner() = default;

	/// Construct with a target.
	ArrayScanner(Array* target) : BaseScanner(target) {}

	/// Returns true if there's more to scan.
	template<typename VisitorT>
	bool scan(VisitorT& visitor, Array* target) {
		target_ = target;
		return scan(visitor);
	}

	/// Continue scanning the target.
	template<typename VisitorT>
	bool scan(VisitorT& visitor) {
		visitHeader(target_->header(), visitor);
		return false;
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ARRAYSCANNER_HPP_
