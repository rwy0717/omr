/*******************************************************************************
 * Copyright (c) 1991, 2015 IBM Corp. and others
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

#include "omrport.h"
#include "omrsignal_context.h"

#define NUM_REGS 16

void fillInUnixSignalInfo(struct OMRPortLibrary* portLibrary, void* contextInfo, struct OMRUnixSignalInfo* j9Info)
{
    j9Info->platformSignalInfo.context = (ucontext_t*)contextInfo;
    /* module info is filled on demand */
}

uint32_t infoForSignal(
    struct OMRPortLibrary* portLibrary, struct OMRUnixSignalInfo* info, int32_t index, const char** name, void** value)
{
    *name = "";

    switch (index) {

    case OMRPORT_SIG_SIGNAL_TYPE:
    case 0:
        *name = "J9Generic_Signal_Number";
        *value = &info->portLibrarySignalType;
        return OMRPORT_SIG_VALUE_32;

    case OMRPORT_SIG_SIGNAL_PLATFORM_SIGNAL_TYPE:
    case 1:
        *name = "Signal_Number";
        *value = &info->sigInfo->si_signo;
        return OMRPORT_SIG_VALUE_32;

    case OMRPORT_SIG_SIGNAL_ERROR_VALUE:
    case 2:
        *name = "Error_Value";
        *value = &info->sigInfo->si_errno;
        return OMRPORT_SIG_VALUE_32;

    case OMRPORT_SIG_SIGNAL_CODE:
    case 3:
        *name = "Signal_Code";
        *value = &info->sigInfo->si_code;
        return OMRPORT_SIG_VALUE_32;

    case OMRPORT_SIG_SIGNAL_HANDLER:
    case 4:
        *name = "Handler1";
        *value = &info->handlerAddress;
        return OMRPORT_SIG_VALUE_ADDRESS;

    case 5:
        *name = "Handler2";
        *value = &info->handlerAddress2;
        return OMRPORT_SIG_VALUE_ADDRESS;

    case OMRPORT_SIG_SIGNAL_INACCESSIBLE_ADDRESS:
    case 6:
        /* si_code > 0 indicates that the signal was generated by the kernel */
        if (info->sigInfo->si_code > 0) {
            if ((info->sigInfo->si_signo == SIGBUS) || (info->sigInfo->si_signo == SIGSEGV)) {
                *name = "InaccessibleAddress";
                *value = &info->sigInfo->si_addr;
                return OMRPORT_SIG_VALUE_ADDRESS;
            }
        }
        return OMRPORT_SIG_VALUE_UNDEFINED;

    default:
        return OMRPORT_SIG_VALUE_UNDEFINED;
    }
}

uint32_t infoForFPR(
    struct OMRPortLibrary* portLibrary, struct OMRUnixSignalInfo* info, int32_t index, const char** name, void** value)
{
    const char* n_fpr[NUM_REGS] = { "fpr0", "fpr1", "fpr2", "fpr3", "fpr4", "fpr5", "fpr6", "fpr7", "fpr8", "fpr9",
        "fpr10", "fpr11", "fpr12", "fpr13", "fpr14", "fpr15" };

    *name = "";

    if ((index >= 0) && (index < NUM_REGS)) {
        *name = n_fpr[index];
        *value = &(info->platformSignalInfo.context->uc_mcontext.fpregs.fprs[index].d);
        return OMRPORT_SIG_VALUE_FLOAT_64;
    }

    return OMRPORT_SIG_VALUE_UNDEFINED;
}

uint32_t infoForGPR(
    struct OMRPortLibrary* portLibrary, struct OMRUnixSignalInfo* info, int32_t index, const char** name, void** value)
{

    const char* n_gpr[NUM_REGS * 2]
        = { "gpr0",
              "gpr1",
              "gpr2",
              "gpr3",
              "gpr4",
              "gpr5",
              "gpr6",
              "gpr7",
              "gpr8",
              "gpr9",
              "gpr10",
              "gpr11",
              "gpr12",
              "gpr13",
              "gpr14",
              "gpr15"
#if !defined(OMR_ENV_DATA64)
              ,
              "hgpr0",
              "hgpr1",
              "hgpr2",
              "hgpr3",
              "hgpr4",
              "hgpr5",
              "hgpr6",
              "hgpr7",
              "hgpr8",
              "hgpr9",
              "hgpr10",
              "hgpr11",
              "hgpr12",
              "hgpr13",
              "hgpr14",
              "hgpr15"
#endif
          };

