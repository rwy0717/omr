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

#if !defined(OMR_PLATFORMINTERFACE_HPP_)
#define OMR_PLATFORMINTERFACE_HPP_

#include <assert.h>
#include <omr.h>
#include <omrcfg.h>
#include <stdlib.h>
#include <omrport.h>

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
	 */
	OMRPortLibrary* library() { return &_library; }

	const OMRPortLibrary* library() const { return &_library; }

	/**
	 * Access the platform threading interface
	 */
	const ThreadingInterface& threading() const { return _threading; }

	/**
	 * @}
	 */

	int32_t port_isFunctionOverridden(uintptr_t offset) { return _library.port_isFunctionOverridden(&_library, offset); }

	void port_tls_free() { _library.port_tls_free(&_library); }

	int32_t error_last_error_number() { return _library.error_last_error_number(&_library); }

	const char* error_last_error_message() { return _library.error_last_error_message(&_library); }

	/**
	 * @group Time
	 * @{
	 */

	uintptr_t time_msec_clock() { return _library.time_msec_clock(&_library); }

	uintptr_t time_usec_clock() { return _library.time_usec_clock(&_library); }

	uint64_t time_current_time_nanos(uintptr_t* success) { return _library.time_current_time_nanos(&_library, success); }

	int64_t time_current_time_millis() { return _library.time_current_time_millis(&_library); }

	int64_t time_nano_time() { return _library.time_nano_time(&_library); }

	uint64_t time_hires_clock() { return _library.time_hires_clock(&_library); }

	uint64_t time_hires_frequency() { return _library.time_hires_frequency(&_library); }

	uint64_t time_hires_delta(uint64_t startTime, uint64_t endTime, uint64_t requiredResolution)
	{
		return _library.time_hires_delta(&_library, startTime, endTime, requiredResolution);
	}

	/**
	 * @}
	 */

	/**
	 * @group Sysinfo
	 */

	intptr_t sysinfo_process_exists(uintptr_t pid) { return _library.sysinfo_process_exists(&_library, pid); }

	uintptr_t sysinfo_get_egid() { return _library.sysinfo_get_egid(&_library); }

	uintptr_t sysinfo_get_euid() { return _library.sysinfo_get_euid(&_library); }

	intptr_t sysinfo_get_groups(uint32_t** gidList, uint32_t categoryCode)
	{
		return _library.sysinfo_get_groups(&_library, gidList, categoryCode);
	}

	uintptr_t sysinfo_get_pid() { return _library.sysinfo_get_pid(&_library); }

	uintptr_t sysinfo_get_ppid() { return _library.sysinfo_get_ppid(&_library); }

	/**
	 * Note: This interface is missing a variant version of this function called sysinfo_get_memory_info_with_flag
	 */
	int32_t sysinfo_get_memory_info(J9MemoryInfo* memInfo) { return _library.sysinfo_get_memory_info(&_library, memInfo); }

	int32_t sysinfo_get_processor_info(J9ProcessorInfos* procInfo)
	{
		return _library.sysinfo_get_processor_info(&_library, procInfo);
	}

	void sysinfo_destroy_processor_info(J9ProcessorInfos* procInfos)
	{
		_library.sysinfo_destroy_processor_info(&_library, procInfos);
	}

	uint64_t sysinfo_get_addressable_physical_memory()
	{
		return _library.sysinfo_get_addressable_physical_memory(&_library);
	}

	uint64_t sysinfo_get_physical_memory() { return _library.sysinfo_get_physical_memory(&_library); }

	const char* sysinfo_get_OS_version() { return _library.sysinfo_get_OS_version(&_library); }

	intptr_t sysinfo_get_env(const char* envVar, char* infoString, uintptr_t bufSize)
	{
		return _library.sysinfo_get_env(&_library, envVar, infoString, bufSize);
	}

	const char* sysinfo_get_CPU_architecture() { return _library.sysinfo_get_CPU_architecture(&_library); }

	const char* sysinfo_get_OS_type() { return _library.sysinfo_get_OS_type(&_library); }

	intptr_t sysinfo_get_executable_name(const char* argv0, char** result)
	{
		return _library.sysinfo_get_executable_name(&_library, argv0, result);
	}

	intptr_t sysinfo_get_username(char* buffer, uintptr_t length)
	{
		return _library.sysinfo_get_username(&_library, buffer, length);
	}

	intptr_t sysinfo_get_groupname(char* buffer, uintptr_t length)
	{
		return _library.sysinfo_get_groupname(&_library, buffer, length);
	}

	intptr_t sysinfo_get_hostname(char* buffer, size_t length)
	{
		return _library.sysinfo_get_hostname(&_library, buffer, length);
	}

	intptr_t sysinfo_get_load_average(J9PortSysInfoLoadData* loadAverageData)
	{
		return _library.sysinfo_get_load_average(&_library, loadAverageData);
	}

	intptr_t sysinfo_get_CPU_utilization(J9SysinfoCPUTime* cpuTime)
	{
		return _library.sysinfo_get_CPU_utilization(&_library, cpuTime);
	}

	int32_t sysinfo_limit_iterator_init(J9SysinfoLimitIteratorState* state)
	{
		return _library.sysinfo_limit_iterator_init(&_library, state);
	}

	bool sysinfo_limit_iterator_hasNext(J9SysinfoLimitIteratorState* state)
	{
		return _library.sysinfo_limit_iterator_hasNext(&_library, state);
	}

	int32_t sysinfo_limit_iterator_next(J9SysinfoLimitIteratorState* state, J9SysinfoUserLimitElement* limitElement)
	{
		return _library.sysinfo_limit_iterator_next(&_library, state, limitElement);
	}

	int32_t sysinfo_env_iterator_init(J9SysinfoEnvIteratorState* state, void* buffer, uintptr_t bufferSizeBytes)
	{
		return _library.sysinfo_env_iterator_init(&_library, state, buffer, bufferSizeBytes);
	}

	bool sysinfo_env_iterator_hasNext(J9SysinfoEnvIteratorState* state)
	{
		return _library.sysinfo_env_iterator_hasNext(&_library, state);
	}

	int32_t sysinfo_env_iterator_next(J9SysinfoEnvIteratorState* state, J9SysinfoEnvElement* envElement)
	{
		return _library.sysinfo_env_iterator_next(&_library, state, envElement);
	}

	uint32_t sysinfo_get_limit(uint32_t resourceID, uint64_t* limit)
	{
		return _library.sysinfo_get_limit(&_library, resourceID, limit);
	}

	uint32_t sysinfo_set_limit(uint32_t resourceID, uint64_t limit)
	{
		return _library.sysinfo_set_limit(&_library, resourceID, limit);
	}

	uintptr_t sysinfo_get_number_CPUs_by_type(uintptr_t type)
	{
		return _library.sysinfo_get_number_CPUs_by_type(&_library, type);
	}

	intptr_t sysinfo_get_cwd(char* buf, uintptr_t bufLen) { return _library.sysinfo_get_cwd(&_library, buf, bufLen); }

	intptr_t sysinfo_get_tmp(char* buf, uintptr_t bufLen, bool ignoreEnvVariable)
	{
		return _library.sysinfo_get_tmp(&_library, buf, bufLen, ignoreEnvVariable);
	}

	void sysinfo_set_number_user_specified_CPUs(uintptr_t number)
	{
		_library.sysinfo_set_number_user_specified_CPUs(&_library, number);
	}

	int32_t sysinfo_get_open_file_count(uint64_t* count) { return _library.sysinfo_get_open_file_count(&_library, count); }

	intptr_t sysinfo_get_os_description(OMROSDesc* desc) { return _library.sysinfo_get_os_description(&_library, desc); }

	bool sysinfo_os_has_feature(OMROSDesc* desc, uint32_t feature)
	{
		return _library.sysinfo_os_has_feature(&_library, desc, feature);
	}

	bool sysinfo_os_kernel_info(OMROSKernelInfo* kernelInfo)
	{
		return _library.sysinfo_os_kernel_info(&_library, kernelInfo);
	}

	bool sysinfo_cgroup_is_system_available() { return _library.sysinfo_cgroup_is_system_available(&_library); }

	uint64_t sysinfo_cgroup_get_available_subsystems()
	{
		return _library.sysinfo_cgroup_get_available_subsystems(&_library);
	}

	uint64_t sysinfo_cgroup_are_subsystems_available(uint64_t subsystemFlags)
	{
		return _library.sysinfo_cgroup_are_subsystems_available(&_library, subsystemFlags);
	}

	uint64_t sysinfo_cgroup_get_enabled_subsystems() { return _library.sysinfo_cgroup_get_enabled_subsystems(&_library); }

	uint64_t sysinfo_cgroup_enable_subsystems(uint64_t requestedSubsystems)
	{
		return _library.sysinfo_cgroup_enable_subsystems(&_library, requestedSubsystems);
	}

	uint64_t sysinfo_cgroup_are_subsystems_enabled(uint64_t subsystemFlags)
	{
		return _library.sysinfo_cgroup_are_subsystems_enabled(&_library, subsystemFlags);
	}

	int32_t sysinfo_cgroup_get_memlimit(uint64_t* limit) { return _library.sysinfo_cgroup_get_memlimit(&_library, limit); }

	bool sysinfo_cgroup_is_memlimit_set() { return _library.sysinfo_cgroup_is_memlimit_set(&_library); }

	OMRCgroupEntry* sysinfo_get_cgroup_subsystem_list() { return _library.sysinfo_get_cgroup_subsystem_list(&_library); }

	bool sysinfo_is_running_in_container() { return _library.sysinfo_is_running_in_container(&_library); }

	int32_t sysinfo_cgroup_subsystem_iterator_init(uint64_t subsystem, OMRCgroupMetricIteratorState* state)
	{
		return _library.sysinfo_cgroup_subsystem_iterator_init(&_library, subsystem, state);
	}

	bool sysinfo_cgroup_subsystem_iterator_hasNext(const OMRCgroupMetricIteratorState* state)
	{
		return _library.sysinfo_cgroup_subsystem_iterator_hasNext(&_library, state);
	}

	int32_t
	sysinfo_cgroup_subsystem_iterator_metricKey(const OMRCgroupMetricIteratorState* state, const char** metricKey)
	{
		return _library.sysinfo_cgroup_subsystem_iterator_metricKey(&_library, state, metricKey);
	}

	int32_t sysinfo_cgroup_subsystem_iterator_next(OMRCgroupMetricIteratorState* state,
	                                               OMRCgroupMetricElement* metricElement)
	{
		return _library.sysinfo_cgroup_subsystem_iterator_next(&_library, state, metricElement);
	}

	void sysinfo_cgroup_subsystem_iterator_destroy(OMRCgroupMetricIteratorState* state)
	{
		_library.sysinfo_cgroup_subsystem_iterator_destroy(&_library, state);
	}

	/**
	 * @}
	 */

	/**
	 * @group File
	 */

	intptr_t file_write(intptr_t fd, const void* buf, intptr_t nbytes)
	{
		return _library.file_write(&_library, fd, buf, nbytes);
	}

	intptr_t file_write_text(intptr_t fd, const char* buf, intptr_t nbytes)
	{
		return _library.file_write_text(&_library, fd, buf, nbytes);
	}

	int32_t file_get_text_encoding(char* charsetName, uintptr_t nbytes)
	{
		return _library.file_get_text_encoding(&_library, charsetName, nbytes);
	}

	void file_vprintf(intptr_t fd, const char* format, va_list args) { _library.file_vprintf(&_library, fd, format, args); }

	/* TODO: void file_printf(intptr_t fd, const char* format, ...) { _library.file_printf(&_library); } */

	intptr_t file_open(const char* path, int32_t flags, int32_t mode)
	{
		return _library.file_open(&_library, path, flags, mode);
	}

	int32_t file_close(intptr_t fd) { return _library.file_close(&_library, fd); }

	int64_t file_seek(intptr_t fd, int64_t offset, int32_t whence)
	{
		return _library.file_seek(&_library, fd, offset, whence);
	}

	intptr_t file_read(intptr_t fd, void* buf, intptr_t nbytes) { return _library.file_read(&_library, fd, buf, nbytes); }

	int32_t file_unlink(const char* path) { return _library.file_unlink(&_library, path); }

	int32_t file_attr(const char* path) { return _library.file_attr(&_library, path); }

	int32_t file_chmod(const char* path, int32_t mode) { return _library.file_chmod(&_library, path, mode); }

	intptr_t file_chown(const char* path, uintptr_t owner, uintptr_t group)
	{
		return _library.file_chown(&_library, path, owner, group);
	}

	int64_t file_lastmod(const char* path) { return _library.file_lastmod(&_library, path); }

	int64_t file_length(const char* path) { return _library.file_length(&_library, path); }

	int64_t file_flength(intptr_t fd) { return _library.file_flength(&_library, fd); }

	int32_t file_set_length(intptr_t fd, int64_t newLength) { return _library.file_set_length(&_library, fd, newLength); }

	int32_t file_sync(intptr_t fd) { return _library.file_sync(&_library, fd); }

	int32_t file_fstat(intptr_t fd, J9FileStat* buf) { return _library.file_fstat(&_library, fd, buf); }

	int32_t file_stat(const char* path, uint32_t flags, J9FileStat* buf)
	{
		return _library.file_stat(&_library, path, flags, buf);
	}

	int32_t file_stat_filesystem(const char* path, uint32_t flags, J9FileStatFilesystem* buf)
	{
		return _library.file_stat_filesystem(&_library, path, flags, buf);
	}

	intptr_t file_blockingasync_open(const char* path, int32_t flags, int32_t mode)
	{
		return _library.file_blockingasync_open(&_library, path, flags, mode);
	}

	int32_t file_blockingasync_close(intptr_t fd) { return _library.file_blockingasync_close(&_library, fd); }

	intptr_t file_blockingasync_read(intptr_t fd, void* buf, intptr_t nbytes)
	{
		return _library.file_blockingasync_read(&_library, fd, buf, nbytes);
	}

	intptr_t file_blockingasync_write(intptr_t fd, const void* buf, intptr_t nbytes)
	{
		return _library.file_blockingasync_write(&_library, fd, buf, nbytes);
	}

	int32_t file_blockingasync_set_length(intptr_t fd, int64_t newLength)
	{
		return _library.file_blockingasync_set_length(&_library, fd, newLength);
	}

	int64_t file_blockingasync_flength(intptr_t fd) { return _library.file_blockingasync_flength(&_library, fd); }

	char* file_read_text(intptr_t fd, char* buf, intptr_t nbytes)
	{
		return _library.file_read_text(&_library, fd, buf, nbytes);
	}

	int32_t file_mkdir(const char* path) { return _library.file_mkdir(&_library, path); }

	int32_t file_move(const char* pathExist, const char* pathNew)
	{
		return _library.file_move(&_library, pathExist, pathNew);
	}

	int32_t file_unlinkdir(const char* path) { return _library.file_unlinkdir(&_library, path); }

	uintptr_t file_findfirst(const char* path, char* resultbuf)
	{
		return _library.file_findfirst(&_library, path, resultbuf);
	}

	int32_t file_findnext(uintptr_t findhandle, char* resultbuf)
	{
		return _library.file_findnext(&_library, findhandle, resultbuf);
	}

	void file_findclose(uintptr_t findhandle) { _library.file_findclose(&_library, findhandle); }

	const char* file_error_message() { return _library.file_error_message(&_library); }

	int32_t file_unlock_bytes(intptr_t fd, uint64_t offset, uint64_t length)
	{
		return _library.file_unlock_bytes(&_library, fd, offset, length);
	}

	int32_t file_lock_bytes(intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length)
	{
		return _library.file_lock_bytes(&_library, fd, lockFlags, offset, length);
	}

	intptr_t file_convert_native_fd_to_omrfile_fd(intptr_t nativeFD)
	{
		return _library.file_convert_native_fd_to_omrfile_fd(&_library, nativeFD);
	}

	intptr_t file_convert_omrfile_fd_to_native_fd(intptr_t omrfileFD)
	{
		return _library.file_convert_omrfile_fd_to_native_fd(&_library, omrfileFD);
	}

	int32_t file_blockingasync_unlock_bytes(intptr_t fd, uint64_t offset, uint64_t length)
	{
		return _library.file_blockingasync_unlock_bytes(&_library, fd, offset, length);
	}

	int32_t file_blockingasync_lock_bytes(intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length)
	{
		return _library.file_blockingasync_lock_bytes(&_library, fd, lockFlags, offset, length);
	}

	/**
	 * @}
	 */

	/**
	 * @group Filestream
	 */

	OMRFileStream* filestream_open(const char* path, int32_t flags, int32_t mode)
	{
		return _library.filestream_open(&_library, path, flags, mode);
	}

	int32_t filestream_close(OMRFileStream* fileStream) { return _library.filestream_close(&_library, fileStream); }

	intptr_t filestream_write(OMRFileStream* fileStream, const void* buf, intptr_t nbytes)
	{
		return _library.filestream_write(&_library, fileStream, buf, nbytes);
	}

	intptr_t filestream_write_text(OMRFileStream* fileStream, const char* buf, intptr_t nbytes, int32_t toCode)
	{
		return _library.filestream_write_text(&_library, fileStream, buf, nbytes, toCode);
	}

	void filestream_vprintf(OMRFileStream* fileStream, const char* format, va_list args)
	{
		_library.filestream_vprintf(&_library, fileStream, format, args);
	}

	void filestream_printf(OMRFileStream* filestream, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		_library.filestream_vprintf(&_library, filestream, format, args);
		va_end(args);
	}

	int32_t filestream_sync(OMRFileStream* fileStream) { return _library.filestream_sync(&_library, fileStream); }

	int32_t filestream_setbuffer(OMRFileStream* fileStream, char* buffer, int32_t mode, uintptr_t size)
	{
		return _library.filestream_setbuffer(&_library, fileStream, buffer, mode, size);
	}

	OMRFileStream* filestream_fdopen(intptr_t fd, int32_t flags)
	{
		return _library.filestream_fdopen(&_library, fd, flags);
	}

	intptr_t filestream_fileno(OMRFileStream* stream) { return _library.filestream_fileno(&_library, stream); }

	/**
	 * @}
	 */

	/**
	 * @group SL
	 */

	uintptr_t sl_close_shared_library(uintptr_t descriptor)
	{
		return _library.sl_close_shared_library(&_library, descriptor);
	}

	uintptr_t sl_open_shared_library(char* name, uintptr_t* descriptor, uintptr_t flags)
	{
		return _library.sl_open_shared_library(&_library, name, descriptor, flags);
	}

	uintptr_t sl_lookup_name(uintptr_t descriptor, char* name, uintptr_t* func, const char* argSignature)
	{
		return _library.sl_lookup_name(&_library, descriptor, name, func, argSignature);
	}

	/**
	 * @}
	 */

	/**
	 * @group TTY
	 */

	void tty_printf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		_library.tty_vprintf(&_library, format, args);
		va_end(args);
	}

	void tty_vprintf(const char* format, va_list args) { _library.tty_vprintf(&_library, format, args); }

	intptr_t tty_get_chars(char* s, uintptr_t length) { return _library.tty_get_chars(&_library, s, length); }

	void tty_err_printf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		_library.tty_err_vprintf(&_library, format, args);
		va_end(args);
	}

	void tty_err_vprintf(const char* format, va_list args) { _library.tty_err_vprintf(&_library, format, args); }

	intptr_t tty_available() { return _library.tty_available(&_library); }

	void tty_daemonize() { _library.tty_daemonize(&_library); }

	/**
	 * @}
	 */

	/**
	 * @group Heap
	 */

	J9Heap* heap_create(void* heapBase, uintptr_t heapSize, uint32_t heapFlags)
	{
		return _library.heap_create(&_library, heapBase, heapSize, heapFlags);
	}

	void* heap_allocate(J9Heap* heap, uintptr_t byteAmount) { return _library.heap_allocate(&_library, heap, byteAmount); }

	void heap_free(J9Heap* heap, void* address) { _library.heap_free(&_library, heap, address); }

	void* heap_reallocate(J9Heap* heap, void* address, uintptr_t byteAmount)
	{
		return _library.heap_reallocate(&_library, heap, address, byteAmount);
	}

	uintptr_t heap_query_size(J9Heap* heap, void* address) { return _library.heap_query_size(&_library, heap, address); }

	bool heap_grow(J9Heap* heap, uintptr_t growAmount) { return _library.heap_grow(&_library, heap, growAmount); }

	/**
	 * @}
	 */

	/**
	 * @group Mem (Memory)
	 */

	void* mem_allocate_memory(uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _library.mem_allocate_memory(&_library, byteAmount, callSite, category);
	}

	void mem_free_memory(void* memoryPointer) { _library.mem_free_memory(&_library, memoryPointer); }

	void mem_advise_and_free_memory(void* memoryPointer) { _library.mem_advise_and_free_memory(&_library, memoryPointer); }

	void* mem_reallocate_memory(void* memoryPointer, uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _library.mem_reallocate_memory(&_library, memoryPointer, byteAmount, callSite, category);
	}

	void* mem_allocate_memory32(uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _library.mem_allocate_memory32(&_library, byteAmount, callSite, category);
	}

	void mem_free_memory32(void* memoryPointer) { _library.mem_free_memory32(&_library, memoryPointer); }

	uintptr_t mem_ensure_capacity32(uintptr_t byteAmount) { return _library.mem_ensure_capacity32(&_library, byteAmount); }

	void mem_walk_categories(OMRMemCategoryWalkState* state) { _library.mem_walk_categories(&_library, state); }

	OMRMemCategory* mem_get_category(uint32_t categoryCode) { return _library.mem_get_category(&_library, categoryCode); }

	void mem_categories_increment_counters(OMRMemCategory* category, uintptr_t size)
	{
		_library.mem_categories_increment_counters(category, size);
	}

	void mem_categories_decrement_counters(OMRMemCategory* category, uintptr_t size)
	{
		_library.mem_categories_decrement_counters(category, size);
	}

	/**
	 * @}
	 */

	/**
	 * @group CPU
	 */
	void cpu_flush_icache(void* memoryPointer, uintptr_t byteAmount)
	{
		_library.cpu_flush_icache(&_library, memoryPointer, byteAmount);
	}

	int32_t cpu_get_cache_line_size(int32_t* lineSize) { return _library.cpu_get_cache_line_size(&_library, lineSize); }

	/**
	 * @}
	 */

	/**
	 * @group VMem (Virtual Memory)
	 */

	void* vmem_commit_memory(void* address, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_commit_memory(&_library, address, byteAmount, identifier);
	}

	intptr_t vmem_decommit_memory(void* address, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_decommit_memory(&_library, address, byteAmount, identifier);
	}

	int32_t vmem_free_memory(void* userAddress, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_free_memory(&_library, userAddress, byteAmount, identifier);
	}

	int32_t vmem_vmem_params_init(J9PortVmemParams* params) { return _library.vmem_vmem_params_init(&_library, params); }

	void* vmem_reserve_memory(void* address,
	                          uintptr_t byteAmount,
	                          J9PortVmemIdentifier* identifier,
	                          uintptr_t mode,
	                          uintptr_t pageSize,
	                          uint32_t category)
	{
		return _library.vmem_reserve_memory(&_library, address, byteAmount, identifier, mode, pageSize, category);
	}

	void* vmem_reserve_memory_ex(J9PortVmemIdentifier* identifier, J9PortVmemParams* params)
	{
		return _library.vmem_reserve_memory_ex(&_library, identifier, params);
	}

	void* vmem_get_contiguous_region_memory(void* addresses[],
	                                        uintptr_t addressesCount,
	                                        uintptr_t addressSize,
	                                        uintptr_t byteAmount,
	                                        J9PortVmemIdentifier* oldIdentifier,
	                                        J9PortVmemIdentifier* newIdentifier,
	                                        uintptr_t mode,
	                                        uintptr_t pageSize,
	                                        OMRMemCategory* category)
	{
		return _library.vmem_get_contiguous_region_memory(&_library, addresses, addressesCount, addressSize, byteAmount,
		                                               oldIdentifier, newIdentifier, mode, pageSize, category);
	}

	uintptr_t vmem_get_page_size(J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_get_page_size(&_library, identifier);
	}

	uintptr_t vmem_get_page_flags(J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_get_page_flags(&_library, identifier);
	}

	uintptr_t* vmem_supported_page_sizes() { return _library.vmem_supported_page_sizes(&_library); }

	uintptr_t* vmem_supported_page_flags() { return _library.vmem_supported_page_flags(&_library); }

	void vmem_default_large_page_size_ex(uintptr_t mode, uintptr_t* pageSize, uintptr_t* pageFlags)
	{
		_library.vmem_default_large_page_size_ex(&_library, mode, pageSize, pageFlags);
	}

	intptr_t
	vmem_find_valid_page_size(uintptr_t mode, uintptr_t* pageSize, uintptr_t* pageFlags, BOOLEAN* isSizeSupported)
	{
		return _library.vmem_find_valid_page_size(&_library, mode, pageSize, pageFlags, isSizeSupported);
	}

	intptr_t vmem_numa_set_affinity(uintptr_t numaNode,
	                                void* address,
	                                uintptr_t byteAmount,
	                                J9PortVmemIdentifier* identifier)
	{
		return _library.vmem_numa_set_affinity(&_library, numaNode, address, byteAmount, identifier);
	}

	intptr_t vmem_numa_get_node_details(J9MemoryNodeDetail* numaNodes, uintptr_t* nodeCount)
	{
		return _library.vmem_numa_get_node_details(&_library, numaNodes, nodeCount);
	}

	int32_t vmem_get_available_physical_memory(uint64_t* freePhysicalMemorySize)
	{
		return _library.vmem_get_available_physical_memory(&_library, freePhysicalMemorySize);
	}

	int32_t vmem_get_process_memory_size(J9VMemMemoryQuery queryType, uint64_t* memorySize)
	{
		return _library.vmem_get_process_memory_size(&_library, queryType, memorySize);
	}

	/**
	 * @}
	 */

	/**
	 * @name Str (Strings)
	 * @{
	 */

	uintptr_t str_printf(char* buf, uintptr_t len, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		uintptr_t rc = _library.str_vprintf(&_library, buf, len, format, args);
		va_end(args);
		return rc;
	}

	uintptr_t str_vprintf(char* buf, uintptr_t bufLen, const char* format, va_list args)
	{
		return _library.str_vprintf(&_library, buf, bufLen, format, args);
	}

	J9StringTokens* str_create_tokens(int64_t timeMillis) { return _library.str_create_tokens(&_library, timeMillis); }

	/* TODO: intptr_t str_set_token(J9StringTokens* tokens, const char* key, const char* format, ...) { return _library.str_set_token(&_library, tokens, key, format); } */

	uintptr_t str_subst_tokens(char* buf, uintptr_t bufLen, const char* format, J9StringTokens* tokens)
	{
		return _library.str_subst_tokens(&_library, buf, bufLen, format, tokens);
	}

	void str_free_tokens(J9StringTokens* tokens) { _library.str_free_tokens(&_library, tokens); }

	intptr_t str_set_time_tokens(J9StringTokens* tokens, int64_t timeMillis)
	{
		return _library.str_set_time_tokens(&_library, tokens, timeMillis);
	}

	int32_t str_convert(int32_t fromCode,
	                    int32_t toCode,
	                    const char* inBuffer,
	                    uintptr_t inBufferSize,
	                    char* outBuffer,
	                    uintptr_t outBufferSize)
	{
		return _library.str_convert(&_library, fromCode, toCode, inBuffer, inBufferSize, outBuffer, outBufferSize);
	}

	uintptr_t str_ftime(char* buf, uintptr_t bufLen, const char* format, int64_t timeMillis)
	{
		return _library.str_ftime(&_library, buf, bufLen, format, timeMillis);
	}

	/**
	 * @}
	 */

	/**
	 * @name Exit and Image Dumps
	 * @{
	 */

	int32_t exit_get_exit_code() { return _library.exit_get_exit_code(&_library); }

	void exit_shutdown_and_exit(int32_t exitCode) { _library.exit_shutdown_and_exit(&_library, exitCode); }

	uintptr_t dump_create(char* filename, char* dumpType, void* userData)
	{
		return _library.dump_create(&_library, filename, dumpType, userData);
	}

	/**
	 * @}
	 */

	/**
	 * @name NLS (Native Language Support)
	 * @{
	 */

	void nls_free_cached_data() { _library.nls_free_cached_data(&_library); }

	void nls_shutdown() { _library.nls_shutdown(&_library); }

	void nls_set_catalog(const char** paths, const int nPaths, const char* baseName, const char* extension)
	{
		_library.nls_set_catalog(&_library, paths, nPaths, baseName, extension);
	}

	void nls_set_locale(const char* lang, const char* region, const char* variant)
	{
		_library.nls_set_locale(&_library, lang, region, variant);
	}

	const char* nls_get_language() { return _library.nls_get_language(&_library); }

	const char* nls_get_region() { return _library.nls_get_region(&_library); }

	const char* nls_get_variant() { return _library.nls_get_variant(&_library); }

	void nls_printf(uintptr_t flags, uint32_t module_name, uint32_t message_num, ...)
	{
		va_list args;
		va_start(args, message_num);
		_library.nls_vprintf(&_library, flags, module_name, message_num, args);
		va_end(args);
	}

	void nls_vprintf(uintptr_t flags, uint32_t module_name, uint32_t message_num, va_list args)
	{
		_library.nls_vprintf(&_library, flags, module_name, message_num, args);
	}

	const char*
	nls_lookup_message(uintptr_t flags, uint32_t module_name, uint32_t message_num, const char* default_string)
	{
		return _library.nls_lookup_message(&_library, flags, module_name, message_num, default_string);
	}

	/**
	 * @}
	 */

	int32_t port_control(const char* key, uintptr_t value) { return _library.port_control(&_library, key, value); }

	/**
	 * @name Sig (Signal Handling)
	 * @{
	 */

	int32_t sig_protect(omrsig_protected_fn fn,
	                    void* fn_arg,
	                    omrsig_handler_fn handler,
	                    void* handler_arg,
	                    uint32_t flags,
	                    uintptr_t* result)
	{
		return _library.sig_protect(&_library, fn, fn_arg, handler, handler_arg, flags, result);
	}

	int32_t sig_can_protect(uint32_t flags) { return _library.sig_can_protect(&_library, flags); }

	int32_t sig_set_async_signal_handler(omrsig_handler_fn handler, void* handler_arg, uint32_t flags)
	{
		return _library.sig_set_async_signal_handler(&_library, handler, handler_arg, flags);
	}

	int32_t sig_set_single_async_signal_handler(omrsig_handler_fn handler,
	                                            void* handler_arg,
	                                            uint32_t portlibSignalFlag,
	                                            void** oldOSHandler)
	{
		return _library.sig_set_single_async_signal_handler(&_library, handler, handler_arg, portlibSignalFlag,
		                                                 oldOSHandler);
	}

	uint32_t sig_map_os_signal_to_portlib_signal(uint32_t osSignalValue)
	{
		return _library.sig_map_os_signal_to_portlib_signal(&_library, osSignalValue);
	}

	int32_t sig_map_portlib_signal_to_os_signal(uint32_t portlibSignalFlag)
	{
		return _library.sig_map_portlib_signal_to_os_signal(&_library, portlibSignalFlag);
	}

	int32_t sig_register_os_handler(uint32_t portlibSignalFlag, void* newOSHandler, void** oldOSHandler)
	{
		return _library.sig_register_os_handler(&_library, portlibSignalFlag, newOSHandler, oldOSHandler);
	}

	bool sig_is_master_signal_handler(void* osHandler)
	{
		return _library.sig_is_master_signal_handler(&_library, osHandler);
	}

	int32_t sig_is_signal_ignored(uint32_t portlibSignalFlag, BOOLEAN* isSignalIgnored)
	{
		return _library.sig_is_signal_ignored(&_library, portlibSignalFlag, isSignalIgnored);
	}

	uint32_t sig_info(void* info, uint32_t category, int32_t index, const char** name, void** value)
	{
		return _library.sig_info(&_library, info, category, index, name, value);
	}

	uint32_t sig_info_count(void* info, uint32_t category) { return _library.sig_info_count(&_library, info, category); }

	int32_t sig_set_options(uint32_t options) { return _library.sig_set_options(&_library, options); }

	uint32_t sig_get_options() { return _library.sig_get_options(&_library); }

	intptr_t sig_get_current_signal() { return _library.sig_get_current_signal(&_library); }

	int32_t sig_set_reporter_priority(uintptr_t priority)
	{
		return _library.sig_set_reporter_priority(&_library, priority);
	}

	/**
	 * @}
	 */

	/**
	 * @name MMap (Memory Mappings, Virtual Memory)
	 * @{
	 */

	int32_t mmap_capabilities() { return _library.mmap_capabilities(&_library); }

	J9MmapHandle* mmap_map_file(intptr_t file,
	                            uint64_t offset,
	                            uintptr_t size,
	                            const char* mappingName,
	                            uint32_t flags,
	                            uint32_t category)
	{
		return _library.mmap_map_file(&_library, file, offset, size, mappingName, flags, category);
	}

	void mmap_unmap_file(J9MmapHandle* handle) { _library.mmap_unmap_file(&_library, handle); }

	intptr_t mmap_msync(void* start, uintptr_t length, uint32_t flags)
	{
		return _library.mmap_msync(&_library, start, length, flags);
	}

	intptr_t mmap_protect(void* address, uintptr_t length, uintptr_t flags)
	{
		return _library.mmap_protect(&_library, address, length, flags);
	}

	uintptr_t mmap_get_region_granularity(void* address)
	{
		return _library.mmap_get_region_granularity(&_library, address);
	}

	void mmap_dont_need(const void* startAddress, size_t length)
	{
		_library.mmap_dont_need(&_library, startAddress, length);
	}

	/**
	 * @}
	 */

	/**
	 * @name Syslog
	 * @{
	 */

	uintptr_t syslog_write(uintptr_t flags, const char* message)
	{
		return _library.syslog_write(&_library, flags, message);
	}

	uintptr_t syslog_query() { return _library.syslog_query(&_library); }

	void syslog_set(uintptr_t options) { return _library.syslog_set(&_library, options); }

	/**
	 * @}
	 */

	/**
	 * @name Introspect
	 * @{
	 */

	int32_t introspect_set_suspend_signal_offset(int32_t signalOffset)
	{
		return _library.introspect_set_suspend_signal_offset(&_library, signalOffset);
	}

	J9PlatformThread* introspect_threads_startDo(J9Heap* heap, J9ThreadWalkState* state)
	{
		return _library.introspect_threads_startDo(&_library, heap, state);
	}

	J9PlatformThread*
	introspect_threads_startDo_with_signal(J9Heap* heap, J9ThreadWalkState* state, void* signal_info)
	{
		return _library.introspect_threads_startDo_with_signal(&_library, heap, state, signal_info);
	}

	J9PlatformThread* introspect_threads_nextDo(J9ThreadWalkState* state)
	{
		return _library.introspect_threads_nextDo(state);
	}

	uintptr_t introspect_backtrace_thread(J9PlatformThread* thread, J9Heap* heap, void* signalInfo)
	{
		return _library.introspect_backtrace_thread(&_library, thread, heap, signalInfo);
	}

	uintptr_t introspect_backtrace_symbols(J9PlatformThread* thread, J9Heap* heap)
	{
		return _library.introspect_backtrace_symbols(&_library, thread, heap);
	}

	/**
	 * @}
	 */

