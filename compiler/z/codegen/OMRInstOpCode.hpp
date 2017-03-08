/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

#ifndef OMR_Z_INSTOPCODE_INCL
#define OMR_Z_INSTOPCODE_INCL

/*
 * The following #define and typedef must appear before any #includes in this file
 */
#ifndef OMR_INSTOPCODE_CONNECTOR
#define OMR_INSTOPCODE_CONNECTOR
namespace OMR { namespace Z { class InstOpCode; } }
namespace OMR { typedef OMR::Z::InstOpCode InstOpCodeConnector; }
#else
#error OMR::Z::InstOpCode expected to be a primary connector, but a OMR connector is already defined
#endif

#include "compiler/codegen/OMRInstOpCode.hpp"

#include "il/DataTypes.hpp"

namespace TR { class CodeGenerator; }
namespace TR { class Node; }

namespace OMR
{

namespace Z
{

/* Instruction Format
 *
 * Note this is different than the IA32 approach - this uses nibbles for the OpCode length, not bytes since
 * some instructions are 3 nibbles long
 * It also has an extra length field - the length of the entire instruction (not just the opcode)
 * The following are the different formats of op-codes (from the 390 POP)

   E Format
    __________________
   |      Op Code     |
   |__________________|
   0                 15

   RR Format
    ________ ____ ____
   |Op Code | R1 | R2 |
   |________|____|____|
   0         8   12  15

   RRE Format
    _________________ ________ ____ ____
   |     Op Code     |////////| R1 | R2 |
   |_________________|________|____|____|
   0                 16       24   28  31

   RRF Format
    ________________ ____ ____ ____ ____
   |    Op Code     | R1 |////| R3 | R2 |
   |________________|____|____|____|____|
   0                16   20   24   28  31

    ________________ ____ ____ ____ ____
   |    Op Code     | M3 |////| R1 | R2 |
   |________________|____|____|____|____|
   0                16   20   24   28  31

    ________________ ____ ____ ____ ____
   |    Op Code     | M3 | M4 | R1 | R2 |
   |________________|____|____|____|____|
   0                16   20   24   28  31

   The most up-to-date RRF formats combine a-b and c-e as the following:
   RRF-a-b format
    ________________ ____ ____ ____ ____
   |    Op Code     | R3 | M4*| R1 | R2 |
   |________________|____|____|____|____|
   0                16   20   24   28  31

   RRF-c-e format
    ________________ ____ ____ ____ ____
   |    Op Code     | M3*| M4*| R1 | R2 |
   |________________|____|____|____|____|
   0                16   20   24   28  31

   RX Format
    ________ ____ ____ ____ ____________
   |Op Code | R1 | X2 | B2 |     D2     |
   |________|____|____|____|____________|
   0         8   12   16   20          31

   RXE Format
    ________ ____ ____ ____ __/__ ________ ________
   |Op Code | R1 | X2 | B2 | D2  |////////|Op Code |
   |________|____|____|____|__/__|________|________|
   0         8   12   16   20    32       40      47

   RXY Format
    ________ ____ ____ ____ __/__ _______ ________
   |Op Code | R1 | X2 | B2 | D2  |D2 High|Op Code |
   |________|____|____|____|__/__|_______|________|
   0         8   12   16   20    32      40      47

   RXF Format
    ________ ____ ____ ____ _/__ ____ ____ ________
   |Op Code | R3 | X2 | B2 | D2 | R1 |////|Op Code |
   |________|____|____|____|_/__|____|____|________|
   0         8   12   16   20   32   36   40      47

   RS Format
    ________ ____ ____ ____ ____________
   |Op Code | R1 | R3 | B2 |     D2     |
   |________|____|____|____|____________|
   0         8   12   16   20          31

    ________ ____ ____ ____ ____________
   |Op Code | R1 | M3 | B2 |     D2     |
   |________|____|____|____|____________|
   0         8   12   16   20          31

   RSY Format
    ________ ____ ____ ____ ____________ ________
   |Op Code | R1 | R3 | B2 |     D2     |Op Code |
   |________|____|____|____|____________|________|
   0         8   12   16   20           40       47


   RSE Format
    ________ ____ ____ ____ __/__ ________ ________
   |Op Code | R1 | R3 | B2 | D2  |////////|Op Code |
   |________|____|____|____|__/__|________|________|
   0         8   12   16   20    32       40      47

    ________ ____ ____ ____ __/__ ________ ________
   |Op Code | R1 | M3 | B2 | D2  |////////|Op Code |
   |________|____|____|____|__/__|________|________|
   0         8   12   16   20    32       40      47

   RSL Format
    ________ ____ ____ ____ __/__ ________ ________
   |Op Code | L1 | // | B2 | D2  |////////|Op Code |
   |________|____|____|____|__/__|________|________|
   0         8   12   16   20    32       40      47

   RSI Format
    ________ ____ ____ _________________
   |Op Code | R1 | R3 |       I2        |
   |________|____|____|_________________|
   0         8   12   16               31

   RI Format
    ________ ____ ____ _________________
   |Op Code | R1 |OpCd|       I2        |
   |________|____|____|_________________|
   0         8   12   16               31

   RIL Format
    ________ ____ ____ ____ _______________________
   |Op Code | R1 |OpCd|            I2              |
   |________|____|____|____|_______________________|
   0         8   12   16   20                     47

   SI Format
    ________ _________ ____ ____________
   |Op Code |    I2   | B1 |     D1     |
   |________|_________|____|____________|
   0         8        16   20          31

   SIY Format
   ________ __________ ____ __/__ ____________________
   |Op Code |    I2   | B1 | D1 Low |D1 High| Op Code|
   |________|_________|____|__/_____|_______|________|
   0         8        16   20       32      40      47

   S Format
    __________________ ____ ____________
   |     Op Code      | B2 |     D2     |
   |__________________|____|____________|
   0                  16   20          31

   SS Format
    ________ _________ ____ ___/____ ____ ____/____
   |Op Code |    L    | B1 |   D1   | B2 |   D2    |
   |________|_________|____|___/____|____|____/____|
   0         8        16   20       32   36       47

    ________ ____ ____ ____ ___/____ ____ ____/____
   |Op Code | L1 | L2 | B1 |   D1   | B2 |   D2    |
   |________|____|____|____|___/____|____|____/____|
   0         8   12   16   20       32   36       47

    ________ ____ ____ ____ ___/____ ____ ____/____
   |Op Code | R1 | R3 | B1 |   D1   | B2 |   D2    |
   |________|____|____|____|___/____|____|____/____|
   0         8   12   16   20       32   36       47

 |  ________ ____ ____ ____ ___/____ ____ ____/____
 | |Op Code | R1 | R3 | B2 |   D2   | B4 |   D4    |
 | |________|____|____|____|___/____|____|____/____|
 | 0         8   12   16   20       32   36       47

   SSE Format
    __________________ ____ ___/____ ____ ____/____
   |     Op Code      | B1 |   D1   | B2 |   D2    |
   |__________________|____|___/____|____|____/____|
   0                  16   20       32   36       47

   RIE Format
    ________ ____ ____ ____/__ __ ________ ________
   |Op Code | R1 | R3 |    I2    |////////|Op Code |
   |________|____|____|____/_____|________|________|
   0         8   12   16         32       40      47


   RRR Format
    __________________ ____ ____ ____ ____
   |     Op Code      | R3 |////| R1 | R2 |
   |__________________|____|____|____|____|
   0                  16   20   24  28   31

   SSF Format
    ________ ____ ____ ____ ___/____ ____ ____/____
   |Op Code | R3 | Op | B1 |   D1   | B2 |   D2    |
   |________|____|____|____|___/____|____|____/____|
   0         8   12   16   20       32   36       47

*/
#define   PSEUDO        0
#define   DC_FORMAT     1
#define   E_FORMAT      2   //83180:  This define is busting a pragma report(level,E) in Node.hpp
#define   RR_FORMAT     3
#define   RRE_FORMAT    4
#define   RRF_FORMAT    5
#define   RRF2_FORMAT   6
#define   RRF3_FORMAT   7
#define   RX_FORMAT     8
#define   RS_FORMAT     9
#define   RSI_FORMAT    10
#define   RI_FORMAT     11
#define   SI_FORMAT     12
#define   S_FORMAT      13
#define   RXE_FORMAT    14
#define   RXF_FORMAT    15
#define   SS_FORMAT     16
#define   SSE_FORMAT    17
#define   SS1_FORMAT    18
#define   RIL_FORMAT    19
#define   RSE_FORMAT    20
#define   RSL_FORMAT    21
#define   RIE_FORMAT    22
#define   RXY_FORMAT    23
#define   RSY_FORMAT    24
#define   SIY_FORMAT    25
#define   RRR_FORMAT    26
#define   RRS_FORMAT    27
#define   RIS_FORMAT    28
#define   SIL_FORMAT    29
#define   I_FORMAT      30
#define   SSF_FORMAT    31
#define   SMI_FORMAT    32
#define   MII_FORMAT    33
#define   IE_FORMAT     34

#define   VRIa_FORMAT   35 // VRI denotes a vector register-and-immediate operation and an extended op-code field.
#define   VRIb_FORMAT   36
#define   VRIc_FORMAT   37
#define   VRId_FORMAT   38
#define   VRIe_FORMAT   39
#define   VRIf_FORMAT   40
#define   VRIg_FORMAT   41
#define   VRIh_FORMAT   42
#define   VRIi_FORMAT   43

#define   VRRa_FORMAT   44 // VRR denotes a vector register-and-register operation and an extended op-code field.
#define   VRRb_FORMAT   45
#define   VRRc_FORMAT   46
#define   VRRd_FORMAT   47
#define   VRRe_FORMAT   48
#define   VRRf_FORMAT   49
#define   VRRg_FORMAT   50
#define   VRRh_FORMAT   51
#define   VRRi_FORMAT   52

#define   VRSa_FORMAT   53 // VRS denotes a vector register-and-storage operation and an extended op-code field.
#define   VRSb_FORMAT   54
#define   VRSc_FORMAT   55
#define   VRSd_FORMAT   56

#define   VRV_FORMAT    57 // VRV denotes a vector register-and-vector-index-storage operation and an extended op-code field.
#define   VRX_FORMAT    58 // VRX denotes a vector register-and-index-storage operation and an extended op-code field
#define   VSI_FORMAT    59

/* Instruction Properties (One hot encoding) */
#define S390OpProp_UsesTarget             static_cast<uint64_t>(0x0000000000000001ull)
#define S390OpProp_SingleFP               static_cast<uint64_t>(0x0000000000000002ull)
#define S390OpProp_DoubleFP               static_cast<uint64_t>(0x0000000000000004ull)
#define S390OpProp_SetsZeroFlag           static_cast<uint64_t>(0x0000000000000008ull)
#define S390OpProp_SetsSignFlag           static_cast<uint64_t>(0x0000000000000010ull)
#define S390OpProp_SetsCarryFlag          static_cast<uint64_t>(0x0000000000000020ull)
#define S390OpProp_SetsOverflowFlag       static_cast<uint64_t>(0x0000000000000040ull)
#define S390OpProp_BranchOp               static_cast<uint64_t>(0x0000000000000080ull)
#define S390OpProp_IsLoad                 static_cast<uint64_t>(0x0000000000000100ull)
#define S390OpProp_IsStore                static_cast<uint64_t>(0x0000000000000200ull)
#define S390OpProp_IsSync                 static_cast<uint64_t>(0x0000000000000400ull)
#define S390OpProp_ReadsMultipleRegs      static_cast<uint64_t>(0x0000000000000800ull)
#define S390OpProp_SetsMultipleRegs       static_cast<uint64_t>(0x0000000000001000ull)
#define S390OpProp_Is64Bit                static_cast<uint64_t>(0x0000000000002000ull)
#define S390OpProp_Is32Bit                static_cast<uint64_t>(0x0000000000004000ull)
#define S390OpProp_Is32To64Bit            static_cast<uint64_t>(0x0000000000008000ull)
#define S390OpProp_IsCall                 static_cast<uint64_t>(0x0000000000010000ull)
#define S390OpProp_ReadsCC                static_cast<uint64_t>(0x0000000000020000ull)
#define S390OpProp_SetsCompareFlag        static_cast<uint64_t>(0x0000000000040000ull)
#define S390OpProp_SetsCC                 static_cast<uint64_t>(0x0000000000080000ull)
#define S390OpProp_LongDispSupported      static_cast<uint64_t>(0x0000000000100000ull)
#define S390OpProp_IsExtendedImmediate    static_cast<uint64_t>(0x0000000000200000ull)
#define S390OpProp_UsesRegPairForTarget   static_cast<uint64_t>(0x0000000000400000ull)
#define S390OpProp_UsesRegPairForSource   static_cast<uint64_t>(0x0000000000800000ull)
#define S390OpProp_UsesRegRangeForTarget  static_cast<uint64_t>(0x0000000001000000ull)
#define S390OpProp_IsRegCopy              static_cast<uint64_t>(0x0000000002000000ull)
#define S390OpProp_Trap                   static_cast<uint64_t>(0x0000000004000000ull)
#define S390OpProp_ReadsDFPRoundingMode   static_cast<uint64_t>(0x0000000008000000ull)
#define S390OpProp_SetsDFPRoundingMode    static_cast<uint64_t>(0x0000000010000000ull)
#define S390OpProp_ReadsFPC               static_cast<uint64_t>(0x0000000020000000ull)
#define S390OpProp_SetsFPC                static_cast<uint64_t>(0x0000000040000000ull)
#define S390OpProp_UsesImplicitRegisters  static_cast<uint64_t>(0x0000000080000000ull)
#define S390OpProp_TargetHW               static_cast<uint64_t>(0x0000000100000000ull)
#define S390OpProp_TargetLW               static_cast<uint64_t>(0x0000000200000000ull)
#define S390OpProp_SrcHW                  static_cast<uint64_t>(0x0000000400000000ull)
#define S390OpProp_SrcLW                  static_cast<uint64_t>(0x0000000800000000ull)
#define S390OpProp_Src2HW                 static_cast<uint64_t>(0x0000001000000000ull)
#define S390OpProp_Src2LW                 static_cast<uint64_t>(0x0000002000000000ull)
#define S390OpProp_HasTwoMemoryReferences static_cast<uint64_t>(0x0000004000000000ull)
#define S390OpProp_ImplicitlyUsesGPR0     static_cast<uint64_t>(0x0000008000000000ull)
#define S390OpProp_ImplicitlyUsesGPR1     static_cast<uint64_t>(0x0000010000000000ull)
#define S390OpProp_ImplicitlyUsesGPR2     static_cast<uint64_t>(0x0000020000000000ull)
#define S390OpProp_ImplicitlySetsGPR0     static_cast<uint64_t>(0x0000040000000000ull)
#define S390OpProp_ImplicitlySetsGPR1     static_cast<uint64_t>(0x0000080000000000ull)
#define S390OpProp_ImplicitlySetsGPR2     static_cast<uint64_t>(0x0000100000000000ull)
#define S390OpProp_IsCompare              static_cast<uint64_t>(0x0000200000000000ull)
#define S390OpProp_MayUseRegPairForTarget static_cast<uint64_t>(0x0000400000000000ull)
#define S390OpProp_IsYForm                static_cast<uint64_t>(0x0000800000000000ull)
#define S390OpProp_IsImplicitLoad         static_cast<uint64_t>(0x0001000000000000ull)
#define S390OpProp_IsImplicitStore        static_cast<uint64_t>(0x0002000000000000ull)
#define S390OpProp_MemRefOneIsStore       static_cast<uint64_t>(0x0004000000000000ull)
#define S390OpProp_MemRefOneIsLoad        static_cast<uint64_t>(0x0008000000000000ull)
#define S390OpProp_MemRefTwoIsStore       static_cast<uint64_t>(0x0010000000000000ull)
#define S390OpProp_MemRefTwoIsLoad        static_cast<uint64_t>(0x0020000000000000ull)
#define S390OpProp_SSSchedulingCandidate  static_cast<uint64_t>(0x0040000000000000ull)
#define S390OpProp_IsExecutionHint        static_cast<uint64_t>(0x0080000000000000ull)
#define S390OpProp_SetsOperand1           static_cast<uint64_t>(0x0100000000000000ull)
#define S390OpProp_SetsOperand2           static_cast<uint64_t>(0x0200000000000000ull)
#define S390OpProp_SetsOperand3           static_cast<uint64_t>(0x0400000000000000ull)
#define S390OpProp_SetsOperand4           static_cast<uint64_t>(0x0800000000000000ull)
#define S390OpProp_ImplicitlySetsGPR3     static_cast<uint64_t>(0x1000000000000000ull)
#define S390OpProp_ImplicitlySetsGPR4     static_cast<uint64_t>(0x2000000000000000ull)
#define S390OpProp_ImplicitlySetsGPR5     static_cast<uint64_t>(0x4000000000000000ull)
#define S390OpProp_IsSignedImmediate      static_cast<uint64_t>(0x8000000000000000ull)

/* Instruction Properties 2 (One hot encoding) */
#define S390OpProp2_UsesM3                static_cast<uint64_t>(0x0000000000000001ull)
#define S390OpProp2_UsesM4                static_cast<uint64_t>(0x0000000000000002ull)
#define S390OpProp2_UsesM5                static_cast<uint64_t>(0x0000000000000004ull)
#define S390OpProp2_UsesM6                static_cast<uint64_t>(0x0000000000000008ull)
#define S390OpProp2_HasExtendedMnemonic   static_cast<uint64_t>(0x0000000000000010ull)
#define S390OpProp2_VectorStringOp        static_cast<uint64_t>(0x0000000000000020ull)
#define S390OpProp2_VectorFPOp            static_cast<uint64_t>(0x0000000000000040ull)

class InstOpCode: public OMR::InstOpCode
   {
   protected:

