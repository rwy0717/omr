#include <OMR/TypeTraits.hpp>
#include <gtest/gtest.h>

namespace OMR
{

TEST(TestTypeTraits, IntegralConst)
{
	EXPECT_EQ(0, (IntegralConstant<int, 0>::VALUE));
	EXPECT_EQ(0xdeadbeef, (IntegralConstant<unsigned int, 0xdeadbeef>::VALUE));
}

TEST(TestTypeTraits, BooleanConstant)
{
	EXPECT_TRUE((BoolConstant<true>::VALUE));
	EXPECT_TRUE(TrueConstant::VALUE);

	EXPECT_FALSE((BoolConstant<false>::VALUE));
	EXPECT_FALSE(FalseConstant::VALUE);
}

TEST(TestTypeTraits, IsSame)
{
	EXPECT_TRUE((IsSame<int, int>::VALUE));
	EXPECT_FALSE((IsSame<int, double>::VALUE));
	EXPECT_TRUE((IsSame<void*, void*>::VALUE));
	EXPECT_FALSE((IsSame<void* const, void*>::VALUE));
	EXPECT_FALSE((IsSame<void*, const void*>::VALUE));
}

TEST(TestTypeTraits, TypeAlias)
{
	EXPECT_TRUE((IsSame<void, TypeAlias<void>::Type>::VALUE));
	EXPECT_TRUE((IsSame<void*, TypeAlias<void*>::Type>::VALUE));
	EXPECT_TRUE((IsSame<const int, TypeAlias<const int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int&, TypeAlias<int&>::Type>::VALUE));
}

TEST(TestTypeTraits, RemoveConst)
{
	EXPECT_TRUE((IsSame<int, RemoveConst<int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveConst<const int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<volatile int, RemoveConst<const volatile int>::Type>::VALUE));
}

TEST(TestTypeTraits, RemoveVolatile)
{
	EXPECT_TRUE((IsSame<int, RemoveVolatile<int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveVolatile<volatile int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<const int, RemoveVolatile<const volatile int>::Type>::VALUE));
}

TEST(TestTypeTraits, RemoveCv)
{
	EXPECT_TRUE((IsSame<int, RemoveCv<int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCv<const int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCv<const volatile int>::Type>::VALUE));
}

TEST(TestTypeTraits, RemoveReference)
{
	EXPECT_TRUE((IsSame<int, RemoveReference<int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveReference<int&>::Type>::VALUE));
	// TODO: EXPECT_TRUE((IsSame<int, RemoveReference<int&&>::Type>::VALUE));
}

TEST(TestTypeTraits, RemoveCvRef)
{
	EXPECT_TRUE((IsSame<int, RemoveCvRef<int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCvRef<const int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCvRef<const volatile int>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCvRef<int&>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCvRef<const int&>::Type>::VALUE));
	EXPECT_TRUE((IsSame<int, RemoveCvRef<const volatile int&>::Type>::VALUE));
	// TODO: EXPECT_TRUE((IsSame<int, RemoveCvRef<int&&>::Type>::VALUE));
}

TEST(TestTypeTraits, IsReference)
{
	EXPECT_FALSE((IsReference<int>::VALUE));
	EXPECT_TRUE((IsReference<int&>::VALUE));
	// TODO: EXPECT_TRUE((IsReference<int&&>::VALUE));
}

TEST(TestTypeTraits, IsPointer)
{
	EXPECT_TRUE((IsPointer<int*>::VALUE));

	EXPECT_TRUE((IsPointer<void*>::VALUE));
	EXPECT_TRUE((IsPointer<void* const>::VALUE));
	EXPECT_TRUE((IsPointer<void* volatile>::VALUE));
	EXPECT_TRUE((IsPointer<void* const volatile>::VALUE));

	EXPECT_TRUE((IsPointer<void*&>::VALUE));
	EXPECT_TRUE((IsPointer<void* const&>::VALUE));
	EXPECT_TRUE((IsPointer<void* volatile&>::VALUE));
	EXPECT_TRUE((IsPointer<void* const volatile&>::VALUE));

	EXPECT_TRUE((IsPointer<void**>::VALUE));

	EXPECT_FALSE((IsPointer<int>::VALUE));
	EXPECT_FALSE((IsPointer<void>::VALUE));
	EXPECT_FALSE((IsPointer<int&>::VALUE));
}

TEST(TestTypeTraits, IsVoid)
{
	EXPECT_TRUE((IsVoid<void>::VALUE));
	EXPECT_TRUE((IsVoid<const void>::VALUE));
	EXPECT_TRUE((IsVoid<volatile void>::VALUE));
	EXPECT_TRUE((IsVoid<const volatile void>::VALUE));

	EXPECT_FALSE((IsVoid<int>::VALUE));
	EXPECT_FALSE((IsVoid<const int>::VALUE));
	EXPECT_FALSE((IsVoid<volatile int>::VALUE));
	EXPECT_FALSE((IsVoid<const volatile int>::VALUE));

	EXPECT_FALSE((IsVoid<void*>::VALUE));
}

}  // namespace OMR
