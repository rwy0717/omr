/*******************************************************************************
 * Copyright (c) 2000, 2016 IBM Corp. and others
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
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

/*
 * This file will be included within an enum.  Only comments and enumerator
 * definitions are permitted.
 */

NoReg = 0, gr0 = 1, FirstGPR = gr0, gr1 = 2, dsa = gr1, gr2 = 3, rtoc = gr2, gr3 = 4, gr4 = 5, gr5 = 6, gr6 = 7,
           gr7 = 8, gr8 = 9, gr9 = 10, gr10 = 11, gr11 = 12, gr12 = 13, gr13 = 14, gr14 = 15, gr15 = 16, gr16 = 17,
           gr17 = 18, gr18 = 19, gr19 = 20, gr20 = 21, gr21 = 22, gr22 = 23, gr23 = 24, gr24 = 25, gr25 = 26, gr26 = 27,
           gr27 = 28, gr28 = 29, gr29 = 30, gr30 = 31, gr31 = 32, LastGPR = gr31, LastAssignableGPR = gr31, fp0 = 33,
           FirstFPR = fp0, fp1 = 34, fp2 = 35, fp3 = 36, fp4 = 37, fp5 = 38, fp6 = 39, fp7 = 40, fp8 = 41, fp9 = 42,
           fp10 = 43, fp11 = 44, fp12 = 45, fp13 = 46, fp14 = 47, fp15 = 48, fp16 = 49, fp17 = 50, fp18 = 51, fp19 = 52,
           fp20 = 53, fp21 = 54, fp22 = 55, fp23 = 56, fp24 = 57, fp25 = 58, fp26 = 59, fp27 = 60, fp28 = 61, fp29 = 62,
           fp30 = 63, fp31 = 64, LastFPR = fp31, LastAssignableFPR = fp31,

           vr0 = 65, FirstVRF = vr0, vr1 = 66, vr2 = 67, vr3 = 68, vr4 = 69, vr5 = 70, vr6 = 71, vr7 = 72, vr8 = 73,
           vr9 = 74, vr10 = 75, vr11 = 76, vr12 = 77, vr13 = 78, vr14 = 79, vr15 = 80, vr16 = 81, vr17 = 82, vr18 = 83,
           vr19 = 84, vr20 = 85, vr21 = 86, vr22 = 87, vr23 = 88, vr24 = 89, vr25 = 90, vr26 = 91, vr27 = 92, vr28 = 93,
           vr29 = 94, vr30 = 95, vr31 = 96, LastVRF = vr31,
           // VSR can be alises for now
    vsr0 = fp0, FirstVSR = vsr0, vsr1 = fp1, vsr2 = fp2, vsr3 = fp3, vsr4 = fp4, vsr5 = fp5, vsr6 = fp6, vsr7 = fp7,
           vsr8 = fp8, vsr9 = fp9, vsr10 = fp10, vsr11 = fp11, vsr12 = fp12, vsr13 = fp13, vsr14 = fp14, vsr15 = fp15,
           vsr16 = fp16, vsr17 = fp17, vsr18 = fp18, vsr19 = fp19, vsr20 = fp20, vsr21 = fp21, vsr22 = fp22,
           vsr23 = fp23, vsr24 = fp24, vsr25 = fp25, vsr26 = fp26, vsr27 = fp27, vsr28 = fp28, vsr29 = fp29,
           vsr30 = fp30, vsr31 = fp31, vsr32 = vr0, vsr33 = vr1, vsr34 = vr2, vsr35 = vr3, vsr36 = vr4, vsr37 = vr5,
           vsr38 = vr6, vsr39 = vr7, vsr40 = vr8, vsr41 = vr9, vsr42 = vr10, vsr43 = vr11, vsr44 = vr12, vsr45 = vr13,
           vsr46 = vr14, vsr47 = vr15, vsr48 = vr16, vsr49 = vr17, vsr50 = vr18, vsr51 = vr19, vsr52 = vr20,
           vsr53 = vr21, vsr54 = vr22, vsr55 = vr23, vsr56 = vr24, vsr57 = vr25, vsr58 = vr26, vsr59 = vr27,
           vsr60 = vr28, vsr61 = vr29, vsr62 = vr30, vsr63 = vr31, LastVSR = vsr63, LastAssignableVSR = vsr63,

           cr0 = 97, FirstCCR = cr0, cr1 = 98, cr2 = 99, cr3 = 100, cr4 = 101, cr5 = 102, cr6 = 103, cr7 = 104,
           LastCCR = cr7, LastAssignableCCR = cr7, mq = 105, FirstSPR = mq, lr = 106, ctr = 107, SpilledReg = 108,
           NumRegisters = SpilledReg + 1