   InstOpCode():  OMR::InstOpCode(BAD)  {}
   InstOpCode(Mnemonic m): OMR::InstOpCode(m)  {}

   public:

   enum S390BranchCondition
      {
      COND_NOPR,
      COND_JNOP,
      COND_BRO,
      COND_JO,
      COND_BRH,
      COND_JH,
      COND_BRP,
      COND_JP,
      COND_BRL,
      COND_JL,
      COND_BRM,
      COND_JM,
      COND_BRNE,
      COND_JNE,
      COND_BRNZ,
      COND_JNZ,
      COND_BRE,
      COND_JE,
      COND_BRZ,
      COND_JZ,
      COND_BRNH,
      COND_JNH,
      COND_BRNL,
      COND_JNL,
      COND_BRNM,
      COND_JNM,
      COND_BRNP,
      COND_JNP,
      COND_BRNO,
      COND_JNO,
      COND_B,
      COND_BR,
      COND_BRU,
      COND_J,
      COND_BRUL,
      COND_JLU,
      COND_BC,
      COND_BCR,
      COND_BCRC,
      COND_BE,
      COND_BER,
      COND_BERC,
      COND_BH,
      COND_BHR,
      COND_BHRC,
      COND_BL,
      COND_BLR,
      COND_BLRC,
      COND_BM,
      COND_BMR,
      COND_BMRC,
      COND_BNANRC,
      COND_BNCRC,
      COND_BNE,
      COND_BNER,
      COND_BNERC,
      COND_BNH,
      COND_BNHR,
      COND_BNHRC,
      COND_BNL,
      COND_BNLR,
      COND_BNLRC,
      COND_BNM,
      COND_BNMR,
      COND_BNMRC,
      COND_BNNANRC,
      COND_BNO,
      COND_BNOR,
      COND_BNORC,
      COND_BNP,
      COND_BNPR,
      COND_BNPRC,
      COND_BNZ,
      COND_BNZR,
      COND_BNZRC,
      COND_BNZORC,
      COND_BO,
      COND_BOR,
      COND_BORC,
      COND_BP,
      COND_BPR,
      COND_BPRC,
      COND_BRC,
      COND_BZ,
      COND_BZR,
      COND_BZRC,
      COND_NOP,
      COND_VGNOP,
      COND_MASK0,
      COND_MASK1,
      COND_MASK2,
      COND_MASK3,
      COND_MASK4,
      COND_MASK5,
      COND_MASK6,
      COND_MASK7,
      COND_MASK8,
      COND_MASK9,
      COND_MASK10,
      COND_MASK11,
      COND_MASK12,
      COND_MASK13,
      COND_MASK14,
      COND_MASK15,
      COND_CC0 = COND_MASK8,
      COND_CC1 = COND_MASK4,
      COND_CC2 = COND_MASK2,
      COND_CC3 = COND_MASK1,
      lastBranchCondition = COND_MASK15,
      S390NumBranchConditions = lastBranchCondition +1
      };

