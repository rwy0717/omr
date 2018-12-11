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

#if !defined(OMR_THREADINGINTERFACE_HPP_)
#define OMR_THREADINGINTERFACE_HPP_

#include <assert.h>
#include <omr.h>
#include <omrcfg.h>
#include <thread_api.h>

namespace OMR
{

/// OMR Thread library wrapper.
class ThreadingInterface
{
public:
	ThreadingInterface() : _initialized(false) {}

	~ThreadingInterface() { assert(!_initialized); }

	omr_error_t initialize()
	{
		assert(!_initialized);
		omr_error_t error = (omr_error_t)omrthread_init_library();
		if (error == OMR_ERROR_NONE) {
			_initialized = true;
		}
		return error;
	}

	void tearDown()
	{
		if (_initialized) {
			omrthread_shutdown_library();
			_initialized = false;
		}
	}

	bool initialized() const { return _initialized; }

private:
	bool _initialized;
};

} // namespace OMR

#endif // OMR_THREADINGINTERFACE_HPP_
