/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2016, 2016
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
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <errno.h>

#include "Jit.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/MethodBuilder.hpp"
#include "DotProduct.hpp"

static void printString(int64_t ptr)
   {
   #define PRINTSTRING_LINE LINETOSTR(__LINE__)
   char *str = (char *) ptr;
   printf("%s", str);
   }

static void printInt32(int32_t val)
   {
   #define PRINTINT32_LINE LINETOSTR(__LINE__)
   printf("%d", val);
   }

static void printDouble(double val)
   {
   #define PRINTDOUBLE_LINE LINETOSTR(__LINE__)
   printf("%lf", val);
   }

static void printPointer(int64_t val)
   {
   #define PRINTPOINTER_LINE LINETOSTR(__LINE__)
   printf("%llx", val);
   }

DotProduct::DotProduct(TR::TypeDictionary *types)
   : MethodBuilder(types)

   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("dotproduct");

   pDouble = types->PointerTo(Double);

   DefineParameter("result", pDouble);
   DefineParameter("vector1", pDouble);
   DefineParameter("vector2", pDouble);
   DefineParameter("length", Int32);

   DefineReturnType(NoType);

   DefineFunction((char *)"printString", 
                  (char *)__FILE__,
                  (char *)PRINTSTRING_LINE,
                  (void *)&printString,
                  NoType,
                  1,
                  Int64);
   DefineFunction((char *)"printInt32", 
                  (char *)__FILE__,
                  (char *)PRINTINT32_LINE,
                  (void *)&printInt32,
                  NoType,
                  1,
                  Int32);
   DefineFunction((char *)"printDouble", 
                  (char *)__FILE__,
                  (char *)PRINTDOUBLE_LINE,
                  (void *)&printDouble,
                  NoType,
                  1,
                  Double);
   DefineFunction((char *)"printPointer", 
                  (char *)__FILE__,
                  (char *)PRINTPOINTER_LINE,
                  (void *)&printPointer,
                  NoType,
                  1,
                  Int64);
   }

void
DotProduct::PrintString(TR::IlBuilder *bldr, const char *s)
   {
   bldr->Call("printString", 1,
   bldr->   ConstInt64((int64_t)(char *)s));
   }

bool
DotProduct::buildIL()
   {
   PrintString(this, "dotproduct parameters:\n");

   PrintString(this, "   result is ");
   Call("printPointer", 1,
      Load("result"));
   PrintString(this, "\n");

   PrintString(this, "   vector1 is ");
   Call("printPointer", 1,
      Load("vector1"));
   PrintString(this, "\n");

   PrintString(this, "   vector2 is ");
   Call("printPointer", 1,
      Load("vector2"));
   PrintString(this, "\n");

   TR::IlBuilder *loop = NULL;
   ForLoopUp("i", &loop,
      ConstInt32(0),
      Load("length"),
      ConstInt32(1));

   loop->StoreAt(
   loop->   IndexAt(pDouble,
   loop->      Load("result"),
   loop->      Load("i")),
   loop->   Mul(
   loop->      LoadAt(pDouble,
   loop->         IndexAt(pDouble,
   loop->            Load("vector1"),
   loop->            Load("i"))),
   loop->      LoadAt(pDouble,
   loop->         IndexAt(pDouble,
   loop->            Load("vector2"),
   loop->            Load("i")))));

   Return();

   return true;
   }


int
main(int argc, char *argv[])
   {
   printf("Step 1: initialize JIT\n");
   bool initialized = initializeJit();
   if (!initialized)
      {
      fprintf(stderr, "FAIL: could not initialize JIT\n");
      exit(-1);
      }

   printf("Step 2: define type dictionary\n");
   TR::TypeDictionary types;

   printf("Step 3: compile method builder\n");
   DotProduct method(&types);
   uint8_t *entry=0;
   int32_t rc = compileMethodBuilder(&method, &entry);
   if (rc != 0)
      {
      fprintf(stderr,"FAIL: compilation error %d\n", rc);
      exit(-2);
      }

   printf("Step 4: define values\n");
   double result[10] = { 0 };
   double values1[10] = { 1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5 };
   double values2[10] = { 10.5,9.5,8.5,7.5,6.5,5.5,4.5,3.5,2.5,1.5 };

   printf("Step 5: invoke compiled code and verify results\n");
   DotProductFunctionType *test = (DotProductFunctionType *)entry;
   test(result, values1, values2, 10);

   printf("result = [\n");
   for (int32_t i=0;i < 10;i++)
      printf("           %lf\n", result[i]);
   printf("         ]\n\n");

   printf ("Step 6: shutdown JIT\n");
   shutdownJit();

   printf("PASS\n");
   }