   typedef struct
         {
         uint8_t bytes[2];
         uint8_t instructionFormat;
         } OpCodeBinaryEntry;


   /* Static tables(array) that uses the OpCode as the index */
   static const uint64_t            properties[NumOpCodes];
   static const uint64_t            properties2[NumOpCodes];
   static const OpCodeBinaryEntry   binaryEncodings[NumOpCodes];
   static const char *              opCodeToNameMap[NumOpCodes];


   uint8_t getFirstByte(){return binaryEncodings[_mnemonic].bytes[0];}
   uint8_t getSecondByte() {return binaryEncodings[_mnemonic].bytes[1];}


   /* Queries for instruction properties */
   uint32_t hasBypass();
   uint32_t isAdmin();
   uint32_t isHighWordInstruction();
   uint64_t isOperandHW(uint32_t i);
   uint64_t isOperandLW(uint32_t i);
   uint64_t setsOperand(uint32_t opNum);

   uint64_t singleFPOp() {return properties[_mnemonic] & S390OpProp_SingleFP;}
   uint64_t doubleFPOp() {return properties[_mnemonic] & S390OpProp_DoubleFP;}
   uint64_t gprOp() {return (properties[_mnemonic] & (S390OpProp_DoubleFP | S390OpProp_SingleFP)) == 0;}
   uint64_t fprOp() {return properties[_mnemonic] & (S390OpProp_DoubleFP | S390OpProp_SingleFP);}

