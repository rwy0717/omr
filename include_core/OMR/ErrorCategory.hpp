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
 
#if !defined(OMR_ERRORCATEGORY_HPP_)
#define OMR_ERRORCATEGORY_HPP_

#include <omrcfg.h>
#include <OMR/UniqueSingleton.hpp>

namespace OMR
{

/// An ErrorCategory is used to distinguish error codes from multiple sources.
/// Each category is able to translate error codes into string messages.
/// The ErrorCode is responsible for pairing a category with an integer code.
///
/// To define a new ErrorCategory, subclass this type and implement the message
/// virtual member function. Create a const singleton instance of that type,
/// using that as your category when creating ErrorCodes.
///
/// Category equivalence is determined by address/identity, so no two different
/// objects will ever be equal. This means it is safe to reuse a single
/// implementation for two different categories.
///
/// There exists one built-in category, GENERIC_ERROR_CATEGORY, which is
/// generally used as a default when no category is used.
class ErrorCategory : public UniqueSingleton
{
public:
	ErrorCategory() {}

	/// Convert a code to a message. The string is static, unowned data.
	/// Subclasses implement this function to translate integer codes to
	/// string messages. No default implementation.
	virtual const char* message(int code) const = 0;
};

}  // namespace OMR

#endif  // OMR_ERRORCATEGORY_HPP_
