#ifndef OMR_OM_ALLOCATOR_HPP_
#define OMR_OM_ALLOCATOR_HPP_

#include <OMR/Om/Allocation.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/GC/Handle.hpp>
#include <OMR/Om/Id.hpp>
#include <OMR/GC/StackRoot.hpp>

#include <cstdlib>
#include <omrgc.h>

namespace OMR {
namespace Om {

class Cell;
class Context;

constexpr bool DEBUG_SLOW = true;

struct BaseAllocator {
	template<typename ResultT = Cell, typename InitializerT>
	static ResultT*
	allocate(Context& cx, InitializerT& init, std::size_t size = sizeof(ResultT)) {
		Allocation allocation(cx, init, size);
		ResultT* result = (ResultT*)allocation.allocateAndInitializeObject(cx.gc().vm());

		if (DEBUG_SLOW) {
			GC::StackRoot<ResultT> root(cx.gc(), result);
			OMR_GC_SystemCollect(cx.gc().vm(), 0);
			result = root.get();
		}

		return result;
	}
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ALLOCATOR_HPP_
