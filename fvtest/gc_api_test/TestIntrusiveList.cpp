#include <OMR/IntrusiveList.hpp>
#include <gtest/gtest.h>

struct LinkedInt;
struct LinkedIntTraits;

using LinkedIntListNode = IntrusiveListNode<LinkedInt>;

static_assert(std::is_standard_layout<LinkedIntListNode>::value,
              "IntrusiveListNode<T> guarantees that it is a standard layout type.");

/// Forms a linked list of integers.
struct LinkedInt
{
	int value;
	IntrusiveListNode<LinkedInt> node;
};

static_assert(std::is_standard_layout<LinkedInt>::value,
              "IntrusiveListNode<T> guarantees that it is a standard layout type.");

template <>
struct ::OMR::IntrusiveListNodeAccessor<LinkedInt> {

	static constexpr LinkedIntListNode& node(LinkedInt& i) noexcept { return i.node; }

	static constexpr const LinkedIntListNode& node(const LinkedInt& i) noexcept
	{
		return i.node;
	}
};

using LinkedIntList = IntrusiveList<LinkedInt>;

static_assert(std::is_standard_layout<LinkedIntList>::value,
              "IntrusiveList<T> guarantees that it is a standard layout type.");

using LinkedIntListIterator = LinkedIntList::Iterator;

using ConstLinkedIntListIterator = LinkedIntList::ConstIterator;

namespace OMR
{
namespace Test
{

TEST(TestIntrusiveList, Empty)
{
	LinkedIntList list;
	EXPECT_TRUE(list.empty());
	EXPECT_EQ(list.begin(), list.end());
	EXPECT_EQ(list.cbegin(), list.cend());
	for(const LinkedInt& i : list) {
		FAIL();
	}
}

TEST(TestIntrusiveList, ConvertIterToConstIter)
{
	LinkedIntList list;
	LinkedIntListIterator a = list.begin();
	ConstLinkedIntListIterator b = a;
	EXPECT_EQ(a, b);
	EXPECT_EQ(b, a);
}

TEST(TestIntrusiveList, AddOne)
{
	LinkedIntList list;
	LinkedInt a = {1};
	list.add(&a);
	EXPECT_FALSE(list.empty());
}

TEST(TestIntrusiveList, IterateOne)
{
	LinkedIntList list;

	LinkedInt a = {1};
	list.add(&a);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_NE(iter, list.end());
	EXPECT_EQ(iter->value, 1);

	++iter;
	EXPECT_EQ(iter, list.end());

	LinkedIntList::ConstIterator citer = list.begin();
}

TEST(TestIntrusiveList, IterateTwo)
{
	LinkedIntList list;

	LinkedInt a = {1};
	list.add(&a);

	LinkedInt b = {2};
	list.add(&b);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_EQ(iter->value, 2);

	++iter;
	EXPECT_EQ(iter->value, 1);

	++iter;
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddThenRemoveOne)
{
	LinkedIntList list;
	EXPECT_TRUE(list.empty());

	LinkedInt a = {1};
	list.add(&a);
	EXPECT_FALSE(list.empty());

	list.remove(&a);
	EXPECT_TRUE(list.empty());

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddTwoThenRemoveFirst)
{
	LinkedIntList list;
	LinkedInt a = {1};
	LinkedInt b = {2};

	list.add(&a);
	list.add(&b);
	list.remove(&a);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_NE(iter, list.end());
	EXPECT_EQ(iter->value, 2);
	++iter;
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddTwoThenRemoveSecond)
{
	LinkedIntList list;
	LinkedInt a = {1};
	LinkedInt b = {2};

	list.add(&a);
	list.add(&b);
	list.remove(&b);

	LinkedIntList::Iterator iter = list.begin();
	EXPECT_NE(iter, list.end());
	EXPECT_EQ(iter->value, 1);
	++iter;
	EXPECT_EQ(iter, list.end());
}

TEST(TestIntrusiveList, AddTwoThenRemoveBoth)
{
	LinkedIntList list;
	LinkedInt a = {1};
	LinkedInt b = {2};

	list.add(&a);
	list.add(&b);
	list.remove(&a);
	list.remove(&b);

	EXPECT_TRUE(list.empty());
	EXPECT_EQ(list.begin(), list.end());
	for(const LinkedInt& i : list) {
		FAIL();
	}
}

TEST(TestIntrusiveList, AddTwoThenRemoveBothInReverse)
{
	LinkedIntList list;
	LinkedInt a = {1};
	LinkedInt b = {2};

	list.add(&a);
	list.add(&b);
	list.remove(&b);
	list.remove(&a);

	EXPECT_TRUE(list.empty());
	EXPECT_EQ(list.begin(), list.end());
	for(const LinkedInt& i : list) {
		FAIL();
	}
}

} // namespace Test
} // namespace OMR
