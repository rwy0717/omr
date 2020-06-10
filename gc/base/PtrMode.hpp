/*******************************************************************************
 * Copyright (c) 2020, 2020 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/


#if !defined(PTRMODE_HPP_)
#define PTRMODE_HPP_

#include "omrcfg.h"

enum GC_PtrMode
{
#if defined(OMR_GC_FULL_POINTERS)
	PTR_MODE_FULL,
#endif
#if defined(OMR_GC_COMPRESSED_POINTERS)
	PTR_MODE_COMPRESSED,
#endif
};

/**
 * True if the pointer mode indicates that pointers must be compressed.
 */
MMINLINE bool
isCompressed(GC_PtrMode mode)
{
#if defined(OMR_GC_COMPRESSED_POINTERS)
	return mode == PTR_MODE_COMPRESSED;
#else
	return false;
#endif
}

#endif /* defined(PTRMODE_HPP_) */
