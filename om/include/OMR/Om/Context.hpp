#ifndef OMR_OM_CONTEXT_HPP_
#define OMR_OM_CONTEXT_HPP_

#include <OMR/Om/Globals.hpp>

#include <OMR/GC/System.hpp>

namespace OMR {
namespace Om {

class System {
public:
	friend class SystemAssertions;

	inline explicit System(OMR::Runtime& runtime);

	System(const System&) = delete;

	System(System&&) = delete;

	~System() = default;

	GC::System& gc() { return gc_; }

	Globals& globals() { return globals_; }

	const Globals& globals() const { return globals_; }

	void scan(GC::MarkingVisitor& mark);

private:
	GC::System gc_;
	Globals globals_;
};

class SystemAssertions {
	static_assert(offsetof(System, gc_) == 0,
	              "GC::System must be the first element in the Om::System.");
};

class Context {
public:
	friend class ContextStaticAssertions;

	Context(Context&) = delete;

	Context(Context&&) = delete;

	~Context() = default;

	GC::Context& gc() noexcept { return gc_; }

	const GC::Context& gc() const noexcept { return gc_; }

	System& system() { return reinterpret_cast<System&>(gc().system()); }

	Globals& globals() { return system().globals(); }

protected:
	explicit Context(System& system) : gc_(system.gc()) {}

private:
	GC::RunContext gc_;
};

class ContextStaticAssertions {
	static_assert(offsetof(Context, gc_) == 0,
	              "OMR::GC::Context must be the first field of OMR::Om::Context.");
};

class RunContext : public Context {
public:
	RunContext(System& system) : Context(system) {}
};

class StartupContext : public Context {
protected:
	friend class System;

	StartupContext(System& system) : Context(system) {}
};

inline Context& getContext(MM_EnvironmentBase* env) {
	return reinterpret_cast<Context&>(GC::getContext(env));
}

inline System::System(OMR::Runtime& runtime) : gc_(runtime) {
	StartupContext cx(*this);

	/// call System::scan.
	/// Must occur before any allocation.
	gc().markingFns().push_back([this](GC::MarkingVisitor& mark) { scan(mark); });

	bool ok = globals().init(cx);
	if (!ok) {
		throw std::runtime_error("failed to init globals");
	}
}

inline void System::scan(GC::MarkingVisitor& visitor) { globals().scan(visitor); }

} // namespace Om
} // namespace OMR

#endif // OMR_OM_CONTEXT_HPP_
