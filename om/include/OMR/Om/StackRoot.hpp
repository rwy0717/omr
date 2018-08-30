#ifndef OMR_OM_STACKROOT_HPP_
#define OMR_OM_STACKROOT_HPP_

#include <OMR/Om/Context.hpp>

#include <OMR/GC/StackRoot.hpp>

namespace OMR {
namespace Om {

/// Helper for creating GC stack roots from om contexts.
template<typename T>
GC::StackRoot<T> makeStackRoot(Context& cx, T* ref) {
	return GC::StackRoot<T>(cx.gc(), ref);
}

} // namespace Om
} // namespace OMR

#endif // OMR_OM_STACKROOT_HPP_
