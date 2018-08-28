#if !defined(OMRCLIENT_GC_OBJECTSCANNER_HPP_)
#define OMRCLIENT_GC_OBJECTSCANNER_HPP_

namespace OMRClient {
namespace GC {

class ObjectScanner {
public:
	template <typename VisitorT>
	bool scan(VisitorT visitor, void* object) {
		assert(0);
		return false;
	}

	template <typename VisitorT>
	bool scan(VisitorT visitor) {
		assert(0);
		return false;
	}
};

}  // namespace GC
}  // namespace OMRClient

#endif // OMRCLIENT_GC_OBJECTSCANNER_HPP_