   uint64_t isBranchOp() {return properties[_mnemonic] & S390OpProp_BranchOp;}
   uint64_t isTrap() {return (properties[_mnemonic] & S390OpProp_Trap)!=0;}
   uint64_t isLoad() {return properties[_mnemonic] & S390OpProp_IsLoad;}
   uint64_t isStore() {return properties[_mnemonic] & S390OpProp_IsStore;}
   uint64_t isCall() {return properties[_mnemonic] & S390OpProp_IsCall;}
   uint64_t isCompare() {return properties[_mnemonic] & S390OpProp_IsCompare;}
   uint64_t isExtendedImmediate() {return properties[_mnemonic] & S390OpProp_IsExtendedImmediate;}
   uint64_t isSignedImmediate() {return properties[_mnemonic] & S390OpProp_IsSignedImmediate;}
   uint64_t isTargetHW() {return properties[_mnemonic] & S390OpProp_TargetHW;}
   uint64_t isTargetLW() {return properties[_mnemonic] & S390OpProp_TargetLW;}
   uint64_t isSrcHW() {return properties[_mnemonic] & S390OpProp_SrcHW;}
   uint64_t isSrcLW() {return properties[_mnemonic] & S390OpProp_SrcLW;}
   uint64_t isSrc2HW() {return properties[_mnemonic] & S390OpProp_Src2HW;}
   uint64_t isSrc2LW() {return properties[_mnemonic] & S390OpProp_Src2LW;}
   uint64_t usesTarget() {return properties[_mnemonic] & S390OpProp_UsesTarget;}

