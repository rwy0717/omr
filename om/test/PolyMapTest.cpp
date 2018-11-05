#include <OMR/Om/Polymap.hpp>

#include <OMR/GC/StackRoot.hpp>

#include <omrgcstartup.hpp>
#include <omrgc.h>
#include <gtest/gtest.h>

namespace OMR {
namespace Om {
namespace Test {

Runtime runtime;

TEST(SizeP2Test, GetRightNumbers) {
	constexpr SizeP2 size = SIZEP2_128;
	EXPECT_EQ(size.get(), 128);
	EXPECT_EQ(size.x2().get(), 256);
	EXPECT_EQ(129 % size, 1);
}

TEST(PolymapTest, AllocateEmptyMap) {
	System system(runtime);
	RunContext cx(system);

	GC::StackRoot<Polymap> map(cx.gc(), Polymap::allocate(cx));
	OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
	EXPECT_EQ(Polymap::size(cx, map), 0);
}

TEST(PolymapTest, StoreWithoutResizing) {
	System system(runtime);
	RunContext cx(system);

	constexpr SizeP2 size = SIZEP2_128;

	GC::StackRoot<Polymap> map(cx.gc(), Polymap::allocate(cx, size));
	ASSERT_NE(map.get(), nullptr);

	OMR_GC_SystemCollect(cx.gc().vmContext(), 0);

	for(std::size_t i = 0; i < size.get(); ++i) {
		Polymap::insert(cx, map, Id(i), {AS_UINT48, i});

		for (std::size_t j = 0; j < i; ++j) {
			auto look = Polymap::lookup(cx, map.get(), Id(j));
			EXPECT_TRUE(look.found);
			EXPECT_EQ(look.value, Om::PolyVal(AS_UINT48, j));
		}
	}
}

TEST(PolymapTest, StoreWithResizing) {
	System system(runtime);
	RunContext cx(system);

	constexpr SizeP2 init = SIZEP2_2;
	constexpr std::size_t insertions = 256;

	GC::StackRoot<Polymap> map(cx.gc(), Polymap::allocate(cx, init));
	ASSERT_NE(map, nullptr);

	OMR_GC_SystemCollect(cx.gc().vmContext(), 0);

	for(std::size_t i = 0; i < insertions; ++i) {
		ASSERT_TRUE(Polymap::insert(cx, map, Id(i), {AS_UINT48, i}));
		
		OMR_GC_SystemCollect(cx.gc().vmContext(), 0);

		for (std::size_t j = 0; j <= i; ++j) {
			auto look = Polymap::lookup(cx, map.get(), Id(j));
			ASSERT_TRUE(look.found);
			ASSERT_EQ(look.value, Om::PolyVal(AS_UINT48, j));
		}
	}
}

TEST(PolymapTest, StoreAndErase) {
	System system(runtime);
	RunContext cx(system);

	constexpr SizeP2 init = SIZEP2_2;
	constexpr std::size_t steps = 128;

	GC::StackRoot<Polymap> map(cx.gc(), Polymap::allocate(cx, init));
	ASSERT_NE(map, nullptr);

	OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
	ASSERT_NE(map, nullptr);

	for(std::size_t i = 0; i < steps; ++i) {
		
		ASSERT_TRUE(Polymap::insert(cx, map, Id(i), {AS_UINT48, i}));
		OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
		
		auto look = Polymap::lookup(cx, map.get(), Id(i));
		ASSERT_TRUE(look.found);
		ASSERT_EQ(look.value, Om::Value(AS_UINT48, i));

		Polymap::erase(cx, map, Id(i));
		ASSERT_FALSE(Polymap::lookup(cx, map.get(), Id(i)).found);
		OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
	}
}

TEST(PolymapTest, HighContentionSlot) {
	System system(runtime);
	RunContext cx(system);

	constexpr SizeP2 init = SIZEP2_2;
	constexpr SizeP2 size = SIZEP2_128;

	GC::StackRoot<Polymap> map(cx.gc(), Polymap::allocate(cx, init));
	ASSERT_NE(map, nullptr);

	OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
	ASSERT_NE(map, nullptr);

	for(std::size_t i = 0; i < size.get(); ++i) {
		ASSERT_TRUE(Polymap::insert(cx, map, Id(i), {AS_UINT48, i}));
		{
			auto look = Polymap::lookup(cx, map.get(), Id(i));
			ASSERT_TRUE(look.found);
			ASSERT_EQ(look.value, Om::PolyVal(AS_UINT48, i));
		}

		Polymap::erase(cx, map, Id(i));
		ASSERT_FALSE(Polymap::lookup(cx, map.get(), Id(i)).found);

		OMR_GC_SystemCollect(cx.gc().vmContext(), 0);
	}
}

} // namespace Test
} // namespace Om
} // namespace OMR
