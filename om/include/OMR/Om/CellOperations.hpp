#if !defined(OMR_CELLOPERATIONS_HPP_)
#define OMR_CELLOPERATIONS_HPP_

#include <OMR/Om/Cell.hpp>
#include <OMR/Om/CellKind.hpp>
#include <OMR/Om/Shape.hpp>

namespace OMR {
namespace Om {

/// Obtain the type of thing this cell is. The type can be found by looking at the instance data in
/// the Shape that lays out this Cell.
inline CellKind cellKind(const Cell& cell) noexcept { return cell.layout()->instanceKind(); }

inline CellKind cellKind(const Cell* cell) noexcept { return cellKind(*cell); }

} // namespace Om
} // namespace OMR

#endif // OMR_CELLOPERATIONS_HPP_
