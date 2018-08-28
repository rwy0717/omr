#ifndef OMR_OM_CONTEXT_HPP_
#define OMR_OM_CONTEXT_HPP_

#include <OMR/GC/System.hpp>

namespace OMR {
namespace Om {

class System {
public:
	OMR::GC::System& gc() { return gc_; }
};

class Context {
public:
	Context(System& system) : system_(&system) {}
	
	Context(Context&) = delete;

	Context(Context&&) = default;

	GC::Context& gc() noexcept { return gc_; }

	const GC::Context& gc() const noexcept { return gc_; }

private:
	GC::Context gc_;
};

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CONTEXT_HPP_
