/*******************************************************************************
 *  Copyright (c) 2018, 2019 IBM and others
 *
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License 2.0 which accompanies this
 *  distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 *  or the Apache License, Version 2.0 which accompanies this distribution and
 *  is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  This Source Code may also be made available under the following
 *  Secondary Licenses when the conditions for such availability set
 *  forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 *  General Public License, version 2 with the GNU Classpath
 *  Exception [1] and GNU General Public License, version 2 with the
 *  OpenJDK Assembly Exception [2].
 *
 *  [1] https://www.gnu.org/software/classpath/license.html
 *  [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include <OMR/GC/Handle.hpp>
#include <gtest/gtest.h>

namespace OMR
{
namespace GC
{

TEST(TestHandle, HandleToVoid)
{
	/* Ensure these types don't cause a compilation error. */
	Handle<void> h0;
	Handle<volatile void> h1;
	Handle<const void> h2;
	Handle<const volatile void> h3;
}

TEST(TestHandle, HandleToNull)
{
	void* root = NULL;
	Handle<void> handle(&root);
	EXPECT_EQ(handle, (void*)NULL);
	EXPECT_EQ(handle.root(), &root);
}

TEST(TestHandle, HandleToInt)
{
	int referent = 123;
	int* root = &referent;
	Handle<int> handle(&root);
	EXPECT_EQ(*handle, referent);
	EXPECT_EQ(handle, &referent);
	*handle = 456;
	EXPECT_EQ(referent, 456);
}

TEST(TestHandle, ReassignRoot)
{
	int a = 123;
	int b = 456;
	int* root = &a;
	Handle<int> handle(&root);
	EXPECT_EQ(a, *handle);
	EXPECT_EQ(&a, handle);
	root = &b;
	EXPECT_EQ(b, *handle);
	EXPECT_EQ(&b, handle);
}

TEST(TestHandle, Assign)
{
	int referent = 123;
	int* root = &referent;
	Handle<int> a(&root);
	Handle<int> b;

	b = a;
	EXPECT_EQ(a, b);
	EXPECT_EQ(a.get(), b.get());
	EXPECT_EQ(a.root(), b.root());

	*b = 456;
	EXPECT_EQ(*a, 456);
}

TEST(TestHandle, ConvertingAssign)
{
	int referent = 123;
	int* root = &referent;
	Handle<int> a(&root);
	Handle<const int> b;
	b = a;
	EXPECT_EQ(a, b);
	EXPECT_EQ(a.get(), b.get());
	EXPECT_EQ(a.root(), b.root());
}

struct IntPair
{
	int x;
	int y;
};

TEST(TestHandle, HandleToStruct)
{
	IntPair referent = {1, 2};
	IntPair* root = &referent;
	Handle<IntPair> handle(&root);
	EXPECT_EQ(handle->x, referent.x);
	EXPECT_EQ(handle->y, referent.y);
}

TEST(TestHandle, ConvertingConstruct)
{
	int referent = 123;
	int* root = &referent;
	Handle<int> a(&root);
	Handle<const int> b = a;
	EXPECT_EQ(*a, *b);
	EXPECT_EQ(a, b);
	EXPECT_EQ(a.root(), b.root());
}

}  /* namespace GC */
}  /* namespace OMR */
