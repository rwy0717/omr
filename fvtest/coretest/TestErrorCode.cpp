/*******************************************************************************
 *  Copyright (c) 2018, 2018 IBM and others
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

#include <OMR/ErrorCode.hpp>
#include <gtest/gtest.h>

namespace OMR {

class ExampleErrorCategory : public ErrorCategory
{
public:
	virtual const char* message(int code) const {
		const char* m = NULL;
		switch(code) {
		case 0:
			m = "example: success";
			break;
		default:
			m = "example: failure";
			break;
		}
		return m;
	}
};

static const ExampleErrorCategory EXAMPLE_ERROR_CATEGORY;

TEST(TestErrorCategory, CompareCategories)
{
	const ExampleErrorCategory a;
	const ExampleErrorCategory b;
	EXPECT_EQ(a, a);
	EXPECT_EQ(b, b);
	EXPECT_NE(a, b);
}

TEST(TestErrorCategory, CheckGenericCategoryMessages)
{
	EXPECT_STREQ(GENERIC_ERROR_CATEGORY.message(0), "success (generic)");
	EXPECT_STREQ(GENERIC_ERROR_CATEGORY.message(1), "failure (generic)");
	EXPECT_STREQ(GENERIC_ERROR_CATEGORY.message(-1), "failure (generic)");
}

TEST(TestErrorCategory, CheckExampleCategoryMessages)
{
	EXPECT_STREQ(EXAMPLE_ERROR_CATEGORY.message(0), "example: success");
	EXPECT_STREQ(EXAMPLE_ERROR_CATEGORY.message(1), "example: failure");
	EXPECT_STREQ(EXAMPLE_ERROR_CATEGORY.message(-1), "example: failure");
}

TEST(TestErrorCode, NoError)
{
	const ErrorCode error(0);

	EXPECT_EQ(error.value(), 0);
	EXPECT_EQ(error.category(), GENERIC_ERROR_CATEGORY);
	EXPECT_TRUE(error.ok());
	EXPECT_FALSE(error.bad());
}

TEST(TestErrorCode, DefaultError)
{
	const ErrorCode error(1);
	EXPECT_TRUE(error.bad());
	EXPECT_FALSE(error.ok());
	EXPECT_EQ(error.category(), GENERIC_ERROR_CATEGORY);
}

TEST(TestErrorCode, Copyable)
{
	const ErrorCode a(EXAMPLE_ERROR_CATEGORY, 42);
	const ErrorCode b = a;

	EXPECT_EQ(a, b);
	EXPECT_EQ(a.value(), b.value());
	EXPECT_EQ(a.category(), b.category());

	EXPECT_TRUE(a.bad());
	EXPECT_TRUE(b.bad());
}

TEST(TestErrorCode, Assignable)
{
	const ErrorCode a(EXAMPLE_ERROR_CATEGORY, 42);
	ErrorCode b;
	b = a;
	EXPECT_EQ(a, b);
	EXPECT_EQ(a.value(), b.value());
	EXPECT_EQ(a.category(), b.category());
}

TEST(TestErrorCode, AssignValueOnly)
{
	ErrorCode error(EXAMPLE_ERROR_CATEGORY);
	error = 1;
	EXPECT_EQ(error.category(), EXAMPLE_ERROR_CATEGORY);
	EXPECT_EQ(error.value(), 1);
}

TEST(TestErrorCode, Categorize)
{
	ErrorCode e;
	e.categorize(EXAMPLE_ERROR_CATEGORY);
	EXPECT_EQ(e.category(), EXAMPLE_ERROR_CATEGORY);
}

TEST(TestErrorCode, Message)
{
	EXPECT_STREQ(ErrorCode(EXAMPLE_ERROR_CATEGORY, 0).message(), "example: success");
	EXPECT_STREQ(ErrorCode(EXAMPLE_ERROR_CATEGORY, 1).message(), "example: failure");
	EXPECT_STREQ(ErrorCode(EXAMPLE_ERROR_CATEGORY, -1).message(), "example: failure");
}

}  // namespace OMR
