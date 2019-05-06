/*******************************************************************************
 *  Copyright (c) 2019, 2019 IBM and others
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

#if !defined(OMR_PORTLIBRARYHANDLE_HPP_)
#define OMR_PORTLIBRARYHANDLE_HPP_

#include "omrcfg.h"

#include <assert.h>
#include <stdlib.h>

#include "omr.h"
#include "omrport.h"

namespace OMR
{

/**
 * C++ wrapper around the port-library function table.
 */
class PortLibraryHandle
{
public:
	explicit PortLibraryHandle(OMRPortLibrary* lib) : _lib(lib) {}

	OMRPortLibrary* toStruct() const { return _lib; }

	int32_t port_isFunctionOverridden(uintptr_t offset) { return _lib->port_isFunctionOverridden(_lib, offset); }

	void port_tls_free() { _lib->port_tls_free(_lib); }

	int32_t error_last_error_number() { return _lib->error_last_error_number(_lib); }

	const char* error_last_error_message() { return _lib->error_last_error_message(_lib); }

	/**
	 * @group Time
	 * @{
	 */

	uintptr_t time_msec_clock() { return _lib->time_msec_clock(_lib); }

	uintptr_t time_usec_clock() { return _lib->time_usec_clock(_lib); }

	uint64_t time_current_time_nanos(uintptr_t* success) { return _lib->time_current_time_nanos(_lib, success); }

	int64_t time_current_time_millis() { return _lib->time_current_time_millis(_lib); }

	int64_t time_nano_time() { return _lib->time_nano_time(_lib); }

	uint64_t time_hires_clock() { return _lib->time_hires_clock(_lib); }

	uint64_t time_hires_frequency() { return _lib->time_hires_frequency(_lib); }

	uint64_t time_hires_delta(uint64_t startTime, uint64_t endTime, uint64_t requiredResolution)
	{
		return _lib->time_hires_delta(_lib, startTime, endTime, requiredResolution);
	}

	/**
	 * @}
	 */

	/**
	 * @group Sysinfo
	 */

	intptr_t sysinfo_process_exists(uintptr_t pid) { return _lib->sysinfo_process_exists(_lib, pid); }

	uintptr_t sysinfo_get_egid() { return _lib->sysinfo_get_egid(_lib); }

	uintptr_t sysinfo_get_euid() { return _lib->sysinfo_get_euid(_lib); }

	intptr_t sysinfo_get_groups(uint32_t** gidList, uint32_t categoryCode)
	{
		return _lib->sysinfo_get_groups(_lib, gidList, categoryCode);
	}

	uintptr_t sysinfo_get_pid() { return _lib->sysinfo_get_pid(_lib); }

	uintptr_t sysinfo_get_ppid() { return _lib->sysinfo_get_ppid(_lib); }

	/**
	 * Note: This interface is missing a variant version of this function called sysinfo_get_memory_info_with_flag
	 */
	int32_t sysinfo_get_memory_info(J9MemoryInfo* memInfo) { return _lib->sysinfo_get_memory_info(_lib, memInfo); }

	int32_t sysinfo_get_processor_info(J9ProcessorInfos* procInfo)
	{
		return _lib->sysinfo_get_processor_info(_lib, procInfo);
	}

	void sysinfo_destroy_processor_info(J9ProcessorInfos* procInfos)
	{
		_lib->sysinfo_destroy_processor_info(_lib, procInfos);
	}

	uint64_t sysinfo_get_addressable_physical_memory()
	{
		return _lib->sysinfo_get_addressable_physical_memory(_lib);
	}

	uint64_t sysinfo_get_physical_memory() { return _lib->sysinfo_get_physical_memory(_lib); }

	const char* sysinfo_get_OS_version() { return _lib->sysinfo_get_OS_version(_lib); }

	intptr_t sysinfo_get_env(const char* envVar, char* infoString, uintptr_t bufSize)
	{
		return _lib->sysinfo_get_env(_lib, envVar, infoString, bufSize);
	}

	const char* sysinfo_get_CPU_architecture() { return _lib->sysinfo_get_CPU_architecture(_lib); }

	const char* sysinfo_get_OS_type() { return _lib->sysinfo_get_OS_type(_lib); }

	intptr_t sysinfo_get_executable_name(const char* argv0, char** result)
	{
		return _lib->sysinfo_get_executable_name(_lib, argv0, result);
	}

	intptr_t sysinfo_get_username(char* buffer, uintptr_t length)
	{
		return _lib->sysinfo_get_username(_lib, buffer, length);
	}

	intptr_t sysinfo_get_groupname(char* buffer, uintptr_t length)
	{
		return _lib->sysinfo_get_groupname(_lib, buffer, length);
	}

	intptr_t sysinfo_get_hostname(char* buffer, size_t length)
	{
		return _lib->sysinfo_get_hostname(_lib, buffer, length);
	}

	intptr_t sysinfo_get_load_average(J9PortSysInfoLoadData* loadAverageData)
	{
		return _lib->sysinfo_get_load_average(_lib, loadAverageData);
	}

	intptr_t sysinfo_get_CPU_utilization(J9SysinfoCPUTime* cpuTime)
	{
		return _lib->sysinfo_get_CPU_utilization(_lib, cpuTime);
	}

	int32_t sysinfo_limit_iterator_init(J9SysinfoLimitIteratorState* state)
	{
		return _lib->sysinfo_limit_iterator_init(_lib, state);
	}

	bool sysinfo_limit_iterator_hasNext(J9SysinfoLimitIteratorState* state)
	{
		return _lib->sysinfo_limit_iterator_hasNext(_lib, state);
	}

	int32_t sysinfo_limit_iterator_next(J9SysinfoLimitIteratorState* state, J9SysinfoUserLimitElement* limitElement)
	{
		return _lib->sysinfo_limit_iterator_next(_lib, state, limitElement);
	}

	int32_t sysinfo_env_iterator_init(J9SysinfoEnvIteratorState* state, void* buffer, uintptr_t bufferSizeBytes)
	{
		return _lib->sysinfo_env_iterator_init(_lib, state, buffer, bufferSizeBytes);
	}

	bool sysinfo_env_iterator_hasNext(J9SysinfoEnvIteratorState* state)
	{
		return _lib->sysinfo_env_iterator_hasNext(_lib, state);
	}

	int32_t sysinfo_env_iterator_next(J9SysinfoEnvIteratorState* state, J9SysinfoEnvElement* envElement)
	{
		return _lib->sysinfo_env_iterator_next(_lib, state, envElement);
	}

