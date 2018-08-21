#if !defined(OMR_OM_ANY_HPP_)
#define OMR_OM_ANY_HPP_

#include <OMR/Om/Array.hpp>
#include <OMR/Om/Cell.hpp>
#include <OMR/Om/CellOperations.hpp>
#include <OMR/Om/Object.hpp>
#include <OMR/Om/Shape.hpp>

namespace OMR {
namespace Om {

union AnyUnion {
	Cell cell;
	Object object;
	Shape shape;
	Array array;
};

class Any {
public:
	Any() = delete;

	CellKind kind() { return cellKind(as.cell); }

	AnyUnion as;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_ANY_HPP_
