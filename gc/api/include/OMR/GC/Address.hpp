#if !defined(OMR_GC_ADDRESS_HPP_)
#define OMR_GC_ADDRESS_HPP_

#include <cstdint>

namespace OMR {
namespace GC {

/// Unsigned integer type capable of storing the address of any GC cell. In a compressed references
/// scheme, the size of this type is smaller.

using Address = std::uintptr_t;

} // namespace GC
} // namespace OMR

#endif // OMR_GC_ADDRESS_HPP_
