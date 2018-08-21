#if !defined OMR_OM_CELLKIND_HPP_
#define OMR_OM_CELLKIND_HPP_

namespace OMR {
namespace Om {

/// 0 indicates uninitialized.
enum class CellKind { OBJECT = 1, SHAPE, ARRAY };

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CELLKIND_HPP_
