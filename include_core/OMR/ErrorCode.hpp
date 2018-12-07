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

#if !defined(OMR_ERRORCODE_HPP_)
#define OMR_ERRORCODE_HPP_

#include <omrcfg.h>
#include <OMR/GenericErrorCategory.hpp>

namespace OMR
{

/// An integer error code with an associated category.
/// An error code of zero is considered to be a success, or non-error. Any other
/// value indicates an error. The default category is the OMR::GENERIC_ERROR_CATEGORY.
///
/// The ErrorCategories are not 
class ErrorCode
{
public:
	ErrorCode() : _value(0), _category(&GENERIC_ERROR_CATEGORY) {}

	explicit ErrorCode(int value) : _value(value), _category(&GENERIC_ERROR_CATEGORY) {}

	explicit ErrorCode(const ErrorCategory& category) : _value(0), _category(&category) {}

	ErrorCode(const ErrorCategory& category, int value) : _value(value), _category(&category) {}

	ErrorCode(const ErrorCode& other) : _value(other.value()), _category(&other.category()) {}

	int value() const { return _value; }

	const ErrorCategory& category() const { return *_category; }

	/// Set the category
	ErrorCode& categorize(const ErrorCategory& category)
	{
		_category = &category;
		return *this;
	}

	/// Compare two ErrorCodes.
	/// For two ErrorCodes to be equal, they must have the same code and category.
	/// To check just the value, use one of value(), ok(), or bad().
	bool operator==(const ErrorCode& rhs) const
	{
		return _value == rhs.value() && _category == &rhs.category();
	}

	bool operator!=(const ErrorCode& rhs) const
	{
		return _value != rhs.value() && _category != &rhs.category();
	}

	/// Assign only the value, leaving the category unchanged.
	ErrorCode& operator=(int value)
	{
		_value = value;
		return *this;
	}

	/// Assign both the category and the value.
	ErrorCode& operator=(const ErrorCode& other)
	{
		_value = other.value();
		_category = &other.category();
		return *this;
	}

	/// Zero the error code and reset the category to default.
	void clear()
	{
		_value = 0;
		_category = &GENERIC_ERROR_CATEGORY;
	}

	/// Convert the error code to a message using the error category.
	/// The returned string is static constant data.
	const char* message() const { return category().message(value()); }

	/// True if the error code is zero.
	bool ok() const { return value() == 0; }

	/// True if the error code is nonzero.
	bool bad() const { return value() != 0; }

private:
	int _value;
	const ErrorCategory* _category;
};

}  // namespace OMR

#endif // OMR_ERRORCODE_HPP_
