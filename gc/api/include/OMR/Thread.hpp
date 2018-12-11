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

#if !defined(OMR_THREAD_HPP_)
#define OMR_THREAD_HPP_

#include <OMR/ThreadingInterface.hpp>
#include <omrcfg.h>

namespace OMR
{

class Thread
{
public:
	Thread() : _attached(false) {}

	~Thread() { assert(!_attached); }

	omr_error_t attach(const ThreadingInterface& threading)
	{
		assert(threading.initialized());
		omr_error_t error = (omr_error_t)omrthread_attach_ex(&_self, J9THREAD_ATTR_DEFAULT);
		if (error == OMR_ERROR_NONE) {
			_attached = true;
		}
		return error;
	}

	void detach()
	{
		if (_attached) {
			omrthread_detach(_self);
			_attached = false;
		}
	}

	bool attached() const { return _attached; }

	omrthread_t data() { return _self; }

	const omrthread_t data() const { return _self; }

private:
	omrthread_t _self;
	bool _attached;
};

} // namespace OMR

#endif // OMR_THREAD_HPP_
