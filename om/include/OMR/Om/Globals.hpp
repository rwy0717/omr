#if !defined(OMR_OM_GLOBALS_HPP_)
#define OMR_OM_GLOBALS_HPP_

#include <OMR/Om/Shape.hpp>

#include <OMR/GC/MarkingFn.hpp>

#include <MarkingScheme.hpp>

#include <iostream>

namespace OMR {
namespace Om {

class StartupContext;
class Cell;

/// A collection of singleton GC cells. The collection is initialized at startup.
class Globals {
public:
	Shape* metaShape() const noexcept { return metaShape_; }

	Shape* arrayBufferShape() const noexcept { return arrayBufferShape_; }

	void scan(GC::MarkingVisitor& visitor) {
		if (metaShape_ != nullptr)
			visitor.edge(this, BasicSlotHandle(&metaShape_));

		if (arrayBufferShape_ != nullptr) {
			visitor.edge(this, BasicSlotHandle(&arrayBufferShape_));
		}
	}

protected:
	friend class System;

	/// Allocate the globals. Throws StartupError if any allocation fails.
	bool init(StartupContext& cx);

private:
	Shape* metaShape_ = nullptr;
	Shape* arrayBufferShape_ = nullptr;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_GLOBALS_HPP_
