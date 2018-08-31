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

#if !defined(OMR_INFRA_POINTERUTILITIES_HPP_)
#define OMR_INFRA_POINTERUTILITIES_HPP_

#include <cstddef>
#include <cstdint>

namespace OMR {
namespace Om {
template<typename T, typename U>
std::ptrdiff_t ptrdiff(T* base, U* ptr) {
	return reinterpret_cast<std::uintptr_t>(ptr) - reinterpret_cast<std::uintptr_t>(base);
}

template<typename ResultT, typename BaseT>
ResultT* ptradd(BaseT* base, std::ptrdiff_t offset) {
	return reinterpret_cast<ResultT*>(reinterpret_cast<std::uintptr_t>(base) + offset);
}

} // namespace Om
} // namespace OMR

#endif // OMR_INFRA_POINTERUTILITIES_HPP_
