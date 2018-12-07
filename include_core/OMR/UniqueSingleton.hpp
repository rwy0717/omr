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

#if !defined(OMR_UNIQUESINGLETON_HPP_)
#define OMR_UNIQUESINGLETON_HPP_

#include <omrcfg.h>
#include <OMR/Unique.hpp>

namespace OMR
{

/// A static, globally unique constant.
///
/// UniqueSingletons are unique values that are not copyable, nor assignable.
///
/// This is a utility for creating globally unique constants. A library can
/// extern a set of UniqueSingleton constants, which can be compared to
/// constants in other libraries. In some ways, UniqueSingletons represent
/// an alternative way to achieve extensible enumerations.
class UniqueSingleton : public Unique
{
public:
	UniqueSingleton() : Unique() {}

private:
	UniqueSingleton(const UniqueSingleton&);

	UniqueSingleton& operator=(const UniqueSingleton&);
};

}  // namespace OMR

#endif // OMR_UNIQUESINGLETON_HPP_
