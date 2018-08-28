#include <OMR/GC/System.hpp>

#include "GCExtensionsBase.hpp"
#include "Heap.hpp"
#include "OMR_VMThread.hpp"
#include "StartupManagerImpl.hpp"
#include "mminitcore.h"
#include "omr.h"
#include "omrgcstartup.hpp"
#include "omrport.h"
#include "omrutil.h"
#include "omrvm.h"
#include "thread_api.h"
#include <ParallelDispatcher.hpp>
#include <cassert>
#include <iostream>

namespace OMR {
namespace GC {

System::System(Runtime& runtime) : _runtime(&runtime) {
#if defined(OMR_GC_TRACE)
	std::cerr << __PRETTY_FUNCTION__ << std::endl;
#endif // OMR_GC_TRACE

	Thread self(runtime.platform().thread());

	initVm();
	initCollector();

	StartupContext cx(*this);
	initGcSlaveThreads(cx);
}

System::~System() {
	Thread self(runtime().platform().thread());
	// TODO: Shut down the heap (requires a thread (boo!!))
	omr_detach_vm_from_runtime(&_vm);
}

void System::initVm() {
	memset(&_vm, 0, sizeof(OMR_VM));
	_vm._runtime = &runtime().data();
	_vm._language_vm = this;
	_vm._vmThreadList = nullptr;
	_vm._gcOmrVMExtensions = nullptr;
	_vm._languageThreadCount = 0;

	auto e = omr_attach_vm_to_runtime(&_vm);

	if (e != 0) {
		throw PlatformError(e);
	}
}

void System::initCollector() {
	MM_StartupManagerImpl startupManager(&_vm);
	auto e = OMR_GC_IntializeHeapAndCollector(&_vm, &startupManager);
	if (e != 0) {
		throw PlatformError(e);
	}
}

void System::initGcSlaveThreads(StartupContext& cx) {
	auto extensions = cx.env()->getExtensions();
	assert(extensions != nullptr);
	auto ok = extensions->dispatcher->startUpThreads();
	if (!ok) {
		extensions->dispatcher->shutDownThreads();
		throw PlatformError(0);
	}
}

/// Note:
void Context::attachVmContext(OMR_VM& vm, Context& cx) {
	omrthread_t self;

	// assert(nullptr == omr_vmthread_getCurrent(&vm));

	assert(omrthread_attach_ex(&self, J9THREAD_ATTR_DEFAULT) == 0);
	omrthread_monitor_enter(vm._vmThreadListMutex);

	///////// FIRSTATTACH

	assert(omr_vmthread_firstAttach(&vm, &cx._vmContext) == 0);
	setOMRVMThreadNameWithFlag(nullptr, cx._vmContext, (char*)"OMR::Om::Context", TRUE);
	cx._vmContext->_language_vmthread = &cx;
	assert(initializeMutatorModel(cx._vmContext) == 0);
	omrthread_monitor_exit(vm._vmThreadListMutex);
	omrthread_detach(self);
}

void Context::detachVmContext(OMR_VM& vm, Context& cx) {
	omrthread_t self;

	assert(cx._vmContext->_attachCount == 1);
	// assert(nullptr != omr_vmthread_getCurrent(&vm));

	omrthread_attach_ex(&self, J9THREAD_ATTR_DEFAULT);

	/////// LASTFREE

	omrthread_monitor_enter(vm._vmThreadListMutex);
	cleanupMutatorModel(cx._vmContext, TRUE);
	setOMRVMThreadNameWithFlag(nullptr, cx._vmContext, NULL, FALSE);
	omr_vmthread_lastDetach(cx._vmContext);
	omrthread_monitor_exit(vm._vmThreadListMutex);

	omrthread_detach(self);
}

} // namespace GC
} // namespace OMR