   uint64_t is64bit() {return properties[_mnemonic] & S390OpProp_Is64Bit;}
   uint64_t is32bit() {return properties[_mnemonic] & S390OpProp_Is32Bit;}
   uint64_t is32to64bit() {return properties[_mnemonic] & S390OpProp_Is32To64Bit;}

   uint64_t usesImplicitRegisters() { return properties[_mnemonic] & S390OpProp_UsesImplicitRegisters;}
   uint64_t hasLongDispSupport() {return properties[_mnemonic] & S390OpProp_LongDispSupported;}
   uint64_t usesRegPairForTarget() {return properties[_mnemonic] & S390OpProp_UsesRegPairForTarget; }
   uint64_t mayUseRegPairForTarget() { return properties[_mnemonic] & S390OpProp_MayUseRegPairForTarget; }
   uint64_t usesRegPairForSource() {return properties[_mnemonic] & S390OpProp_UsesRegPairForSource; }
   uint64_t usesRegRangeForTarget(){return properties[_mnemonic] & S390OpProp_UsesRegRangeForTarget; }
   uint64_t canUseRegPairForTarget() {return usesRegPairForTarget() || usesRegRangeForTarget() || mayUseRegPairForTarget(); }
   uint64_t shouldUseRegPairForTarget() {return usesRegPairForTarget(); }