#if defined(OMR_OPT_CUDA)

	/* TODO RWY */

	int32_t cuda_deviceAlloc(uint32_t deviceId, uintptr_t size, void** deviceAddressOut)
	{
		return _library.cuda_deviceAlloc(&_library, deviceId, size, deviceAddressOut);
	}

	int32_t cuda_deviceCanAccessPeer(uint32_t deviceId, uint32_t peerDeviceId, BOOLEAN* canAccessPeerOut)
	{
		return _library.cuda_deviceCanAccessPeer(&_library, deviceId, peerDeviceId, canAccessPeerOut);
	}

	int32_t cuda_deviceDisablePeerAccess(uint32_t deviceId, uint32_t peerDeviceId)
	{
		return _library.cuda_deviceDisablePeerAccess(&_library, deviceId, peerDeviceId);
	}

	int32_t cuda_deviceEnablePeerAccess(uint32_t deviceId, uint32_t peerDeviceId)
	{
		return _library.cuda_deviceEnablePeerAccess(&_library, deviceId, peerDeviceId);
	}

	int32_t cuda_deviceFree(OMRPortLibrary* OMRPortLibrary, uint32_t deviceId, void* devicePointer)
	{
		return _library.cuda_deviceFree(&_library, deviceId, devicePointer);
	}

	int32_t cuda_deviceGetAttribute(uint32_t deviceId, J9CudaDeviceAttribute attribute, int32_t* valueOut)
	{
		return _library.cuda_deviceGetAttribute(&_library, deviceId, attribute, valueOut);
	}

	int32_t cuda_deviceGetCacheConfig(uint32_t deviceId, J9CudaCacheConfig* configOut)
	{
		return _library.cuda_deviceGetCacheConfig(&_library, deviceId, configOut);
	}

	int32_t cuda_deviceGetCount(uint32_t* countOut) { return _library.cuda_deviceGetCount(&_library, countOut); }

	int32_t cuda_deviceGetLimit(uint32_t deviceId, J9CudaDeviceLimit limit, uintptr_t* valueOut)
	{
		return _library.cuda_deviceGetLimit(&_library, deviceId, limit, valueOut);
	}

	int32_t cuda_deviceGetMemInfo(uint32_t deviceId, uintptr_t* freeOut, uintptr_t* totalOut)
	{
		return _library.cuda_deviceGetMemInfo(&_library, deviceId, freeOut, totalOut);
	}

	int32_t cuda_deviceGetName(uint32_t deviceId, uint32_t nameSize, char* nameOut)
	{
		return _library.cuda_deviceGetName(&_library, deviceId, nameSize, nameOut);
	}

	int32_t cuda_deviceGetSharedMemConfig(uint32_t deviceId, J9CudaSharedMemConfig* configOut)
	{
		return _library.cuda_deviceGetSharedMemConfig(&_library, deviceId, configOut);
	}

	int32_t
	cuda_deviceGetStreamPriorityRange(uint32_t deviceId, int32_t* leastPriorityOut, int32_t* greatestPriorityOut)
	{
		return _library.cuda_deviceGetStreamPriorityRange(&_library, deviceId, leastPriorityOut, greatestPriorityOut);
	}

	int32_t cuda_deviceReset(uint32_t deviceId) { return _library.cuda_deviceReset(&_library, deviceId); }

	int32_t cuda_deviceSetCacheConfig(uint32_t deviceId, J9CudaCacheConfig config)
	{
		return _library.cuda_deviceSetCacheConfig(&_library, deviceId, config);
	}

	int32_t cuda_deviceSetLimit(uint32_t deviceId, J9CudaDeviceLimit limit, uintptr_t value)
	{
		return _library.cuda_deviceSetLimit(&_library, deviceId, limit, value);
	}

	int32_t cuda_deviceSetSharedMemConfig(uint32_t deviceId, J9CudaSharedMemConfig config)
	{
		return _library.cuda_deviceSetSharedMemConfig(&_library, deviceId, config);
	}

	int32_t cuda_deviceSynchronize(uint32_t deviceId) { return _library.cuda_deviceSynchronize(&_library, deviceId); }

	int32_t cuda_driverGetVersion(uint32_t* versionOut) { return _library.cuda_driverGetVersion(&_library, versionOut); }

	int32_t cuda_eventCreate(uint32_t deviceId, uint32_t flags, J9CudaEvent* eventOut)
	{
		return _library.cuda_eventCreate(&_library, deviceId, flags, eventOut);
	}

	int32_t cuda_eventDestroy(uint32_t deviceId, J9CudaEvent event) { return _library.cuda_eventDestroy(&_library, deviceId, event); }

	int32_t cuda_eventElapsedTime(J9CudaEvent start, J9CudaEvent end, float* elapsedMillisOut)
	{
		return _library.cuda_eventElapsedTime(&_library, start, end, elapsedMillisOut);
	}

	int32_t cuda_eventQuery(J9CudaEvent event) { return _library.cuda_eventQuery(&_library, event); }

	int32_t cuda_eventRecord(uint32_t deviceId, J9CudaEvent event, J9CudaStream stream)
	{
		return _library.cuda_eventRecord(&_library, deviceId, event, stream);
	}

	int32_t cuda_eventSynchronize(J9CudaEvent event) { return _library.cuda_eventSynchronize(&_library, event); }

	int32_t cuda_funcGetAttribute(uint32_t deviceId,
	                              J9CudaFunction function,
	                              J9CudaFunctionAttribute attribute,
	                              int32_t* valueOut)
	{
		return _library.cuda_funcGetAttribute(&_library, deviceId, function, attribute, valueOut);
	}

	int32_t cuda_funcMaxActiveBlocksPerMultiprocessor(uint32_t deviceId,
	                                                  J9CudaFunction function,
	                                                  uint32_t blockSize,
	                                                  uint32_t dynamicSharedMemorySize,
	                                                  uint32_t flags,
	                                                  uint32_t* valueOut)
	{
		return _library.cuda_funcMaxActiveBlocksPerMultiprocessor(&_library, deviceId, function, blockSize, dynamicSharedMemorySize, flags, valueOut);
	}

	int32_t cuda_funcMaxPotentialBlockSize(uint32_t deviceId,
	                                       J9CudaFunction function,
	                                       J9CudaBlockToDynamicSharedMemorySize dynamicSharedMemoryFunction,
	                                       uintptr_t userData,
	                                       uint32_t blockSizeLimit,
	                                       uint32_t flags,
	                                       uint32_t* minGridSizeOut,
	                                       uint32_t* maxBlockSizeOut)
	{
		return _library.cuda_funcMaxPotentialBlockSize(&_library, deviceId, function, dynamicSharedMemoryFunction, userData, blockSizeLimit, flags, minGridSizeOut, maxBlockSizeOut);
	}

	int32_t cuda_funcSetCacheConfig(uint32_t deviceId, J9CudaFunction function, J9CudaCacheConfig config)
	{
		return _library.cuda_funcSetCacheConfig(&_library, deviceId, function, config);
	}

	int32_t cuda_funcSetSharedMemConfig(uint32_t deviceId, J9CudaFunction function, J9CudaSharedMemConfig config)
	{
		return _library.cuda_funcSetSharedMemConfig(&_library, deviceId, function, config);
	}

	const char* cuda_getErrorString(int32_t error) { return _library.cuda_getErrorString(&_library, error); }

	int32_t cuda_hostAlloc(uintptr_t size, uint32_t flags, void** hostAddressOut) { return _library.cuda_hostAlloc(&_library, size, flags, hostAddressOut); }

	int32_t cuda_hostFree(void* hostAddress) { return _library.cuda_hostFree(&_library, hostAddress); }

	int32_t cuda_launchKernel(uint32_t deviceId,
	                          J9CudaFunction function,
	                          uint32_t gridDimX,
	                          uint32_t gridDimY,
	                          uint32_t gridDimZ,
	                          uint32_t blockDimX,
	                          uint32_t blockDimY,
	                          uint32_t blockDimZ,
	                          uint32_t sharedMemBytes,
	                          J9CudaStream stream,
	                          void** kernelParms)
	{
		return _library.cuda_launchKernel(&_library, deviceId, function, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, stream, kernelParms);
	}

	int32_t cuda_linkerAddData(uint32_t deviceId,
	                           J9CudaLinker linker,
	                           J9CudaJitInputType type,
	                           void* data,
	                           uintptr_t size,
	                           const char* name,
	                           J9CudaJitOptions* options)
	{
		return _library.cuda_linkerAddData(&_library, deviceId, linker, type, data, size, name, options);
	}

	int32_t cuda_linkerComplete(uint32_t deviceId, J9CudaLinker linker, void** cubinOut, uintptr_t* sizeOut)
	{
		return _library.cuda_linkerComplete(&_library, deviceId, linker, cubinOut, sizeOut);
	}

	int32_t cuda_linkerCreate(uint32_t deviceId, J9CudaJitOptions* options, J9CudaLinker* linkerOut)
	{
		return _library.cuda_linkerCreate(&_library, deviceId, options, linkerOut);
	}

	int32_t cuda_linkerDestroy(uint32_t deviceId, J9CudaLinker linker) { return _library.cuda_linkerDestroy(&_library, deviceId, linker); }

	int32_t cuda_memcpy2DDeviceToDevice(uint32_t deviceId,
	                                    void* targetAddress,
	                                    size_t targetPitch,
	                                    const void* sourceAddress,
	                                    size_t sourcePitch,
	                                    uintptr_t width,
	                                    uintptr_t height)
	{
		return _library.cuda_memcpy2DDeviceToDevice(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
	}

	int32_t cuda_memcpy2DDeviceToDeviceAsync(uint32_t deviceId,
	                                         void* targetAddress,
	                                         size_t targetPitch,
	                                         const void* sourceAddress,
	                                         size_t sourcePitch,
	                                         uintptr_t width,
	                                         uintptr_t height,
	                                         J9CudaStream stream)
	{
		return _library.cuda_memcpy2DDeviceToDeviceAsync(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpy2DDeviceToHost(uint32_t deviceId,
	                                  void* targetAddress,
	                                  size_t targetPitch,
	                                  const void* sourceAddress,
	                                  size_t sourcePitch,
	                                  uintptr_t width,
	                                  uintptr_t height)
	{
		return _library.cuda_memcpy2DDeviceToHost(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
	}

	int32_t cuda_memcpy2DDeviceToHostAsync(uint32_t deviceId,
	                                       void* targetAddress,
	                                       size_t targetPitch,
	                                       const void* sourceAddress,
	                                       size_t sourcePitch,
	                                       uintptr_t width,
	                                       uintptr_t height,
	                                       J9CudaStream stream)
	{
		return _library.cuda_memcpy2DDeviceToHostAsync(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpy2DHostToDevice(uint32_t deviceId,
	                                  void* targetAddress,
	                                  size_t targetPitch,
	                                  const void* sourceAddress,
	                                  size_t sourcePitch,
	                                  uintptr_t width,
	                                  uintptr_t height)
	{
		return _library.cuda_memcpy2DHostToDevice(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
	}

	int32_t cuda_memcpy2DHostToDeviceAsync(uint32_t deviceId,
	                                       void* targetAddress,
	                                       size_t targetPitch,
	                                       const void* sourceAddress,
	                                       size_t sourcePitch,
	                                       uintptr_t width,
	                                       uintptr_t height,
	                                       J9CudaStream stream)
	{
		return _library.cuda_memcpy2DHostToDeviceAsync(&_library, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpyDeviceToDevice(uint32_t deviceId,
	                                  void* targetAddress,
	                                  const void* sourceAddress,
	                                  uintptr_t byteCount)
	{
		return _library.cuda_memcpyDeviceToDevice(&_library, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyDeviceToDeviceAsync(uint32_t deviceId,
	                                       void* targetAddress,
	                                       const void* sourceAddress,
	                                       uintptr_t byteCount,
	                                       J9CudaStream stream)
	{
		return _library.cuda_memcpyDeviceToDeviceAsync(&_library, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memcpyDeviceToHost(uint32_t deviceId, void* targetAddress, const void* sourceAddress, uintptr_t byteCount)
	{
		return _library.cuda_memcpyDeviceToHost(&_library, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyDeviceToHostAsync(uint32_t deviceId,
	                                     void* targetAddress,
	                                     const void* sourceAddress,
	                                     uintptr_t byteCount,
	                                     J9CudaStream stream)
	{
		return _library.cuda_memcpyDeviceToHostAsync(&_library, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memcpyHostToDevice(uint32_t deviceId, void* targetAddress, const void* sourceAddress, uintptr_t byteCount)
	{
		return _library.cuda_memcpyHostToDevice(&_library, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyHostToDeviceAsync(uint32_t deviceId,
	                                     void* targetAddress,
	                                     const void* sourceAddress,
	                                     uintptr_t byteCount,
	                                     J9CudaStream stream)
	{
		return _library.cuda_memcpyHostToDeviceAsync(&_library, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t cuda_memcpyPeer(uint32_t targetDeviceId,
	                        void* targetAddress,
	                        uint32_t sourceDeviceId,
	                        const void* sourceAddress,
	                        uintptr_t byteCount)
	{
		return _library.cuda_memcpyPeer(&_library, targetDeviceId, targetAddress, sourceDeviceId, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyPeerAsync(uint32_t targetDeviceId,
	                             void* targetAddress,
	                             uint32_t sourceDeviceId,
	                             const void* sourceAddress,
	                             uintptr_t byteCount,
	                             J9CudaStream stream)
	{
		return _library.cuda_memcpyPeerAsync(&_library, targetDeviceId, targetAddress, sourceDeviceId, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memset8Async(uint32_t deviceId, void* deviceAddress, uint8_t value, uintptr_t count, J9CudaStream stream)
	{
		return _library.cuda_memset8Async(&_library, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_memset16Async(uint32_t deviceId, void* deviceAddress, uint16_t value, uintptr_t count, J9CudaStream stream)
	{
		return _library.cuda_memset16Async(&_library, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_memset32Async(uint32_t deviceId, void* deviceAddress, uint32_t value, uintptr_t count, J9CudaStream stream)
	{
		return _library.cuda_memset32Async(&_library, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_moduleGetFunction(uint32_t deviceId, J9CudaModule module, const char* name, J9CudaFunction* functionOut)
	{
		return _library.cuda_moduleGetFunction(&_library, deviceId, module, name, functionOut);
	}

	int32_t cuda_moduleGetGlobal(uint32_t deviceId,
	                             J9CudaModule module,
	                             const char* name,
	                             uintptr_t* addressOut,
	                             uintptr_t* sizeOut)
	{
		return _library.cuda_moduleGetGlobal(&_library, deviceId, module, name, addressOut, sizeOut);
	}

	int32_t
	cuda_moduleGetSurfaceRef(uint32_t deviceId, J9CudaModule module, const char* name, uintptr_t* surfRefOut)
	{
		return _library.cuda_moduleGetSurfaceRef(&_library, deviceId, module, name, surfRefOut);
	}

	int32_t cuda_moduleGetTextureRef(uint32_t deviceId, J9CudaModule module, const char* name, uintptr_t* texRefOut)
	{
		return _library.cuda_moduleGetTextureRef(&_library, deviceId, module, name, texRefOut);
	}

	int32_t
	cuda_moduleLoad(uint32_t deviceId, const void* image, J9CudaJitOptions* options, J9CudaModule* moduleOut)
	{
		return _library.cuda_moduleLoad(&_library, deviceId, image, options, moduleOut);
	}

	int32_t cuda_moduleUnload(uint32_t deviceId, J9CudaModule module) { return _library.cuda_moduleUnload(&_library, deviceId, module); }

	int32_t cuda_runtimeGetVersion(uint32_t* versionOut) { return _library.cuda_runtimeGetVersion(&_library, versionOut); }

	int32_t cuda_streamAddCallback(uint32_t deviceId,
	                               J9CudaStream stream,
	                               J9CudaStreamCallback callback,
	                               uintptr_t userData)
	{
		return _library.cuda_streamAddCallback(&_library, deviceId, stream, callback, userData);
	}

	int32_t cuda_streamCreate(uint32_t deviceId, J9CudaStream* streamOut) { return _library.cuda_streamCreate(&_library, deviceId, streamOut); }

	int32_t
	cuda_streamCreateWithPriority(uint32_t deviceId, int32_t priority, uint32_t flags, J9CudaStream* streamOut)
	{
		return _library.cuda_streamCreateWithPriority(&_library, deviceId, priority, flags, streamOut);
	}

	int32_t cuda_streamDestroy(uint32_t deviceId, J9CudaStream stream) { return _library.cuda_streamDestroy(&_library, deviceId, stream); }

	int32_t cuda_streamGetFlags(uint32_t deviceId, J9CudaStream stream, uint32_t* flagsOut)
	{
		return _library.cuda_streamGetFlags(&_library, deviceId, stream, flagsOut);
	}

	int32_t cuda_streamGetPriority(uint32_t deviceId, J9CudaStream stream, int32_t* priorityOut)
	{
		return _library.cuda_streamGetPriority(&_library, deviceId, stream, priorityOut);
	}

	int32_t cuda_streamQuery(uint32_t deviceId, J9CudaStream stream) { return _library.cuda_streamQuery(&_library, deviceId, stream); }

	int32_t cuda_streamSynchronize(uint32_t deviceId, J9CudaStream stream) { return _library.cuda_streamSynchronize(&_library, deviceId, stream); }

	int32_t cuda_streamWaitEvent(uint32_t deviceId, J9CudaStream stream, J9CudaEvent event)
	{
		return _library.cuda_streamWaitEvent(&_library, deviceId, stream, event);
	}

#endif /* OMR_OPT_CUDA */

private:
	ThreadingInterface _threading;
	mutable OMRPortLibrary _library;
	bool _initialized;
};

} /* namespace OMR */

#endif /* OMR_PLATFORMINTERFACE_HPP_ */