	uint32_t sysinfo_get_limit(uint32_t resourceID, uint64_t* limit)
	{
		return _lib->sysinfo_get_limit(_lib, resourceID, limit);
	}

	uint32_t sysinfo_set_limit(uint32_t resourceID, uint64_t limit)
	{
		return _lib->sysinfo_set_limit(_lib, resourceID, limit);
	}

	uintptr_t sysinfo_get_number_CPUs_by_type(uintptr_t type)
	{
		return _lib->sysinfo_get_number_CPUs_by_type(_lib, type);
	}

	intptr_t sysinfo_get_cwd(char* buf, uintptr_t bufLen) { return _lib->sysinfo_get_cwd(_lib, buf, bufLen); }

	intptr_t sysinfo_get_tmp(char* buf, uintptr_t bufLen, bool ignoreEnvVariable)
	{
		return _lib->sysinfo_get_tmp(_lib, buf, bufLen, ignoreEnvVariable);
	}

	void sysinfo_set_number_user_specified_CPUs(uintptr_t number)
	{
		_lib->sysinfo_set_number_user_specified_CPUs(_lib, number);
	}

	int32_t sysinfo_get_open_file_count(uint64_t* count) { return _lib->sysinfo_get_open_file_count(_lib, count); }

	intptr_t sysinfo_get_os_description(OMROSDesc* desc) { return _lib->sysinfo_get_os_description(_lib, desc); }

	bool sysinfo_os_has_feature(OMROSDesc* desc, uint32_t feature)
	{
		return _lib->sysinfo_os_has_feature(_lib, desc, feature);
	}

	bool sysinfo_os_kernel_info(OMROSKernelInfo* kernelInfo)
	{
		return _lib->sysinfo_os_kernel_info(_lib, kernelInfo);
	}

	bool sysinfo_cgroup_is_system_available() { return _lib->sysinfo_cgroup_is_system_available(_lib); }

	uint64_t sysinfo_cgroup_get_available_subsystems()
	{
		return _lib->sysinfo_cgroup_get_available_subsystems(_lib);
	}

	uint64_t sysinfo_cgroup_are_subsystems_available(uint64_t subsystemFlags)
	{
		return _lib->sysinfo_cgroup_are_subsystems_available(_lib, subsystemFlags);
	}

	uint64_t sysinfo_cgroup_get_enabled_subsystems() { return _lib->sysinfo_cgroup_get_enabled_subsystems(_lib); }

	uint64_t sysinfo_cgroup_enable_subsystems(uint64_t requestedSubsystems)
	{
		return _lib->sysinfo_cgroup_enable_subsystems(_lib, requestedSubsystems);
	}

	uint64_t sysinfo_cgroup_are_subsystems_enabled(uint64_t subsystemFlags)
	{
		return _lib->sysinfo_cgroup_are_subsystems_enabled(_lib, subsystemFlags);
	}

	int32_t sysinfo_cgroup_get_memlimit(uint64_t* limit) { return _lib->sysinfo_cgroup_get_memlimit(_lib, limit); }

	bool sysinfo_cgroup_is_memlimit_set() { return _lib->sysinfo_cgroup_is_memlimit_set(_lib); }

	OMRCgroupEntry* sysinfo_get_cgroup_subsystem_list() { return _lib->sysinfo_get_cgroup_subsystem_list(_lib); }

	bool sysinfo_is_running_in_container() { return _lib->sysinfo_is_running_in_container(_lib); }

	int32_t sysinfo_cgroup_subsystem_iterator_init(uint64_t subsystem, OMRCgroupMetricIteratorState* state)
	{
		return _lib->sysinfo_cgroup_subsystem_iterator_init(_lib, subsystem, state);
	}

	bool sysinfo_cgroup_subsystem_iterator_hasNext(const OMRCgroupMetricIteratorState* state)
	{
		return _lib->sysinfo_cgroup_subsystem_iterator_hasNext(_lib, state);
	}

	int32_t
	sysinfo_cgroup_subsystem_iterator_metricKey(const OMRCgroupMetricIteratorState* state, const char** metricKey)
	{
		return _lib->sysinfo_cgroup_subsystem_iterator_metricKey(_lib, state, metricKey);
	}

	int32_t sysinfo_cgroup_subsystem_iterator_next(OMRCgroupMetricIteratorState* state,
	                                               OMRCgroupMetricElement* metricElement)
	{
		return _lib->sysinfo_cgroup_subsystem_iterator_next(_lib, state, metricElement);
	}

	void sysinfo_cgroup_subsystem_iterator_destroy(OMRCgroupMetricIteratorState* state)
	{
		_lib->sysinfo_cgroup_subsystem_iterator_destroy(_lib, state);
	}

	/**
	 * @}
	 */

	/**
	 * @group File
	 */

	intptr_t file_write(intptr_t fd, const void* buf, intptr_t nbytes)
	{
		return _lib->file_write(_lib, fd, buf, nbytes);
	}

	intptr_t file_write_text(intptr_t fd, const char* buf, intptr_t nbytes)
	{
		return _lib->file_write_text(_lib, fd, buf, nbytes);
	}

	int32_t file_get_text_encoding(char* charsetName, uintptr_t nbytes)
	{
		return _lib->file_get_text_encoding(_lib, charsetName, nbytes);
	}

	void file_vprintf(intptr_t fd, const char* format, va_list args) { _lib->file_vprintf(_lib, fd, format, args); }

	/* TODO: void file_printf(intptr_t fd, const char* format, ...) { _lib->file_printf(_lib); } */

	intptr_t file_open(const char* path, int32_t flags, int32_t mode)
	{
		return _lib->file_open(_lib, path, flags, mode);
	}

	int32_t file_close(intptr_t fd) { return _lib->file_close(_lib, fd); }

	int64_t file_seek(intptr_t fd, int64_t offset, int32_t whence)
	{
		return _lib->file_seek(_lib, fd, offset, whence);
	}

	intptr_t file_read(intptr_t fd, void* buf, intptr_t nbytes) { return _lib->file_read(_lib, fd, buf, nbytes); }

	int32_t file_unlink(const char* path) { return _lib->file_unlink(_lib, path); }

	int32_t file_attr(const char* path) { return _lib->file_attr(_lib, path); }

	int32_t file_chmod(const char* path, int32_t mode) { return _lib->file_chmod(_lib, path, mode); }

	intptr_t file_chown(const char* path, uintptr_t owner, uintptr_t group)
	{
		return _lib->file_chown(_lib, path, owner, group);
	}

	int64_t file_lastmod(const char* path) { return _lib->file_lastmod(_lib, path); }

	int64_t file_length(const char* path) { return _lib->file_length(_lib, path); }

