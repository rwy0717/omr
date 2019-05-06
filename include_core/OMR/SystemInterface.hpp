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

#if !defined(OMR_SYSTEMINTERFACE_HPP_)
#define OMR_SYSTEMINTERFACE_HPP_

#include "omrcfg.h"
#include "omr.h"
#include "OMR/PortLibraryHandle.hpp"

namespace OMR
{

/**
 *  An object representing the threading subsystem.
 */
class ThreadingInterface
{
public:
	ThreadingInterface() : _initialized(false) {}

	~ThreadingInterface() { tearDown(); }

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

/**
 * An RAII object handling thread attachment.
 * For an OS-thread to participate to make calls into the OMR threading subsystem, that thread first has to be attached.
 * This object manages the attach/detach process for the current thread. The CurrentThread instance will automatically
 * detach when it goes out of scope.
 *
 * Usage:
 *     ThreadingInterface threading;
 *     CurrentThread self;
 *     self.attach(threading);
 *
 */
class CurrentThread
{
public:
	/**
	 * Construct a new handle to the current thread that is not attached to the threading subsystem.
	 */
	CurrentThread() : _attached(false) {}

	/**
	 * Construct a handle to the current thread and attach to the threading subsystem.
	 * If the thread fails to attach, abort.
	 */
	CurrentThread(ThreadingInterface* threading) : _attached(false) { assert(attach(threading) == OMR_ERROR_NONE); }

	/**
	 * Destroy the object, and detach the current thread if needed.
	 */
	~CurrentThread() { detach(); }

	/**
	 * Attach this thread to the threading subsystem.
	 * An OS thread must be attached before calling into the threading system.
	 */
	omr_error_t attach(ThreadingInterface* threading)
	{
		assert(threading->initialized());
		omr_error_t error = (omr_error_t)omrthread_attach_ex(&_self, J9THREAD_ATTR_DEFAULT);
		if (error == OMR_ERROR_NONE) {
			_attached = true;
		}
		return error;
	}

	/**
	 * Manually detach a thread from the threading subsystem.
	 * Note that this will occur automatically when this object is destroyed.
	 */
	void detach()
	{
		if (_attached) {
			omrthread_detach(_self);
			_attached = false;
		}
	}

	/**
	 * True if this thread has been attached.
	 */
	bool attached() const { return _attached; }

	omrthread_t data() { return _self; }

	const omrthread_t data() const { return _self; }

private:
	omrthread_t _self;
	bool _attached;
};

class PortLibrary;

/**
 * Thread and Port Library Wrapper.
 */
class SystemInterface
{
public:
	SystemInterface() : _initialized(false) {}

	~SystemInterface() { tearDown(); }

	omr_error_t initialize()
	{
		assert(!_initialized);
		omr_error_t error = _threading.initialize();
		if (error == OMR_ERROR_NONE) {
			CurrentThread self;
			error = self.attach(&_threading);
			if (error == OMR_ERROR_NONE) {
				error = (omr_error_t)omrport_init_library(&_library, sizeof(OMRPortLibrary));
				self.detach();
				if (error == OMR_ERROR_NONE) {
					_initialized = true;
				} else {
					_threading.tearDown();
				}
			} else {
				_threading.tearDown();
			}
		}
		return error;
	}

	void tearDown()
	{
		if (_initialized) {
			CurrentThread self;
			self.attach(&_threading);
			_library.port_shutdown_library(&_library);
			self.detach();
			_threading.tearDown();
			_initialized = false;
		}
	}

	bool initialized() const { return _initialized; }

	/**
	 * @group Subsystem access.
	 * @{
	 */

	/**
	 * Access the underlying function table of the platform interface.
	 * TODO: Const access to the port library.
	 */
	PortLibraryHandle library() { return PortLibraryHandle(&_library); }

	/**
	 * Access the platform threading interface
	 */
	const ThreadingInterface& threading() const { return _threading; }

	/**
	 * @}
	 */

private:
	ThreadingInterface _threading;
	OMRPortLibrary _library;
	bool _initialized;
};


} /* namespace OMR */

#endif /* OMR_SYSTEMINTERFACE_HPP_ */
