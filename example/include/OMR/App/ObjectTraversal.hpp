

#if !defined(OMR_APP_OBJECTTRAVERSAL_HPP_)
#define OMR_APP_OBJECTTRAVERSAL_HPP_

#include <OMR/App/Object.hpp>
#include <OMR/App/SlotHandle.hpp>

#include <stdint.h>

namespace OMR
{
namespace App
{

class ObjectTraversal
{
public:
	/**
	 * Visit each slot of object with visitor.
	 */
	template <class VisitorT>
	size_t visit(Object* object, VisitorT& visitor, size_t sizeToDo = SIZE_MAX)
	{
		for (Slot* slot = object->begin(); slot != object->end(); ++slot) {
			visitor.edge(object, SlotHandle(slot));
		}
	}
};

}  // namespace App
}  // namespace OMR

#endif  // OMR_APP_OBJECTTRAVERSAL_HPP_