	int64_t file_flength(intptr_t fd) { return _lib->file_flength(_lib, fd); }

	int32_t file_set_length(intptr_t fd, int64_t newLength) { return _lib->file_set_length(_lib, fd, newLength); }

	int32_t file_sync(intptr_t fd) { return _lib->file_sync(_lib, fd); }

	int32_t file_fstat(intptr_t fd, J9FileStat* buf) { return _lib->file_fstat(_lib, fd, buf); }

	int32_t file_stat(const char* path, uint32_t flags, J9FileStat* buf)
	{
		return _lib->file_stat(_lib, path, flags, buf);
	}

	int32_t file_stat_filesystem(const char* path, uint32_t flags, J9FileStatFilesystem* buf)
	{
		return _lib->file_stat_filesystem(_lib, path, flags, buf);
	}

	intptr_t file_blockingasync_open(const char* path, int32_t flags, int32_t mode)
	{
		return _lib->file_blockingasync_open(_lib, path, flags, mode);
	}

	int32_t file_blockingasync_close(intptr_t fd) { return _lib->file_blockingasync_close(_lib, fd); }

	intptr_t file_blockingasync_read(intptr_t fd, void* buf, intptr_t nbytes)
	{
		return _lib->file_blockingasync_read(_lib, fd, buf, nbytes);
	}

	intptr_t file_blockingasync_write(intptr_t fd, const void* buf, intptr_t nbytes)
	{
		return _lib->file_blockingasync_write(_lib, fd, buf, nbytes);
	}

	int32_t file_blockingasync_set_length(intptr_t fd, int64_t newLength)
	{
		return _lib->file_blockingasync_set_length(_lib, fd, newLength);
	}

	int64_t file_blockingasync_flength(intptr_t fd) { return _lib->file_blockingasync_flength(_lib, fd); }

	char* file_read_text(intptr_t fd, char* buf, intptr_t nbytes)
	{
		return _lib->file_read_text(_lib, fd, buf, nbytes);
	}

	int32_t file_mkdir(const char* path) { return _lib->file_mkdir(_lib, path); }

	int32_t file_move(const char* pathExist, const char* pathNew)
	{
		return _lib->file_move(_lib, pathExist, pathNew);
	}

	int32_t file_unlinkdir(const char* path) { return _lib->file_unlinkdir(_lib, path); }

	uintptr_t file_findfirst(const char* path, char* resultbuf)
	{
		return _lib->file_findfirst(_lib, path, resultbuf);
	}

	int32_t file_findnext(uintptr_t findhandle, char* resultbuf)
	{
		return _lib->file_findnext(_lib, findhandle, resultbuf);
	}

	void file_findclose(uintptr_t findhandle) { _lib->file_findclose(_lib, findhandle); }

	const char* file_error_message() { return _lib->file_error_message(_lib); }

	int32_t file_unlock_bytes(intptr_t fd, uint64_t offset, uint64_t length)
	{
		return _lib->file_unlock_bytes(_lib, fd, offset, length);
	}

	int32_t file_lock_bytes(intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length)
	{
		return _lib->file_lock_bytes(_lib, fd, lockFlags, offset, length);
	}

	intptr_t file_convert_native_fd_to_omrfile_fd(intptr_t nativeFD)
	{
		return _lib->file_convert_native_fd_to_omrfile_fd(_lib, nativeFD);
	}

	intptr_t file_convert_omrfile_fd_to_native_fd(intptr_t omrfileFD)
	{
		return _lib->file_convert_omrfile_fd_to_native_fd(_lib, omrfileFD);
	}

	int32_t file_blockingasync_unlock_bytes(intptr_t fd, uint64_t offset, uint64_t length)
	{
		return _lib->file_blockingasync_unlock_bytes(_lib, fd, offset, length);
	}

	int32_t file_blockingasync_lock_bytes(intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length)
	{
		return _lib->file_blockingasync_lock_bytes(_lib, fd, lockFlags, offset, length);
	}

	/**
	 * @}
	 */

	/**
	 * @group Filestream
	 */

	OMRFileStream* filestream_open(const char* path, int32_t flags, int32_t mode)
	{
		return _lib->filestream_open(_lib, path, flags, mode);
	}

	int32_t filestream_close(OMRFileStream* fileStream) { return _lib->filestream_close(_lib, fileStream); }

	intptr_t filestream_write(OMRFileStream* fileStream, const void* buf, intptr_t nbytes)
	{
		return _lib->filestream_write(_lib, fileStream, buf, nbytes);
	}

	intptr_t filestream_write_text(OMRFileStream* fileStream, const char* buf, intptr_t nbytes, int32_t toCode)
	{
		return _lib->filestream_write_text(_lib, fileStream, buf, nbytes, toCode);
	}

	void filestream_vprintf(OMRFileStream* fileStream, const char* format, va_list args)
	{
		_lib->filestream_vprintf(_lib, fileStream, format, args);
	}

	void filestream_printf(OMRFileStream* filestream, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		_lib->filestream_vprintf(_lib, filestream, format, args);
		va_end(args);
	}

	int32_t filestream_sync(OMRFileStream* fileStream) { return _lib->filestream_sync(_lib, fileStream); }

	int32_t filestream_setbuffer(OMRFileStream* fileStream, char* buffer, int32_t mode, uintptr_t size)
	{
		return _lib->filestream_setbuffer(_lib, fileStream, buffer, mode, size);
	}

	OMRFileStream* filestream_fdopen(intptr_t fd, int32_t flags)
	{
		return _lib->filestream_fdopen(_lib, fd, flags);
	}

	intptr_t filestream_fileno(OMRFileStream* stream) { return _lib->filestream_fileno(_lib, stream); }

	/**
	 * @}
	 */

	/**
	 * @group SL
	 */

	uintptr_t sl_close_shared_library(uintptr_t descriptor)
	{
		return _lib->sl_close_shared_library(_lib, descriptor);
	}

	uintptr_t sl_open_shared_library(char* name, uintptr_t* descriptor, uintptr_t flags)
	{
		return _lib->sl_open_shared_library(_lib, name, descriptor, flags);
	}

	uintptr_t sl_lookup_name(uintptr_t descriptor, char* name, uintptr_t* func, const char* argSignature)
	{
		return _lib->sl_lookup_name(_lib, descriptor, name, func, argSignature);
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
		_lib->tty_vprintf(_lib, format, args);
		va_end(args);
	}

	void tty_vprintf(const char* format, va_list args) { _lib->tty_vprintf(_lib, format, args); }

