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

#include <omrcfg.h> 
#include <OMR/PlatformErrorCategory.hpp>
#include <stddef.h>

namespace OMR
{

const char *PlatformErrorCategory::message(int code) const
{
	const char *msg = NULL;
	switch (code) {
	case OMR_ERROR_NONE:
		msg = "OMR_ERROR_NONE";
		break;
	case OMR_ERROR_OUT_OF_NATIVE_MEMORY:
		msg = "OMR_ERROR_OUT_OF_NATIVE_MEMORY";
		break;
	case OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD:
		msg = "OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD";
		break;
	case OMR_ERROR_MAXIMUM_VM_COUNT_EXCEEDED:
		msg = "OMR_ERROR_MAXIMUM_VM_COUNT_EXCEEDED";
		break;
	case OMR_ERROR_MAXIMUM_THREAD_COUNT_EXCEEDED:
		msg = "OMR_ERROR_MAXIMUM_THREAD_COUNT_EXCEEDED";
		break;
	case OMR_THREAD_STILL_ATTACHED:
		msg = "OMR_THREAD_STILL_ATTACHED";
		break;
	case OMR_VM_STILL_ATTACHED:
		msg = "OMR_VM_STILL_ATTACHED";
		break;
	case OMR_ERROR_FAILED_TO_ALLOCATE_MONITOR:
		msg = "OMR_ERROR_FAILED_TO_ALLOCATE_MONITOR";
		break;
	case OMR_ERROR_INTERNAL:
		msg = "OMR_ERROR_INTERNAL";
		break;
	case OMR_ERROR_ILLEGAL_ARGUMENT:
		msg = "OMR_ERROR_ILLEGAL_ARGUMENT";
		break;
	case OMR_ERROR_NOT_AVAILABLE:
		msg = "OMR_ERROR_NOT_AVAILABLE";
		break;
	case OMR_THREAD_NOT_ATTACHED:
		msg = "OMR_THREAD_NOT_ATTACHED";
		break;
	case OMR_ERROR_FILE_UNAVAILABLE:
		msg = "OMR_ERROR_FILE_UNAVAILABLE";
		break;
	case OMR_ERROR_RETRY:
		msg = "OMR_ERROR_RETRY";
		break;
	default :
		msg = "Unknown error!";
		break;
	}
	return msg;
}

const PlatformErrorCategory PLATFORM_ERROR_CATEGORY;

}  // namespace OMR
