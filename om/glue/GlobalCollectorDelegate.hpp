/*******************************************************************************
 * Copyright (c) 2017, 2017 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *******************************************************************************/

#ifndef GLOBALCOLLECTORDELEGATE_HPP_
#define GLOBALCOLLECTORDELEGATE_HPP_

#include "EnvironmentBase.hpp"
#include "MarkingScheme.hpp"
#include "omrcfg.h"
#include "omrgcconsts.h"

namespace OMR {
namespace Om {
class Cell;
}
}

class MM_GCExtensionsBase;
class MM_GlobalCollector;
class MM_MemorySubSpace;
class GC_ObjectHeapIterator;
/**
 * Delegate class provides implementations for methods required for gc policies
 * using OMR global collector.
 */
class MM_GlobalCollectorDelegate {
	/*
	 * Data members
	 */
private:
protected:
	MM_GCExtensionsBase* _extensions;
	MM_MarkingScheme* _markingScheme;
	MM_GlobalCollector* _globalCollector;

public:
	/*
	 * Function members
	 */
private:
protected:
public:
	/**
	 * Initialize the delegate.
	 *
	 * @param env environment for calling thread
	 * @param markingScheme the global MM_MarkingScheme instance
	 * @param globalCollector the MM_GlobalCollector instance that the delegate is
	 * bound to
	 * @return true if delegate initialized successfully
	 */
	bool initialize(MM_EnvironmentBase* env,
	                MM_GlobalCollector* globalCollector,
	                MM_MarkingScheme* markingScheme) {
		_extensions = env->getExtensions();
		_markingScheme = markingScheme;
		_globalCollector = globalCollector;

		return true;
	}

	/**
	 * Release resources acquired during delegate initalization and operation.
	 *
	 * @param env environment for calling thread
	 */
	void tearDown(MM_EnvironmentBase* env) {}

	/**
	 * Called on GC master thread prior to commencing a global collection. This is
	 * informational, no specific actions are specified for this method.
	 *
	 * This is called before the master thread begins setting up for the global
	 * collection.
	 *
	 * @param env environment for calling thread
	 */
	void masterThreadGarbageCollectStarted(MM_EnvironmentBase* env) {}

	/**
	 * Called on GC master thread during a global collection. This is
	 * informational, no specific actions are specified for this method.
	 *
	 * This is called on the master thread when the marking phase of the
	 * collection is complete and before the sweeping phase commences.
	 *
	 * @param env environment for calling thread
	 */
	void postMarkProcessing(MM_EnvironmentBase* env);

#define OMR_GC_PAINT_HEAP 1

#if defined(OMR_GC_PAINT_HEAP)



	/// Corrupt the memory of a GC cell.
	/// Note: Be careful about overwriting shapes, shape data is needed for heap walking.
	void poison(OMR::Om::Cell* cell);

	/**
	 * Poison the unmarked objects in a particular region.
	 */
	void poisonUnmarkedObjectsInRegion(GC_ObjectHeapIterator& objectIterator);

	/**
	 * Walk the heap, for objects that are not marked we corrupt them to maximize
	 * the chance we will crash immediately if they are used.  For live objects
	 * validate that they have the expected eyecatcher */
	void poisonUnmarkedObjects(MM_EnvironmentBase* env);

#endif // OMR_GC_PAINT_HEAP

	/**
	 * Called on GC master thread near the end of a global collection. This is
	 * informational, no specific actions are specified for this method.
	 *
	 * This will be called on the master thread when the master thread completes
	 * its participation in the collection. Other GC threads may still be running
	 * at this point.
	 *
	 * @param env environment for calling thread
	 */
	void masterThreadGarbageCollectFinished(MM_EnvironmentBase* env, bool compactedThisCycle) {}

	/**
	 * Called on GC master thread near the end a global collection. This is
	 * informational, no specific actions are specified for this method.
	 *
	 * This is called on the master thread after all GC threads have completed
	 * their participation in the global collection cycle.
	 *
	 * @param env environment for calling thread
	 */
	void postCollect(MM_EnvironmentBase* env, MM_MemorySubSpace* subSpace) {}

	/**
	 * Called on GC master thread prior to commencing a global collection. This is
	 * informational, no specific actions are specified for this method.
	 *
	 * This will be called just before a heap walk is started. It can be used to
	 * set up additional triggers or hooks to be called during the heap walk.
	 *
	 * @param env environment for calling thread
	 */
	void prepareHeapForWalk(MM_EnvironmentBase* env) {}

	/**
	 * In order to allow the heap to remain walkable for diagnostics some fixup is
	 * required after global collection. This method is called to allow the
	 * delegate to suppress fixup if diagnostics are not required.
	 *
	 * @return true if diagnostics are required to be supported
	 */
	bool isAllowUserHeapWalk() { return true; }

	/**
	 * Informational, called when the heap expands.
	 *
	 * @return true to allow heap expansion.
	 */
	bool heapAddRange(MM_EnvironmentBase* env,
	                  MM_MemorySubSpace* subspace,
	                  UDATA size,
	                  void* lowAddress,
	                  void* highAddress) {
		return true;
	}

	/**
	 * Informational, called when the heap contracts.
	 *
	 * @return true to allow heap contraction.
	 */
	bool heapRemoveRange(MM_EnvironmentBase* env,
	                     MM_MemorySubSpace* subspace,
	                     UDATA size,
	                     void* lowAddress,
	                     void* highAddress,
	                     void* lowValidAddress,
	                     void* highValidAddress) {
		return true;
	}

	/**
	 * This method is used to determine whether a generational collection should
	 * be promoted to a global collection. This is used only with concurrent
	 * marking, most implementations should simply return false.
	 *
	 * @return true if a global collection should occur instead of a generational
	 * collection.
	 */
	bool isTimeForGlobalGCKickoff() { return false; }

#if defined(OMR_GC_MODRON_COMPACTION)
	/**
	 * If compaction is enabled, global collector will call this to allow the
	 * delegate to inhibit compaction for the current global colleciton cycle.
	 *
	 * @return true to suppress compaction for this cycle
	 */
	CompactPreventedReason checkIfCompactionShouldBePrevented(MM_EnvironmentBase* env) {
		return COMPACT_PREVENTED_NONE;
	}
#endif /* OMR_GC_MODRON_COMPACTION */

	MM_GlobalCollectorDelegate()
	        : _extensions(NULL), _markingScheme(NULL), _globalCollector(NULL) {}
};
#endif /* GLOBALCOLLECTORDELEGATE_HPP_ */