	intptr_t tty_get_chars(char* s, uintptr_t length) { return _lib->tty_get_chars(_lib, s, length); }

	void tty_err_printf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		_lib->tty_err_vprintf(_lib, format, args);
		va_end(args);
	}

	void tty_err_vprintf(const char* format, va_list args) { _lib->tty_err_vprintf(_lib, format, args); }

	intptr_t tty_available() { return _lib->tty_available(_lib); }

	void tty_daemonize() { _lib->tty_daemonize(_lib); }

	/**
	 * @}
	 */

	/**
	 * @group Heap
	 */

	J9Heap* heap_create(void* heapBase, uintptr_t heapSize, uint32_t heapFlags)
	{
		return _lib->heap_create(_lib, heapBase, heapSize, heapFlags);
	}

	void* heap_allocate(J9Heap* heap, uintptr_t byteAmount) { return _lib->heap_allocate(_lib, heap, byteAmount); }

	void heap_free(J9Heap* heap, void* address) { _lib->heap_free(_lib, heap, address); }

	void* heap_reallocate(J9Heap* heap, void* address, uintptr_t byteAmount)
	{
		return _lib->heap_reallocate(_lib, heap, address, byteAmount);
	}

	uintptr_t heap_query_size(J9Heap* heap, void* address) { return _lib->heap_query_size(_lib, heap, address); }

	bool heap_grow(J9Heap* heap, uintptr_t growAmount) { return _lib->heap_grow(_lib, heap, growAmount); }

	/**
	 * @}
	 */

	/**
	 * @group Mem (Memory)
	 */

	void* mem_allocate_memory(uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _lib->mem_allocate_memory(_lib, byteAmount, callSite, category);
	}

	void mem_free_memory(void* memoryPointer) { _lib->mem_free_memory(_lib, memoryPointer); }

	void mem_advise_and_free_memory(void* memoryPointer) { _lib->mem_advise_and_free_memory(_lib, memoryPointer); }

	void* mem_reallocate_memory(void* memoryPointer, uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _lib->mem_reallocate_memory(_lib, memoryPointer, byteAmount, callSite, category);
	}

	void* mem_allocate_memory32(uintptr_t byteAmount, const char* callSite, uint32_t category)
	{
		return _lib->mem_allocate_memory32(_lib, byteAmount, callSite, category);
	}

	void mem_free_memory32(void* memoryPointer) { _lib->mem_free_memory32(_lib, memoryPointer); }

	uintptr_t mem_ensure_capacity32(uintptr_t byteAmount) { return _lib->mem_ensure_capacity32(_lib, byteAmount); }

	void mem_walk_categories(OMRMemCategoryWalkState* state) { _lib->mem_walk_categories(_lib, state); }

	OMRMemCategory* mem_get_category(uint32_t categoryCode) { return _lib->mem_get_category(_lib, categoryCode); }

	void mem_categories_increment_counters(OMRMemCategory* category, uintptr_t size)
	{
		_lib->mem_categories_increment_counters(category, size);
	}

	void mem_categories_decrement_counters(OMRMemCategory* category, uintptr_t size)
	{
		_lib->mem_categories_decrement_counters(category, size);
	}

	/**
	 * @}
	 */

	/**
	 * @group CPU
	 */
	void cpu_flush_icache(void* memoryPointer, uintptr_t byteAmount)
	{
		_lib->cpu_flush_icache(_lib, memoryPointer, byteAmount);
	}

	int32_t cpu_get_cache_line_size(int32_t* lineSize) { return _lib->cpu_get_cache_line_size(_lib, lineSize); }

	/**
	 * @}
	 */

	/**
	 * @group VMem (Virtual Memory)
	 */

	void* vmem_commit_memory(void* address, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_commit_memory(_lib, address, byteAmount, identifier);
	}

	intptr_t vmem_decommit_memory(void* address, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_decommit_memory(_lib, address, byteAmount, identifier);
	}

	int32_t vmem_free_memory(void* userAddress, uintptr_t byteAmount, J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_free_memory(_lib, userAddress, byteAmount, identifier);
	}

	int32_t vmem_vmem_params_init(J9PortVmemParams* params) { return _lib->vmem_vmem_params_init(_lib, params); }

	void* vmem_reserve_memory(void* address,
	                          uintptr_t byteAmount,
	                          J9PortVmemIdentifier* identifier,
	                          uintptr_t mode,
	                          uintptr_t pageSize,
	                          uint32_t category)
	{
		return _lib->vmem_reserve_memory(_lib, address, byteAmount, identifier, mode, pageSize, category);
	}

	void* vmem_reserve_memory_ex(J9PortVmemIdentifier* identifier, J9PortVmemParams* params)
	{
		return _lib->vmem_reserve_memory_ex(_lib, identifier, params);
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
		return _lib->vmem_get_contiguous_region_memory(_lib, addresses, addressesCount, addressSize, byteAmount,
		                                               oldIdentifier, newIdentifier, mode, pageSize, category);
	}

	uintptr_t vmem_get_page_size(J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_get_page_size(_lib, identifier);
	}

	uintptr_t vmem_get_page_flags(J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_get_page_flags(_lib, identifier);
	}

	uintptr_t* vmem_supported_page_sizes() { return _lib->vmem_supported_page_sizes(_lib); }

	uintptr_t* vmem_supported_page_flags() { return _lib->vmem_supported_page_flags(_lib); }

	void vmem_default_large_page_size_ex(uintptr_t mode, uintptr_t* pageSize, uintptr_t* pageFlags)
	{
		_lib->vmem_default_large_page_size_ex(_lib, mode, pageSize, pageFlags);
	}

	intptr_t
	vmem_find_valid_page_size(uintptr_t mode, uintptr_t* pageSize, uintptr_t* pageFlags, BOOLEAN* isSizeSupported)
	{
		return _lib->vmem_find_valid_page_size(_lib, mode, pageSize, pageFlags, isSizeSupported);
	}

	intptr_t vmem_numa_set_affinity(uintptr_t numaNode,
	                                void* address,
	                                uintptr_t byteAmount,
	                                J9PortVmemIdentifier* identifier)
	{
		return _lib->vmem_numa_set_affinity(_lib, numaNode, address, byteAmount, identifier);
	}

	intptr_t vmem_numa_get_node_details(J9MemoryNodeDetail* numaNodes, uintptr_t* nodeCount)
	{
		return _lib->vmem_numa_get_node_details(_lib, numaNodes, nodeCount);
	}

	int32_t vmem_get_available_physical_memory(uint64_t* freePhysicalMemorySize)
	{
		return _lib->vmem_get_available_physical_memory(_lib, freePhysicalMemorySize);
	}

	int32_t vmem_get_process_memory_size(J9VMemMemoryQuery queryType, uint64_t* memorySize)
	{
		return _lib->vmem_get_process_memory_size(_lib, queryType, memorySize);
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
		uintptr_t rc = _lib->str_vprintf(_lib, buf, len, format, args);
		va_end(args);
		return rc;
	}

	uintptr_t str_vprintf(char* buf, uintptr_t bufLen, const char* format, va_list args)
	{
		return _lib->str_vprintf(_lib, buf, bufLen, format, args);
	}

	J9StringTokens* str_create_tokens(int64_t timeMillis) { return _lib->str_create_tokens(_lib, timeMillis); }

	/* TODO: intptr_t str_set_token(J9StringTokens* tokens, const char* key, const char* format, ...) { return _lib->str_set_token(_lib, tokens, key, format); } */

	uintptr_t str_subst_tokens(char* buf, uintptr_t bufLen, const char* format, J9StringTokens* tokens)
	{
		return _lib->str_subst_tokens(_lib, buf, bufLen, format, tokens);
	}

	void str_free_tokens(J9StringTokens* tokens) { _lib->str_free_tokens(_lib, tokens); }

	intptr_t str_set_time_tokens(J9StringTokens* tokens, int64_t timeMillis)
	{
		return _lib->str_set_time_tokens(_lib, tokens, timeMillis);
	}

	int32_t str_convert(int32_t fromCode,
	                    int32_t toCode,
	                    const char* inBuffer,
	                    uintptr_t inBufferSize,
	                    char* outBuffer,
	                    uintptr_t outBufferSize)
	{
		return _lib->str_convert(_lib, fromCode, toCode, inBuffer, inBufferSize, outBuffer, outBufferSize);
	}

	uintptr_t str_ftime(char* buf, uintptr_t bufLen, const char* format, int64_t timeMillis)
	{
		return _lib->str_ftime(_lib, buf, bufLen, format, timeMillis);
	}

	/**
	 * @}
	 */

	/**
	 * @name Exit and Image Dumps
	 * @{
	 */

	int32_t exit_get_exit_code() { return _lib->exit_get_exit_code(_lib); }

	void exit_shutdown_and_exit(int32_t exitCode) { _lib->exit_shutdown_and_exit(_lib, exitCode); }

	uintptr_t dump_create(char* filename, char* dumpType, void* userData)
	{
		return _lib->dump_create(_lib, filename, dumpType, userData);
	}

	/**
	 * @}
	 */

	/**
	 * @name NLS (Native Language Support)
	 * @{
	 */

	void nls_free_cached_data() { _lib->nls_free_cached_data(_lib); }

	void nls_shutdown() { _lib->nls_shutdown(_lib); }

	void nls_set_catalog(const char** paths, const int nPaths, const char* baseName, const char* extension)
	{
		_lib->nls_set_catalog(_lib, paths, nPaths, baseName, extension);
	}

	void nls_set_locale(const char* lang, const char* region, const char* variant)
	{
		_lib->nls_set_locale(_lib, lang, region, variant);
	}

	const char* nls_get_language() { return _lib->nls_get_language(_lib); }

	const char* nls_get_region() { return _lib->nls_get_region(_lib); }

	const char* nls_get_variant() { return _lib->nls_get_variant(_lib); }

	void nls_printf(uintptr_t flags, uint32_t module_name, uint32_t message_num, ...)
	{
		va_list args;
		va_start(args, message_num);
		_lib->nls_vprintf(_lib, flags, module_name, message_num, args);
		va_end(args);
	}

	void nls_vprintf(uintptr_t flags, uint32_t module_name, uint32_t message_num, va_list args)
	{
		_lib->nls_vprintf(_lib, flags, module_name, message_num, args);
	}

	const char*
	nls_lookup_message(uintptr_t flags, uint32_t module_name, uint32_t message_num, const char* default_string)
	{
		return _lib->nls_lookup_message(_lib, flags, module_name, message_num, default_string);
	}

	/**
	 * @}
	 */

	int32_t port_control(const char* key, uintptr_t value) { return _lib->port_control(_lib, key, value); }

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
		return _lib->sig_protect(_lib, fn, fn_arg, handler, handler_arg, flags, result);
	}

	int32_t sig_can_protect(uint32_t flags) { return _lib->sig_can_protect(_lib, flags); }

	int32_t sig_set_async_signal_handler(omrsig_handler_fn handler, void* handler_arg, uint32_t flags)
	{
		return _lib->sig_set_async_signal_handler(_lib, handler, handler_arg, flags);
	}

	int32_t sig_set_single_async_signal_handler(omrsig_handler_fn handler,
	                                            void* handler_arg,
	                                            uint32_t portlibSignalFlag,
	                                            void** oldOSHandler)
	{
		return _lib->sig_set_single_async_signal_handler(_lib, handler, handler_arg, portlibSignalFlag,
		                                                 oldOSHandler);
	}

	uint32_t sig_map_os_signal_to_portlib_signal(uint32_t osSignalValue)
	{
		return _lib->sig_map_os_signal_to_portlib_signal(_lib, osSignalValue);
	}

	int32_t sig_map_portlib_signal_to_os_signal(uint32_t portlibSignalFlag)
	{
		return _lib->sig_map_portlib_signal_to_os_signal(_lib, portlibSignalFlag);
	}

	int32_t sig_register_os_handler(uint32_t portlibSignalFlag, void* newOSHandler, void** oldOSHandler)
	{
		return _lib->sig_register_os_handler(_lib, portlibSignalFlag, newOSHandler, oldOSHandler);
	}

	bool sig_is_master_signal_handler(void* osHandler)
	{
		return _lib->sig_is_master_signal_handler(_lib, osHandler);
	}

	int32_t sig_is_signal_ignored(uint32_t portlibSignalFlag, BOOLEAN* isSignalIgnored)
	{
		return _lib->sig_is_signal_ignored(_lib, portlibSignalFlag, isSignalIgnored);
	}

	uint32_t sig_info(void* info, uint32_t category, int32_t index, const char** name, void** value)
	{
		return _lib->sig_info(_lib, info, category, index, name, value);
	}

	uint32_t sig_info_count(void* info, uint32_t category) { return _lib->sig_info_count(_lib, info, category); }

	int32_t sig_set_options(uint32_t options) { return _lib->sig_set_options(_lib, options); }

	uint32_t sig_get_options() { return _lib->sig_get_options(_lib); }

	intptr_t sig_get_current_signal() { return _lib->sig_get_current_signal(_lib); }

	int32_t sig_set_reporter_priority(uintptr_t priority)
	{
		return _lib->sig_set_reporter_priority(_lib, priority);
	}

	/**
	 * @}
	 */

	/**
	 * @name MMap (Memory Mappings, Virtual Memory)
	 * @{
	 */

	int32_t mmap_capabilities() { return _lib->mmap_capabilities(_lib); }

	J9MmapHandle* mmap_map_file(intptr_t file,
	                            uint64_t offset,
	                            uintptr_t size,
	                            const char* mappingName,
	                            uint32_t flags,
	                            uint32_t category)
	{
		return _lib->mmap_map_file(_lib, file, offset, size, mappingName, flags, category);
	}

	void mmap_unmap_file(J9MmapHandle* handle) { _lib->mmap_unmap_file(_lib, handle); }

	intptr_t mmap_msync(void* start, uintptr_t length, uint32_t flags)
	{
		return _lib->mmap_msync(_lib, start, length, flags);
	}

	intptr_t mmap_protect(void* address, uintptr_t length, uintptr_t flags)
	{
		return _lib->mmap_protect(_lib, address, length, flags);
	}

	uintptr_t mmap_get_region_granularity(void* address)
	{
		return _lib->mmap_get_region_granularity(_lib, address);
	}

	void mmap_dont_need(const void* startAddress, size_t length)
	{
		_lib->mmap_dont_need(_lib, startAddress, length);
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
		return _lib->syslog_write(_lib, flags, message);
	}

	uintptr_t syslog_query() { return _lib->syslog_query(_lib); }

	void syslog_set(uintptr_t options) { return _lib->syslog_set(_lib, options); }

	/**
	 * @}
	 */

	/**
	 * @name Introspect
	 * @{
	 */

	int32_t introspect_set_suspend_signal_offset(int32_t signalOffset)
	{
		return _lib->introspect_set_suspend_signal_offset(_lib, signalOffset);
	}

	J9PlatformThread* introspect_threads_startDo(J9Heap* heap, J9ThreadWalkState* state)
	{
		return _lib->introspect_threads_startDo(_lib, heap, state);
	}

	J9PlatformThread*
	introspect_threads_startDo_with_signal(J9Heap* heap, J9ThreadWalkState* state, void* signal_info)
	{
		return _lib->introspect_threads_startDo_with_signal(_lib, heap, state, signal_info);
	}

	J9PlatformThread* introspect_threads_nextDo(J9ThreadWalkState* state)
	{
		return _lib->introspect_threads_nextDo(state);
	}

	uintptr_t introspect_backtrace_thread(J9PlatformThread* thread, J9Heap* heap, void* signalInfo)
	{
		return _lib->introspect_backtrace_thread(_lib, thread, heap, signalInfo);
	}

	uintptr_t introspect_backtrace_symbols(J9PlatformThread* thread, J9Heap* heap)
	{
		return _lib->introspect_backtrace_symbols(_lib, thread, heap);
	}

	/**
	 * @}
	 */

