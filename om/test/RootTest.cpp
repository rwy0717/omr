#include <OMR/Om/Allocator.hpp>
#include <OMR/Om/Context.hpp>
#include <OMR/Om/MemHandle.hpp>
#include <OMR/Om/Object.hpp>
#include <OMR/Om/ObjectOperations.hpp>
#include <OMR/Om/Runtime.hpp>
#include <OMR/Om/Shape.hpp>
#include <OMR/Om/StackRoot.hpp>
#include <OMR/Om/TransitionSet.hpp>

#include <OMR/GC/Handle.hpp>
#include <gtest/gtest.h>

namespace OMR {
namespace Om {
namespace Test {

Runtime runtime;

TEST(RootTest, basic) {
	System system(runtime);
	RunContext cx(system);
	GC::StackRoot<Object> root(cx.gc(), allocateEmptyObject(cx));
	GC::Handle<Object> handle(root);
	// MemHandle<Object::Base> member(handle, &Object::base);
}

} // namespace Test
} // namespace Om
} // namespace OMR
