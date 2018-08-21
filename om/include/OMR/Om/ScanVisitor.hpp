#ifndef OMR_OM_SCANVISITOR_HPP_
#define OMR_OM_SCANVISITOR_HPP_

namespace OMR {
namespace Om {

class NoOpScanVisitor {
	/// Visit an edge. Returns true if scanning should continue. Returns false if scanning
	/// should be paused. The SlotHandle is
	template<typename SlotHandleT>
	bool edge(const SlotHandleT& slot) {
		// do nothing.
		return true;
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_SCANVISITOR_HPP_