#if defined(OMR_OPT_CUDA)

	/* TODO RWY */

	int32_t cuda_deviceAlloc(uint32_t deviceId, uintptr_t size, void** deviceAddressOut)
	{
		return _lib->cuda_deviceAlloc(_lib, deviceId, size, deviceAddressOut);
	}

	int32_t cuda_deviceCanAccessPeer(uint32_t deviceId, uint32_t peerDeviceId, BOOLEAN* canAccessPeerOut)
	{
		return _lib->cuda_deviceCanAccessPeer(_lib, deviceId, peerDeviceId, canAccessPeerOut);
	}

	int32_t cuda_deviceDisablePeerAccess(uint32_t deviceId, uint32_t peerDeviceId)
	{
		return _lib->cuda_deviceDisablePeerAccess(_lib, deviceId, peerDeviceId);
	}

	int32_t cuda_deviceEnablePeerAccess(uint32_t deviceId, uint32_t peerDeviceId)
	{
		return _lib->cuda_deviceEnablePeerAccess(_lib, deviceId, peerDeviceId);
	}

	int32_t cuda_deviceFree(OMRPortLibrary* OMRPortLibrary, uint32_t deviceId, void* devicePointer)
	{
		return _lib->cuda_deviceFree(_lib, deviceId, devicePointer);
	}

	int32_t cuda_deviceGetAttribute(uint32_t deviceId, J9CudaDeviceAttribute attribute, int32_t* valueOut)
	{
		return _lib->cuda_deviceGetAttribute(_lib, deviceId, attribute, valueOut);
	}

	int32_t cuda_deviceGetCacheConfig(uint32_t deviceId, J9CudaCacheConfig* configOut)
	{
		return _lib->cuda_deviceGetCacheConfig(_lib, deviceId, configOut);
	}

	int32_t cuda_deviceGetCount(uint32_t* countOut) { return _lib->cuda_deviceGetCount(_lib, countOut); }

	int32_t cuda_deviceGetLimit(uint32_t deviceId, J9CudaDeviceLimit limit, uintptr_t* valueOut)
	{
		return _lib->cuda_deviceGetLimit(_lib, deviceId, limit, valueOut);
	}

	int32_t cuda_deviceGetMemInfo(uint32_t deviceId, uintptr_t* freeOut, uintptr_t* totalOut)
	{
		return _lib->cuda_deviceGetMemInfo(_lib, deviceId, freeOut, totalOut);
	}

	int32_t cuda_deviceGetName(uint32_t deviceId, uint32_t nameSize, char* nameOut)
	{
		return _lib->cuda_deviceGetName(_lib, deviceId, nameSize, nameOut);
	}

	int32_t cuda_deviceGetSharedMemConfig(uint32_t deviceId, J9CudaSharedMemConfig* configOut)
	{
		return _lib->cuda_deviceGetSharedMemConfig(_lib, deviceId, configOut);
	}

	int32_t
	cuda_deviceGetStreamPriorityRange(uint32_t deviceId, int32_t* leastPriorityOut, int32_t* greatestPriorityOut)
	{
		return _lib->cuda_deviceGetStreamPriorityRange(_lib, deviceId, leastPriorityOut, greatestPriorityOut);
	}

	int32_t cuda_deviceReset(uint32_t deviceId) { return _lib->cuda_deviceReset(_lib, deviceId); }

	int32_t cuda_deviceSetCacheConfig(uint32_t deviceId, J9CudaCacheConfig config)
	{
		return _lib->cuda_deviceSetCacheConfig(_lib, deviceId, config);
	}

	int32_t cuda_deviceSetLimit(uint32_t deviceId, J9CudaDeviceLimit limit, uintptr_t value)
	{
		return _lib->cuda_deviceSetLimit(_lib, deviceId, limit, value);
	}

	int32_t cuda_deviceSetSharedMemConfig(uint32_t deviceId, J9CudaSharedMemConfig config)
	{
		return _lib->cuda_deviceSetSharedMemConfig(_lib, deviceId, config);
	}

	int32_t cuda_deviceSynchronize(uint32_t deviceId) { return _lib->cuda_deviceSynchronize(_lib, deviceId); }

	int32_t cuda_driverGetVersion(uint32_t* versionOut) { return _lib->cuda_driverGetVersion(_lib, versionOut); }

	int32_t cuda_eventCreate(uint32_t deviceId, uint32_t flags, J9CudaEvent* eventOut)
	{
		return _lib->cuda_eventCreate(_lib, deviceId, flags, eventOut);
	}

	int32_t cuda_eventDestroy(uint32_t deviceId, J9CudaEvent event) { return _lib->cuda_eventDestroy(_lib, deviceId, event); }

	int32_t cuda_eventElapsedTime(J9CudaEvent start, J9CudaEvent end, float* elapsedMillisOut)
	{
		return _lib->cuda_eventElapsedTime(_lib, start, end, elapsedMillisOut);
	}

	int32_t cuda_eventQuery(J9CudaEvent event) { return _lib->cuda_eventQuery(_lib, event); }

	int32_t cuda_eventRecord(uint32_t deviceId, J9CudaEvent event, J9CudaStream stream)
	{
		return _lib->cuda_eventRecord(_lib, deviceId, event, stream);
	}

	int32_t cuda_eventSynchronize(J9CudaEvent event) { return _lib->cuda_eventSynchronize(_lib, event); }

	int32_t cuda_funcGetAttribute(uint32_t deviceId,
	                              J9CudaFunction function,
	                              J9CudaFunctionAttribute attribute,
	                              int32_t* valueOut)
	{
		return _lib->cuda_funcGetAttribute(_lib, deviceId, function, attribute, valueOut);
	}

	int32_t cuda_funcMaxActiveBlocksPerMultiprocessor(uint32_t deviceId,
	                                                  J9CudaFunction function,
	                                                  uint32_t blockSize,
	                                                  uint32_t dynamicSharedMemorySize,
	                                                  uint32_t flags,
	                                                  uint32_t* valueOut)
	{
		return _lib->cuda_funcMaxActiveBlocksPerMultiprocessor(_lib, deviceId, function, blockSize, dynamicSharedMemorySize, flags, valueOut);
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
		return _lib->cuda_funcMaxPotentialBlockSize(_lib, deviceId, function, dynamicSharedMemoryFunction, userData, blockSizeLimit, flags, minGridSizeOut, maxBlockSizeOut);
	}

	int32_t cuda_funcSetCacheConfig(uint32_t deviceId, J9CudaFunction function, J9CudaCacheConfig config)
	{
		return _lib->cuda_funcSetCacheConfig(_lib, deviceId, function, config);
	}

	int32_t cuda_funcSetSharedMemConfig(uint32_t deviceId, J9CudaFunction function, J9CudaSharedMemConfig config)
	{
		return _lib->cuda_funcSetSharedMemConfig(_lib, deviceId, function, config);
	}

	const char* cuda_getErrorString(int32_t error) { return _lib->cuda_getErrorString(_lib, error); }

	int32_t cuda_hostAlloc(uintptr_t size, uint32_t flags, void** hostAddressOut) { return _lib->cuda_hostAlloc(_lib, size, flags, hostAddressOut); }

	int32_t cuda_hostFree(void* hostAddress) { return _lib->cuda_hostFree(_lib, hostAddress); }

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
		return _lib->cuda_launchKernel(_lib, deviceId, function, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, stream, kernelParms);
	}

	int32_t cuda_linkerAddData(uint32_t deviceId,
	                           J9CudaLinker linker,
	                           J9CudaJitInputType type,
	                           void* data,
	                           uintptr_t size,
	                           const char* name,
	                           J9CudaJitOptions* options)
	{
		return _lib->cuda_linkerAddData(_lib, deviceId, linker, type, data, size, name, options);
	}

	int32_t cuda_linkerComplete(uint32_t deviceId, J9CudaLinker linker, void** cubinOut, uintptr_t* sizeOut)
	{
		return _lib->cuda_linkerComplete(_lib, deviceId, linker, cubinOut, sizeOut);
	}

	int32_t cuda_linkerCreate(uint32_t deviceId, J9CudaJitOptions* options, J9CudaLinker* linkerOut)
	{
		return _lib->cuda_linkerCreate(_lib, deviceId, options, linkerOut);
	}

	int32_t cuda_linkerDestroy(uint32_t deviceId, J9CudaLinker linker) { return _lib->cuda_linkerDestroy(_lib, deviceId, linker); }

	int32_t cuda_memcpy2DDeviceToDevice(uint32_t deviceId,
	                                    void* targetAddress,
	                                    size_t targetPitch,
	                                    const void* sourceAddress,
	                                    size_t sourcePitch,
	                                    uintptr_t width,
	                                    uintptr_t height)
	{
		return _lib->cuda_memcpy2DDeviceToDevice(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
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
		return _lib->cuda_memcpy2DDeviceToDeviceAsync(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpy2DDeviceToHost(uint32_t deviceId,
	                                  void* targetAddress,
	                                  size_t targetPitch,
	                                  const void* sourceAddress,
	                                  size_t sourcePitch,
	                                  uintptr_t width,
	                                  uintptr_t height)
	{
		return _lib->cuda_memcpy2DDeviceToHost(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
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
		return _lib->cuda_memcpy2DDeviceToHostAsync(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpy2DHostToDevice(uint32_t deviceId,
	                                  void* targetAddress,
	                                  size_t targetPitch,
	                                  const void* sourceAddress,
	                                  size_t sourcePitch,
	                                  uintptr_t width,
	                                  uintptr_t height)
	{
		return _lib->cuda_memcpy2DHostToDevice(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height);
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
		return _lib->cuda_memcpy2DHostToDeviceAsync(_lib, deviceId, targetAddress, targetPitch, sourceAddress, sourcePitch, width, height, stream);
	}

	int32_t cuda_memcpyDeviceToDevice(uint32_t deviceId,
	                                  void* targetAddress,
	                                  const void* sourceAddress,
	                                  uintptr_t byteCount)
	{
		return _lib->cuda_memcpyDeviceToDevice(_lib, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyDeviceToDeviceAsync(uint32_t deviceId,
	                                       void* targetAddress,
	                                       const void* sourceAddress,
	                                       uintptr_t byteCount,
	                                       J9CudaStream stream)
	{
		return _lib->cuda_memcpyDeviceToDeviceAsync(_lib, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memcpyDeviceToHost(uint32_t deviceId, void* targetAddress, const void* sourceAddress, uintptr_t byteCount)
	{
		return _lib->cuda_memcpyDeviceToHost(_lib, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyDeviceToHostAsync(uint32_t deviceId,
	                                     void* targetAddress,
	                                     const void* sourceAddress,
	                                     uintptr_t byteCount,
	                                     J9CudaStream stream)
	{
		return _lib->cuda_memcpyDeviceToHostAsync(_lib, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memcpyHostToDevice(uint32_t deviceId, void* targetAddress, const void* sourceAddress, uintptr_t byteCount)
	{
		return _lib->cuda_memcpyHostToDevice(_lib, deviceId, targetAddress, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyHostToDeviceAsync(uint32_t deviceId,
	                                     void* targetAddress,
	                                     const void* sourceAddress,
	                                     uintptr_t byteCount,
	                                     J9CudaStream stream)
	{
		return _lib->cuda_memcpyHostToDeviceAsync(_lib, deviceId, targetAddress, sourceAddress, byteCount, stream);
	}

	int32_t cuda_memcpyPeer(uint32_t targetDeviceId,
	                        void* targetAddress,
	                        uint32_t sourceDeviceId,
	                        const void* sourceAddress,
	                        uintptr_t byteCount)
	{
		return _lib->cuda_memcpyPeer(_lib, targetDeviceId, targetAddress, sourceDeviceId, sourceAddress, byteCount);
	}

	int32_t cuda_memcpyPeerAsync(uint32_t targetDeviceId,
	                             void* targetAddress,
	                             uint32_t sourceDeviceId,
	                             const void* sourceAddress,
	                             uintptr_t byteCount,
	                             J9CudaStream stream)
	{
		return _lib->cuda_memcpyPeerAsync(_lib, targetDeviceId, targetAddress, sourceDeviceId, sourceAddress, byteCount, stream);
	}

	int32_t
	cuda_memset8Async(uint32_t deviceId, void* deviceAddress, uint8_t value, uintptr_t count, J9CudaStream stream)
	{
		return _lib->cuda_memset8Async(_lib, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_memset16Async(uint32_t deviceId, void* deviceAddress, uint16_t value, uintptr_t count, J9CudaStream stream)
	{
		return _lib->cuda_memset16Async(_lib, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_memset32Async(uint32_t deviceId, void* deviceAddress, uint32_t value, uintptr_t count, J9CudaStream stream)
	{
		return _lib->cuda_memset32Async(_lib, deviceId, deviceAddress, value, count, stream);
	}

	int32_t
	cuda_moduleGetFunction(uint32_t deviceId, J9CudaModule module, const char* name, J9CudaFunction* functionOut)
	{
		return _lib->cuda_moduleGetFunction(_lib, deviceId, module, name, functionOut);
	}

	int32_t cuda_moduleGetGlobal(uint32_t deviceId,
	                             J9CudaModule module,
	                             const char* name,
	                             uintptr_t* addressOut,
	                             uintptr_t* sizeOut)
	{
		return _lib->cuda_moduleGetGlobal(_lib, deviceId, module, name, addressOut, sizeOut);
	}

	int32_t
	cuda_moduleGetSurfaceRef(uint32_t deviceId, J9CudaModule module, const char* name, uintptr_t* surfRefOut)
	{
		return _lib->cuda_moduleGetSurfaceRef(_lib, deviceId, module, name, surfRefOut);
	}

	int32_t cuda_moduleGetTextureRef(uint32_t deviceId, J9CudaModule module, const char* name, uintptr_t* texRefOut)
	{
		return _lib->cuda_moduleGetTextureRef(_lib, deviceId, module, name, texRefOut);
	}

	int32_t
	cuda_moduleLoad(uint32_t deviceId, const void* image, J9CudaJitOptions* options, J9CudaModule* moduleOut)
	{
		return _lib->cuda_moduleLoad(_lib, deviceId, image, options, moduleOut);
	}

	int32_t cuda_moduleUnload(uint32_t deviceId, J9CudaModule module) { return _lib->cuda_moduleUnload(_lib, deviceId, module); }

	int32_t cuda_runtimeGetVersion(uint32_t* versionOut) { return _lib->cuda_runtimeGetVersion(_lib, versionOut); }

	int32_t cuda_streamAddCallback(uint32_t deviceId,
	                               J9CudaStream stream,
	                               J9CudaStreamCallback callback,
	                               uintptr_t userData)
	{
		return _lib->cuda_streamAddCallback(_lib, deviceId, stream, callback, userData);
	}

	int32_t cuda_streamCreate(uint32_t deviceId, J9CudaStream* streamOut) { return _lib->cuda_streamCreate(_lib, deviceId, streamOut); }

	int32_t
	cuda_streamCreateWithPriority(uint32_t deviceId, int32_t priority, uint32_t flags, J9CudaStream* streamOut)
	{
		return _lib->cuda_streamCreateWithPriority(_lib, deviceId, priority, flags, streamOut);
	}

	int32_t cuda_streamDestroy(uint32_t deviceId, J9CudaStream stream) { return _lib->cuda_streamDestroy(_lib, deviceId, stream); }

	int32_t cuda_streamGetFlags(uint32_t deviceId, J9CudaStream stream, uint32_t* flagsOut)
	{
		return _lib->cuda_streamGetFlags(_lib, deviceId, stream, flagsOut);
	}

	int32_t cuda_streamGetPriority(uint32_t deviceId, J9CudaStream stream, int32_t* priorityOut)
	{
		return _lib->cuda_streamGetPriority(_lib, deviceId, stream, priorityOut);
	}

	int32_t cuda_streamQuery(uint32_t deviceId, J9CudaStream stream) { return _lib->cuda_streamQuery(_lib, deviceId, stream); }

	int32_t cuda_streamSynchronize(uint32_t deviceId, J9CudaStream stream) { return _lib->cuda_streamSynchronize(_lib, deviceId, stream); }

	int32_t cuda_streamWaitEvent(uint32_t deviceId, J9CudaStream stream, J9CudaEvent event)
	{
		return _lib->cuda_streamWaitEvent(_lib, deviceId, stream, event);
	}

#endif /* OMR_OPT_CUDA */

private:
	OMRPortLibrary* _lib;
};

} /* namespace OMR */

#endif /* OMR_PORTLIBRARYHANDLE_HPP_ */