   uint64_t implicitlyUsesGPR0() { return properties[_mnemonic] & S390OpProp_ImplicitlyUsesGPR0; }
   uint64_t implicitlyUsesGPR1() { return properties[_mnemonic] & S390OpProp_ImplicitlyUsesGPR1; }
   uint64_t implicitlyUsesGPR2() { return properties[_mnemonic] & S390OpProp_ImplicitlyUsesGPR2; }

   uint64_t implicitlySetsGPR0() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR0; }
   uint64_t implicitlySetsGPR1() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR1; }
   uint64_t implicitlySetsGPR2() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR2; }
   uint64_t implicitlySetsGPR3() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR3; }
   uint64_t implicitlySetsGPR4() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR4; }
   uint64_t implicitlySetsGPR5() { return properties[_mnemonic] & S390OpProp_ImplicitlySetsGPR5; }

   uint64_t setsOperand1() { return properties[_mnemonic] & S390OpProp_SetsOperand1; }
   uint64_t setsOperand2() { return properties[_mnemonic] & S390OpProp_SetsOperand2; }
   uint64_t setsOperand3() { return properties[_mnemonic] & S390OpProp_SetsOperand3; }
   uint64_t setsOperand4() { return properties[_mnemonic] & S390OpProp_SetsOperand4; }

   uint64_t setsCC() {return (setsZeroFlag() || setsSignFlag() || setsOverflowFlag() || setsCompareFlag() || setsCarryFlag() || (properties[_mnemonic] & S390OpProp_SetsCC));}
   uint64_t readsCC() {return properties[_mnemonic] & S390OpProp_ReadsCC;}

   uint64_t readsMultipleRegs() {return properties[_mnemonic] & S390OpProp_ReadsMultipleRegs;}
   uint64_t setsMultipleRegs() {return properties[_mnemonic] & S390OpProp_SetsMultipleRegs;}
   uint64_t setsZeroFlag() {return properties[_mnemonic] & S390OpProp_SetsZeroFlag;}
   uint64_t setsSignFlag() {return properties[_mnemonic] & S390OpProp_SetsSignFlag;}
   uint64_t setsCarryFlag() {return properties[_mnemonic] & S390OpProp_SetsCarryFlag;}
   uint64_t setsOverflowFlag() {return properties[_mnemonic] & S390OpProp_SetsOverflowFlag;}
   uint64_t setsCompareFlag() {return properties[_mnemonic] & S390OpProp_SetsCompareFlag;}

   uint64_t isRegCopy() {return properties[_mnemonic] & S390OpProp_IsRegCopy; }
   uint64_t hasTwoMemoryReferences() {return properties[_mnemonic] & S390OpProp_HasTwoMemoryReferences;}

   uint64_t readsDFPRoundingMode() {return properties[_mnemonic] & S390OpProp_ReadsDFPRoundingMode; }
   uint64_t setsDFPRoundingMode() {return properties[_mnemonic] & S390OpProp_SetsDFPRoundingMode; }

   uint64_t readsFPC() {return properties[_mnemonic] & S390OpProp_ReadsFPC; }
   uint64_t setsFPC() {return properties[_mnemonic] & S390OpProp_SetsFPC; }

   uint64_t mr1IsStore()   {return properties[_mnemonic] & S390OpProp_MemRefOneIsStore;}
   uint64_t mr1IsLoad()    {return properties[_mnemonic] & S390OpProp_MemRefOneIsLoad;}
   uint64_t mr2IsStore()   {return properties[_mnemonic] & S390OpProp_MemRefTwoIsStore;}
   uint64_t mr2IsLoad()    {return properties[_mnemonic] & S390OpProp_MemRefTwoIsLoad;}

   uint64_t isSSSchedulingCandidate()  {return properties[_mnemonic] & S390OpProp_SSSchedulingCandidate;}
   uint64_t isYForm() {return properties[_mnemonic] & S390OpProp_IsYForm; }
   uint64_t isImplicitLoad()  {return properties[_mnemonic] & S390OpProp_IsImplicitLoad; }   // like CLCL R1,R2
   uint64_t isImplicitStore() {return properties[_mnemonic] & S390OpProp_IsImplicitStore; }  // like MVCL R1,R2

   uint64_t isLabel() {return _mnemonic == LABEL;}
   uint64_t isBeginBlock() {return _mnemonic == LABEL;}

   uint64_t usesM3() {return properties2[_mnemonic] & S390OpProp2_UsesM3;}
   uint64_t usesM4() {return properties2[_mnemonic] & S390OpProp2_UsesM4;}
   uint64_t usesM5() {return properties2[_mnemonic] & S390OpProp2_UsesM5;}
   uint64_t usesM6() {return properties2[_mnemonic] & S390OpProp2_UsesM6;}
   uint64_t hasExtendedMnemonic() {return properties2[_mnemonic] & S390OpProp2_HasExtendedMnemonic;}
   uint64_t isVectorStringOp() {return properties2[_mnemonic] & S390OpProp2_VectorStringOp;}
   uint64_t isVectorFPOp() {return properties2[_mnemonic] & S390OpProp2_VectorFPOp;}

   /* Static */
   static void copyBinaryToBufferWithoutClear(uint8_t *cursor, Mnemonic i_opCode);
   static void copyBinaryToBuffer(uint8_t *cursor, Mnemonic i_opCode);
   static uint8_t getInstructionLength(Mnemonic i_opCode); /* Using a table that maps values of bit 0-1 to instruction length from POP chapter 5,  topic: Instruction formats (page 5.5)*/
   static uint8_t  getInstructionFormat(Mnemonic i_opCode)  { return binaryEncodings[i_opCode].instructionFormat; }
   static const uint8_t * getOpCodeBinaryRepresentation(Mnemonic i_opCode) { return binaryEncodings[i_opCode].bytes ;}


   /* Non-Static methods that calls static implementations */
   void copyBinaryToBufferWithoutClear(uint8_t *cursor){ copyBinaryToBufferWithoutClear(cursor, _mnemonic); }
   void copyBinaryToBuffer(uint8_t *cursor){ copyBinaryToBuffer(cursor, _mnemonic); }
   uint8_t getInstructionLength() { return getInstructionLength(_mnemonic);}
   uint8_t getInstructionFormat() { return getInstructionFormat(_mnemonic);}
   const uint8_t * getOpCodeBinaryRepresentation() { return getOpCodeBinaryRepresentation(_mnemonic);}


   /* Static methods for getting the correct opCode depending on platform (32bit, 64bit, etc.) */
   static Mnemonic getLoadOnConditionRegOpCode();
   static Mnemonic getLoadAddressOpCode();
   static Mnemonic getLoadOpCode();
   static Mnemonic getLoadAndMaskOpCode();
   static Mnemonic getLoadOpCodeFromNode(TR::Node *node);
   static Mnemonic getExtendedLoadOpCode();
   static Mnemonic getExtendedLoadOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadRegOpCode();
   static Mnemonic getLoadRegOpCodeFromNode(TR::CodeGenerator *cg, TR::Node *node);
   static Mnemonic getLoadTestRegOpCode();
   static Mnemonic getLoadTestRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadComplementOpCode();
   static Mnemonic getLoadComplementOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadHalfWordOpCode();
   static Mnemonic getLoadHalfWordOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadHalfWordImmOpCode();
   static Mnemonic getLoadHalfWordImmOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadPositiveOpCode();
   static Mnemonic getLoadPositiveOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadMultipleOpCode();
   static Mnemonic getLoadNegativeOpCode();
   static Mnemonic getLoadNegativeOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadAndTrapOpCode();
   static Mnemonic getExtendedStoreOpCode();
   static Mnemonic getExtendedStoreOpCodeFromNode(TR::Node *node);
   static Mnemonic getStoreOpCode();
   static Mnemonic getStoreOpCodeFromNode(TR::Node *node);
   static Mnemonic getStoreMultipleOpCode();
   static Mnemonic getCmpHalfWordImmOpCode();
   static Mnemonic getCmpHalfWordImmOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpHalfWordImmToMemOpCode();
   static Mnemonic getCmpHalfWordImmToMemOpCodeFromNode(TR::Node *node);
   static Mnemonic getAndRegOpCode();
   static Mnemonic getAndRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getAndOpCode();
   static Mnemonic getAndOpCodeFromNode(TR::Node *node);
   static Mnemonic getBranchOnCountOpCode();
   static Mnemonic getBranchOnCountOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddOpCode();
   static Mnemonic getAddOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddRegOpCode();
   static Mnemonic getAddRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddThreeRegOpCode();
   static Mnemonic getAddLogicalThreeRegOpCode();
   static Mnemonic getAddLogicalRegRegImmediateOpCode();
   static Mnemonic getSubstractOpCode();
   static Mnemonic getSubstractOpCodeFromNode(TR::Node *node);
   static Mnemonic getSubstractRegOpCode();
   static Mnemonic getSubstractRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getSubtractThreeRegOpCode();
   static Mnemonic getSubtractThreeRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getSubtractLogicalImmOpCode();
   static Mnemonic getSubtractLogicalImmOpCodeFromNode(TR::Node *node);
   static Mnemonic getSubtractLogicalThreeRegOpCode();
   static Mnemonic getMultiplySingleOpCode();
   static Mnemonic getMultiplySingleOpCodeFromNode(TR::Node *node);
   static Mnemonic getMultiplySingleRegOpCode();
   static Mnemonic getMultiplySingleRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getOrOpCode();
   static Mnemonic getOrOpCodeFromNode(TR::Node *node);
   static Mnemonic getOrRegOpCode();
   static Mnemonic getOrRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getOrThreeRegOpCode();
   static Mnemonic getXOROpCode();
   static Mnemonic getXOROpCodeFromNode(TR::Node *node);
   static Mnemonic getXORRegOpCode();
   static Mnemonic getXORRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getXORThreeRegOpCode();
   static Mnemonic getCmpTrapOpCode();
   static Mnemonic getCmpTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpWidenTrapOpCode();
   static Mnemonic getCmpWidenTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpImmOpCode();
   static Mnemonic getCmpImmTrapOpCode();
   static Mnemonic getCmpImmTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalTrapOpCode();
   static Mnemonic getCmpLogicalTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalWidenTrapOpCode();
   static Mnemonic getCmpLogicalWidenTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalImmTrapOpCode();
   static Mnemonic getCmpLogicalImmTrapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpOpCode();
   static Mnemonic getCmpOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpRegOpCode();
   static Mnemonic getCmpRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalOpCode();
   static Mnemonic getCmpLogicalOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalRegOpCode();
   static Mnemonic getCmpLogicalRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpAndSwapOpCode();
   static Mnemonic getCmpAndSwapOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalImmOpCode();
   static Mnemonic getCmpLogicalImmOpCodeFromNode(TR::Node *node);
   static Mnemonic getShiftLeftLogicalSingleOpCode();
   static Mnemonic getShiftRightLogicalSingleOpCode();
   static Mnemonic getShiftLeftLogicalSingleOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddHalfWordImmOpCode();
   static Mnemonic getAddHalfWordImmOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddLogicalOpCode();
   static Mnemonic getAddLogicalOpCodeFromNode(TR::Node *node);
   static Mnemonic getAddLogicalRegOpCode();
   static Mnemonic getAddLogicalRegOpCodeFromNode(TR::Node *node);
   static Mnemonic getBranchOnIndexHighOpCode();
   static Mnemonic getBranchOnIndexEqOrLowOpCode();
   static Mnemonic getBranchRelIndexHighOpCode();
   static Mnemonic getBranchRelIndexEqOrLowOpCode();
   static Mnemonic getMoveHalfWordImmOpCodeFromStoreOpCode(Mnemonic storeOpCode);

   /*
    * Define 32->64 bit widening instructions on 64 bit architecture,
    * on 32 bit architecture, they are just regular instructions.
    */
   static Mnemonic getLoadWidenOpCode();
   static Mnemonic getLoadRegWidenOpCode();
   static Mnemonic getLoadTestRegWidenOpCode();
   static Mnemonic getAddWidenOpCode();
   static Mnemonic getAddRegWidenOpCode();
   static Mnemonic getSubstractWidenOpCode();
   static Mnemonic getSubStractRegWidenOpCode();
   static Mnemonic getCmpWidenOpCode();
   static Mnemonic getCmpWidenOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpRegWidenOpCode();
   static Mnemonic getCmpRegWidenOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalWidenOpCode();
   static Mnemonic getCmpLogicalWidenOpCodeFromNode(TR::Node *node);
   static Mnemonic getCmpLogicalRegWidenOpCode();
   static Mnemonic getCmpLogicalRegWidenOpCodeFromNode(TR::Node *node);
   static Mnemonic getLoadComplementRegWidenOpCode();
   static Mnemonic getLoadPositiveRegWidenOpCode();
   static Mnemonic getSubtractWithBorrowOpCode();

   /*  Define Golden Eagle instructions   */
   static Mnemonic getLoadTestOpCode();

   /*  Define z6 instructions             */
   static Mnemonic getStoreRelativeLongOpCode();
   static Mnemonic getLoadRelativeLongOpCode();
   static Mnemonic getMoveHalfWordImmOpCode();


   };

}

}


#endif /* OMR_S390_ARCH_INSTRUCTION_OPCODE_BASE_INCL */