    *name = "";

    if ((index >= 0) && (index < NUM_REGS)) {
        *name = n_gpr[index];
        *value = &(info->platformSignalInfo.context->uc_mcontext.gregs[index]);
        return OMRPORT_SIG_VALUE_ADDRESS;
    }
#if !defined(OMR_ENV_DATA64)
    else if ((index >= NUM_REGS) && (index < NUM_REGS * 2)) {
        if (UC_EXTENDED == info->platformSignalInfo.context->uc_flags) {
            struct ucontext_extended* extendedContext = (ucontext_extended*)info->platformSignalInfo.context;
            *name = n_gpr[index];
            *value = &(extendedContext->uc_gprs_high[index - NUM_REGS]);
            return OMRPORT_SIG_VALUE_ADDRESS;
        }
    }
#endif
    return OMRPORT_SIG_VALUE_UNDEFINED;
}

uint32_t infoForControl(
    struct OMRPortLibrary* portLibrary, struct OMRUnixSignalInfo* info, int32_t index, const char** name, void** value)
{
    mcontext_t* mcontext = (mcontext_t*)&info->platformSignalInfo.context->uc_mcontext;
    *name = "";

    switch (index) {
    case OMRPORT_SIG_CONTROL_PC:
    case 0:
        *name = "psw";
        *value = &(mcontext->psw.addr);
        return OMRPORT_SIG_VALUE_ADDRESS;
    case 1:
        *name = "mask";
        *value = &(mcontext->psw.mask);
        return OMRPORT_SIG_VALUE_ADDRESS;
    case OMRPORT_SIG_CONTROL_S390_FPC:
    case 2:
        *name = "fpc";
        *value = &(mcontext->fpregs.fpc);
        return OMRPORT_SIG_VALUE_ADDRESS;
    case OMRPORT_SIG_CONTROL_S390_BEA:
    case 3:
        *name = "bea";
        *value = &(info->platformSignalInfo.breakingEventAddr);
        return OMRPORT_SIG_VALUE_ADDRESS;
    default:
        return OMRPORT_SIG_VALUE_UNDEFINED;
    }
}

uint32_t infoForModule(
    struct OMRPortLibrary* portLibrary, struct OMRUnixSignalInfo* info, int32_t index, const char** name, void** value)
{
    Dl_info* dl_info = &(info->platformSignalInfo.dl_info);
    void* address;
    int dl_result;
    mcontext_t* mcontext = (mcontext_t*)&info->platformSignalInfo.context->uc_mcontext;
    *name = "";

#ifdef OMR_ENV_DATA64
    address = (void*)mcontext->psw.addr;
#else
    /* only the low 31 bits are meaningful */
    address = (void*)((uint32_t)mcontext->psw.addr & 0x7FFFFFFF);
#endif

    dl_result = dladdr(address, dl_info);

    switch (index) {
    case OMRPORT_SIG_MODULE_NAME:
    case 0:
        *name = "Module";
        if (dl_result) {
            *value = (void*)(dl_info->dli_fname);
            return OMRPORT_SIG_VALUE_STRING;
        }
        return OMRPORT_SIG_VALUE_UNDEFINED;
    case 1:
        *name = "Module_base_address";
        if (dl_result) {
            *value = (void*)&(dl_info->dli_fbase);
            return OMRPORT_SIG_VALUE_ADDRESS;
        }
        return OMRPORT_SIG_VALUE_UNDEFINED;
    case 2:
        *name = "Symbol";
        if (dl_result) {
            if (dl_info->dli_sname != NULL) {
                *value = (void*)(dl_info->dli_sname);
                return OMRPORT_SIG_VALUE_STRING;
            }
        }
        return OMRPORT_SIG_VALUE_UNDEFINED;
    case 3:
        *name = "Symbol_address";
        if (dl_result) {
            *value = (void*)&(dl_info->dli_saddr);
            return OMRPORT_SIG_VALUE_ADDRESS;
        }
        return OMRPORT_SIG_VALUE_UNDEFINED;
    default:
        return OMRPORT_SIG_VALUE_UNDEFINED;
    }
}
