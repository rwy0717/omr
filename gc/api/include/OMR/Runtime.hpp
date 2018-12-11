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

#if !defined(OMR_RUNTIME_HPP_)
#define OMR_RUNTIME_HPP_

#include <OMR/PlatformInterface.hpp>
#include <omr.h>
#include <omrcfg.h>
#include <string.h>

namespace OMR
{

/// The runtime is for initializing OMR globals.
/// Users may only create a single runtime in each process. The runtime must be initalized and torn down manually.
class Runtime
{
public:
	Runtime() : _platform(), _data(), _initialized(false) {}

	~Runtime() { assert(!initialized()); }

	/// Initialize the process runtime.
	omr_error_t initialize()
	{
		assert(!_initialized);
		omr_error_t error = OMR_ERROR_NONE;
		error = _platform.initialize();
		if (error == OMR_ERROR_NONE) {
			memset(&_data, 0, sizeof(OMR_Runtime));
			_data._configuration._maximum_vm_count = 0;
			_data._vmCount = 0;
			_data._vmList = nullptr;
			_data._portLibrary = &platform().library();
			error = omr_initialize_runtime(&_data);
			if (error == OMR_ERROR_NONE) {
				_initialized = true;
			} else {
				_platform.tearDown();
			}
		}
		return error;
	}

	void tearDown()
	{
		if (_initialized) {
			omr_destroy_runtime(&_data);
			_platform.tearDown();
			_initialized = false;
		}
	}

	bool initialized() const { return _initialized; }

	PlatformInterface& platform() { return _platform; }

	const PlatformInterface& platform() const { return _platform; }

	OMR_Runtime& data() { return _data; }

	const OMR_Runtime& data() const { return _data; }

private:
	PlatformInterface _platform;
	OMR_Runtime _data;
	bool _initialized;
};

} // namespace OMR

#endif // OMR_RUNTIME_HPP_
