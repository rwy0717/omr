#if !defined(OMR_GC_MARKINGFN_HPP_)
#define OMR_GC_MARKINGFN_HPP_

#include <MarkingScheme.hpp>
#include <functional>
#include <vector>

namespace OMR {
namespace GC {

class Visitor;

using MarkingVisitor = MM_MarkingScheme::MarkingVisitor;

using MarkingFn = std::function<void(MarkingVisitor&)>;

using MarkingFnVector = std::vector<MarkingFn>;

} // namespace GC
} // namespace OMR

#endif // OMR_GC_MARKINGFN_HPP_
