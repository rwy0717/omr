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

#include <stdint.h>                                 // for int32_t
#include <string.h>                                 // for NULL, memset
#include "compile/Compilation.hpp"
#include "compile/Method.hpp"                       // for HIGH_VISIT_COUNT
#include "control/Options.hpp"
#include "control/Options_inlines.hpp"
#include "cs2/bitvectr.h"
#include "cs2/tableof.h"                            // for TableOf, etc
#include "env/TRMemory.hpp"                         // for BitVector, etc
#include "il/Block.hpp"                             // for Block, toBlock
#include "il/Node.hpp"                              // for Node, etc
#include "il/TreeTop.hpp"                           // for TreeTop
#include "il/TreeTop_inlines.hpp"                   // for TreeTop::getNode, etc
#include "infra/Assert.hpp"                         // for TR_ASSERT
#include "infra/Cfg.hpp"                            // for CFG
#include "infra/Link.hpp"                           // for TR_LinkHead
#include "infra/List.hpp"
#include "infra/TRCfgEdge.hpp"                      // for CFGEdge
#include "infra/TRCfgNode.hpp"                      // for CFGNode
#include "optimizer/Structure.hpp"
#include "optimizer/DataFlowAnalysis.hpp"

class TR_BitVector;

static int32_t numIterations = 0;

// This file contains the methods specifying the forward
// bit vector analysis rules for each structure. Also contain
// definitions of other methods in class BitVectorAnalysis.
//
//
template<class Container>
bool
TR_BasicDFSetAnalysis<Container *>::
supportsGenAndKillSets()
   {
   return false;
   }

template<class Container>
void
TR_BasicDFSetAnalysis<Container *>::
initializeGenAndKillSetInfo()
   {
   }

template<class Container>
TR_DataFlowAnalysis::Kind
TR_BasicDFSetAnalysis<Container *>::
getKind()
   {
   return BasicDFSetAnalysis;
   }

// Perform the data flow analysis, including initialization
template<class Container>
bool
TR_BasicDFSetAnalysis<Container *>::
performAnalysis(TR_Structure *rootStructure,
                bool checkForChanges)
   {
   LexicalTimer tlex("basicDFSetAnalysis_pA", comp()->phaseTimer());
   //traceMsg(comp(), "DJS perform analysis %d, nodes = %d, bits = %d\n", this->getKind(), comp()->getFlowGraph()->getNextNodeNumber(), getNumberOfBits());
   //comp()->printMemStatsBefore("DJS - Before DFA");
   // Table of bit vectors to be used during the analysis.
   CS2::TableOf<TR::BitVector, TR::Allocator>
      bvTable(64, comp()->allocator());
   _bitVectorTable = &bvTable;
   rootStructure->resetAnalysisInfo();
   rootStructure->resetAnalyzedStatus();
   initializeDFSetAnalysis();
   //comp()->printMemStatsAfter("DJS - After initialize DFA");
   if (!postInitializationProcessing())
      return false;
   doAnalysis(rootStructure, checkForChanges);
   //rootStructure->resetAnalysisInfo();
   //rootStructure->resetAnalyzedStatus();
   //comp()->printMemStatsAfter("DJS - After DFA");
   return true;
   }

template<class Container>
TR::BitVector *
TR_BasicDFSetAnalysis<Container *>::
allocateBitVector()
   {
   CS2::TableIndex i = _bitVectorTable->AddEntry(*_bitVectorTable);
   return &_bitVectorTable->ElementAt(i);
   }

template<class Container>
void
TR_BasicDFSetAnalysis<Container *>::
allocateContainer(Container **result, bool, bool)
   {
   *result = new (trStackMemory()) Container(_numberOfBits, trMemory(), stackAlloc);
   }

template<class Container>
void
TR_BasicDFSetAnalysis<Container *>::
allocateBlockInfoContainer(Container **result, bool, bool)
   {
   *result =  new (trStackMemory()) Container(_numberOfBits, trMemory(), stackAlloc);
   }

template<class Container>
void
TR_BasicDFSetAnalysis<Container *>::
allocateBlockInfoContainer(Container **result, Container*)
   {
   *result =  new (trStackMemory()) Container(_numberOfBits, trMemory(), stackAlloc);
   }

template<class Container>
void
TR_BasicDFSetAnalysis<Container *>::
allocateTempContainer(Container **result, Container*)
   {
   *result =  new (trStackMemory()) Container(_numberOfBits, trMemory(), stackAlloc);
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeBlockInfo(bool allocateLater)
   {
   if (_blockAnalysisInfo)
      return;

   _numberOfNodes = _cfg->getNextNodeNumber();
   TR_ASSERT(_numberOfNodes > 0, "BVA, node numbers not assigned");

   if (_numberOfBits == Container::nullContainerCharacteristic)
      _numberOfBits = getNumberOfBits();

   _blockAnalysisInfo = (Container **)trMemory()->allocateStackMemory(_numberOfNodes*sizeof(Container *));

   if (allocateLater)
      {
      memset(_blockAnalysisInfo, 0, _numberOfNodes*sizeof(Container *));
      }
   else
      {
      for (int32_t i=0;i<_numberOfNodes;i++)
         this->allocateBlockInfoContainer(&_blockAnalysisInfo[i]);
      }
   }


template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeBasicDFSetAnalysis()
   {
   if (_blockAnalysisInfo == NULL)
      initializeBlockInfo();

   _hasImproperRegion = _cfg->getStructure()->markStructuresWithImproperRegions();

   if (comp()->getMethodSymbol()->mayHaveNestedLoops() &&
       !comp()->getOption(TR_DisableNewBVA))
      {
      _hasImproperRegion = false; // Probably use a run time option here to enable old flow analysis behav
      }
   else
      _hasImproperRegion = true;

   if (comp()->getVisitCount() > HIGH_VISIT_COUNT)
      {
      comp()->resetVisitCounts(1);
      dumpOptDetails(comp(), "\nResetting visit counts for this method before bit vector analysis\n");
      }

   this->allocateContainer(&_regularInfo);
   this->allocateContainer(&_exceptionInfo);
   this->allocateContainer(&_temp);
   this->allocateContainer(&_temp2);
   _nodesInCycle = allocateBitVector();

   if (supportsGenAndKillSets())
      {
      int32_t arraySize = _numberOfNodes*sizeof(Container*);
      _regularGenSetInfo  = (Container**)trMemory()->allocateStackMemory(arraySize);
      memset(_regularGenSetInfo, 0, arraySize);
      _regularKillSetInfo = (Container**)trMemory()->allocateStackMemory(arraySize);
      memset(_regularKillSetInfo, 0, arraySize);
      _exceptionGenSetInfo  = (Container**)trMemory()->allocateStackMemory(arraySize);
      memset(_exceptionGenSetInfo, 0, arraySize);
      _exceptionKillSetInfo = (Container**)trMemory()->allocateStackMemory(arraySize);
      memset(_exceptionKillSetInfo, 0, arraySize);

      initializeGenAndKillSetInfo();

      if (!_hasImproperRegion)
         {
         initializeGenAndKillSetInfoForStructures();
         if (traceBVA())
            dumpOptDetails(comp(), "\n ************** Completed initialization of gen and kill sets for all structures ************* \n");
         }
      }
   else
      {
      _regularGenSetInfo  = NULL;
      _regularKillSetInfo = NULL;
      _exceptionGenSetInfo  = NULL;
      _exceptionKillSetInfo = NULL;
      }

  _cfg->getStructure()->resetAnalyzedStatus();

  if (comp()->getVisitCount() > HIGH_VISIT_COUNT)
      {
      comp()->resetVisitCounts(1);
      dumpOptDetails(comp(), "\nResetting visit counts for this method before bit vector analysis\n");
      }
   }



template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeGenAndKillSetInfoForStructures()
   {
   initializeGenAndKillSetInfoPropertyForStructure(_cfg->getStructure(), false);
   initializeGenAndKillSetInfoForStructure(_cfg->getStructure());
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeGenAndKillSetInfoForStructure(TR_Structure *s)
   {
   TR_RegionStructure *region = s->asRegion();
   if (region)
      {
      if (region->containsImproperRegion() ||
          !canGenAndKillForStructure(region))
         {
         TR_RegionStructure::Cursor si(*region);
         TR_StructureSubGraphNode *subNode;
         for (subNode = si.getCurrent(); subNode; subNode = si.getNext())
            {
            addToAnalysisQueue(subNode, 0);
            initializeGenAndKillSetInfoForStructure(subNode->getStructure());
            }

         typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = getAnalysisInfo(s);
         }
      else
         {
         initializeGenAndKillSetInfoForRegion(region);
         }
      }
   else if (!s->containsImproperRegion())
      initializeGenAndKillSetInfoForBlock(s->asBlock());
   }


template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeGenAndKillSetInfoPropertyForStructure(TR_Structure *s, bool inLoop)
   {
   bool b = canGenAndKillForStructure(s);
   TR_RegionStructure *region = s->asRegion();
   if (region)
      {
      if (region->isNaturalLoop())
         inLoop = true;
      }

   if (!inLoop)
      s->setContainsImproperRegion(true);

   if (region)
      {
      TR_RegionStructure::Cursor si(*region);
      TR_StructureSubGraphNode *subNode;
      for (subNode = si.getCurrent(); subNode; subNode = si.getNext())
         {
         initializeGenAndKillSetInfoPropertyForStructure(subNode->getStructure(), inLoop);
         }
      }
   }


template<class Container>bool TR_ForwardDFSetAnalysis<Container *>::canGenAndKillForStructure(TR_Structure *nodeStructure)
   {
   if (!this->supportsGenAndKillSetsForStructures())
      return false;

   if (this->_hasImproperRegion ||
       nodeStructure->containsImproperRegion())
      return false;

   TR_RegionStructure *naturalLoop = nodeStructure->asRegion();

   if (naturalLoop && naturalLoop->isNaturalLoop())
      {
      TR_StructureSubGraphNode *entryNode = naturalLoop->getEntry();

     TR::BitVector seenExitNodes(this->comp()->allocator());
     ListIterator<TR::CFGEdge> ei(&naturalLoop->getExitEdges());
     TR::CFGEdge *edge;
     for (edge = ei.getCurrent(); edge != NULL; edge = ei.getNext())
        {
        TR::CFGNode *toNode = edge->getTo();

        if (seenExitNodes.ValueAt(toNode->getNumber()))
           continue;

        seenExitNodes[toNode->getNumber()] = true;

        bool seenNonBackEdge = false;
        bool seenLoopBackEdge = false;

        TR::CFGEdgeList predList(toNode->getPredecessors());
        predList.insert(predList.end(), toNode->getExceptionPredecessors().begin(), toNode->getExceptionPredecessors().end());
        for (auto pred = predList.begin(); pred != predList.end(); ++pred)
           {
           TR::CFGNode *fromNode = (*pred)->getFrom();
           if (fromNode->hasSuccessor(entryNode) ||
               fromNode->hasExceptionSuccessor(entryNode))
              {
              if (seenNonBackEdge)
                 {
                 naturalLoop->setContainsImproperRegion(true);
                 break;
                 }
              seenLoopBackEdge = true;
              }
           else
              {
              if (seenLoopBackEdge)
                 {
                 naturalLoop->setContainsImproperRegion(true);
                 break;
                 }
              seenNonBackEdge = true;
              }
           }
        }

      if (naturalLoop->containsImproperRegion())
         {
         TR_Structure *parent = naturalLoop->getParent();
         while (parent)
            {
            parent->asRegion()->setContainsImproperRegion(true);
            parent = parent->getParent();
            }

         return false;
         }
      }


   return true;
   }


template<class Container>void TR_ForwardDFSetAnalysis<Container *>::initializeGenAndKillSetInfoForRegion(TR_RegionStructure *region)
   {
   TR::BitVector exitNodes(this->comp()->allocator());

   //
   // Allocate the storage for every exit out of the region
   //
   //
   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(region);
   if (!region->hasBeenAnalyzedBefore())
       region->setAnalyzedStatus(true);
   else
      return;

      {
      analysisInfo->_regularGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_regularKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_exceptionGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_exceptionKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_currentRegularGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_currentRegularKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_currentExceptionGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
      analysisInfo->_currentExceptionKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;

      ListIterator<TR::CFGEdge> ei(&region->getExitEdges());
      for (TR::CFGEdge *edge = ei.getCurrent(); edge != NULL; edge = ei.getNext())
         {
         int32_t toStructureNumber = edge->getTo()->getNumber();
         if (!exitNodes.ValueAt(toStructureNumber))
            {
            Container *b = NULL;

            typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_regularGenSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_regularKillSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_exceptionGenSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_exceptionKillSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_currentRegularGenSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_currentRegularKillSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_currentExceptionGenSetInfo->add(pair);

            pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            analysisInfo->_currentExceptionKillSetInfo->add(pair);

            exitNodes[toStructureNumber] = true;
            }
         }

      if (region->isNaturalLoop())
         {
         int32_t toStructureNumber = region->getNumber();

         Container *b = NULL;

         typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_regularGenSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_regularKillSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_exceptionGenSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_exceptionKillSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_currentRegularGenSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_currentRegularKillSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_currentExceptionGenSetInfo->add(pair);

         pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
         analysisInfo->_currentExceptionKillSetInfo->add(pair);
         }
      }


   TR::BitVector pendingList(this->comp()->allocator());

   // Set the pending list to be all of the region's subnodes.
   //
   TR_RegionStructure::Cursor si(*region);
   TR_StructureSubGraphNode *subNode;
   for (subNode = si.getCurrent(); subNode; subNode = si.getNext())
      pendingList[subNode->getNumber()] = true;

   int32_t numIterations = 1;

   this->_nodesInCycle->Clear();

   if (this->traceBVA())
      traceMsg(this->comp(), "\nGen : Analyzing REGION : %p NUMBER : %d ITERATION NUMBER : %d\n", region, region->getNumber(), numIterations);

   numIterations++;
   this->addToAnalysisQueue(region->getEntry(), 0);
   initializeGenAndKillSetInfo(region, pendingList);

   if (region->isNaturalLoop())
      {
      typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(region);
      Container *killBitVector = analysisInfo->getContainer(analysisInfo->_regularKillSetInfo, region->getNumber());
      Container *genBitVector = analysisInfo->getContainer(analysisInfo->_regularGenSetInfo, region->getNumber());
      Container *exceptionKillBitVector = analysisInfo->getContainer(analysisInfo->_exceptionKillSetInfo, region->getNumber());
      Container *exceptionGenBitVector = analysisInfo->getContainer(analysisInfo->_exceptionGenSetInfo, region->getNumber());
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
      if (analysisInfo->_regularGenSetInfo)
         {
         for (pair = analysisInfo->_regularGenSetInfo->getFirst(); pair; pair = pair->getNext())
            {
            if (pair->_container &&
                (pair->_nodeNumber != region->getNumber()))
               {
               this->_temp->empty();
               if (genBitVector)
                  *this->_temp |= *genBitVector;

               if (exceptionGenBitVector)
                  *this->_temp |= *exceptionGenBitVector;

               Container *killForThisExit = analysisInfo->getContainer(analysisInfo->_regularKillSetInfo, pair->_nodeNumber);
               if (killForThisExit)
                  *this->_temp -= *killForThisExit;

               Container *genForThisExit = analysisInfo->getContainer(analysisInfo->_regularGenSetInfo, pair->_nodeNumber);
               if (genForThisExit)
                  *this->_temp |= *genForThisExit;
               compose((pair->_container), this->_temp);
               }
            }
         }

      if (analysisInfo->_regularKillSetInfo)
         {
         for (pair = analysisInfo->_regularKillSetInfo->getFirst(); pair; pair = pair->getNext())
            {
            if (pair->_container &&
                (pair->_nodeNumber != region->getNumber()))
               {
               this->_temp->empty();
               if (killBitVector)
                  *this->_temp |= *killBitVector;
               if (exceptionKillBitVector)
                  *this->_temp |= *exceptionKillBitVector;

               Container *genForThisExit = analysisInfo->getContainer(analysisInfo->_regularGenSetInfo, pair->_nodeNumber);
               if (genForThisExit)
                  *this->_temp -= *genForThisExit;

               Container *killForThisExit = analysisInfo->getContainer(analysisInfo->_regularKillSetInfo, pair->_nodeNumber);
               if (killForThisExit)
                  *this->_temp |= *killForThisExit;
               inverseCompose((pair->_container), this->_temp);
               }
            }
         }

      if (analysisInfo->_exceptionGenSetInfo)
         {
         for (pair = analysisInfo->_exceptionGenSetInfo->getFirst(); pair; pair = pair->getNext())
            {
            if (pair->_container &&
                (pair->_nodeNumber != region->getNumber()))
               {
               this->_temp->empty();
               if (genBitVector)
                  *this->_temp |= *genBitVector;
               if (exceptionGenBitVector)
                  *this->_temp |= *exceptionGenBitVector;
               Container *killForThisExit = analysisInfo->getContainer(analysisInfo->_exceptionKillSetInfo, pair->_nodeNumber);
               if (killForThisExit)
                  *this->_temp -= *killForThisExit;
               Container *genForThisExit = analysisInfo->getContainer(analysisInfo->_exceptionGenSetInfo, pair->_nodeNumber);
               if (genForThisExit)
                  *this->_temp |= *genForThisExit;
               compose((pair->_container), this->_temp);
               }
            }
         }

      if (analysisInfo->_exceptionKillSetInfo)
         {
         for (pair = analysisInfo->_exceptionKillSetInfo->getFirst(); pair; pair = pair->getNext())
            {
            if (pair->_container &&
                (pair->_nodeNumber != region->getNumber()))
               {
               this->_temp->empty();
               if (killBitVector)
                  *this->_temp |= *killBitVector;
               if (exceptionKillBitVector)
                  *this->_temp |= *exceptionKillBitVector;

               Container *genForThisExit = analysisInfo->getContainer(analysisInfo->_exceptionGenSetInfo, pair->_nodeNumber);
               if (genForThisExit)
                  *this->_temp -= *genForThisExit;
               Container *killForThisExit = analysisInfo->getContainer(analysisInfo->_exceptionKillSetInfo, pair->_nodeNumber);
               if (killForThisExit)
                  *this->_temp |= *killForThisExit;
               inverseCompose((pair->_container), this->_temp);
               }
            }
         }
      }


   // Use pendingList to remember which exit nodes have already been seen
   // when merging the out information for the region.
   //
   pendingList.Clear();
   }




template<class Container>void TR_ForwardDFSetAnalysis<Container *>::initializeGenAndKillSetInfo(TR_RegionStructure *regionStructure, TR::BitVector &pendingList)
   {
   while (this->_analysisQueue.getListHead() &&
          (this->_analysisQueue.getListHead()->getData()->getStructure() != regionStructure))
      {
      if (!this->_analysisInterrupted)
   {
         numIterations++;
         if ((numIterations % 20) == 0)
      {
      numIterations = 0;
            if (this->comp()->compilationShouldBeInterrupted(FBVA_INITIALIZE_CONTEXT))
               this->_analysisInterrupted = true;
      }
   }

      if (this->_analysisInterrupted)
         {
         TR::Compilation *comp = this->comp();
         comp->failCompilation<TR::CompilationInterrupted>("interrupted in forward bit vector analysis");
         }

      TR::CFGNode *node = this->_analysisQueue.getListHead()->getData();
      TR_StructureSubGraphNode *nodeStructure = (TR_StructureSubGraphNode *) node;

      if (!pendingList.ValueAt(nodeStructure->getStructure()->getNumber()) &&
          (*(this->_changedSetsQueue.getListHead()->getData()) == 0))
         {
         this->removeHeadFromAnalysisQueue();
         continue;
         }

      if (this->traceBVA())
         traceMsg(this->comp(), "Gen : Begin analyzing node %p numbered %d in region %p (%d)\n", nodeStructure->getStructure(), node->getNumber(), regionStructure, regionStructure->getNumber());

      bool alreadyVisitedNode = false;
      if (this->_nodesInCycle->ValueAt(nodeStructure->getNumber()))
         alreadyVisitedNode = true;

      (*(this->_nodesInCycle))[nodeStructure->getNumber()] = true;

      if (node != regionStructure->getEntry())
         initializeCurrentGenKillSetInfo();

      typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(regionStructure);
      if (node != regionStructure->getEntry())
         {
         TR::CFGEdgeList predList(node->getPredecessors());
         predList.insert(predList.end(), node->getExceptionPredecessors().begin(), node->getExceptionPredecessors().end());
         bool firstPred = true;
         bool stopAnalyzingThisNode = false;
         int count = 0;
         for (auto pred = predList.begin(); pred != predList.end(); ++pred)
            {
            bool normalPred = (++count <= node->getPredecessors().size());
            TR_StructureSubGraphNode *predNode = (TR_StructureSubGraphNode *) (*pred)->getFrom();
            TR_Structure *predStructure = predNode->getStructure();
            if (pendingList.ValueAt(predStructure->getNumber()) && (!alreadyVisitedNode))
               {
               this->removeHeadFromAnalysisQueue();
               this->addToAnalysisQueue(predNode, 0);
               stopAnalyzingThisNode = true;
               break;
               }

              if (pendingList.ValueAt(predStructure->getNumber()) && !predStructure->hasBeenAnalyzedBefore())  //_firstIteration)
               {
               }
            else
               {
               Container *predBitVector = NULL;
               typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *predInfo = this->getAnalysisInfo(predStructure);
               int32_t nodeNumber = predStructure->asRegion()? node->getNumber() : predNode->getNumber();
               TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>
                  *predCurrentGenSetInfo = normalPred ? predInfo->_currentRegularGenSetInfo : predInfo->_currentExceptionGenSetInfo,
                  *predCurrentKillSetInfo = normalPred ? predInfo->_currentRegularKillSetInfo : predInfo->_currentExceptionKillSetInfo;

               predBitVector = predInfo->getContainer(predCurrentGenSetInfo, nodeNumber);
               if (predBitVector)
                  compose(_currentRegularGenSetInfo, predBitVector);

               predBitVector = predInfo->getContainer(predCurrentKillSetInfo, nodeNumber);
               if (predBitVector)
                  inverseCompose(_currentRegularKillSetInfo, predBitVector);
               }
            firstPred = false;

            if (this->traceBVA())
               {
               dumpOptDetails(this->comp(), "Node %p (%d) pred %p (%d)\n", nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber(), predStructure, predStructure->getNumber());
               _currentRegularGenSetInfo->print(this->comp());
               dumpOptDetails(this->comp(), "\n");
               _currentRegularKillSetInfo->print(this->comp());
               dumpOptDetails(this->comp(), "\n");
               }
            }

         if (stopAnalyzingThisNode)
            continue;
         }

      if (node == regionStructure->getEntry())
         {
            {
            _currentRegularGenSetInfo->empty();
            _currentRegularKillSetInfo->empty();
            }
         }


      Container *temp1;
      Container *temp2;
      this->allocateTempContainer(&temp1, _currentRegularGenSetInfo);
      this->allocateTempContainer(&temp2, _currentRegularKillSetInfo);
      *temp1 = *_currentRegularGenSetInfo;
      *temp2 = *_currentRegularKillSetInfo;

      bool checkForChange = !regionStructure->isAcyclic();
      typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *nodeInfo = this->getAnalysisInfo(nodeStructure->getStructure());
      this->_nodesInCycle->Clear();
      this->initializeGenAndKillSetInfoForStructure(nodeStructure->getStructure());
      pendingList[nodeStructure->getStructure()->getNumber()] = false;
      this->removeHeadFromAnalysisQueue();

      *_currentRegularGenSetInfo = *temp1;
      *_currentRegularKillSetInfo = *temp2;

      if (this->traceBVA())
         {
         typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
         if (nodeInfo->_regularGenSetInfo)
            {
            traceMsg(this->comp(), "\nGen Set Info for Region or Block : %p numbered %d is : \n", nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber());
            for (pair = nodeInfo->_regularGenSetInfo->getFirst(); pair; pair = pair->getNext())
               {
               if (pair->_container)
                  {
                  traceMsg(this->comp(), "Exit or Succ numbered %d : ", pair->_nodeNumber);
                    pair->_container->print(this->comp());
                  traceMsg(this->comp(), "\n");
                  }
               }
             traceMsg(this->comp(), "\n");
            }

         if (nodeInfo->_regularKillSetInfo)
            {
            traceMsg(this->comp(), "\nKill Set Info for Region or Block : %p numbered %d is : \n", nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber());
            for (pair = nodeInfo->_regularKillSetInfo->getFirst(); pair; pair = pair->getNext())
               {
               if (pair->_container)
                  {
                  traceMsg(this->comp(), "Exit or Succ numbered %d : ", pair->_nodeNumber);
                  pair->_container->print(this->comp());
                  traceMsg(this->comp(), "\n");
                  }
               }
              traceMsg(this->comp(), "\n");
            }
         }

      *this->_temp = *_currentRegularGenSetInfo;
      *this->_temp2 = *_currentRegularKillSetInfo;
      TR::CFGEdgeList succList(node->getSuccessors());
      succList.insert(succList.end(), node->getExceptionSuccessors().begin(), node->getExceptionSuccessors().end());
      int count = 0;
      for (auto succ = succList.begin(); succ != succList.end(); ++succ)
         {
         bool normalSucc = (++count <= node->getSuccessors().size());
         TR::CFGNode *succNode = (*succ)->getTo();

         *_currentRegularGenSetInfo = *this->_temp;
         *_currentRegularKillSetInfo = *this->_temp2;
         TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>
            *_nodeGenSetInfo = normalSucc ? nodeInfo->_regularGenSetInfo : nodeInfo->_exceptionGenSetInfo,
            *_nodeKillSetInfo = normalSucc ? nodeInfo->_regularKillSetInfo : nodeInfo->_exceptionKillSetInfo;
         Container *genBitVector = NULL, *killBitVector = NULL;
         int32_t nodeNumber =
            nodeStructure->getStructure()->asRegion() ? succNode->getNumber() : node->getNumber();
         genBitVector = nodeInfo->getContainer(_nodeGenSetInfo, nodeNumber);
         killBitVector = nodeInfo->getContainer(_nodeKillSetInfo, nodeNumber);

         if (killBitVector)
            *_currentRegularGenSetInfo -= *killBitVector;
         if (genBitVector)
            *_currentRegularGenSetInfo |= *genBitVector;
         if (genBitVector)
            *_currentRegularKillSetInfo -= *genBitVector;
         if (killBitVector)
            *_currentRegularKillSetInfo |= *killBitVector;


         if (regionStructure->isExitEdge(*succ) || (succNode == regionStructure->getEntry()))
            {
            TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>
               *_genSetInfo = normalSucc ? analysisInfo->_regularGenSetInfo : analysisInfo->_exceptionGenSetInfo,
               *_killSetInfo = normalSucc ? analysisInfo->_regularKillSetInfo : analysisInfo->_exceptionKillSetInfo;
            typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair =
               analysisInfo->getContainerNodeNumberPair(_genSetInfo, succNode->getNumber());
            if (!pair)
               {
               pair = _genSetInfo->getFirst();
               TR_ASSERT(!pair->getNext(), "There should be only one successor info\n");
               TR_ASSERT((pair->_nodeNumber == node->getNumber()), "Node number inconsistent\n");
               }
            TR_ASSERT(pair, "At least one successor info must be found\n");

            if (!pair->_container)
               {
               pair->_container = initializeInfo(NULL);
               }

            compose(pair->_container, _currentRegularGenSetInfo);

            pair = analysisInfo->getContainerNodeNumberPair(_killSetInfo, succNode->getNumber());
            if (!pair)
               {
               pair = _killSetInfo->getFirst();
               TR_ASSERT(!pair->getNext(), "There should be only one successor info\n");
               TR_ASSERT((pair->_nodeNumber == node->getNumber()), "Node number inconsistent\n");
               }
            TR_ASSERT(pair, "At least one successor info must be found\n");

            if (!pair->_container)
               {
               pair->_container = inverseInitializeInfo(NULL);
               }

            inverseCompose(pair->_container, _currentRegularKillSetInfo);
            }
         else
            {
            TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>
               *_currentGenSetInfo = normalSucc ? nodeInfo->_currentRegularGenSetInfo : nodeInfo->_currentExceptionGenSetInfo,
               *_currentKillSetInfo = normalSucc ? nodeInfo->_currentRegularKillSetInfo : nodeInfo->_currentExceptionKillSetInfo;
            if (!_currentGenSetInfo)
               TR_ASSERT(0, "Gen set info should have been allocated in initialization step\n");
            typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair = NULL;
            pair = nodeInfo->getContainerNodeNumberPair(_currentGenSetInfo, nodeNumber);

            if (!pair)
               {
               pair = nodeInfo->_currentRegularGenSetInfo->getFirst();
               TR_ASSERT(!pair->getNext(), "There should be only one successor info\n");
               TR_ASSERT((pair->_nodeNumber == node->getNumber()), "Node number inconsistent\n");
               }

            TR_ASSERT(pair, "At least one successor info must be found\n");

            if (!pair->_container)
               {
               pair->_container = initializeInfo(NULL);
               }
            compose(pair->_container, _currentRegularGenSetInfo);
            pair = nodeInfo->getContainerNodeNumberPair(_currentKillSetInfo, nodeNumber);

            if (!pair)
               {
               pair = _currentKillSetInfo->getFirst();
               TR_ASSERT(!pair->getNext(), "There should be only one successor info\n");
               TR_ASSERT((pair->_nodeNumber == node->getNumber()), "Node number inconsistent\n");
               }
            TR_ASSERT(pair, "At least one successor info must be found\n");

            if (!pair->_container)
               {
               pair->_container = inverseInitializeInfo(NULL);
               }

            inverseCompose(pair->_container, _currentRegularKillSetInfo);
            }

         if ((!regionStructure->isExitEdge(*succ)) &&
             (pendingList.ValueAt(succNode->getNumber())))
            {
            this->_nodesInCycle->Clear();
            this->addToAnalysisQueue(toStructureSubGraphNode(succNode), 1);
            }
         }
      }
   }


template<class Container>void TR_ForwardDFSetAnalysis<Container *>::initializeGenAndKillSetInfoForBlock(TR_BlockStructure *s)
   {
   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(s);
   if (!s->hasBeenAnalyzedBefore())
       s->setAnalyzedStatus(true);
   else
      return;

   analysisInfo->_regularGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *
      pair = new (this->trStackMemory())
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(this->_regularGenSetInfo[s->getNumber()], s->getNumber());
   analysisInfo->_regularGenSetInfo->add(pair);

   analysisInfo->_regularKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory())
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(this->_regularKillSetInfo[s->getNumber()], s->getNumber());
   analysisInfo->_regularKillSetInfo->add(pair);

   analysisInfo->_exceptionGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory())
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(this->_exceptionGenSetInfo[s->getNumber()], s->getNumber());
   analysisInfo->_exceptionGenSetInfo->add(pair);

   analysisInfo->_exceptionKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory())
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(this->_exceptionKillSetInfo[s->getNumber()], s->getNumber());
   analysisInfo->_exceptionKillSetInfo->add(pair);

   analysisInfo->_currentRegularGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(NULL, s->getNumber());
   analysisInfo->_currentRegularGenSetInfo->add(pair);

   analysisInfo->_currentRegularKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(NULL, s->getNumber());
   analysisInfo->_currentRegularKillSetInfo->add(pair);

   analysisInfo->_currentExceptionGenSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(NULL, s->getNumber());
   analysisInfo->_currentExceptionGenSetInfo->add(pair);

   analysisInfo->_currentExceptionKillSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(NULL, s->getNumber());
   analysisInfo->_currentExceptionKillSetInfo->add(pair);
   }



template<class Container>typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *TR_BasicDFSetAnalysis<Container *>::getAnalysisInfo(TR_Structure *s)
   {
   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = (typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *)s->getAnalysisInfo();
   if (!s->hasBeenAnalyzedBefore())
      {
      if (analysisInfo == NULL)
         {
         analysisInfo = createAnalysisInfo();
         initializeAnalysisInfo(analysisInfo, s);
         s->setAnalysisInfo(analysisInfo);
         }
      else
         clearAnalysisInfo(analysisInfo);
      }
   return analysisInfo;
   }

template<class Container>typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *TR_BasicDFSetAnalysis<Container *>::createAnalysisInfo()
   {
   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = new (this->trStackMemory())  typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo;
   analysisInfo->_inSetInfo = initializeInfo(NULL);
   analysisInfo->_outSetInfo = new (this->trStackMemory()) TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>;
   analysisInfo->_outSetInfo->setFirst(NULL);
   analysisInfo->_regularGenSetInfo = NULL;
   analysisInfo->_regularKillSetInfo = NULL;
   analysisInfo->_exceptionGenSetInfo = NULL;
   analysisInfo->_exceptionKillSetInfo = NULL;
   analysisInfo->_currentRegularGenSetInfo = NULL;
   analysisInfo->_currentRegularKillSetInfo = NULL;
   analysisInfo->_currentExceptionGenSetInfo = NULL;
   analysisInfo->_currentExceptionKillSetInfo = NULL;
   analysisInfo->_containsExceptionTreeTop = false;
   return analysisInfo;
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeAnalysisInfo(typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *info, TR_Structure *s)
   {
   TR_RegionStructure *region = s->asRegion();
   if (region)
      initializeAnalysisInfo(info, region);
   else
      initializeAnalysisInfo(info, s->asBlock()->getBlock());
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeAnalysisInfo(typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *info, TR::Block *block)
   {
   TR::CFGEdgeList succList(block->getSuccessors());
   succList.insert(succList.end() , block->getExceptionSuccessors().begin(), block->getExceptionSuccessors().end());
   for (auto succ = succList.begin(); succ != succList.end(); ++succ)
      {
      TR::CFGNode* succBlock = (*succ)->getTo();
      Container *b = initializeInfo(NULL);
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, succBlock->getNumber());
      info->_outSetInfo->add(pair);
      }
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::initializeAnalysisInfo(typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *info, TR_RegionStructure *region)
   {
   TR::BitVector exitNodes(comp()->allocator());
   //
   // Copy the current out set for comparison the next time we analyze this region
   //
   //
   if (region != this->_cfg->getStructure())
      {
      ListIterator<TR::CFGEdge> ei(&region->getExitEdges());
      for (TR::CFGEdge *edge = ei.getCurrent(); edge != NULL; edge = ei.getNext())
         {
         int32_t toStructureNumber = edge->getTo()->getNumber();
         if (!exitNodes.ValueAt(toStructureNumber))
            {
            Container *b = initializeInfo(NULL);
            typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair = new (this->trStackMemory()) typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair(b, toStructureNumber);
            info->_outSetInfo->add(pair);
            exitNodes[toStructureNumber] = true;
            }
         }
      }
   }

template<class Container>void TR_BasicDFSetAnalysis<Container *>::clearAnalysisInfo(typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *info)
   {
   initializeInfo(info->_inSetInfo);
   typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
   for (pair = info->_outSetInfo->getFirst(); pair; pair = pair->getNext())
      initializeInfo(pair->_container);
   }



template<class Container>void TR_ForwardDFSetAnalysis<Container *>::initializeDFSetAnalysis()
   {
   if (this->supportsGenAndKillSets())
     {
     this->allocateContainer(&_currentRegularGenSetInfo);
     this->allocateContainer(&_currentRegularKillSetInfo);
     }

   this->initializeBasicDFSetAnalysis();

   this->allocateContainer(&_currentInSetInfo);
   this->allocateContainer(&_originalInSetInfo);
   }




template<class Container>bool TR_ForwardDFSetAnalysis<Container *>::analyzeRegionStructure(TR_RegionStructure *regionStructure, bool checkForChange)
   {
   // Use information from last time we analyzed this structure; if
   // analyzing for the first time, initialize information
   //
   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = NULL; //this->getAnalysisInfo(regionStructure);
   if (regionStructure == this->_cfg->getStructure())
      analysisInfo = this->getAnalysisInfo(regionStructure);

   if (!regionStructure->hasBeenAnalyzedBefore())
      regionStructure->setAnalyzedStatus(true);
   else
      {
      analysisInfo = this->getAnalysisInfo(regionStructure);
      bool tryEarlyExit = (*this->_currentInSetInfo == *analysisInfo->_inSetInfo);
      if (tryEarlyExit)
         {
         if (this->traceBVA())
            {
            traceMsg(this->comp(), "\nSkipping re-analysis of Region : %p numbered %d\n", regionStructure, regionStructure->getNumber());
            }
         return false;
        }
      }

   analysisInfo = this->getAnalysisInfo(regionStructure);

   // Copy the current in set for comparison the next time we analyze this region
   //
   this->copyFromInto(this->_currentInSetInfo, analysisInfo->_inSetInfo);

   TR::BitVector pendingList(this->comp()->allocator());

   // Set the pending list to be all of the region's subnodes.
   //
   TR_RegionStructure::Cursor si(*regionStructure);
   TR_StructureSubGraphNode *subNode;
   for (subNode = si.getCurrent(); subNode; subNode = si.getNext())
      {
      pendingList[subNode->getNumber()] = true;
      }

   bool changed = true;
   int32_t numIterations = 1;
   this->_firstIteration = true;

   while (changed)
      {
      this->_nodesInCycle->Clear();

      changed = false;

      if (this->traceBVA())
         traceMsg(this->comp(), "\nAnalyzing REGION : %p NUMBER : %d ITERATION NUMBER : %d\n", regionStructure, regionStructure->getNumber(), numIterations);

      numIterations++;

         {
         this->addToAnalysisQueue(regionStructure->getEntry(), 0);
         if (this->analyzeNodeIfPredecessorsAnalyzed(regionStructure, pendingList))
            {
            if (!this->supportsGenAndKillSets() ||
                !this->canGenAndKillForStructure(regionStructure))
               changed = true;
            }
         }

      this->_firstIteration = false;
      }


   // Use pendingList to remember which exit nodes have already been seen
   // when merging the out information for the region.
   //
   pendingList.Clear();
   if (regionStructure != this->_cfg->getStructure())
      {
      ListIterator<TR::CFGEdge> ei(&regionStructure->getExitEdges());
      for (TR::CFGEdge *edge = ei.getCurrent(); edge != NULL; edge = ei.getNext())
         {
         TR_StructureSubGraphNode *edgeFrom = toStructureSubGraphNode(edge->getFrom());
         int32_t toStructureNumber = edge->getTo()->getNumber();
         Container *toBitVector = analysisInfo->getContainer(analysisInfo->_outSetInfo, toStructureNumber);

         typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *fromAnalysisInfo = this->getAnalysisInfo(edgeFrom->getStructure());
         Container *fromBitVector = fromAnalysisInfo->getContainer(fromAnalysisInfo->_outSetInfo, toStructureNumber);

         if (this->supportsGenAndKillSets() &&
             this->canGenAndKillForStructure(regionStructure))
            {
            if (this->getKind() == TR_DataFlowAnalysis::ReachingDefinitions)
               {
               *this->_temp = *fromBitVector;
               *this->_temp -= *toBitVector;
               if (!this->_temp->isEmpty())
                 {
                 dumpOptDetails(this->comp(), "From %d\n", edgeFrom->getNumber());
                 dumpOptDetails(this->comp(), "To %d\n", toStructureNumber);
                 // not all containers have getFirstElement, so we no longer print it.
                 TR_ASSERT(0, "This should not happen\n");
                 }
               }
            }
         else
            {
            if (!pendingList.ValueAt(toStructureNumber))
               {
               pendingList[toStructureNumber] = true;
               if (checkForChange && !changed &&
                   !(*fromBitVector == *toBitVector))
                  changed = true;
               this->copyFromInto(fromBitVector, toBitVector);
               }
            else
               {
               if (checkForChange && !changed)
                  *this->_temp = *toBitVector;
               this->compose(toBitVector, fromBitVector);
               if (checkForChange && !changed && !(*this->_temp == *toBitVector))
                  changed = true;
               }
            }
         }
      }

   return changed;
   }






template<class Container>bool TR_ForwardDFSetAnalysis<Container *>::analyzeNodeIfPredecessorsAnalyzed(TR_RegionStructure *regionStructure, TR::BitVector &pendingList)
   {
   bool anyNodeChanged = false;

   while (this->_analysisQueue.getListHead() &&
          (this->_analysisQueue.getListHead()->getData()->getStructure() != regionStructure))
      {
      if (!this->_analysisInterrupted)
         {
         numIterations++;
         if ((numIterations % 20) == 0)
            {
            numIterations = 0;
            if (this->comp()->compilationShouldBeInterrupted(FBVA_ANALYZE_CONTEXT))
               this->_analysisInterrupted = true;
            }
         }

      if (this->_analysisInterrupted)
         {
         TR::Compilation *comp = this->comp();
         comp->failCompilation<TR::CompilationInterrupted>("interrupted in forward bit vector analysis");
         }

      TR::CFGNode *node = this->_analysisQueue.getListHead()->getData();
      TR_StructureSubGraphNode *nodeStructure = (TR_StructureSubGraphNode *) node;

      if (!pendingList.ValueAt(nodeStructure->getStructure()->getNumber()) &&
          (*(this->_changedSetsQueue.getListHead()->getData()) == 0))
         {
         this->removeHeadFromAnalysisQueue();
         continue;
         }

      if (this->traceBVA())
         traceMsg(this->comp(), "Begin analyzing node %p numbered %d\n", node, node->getNumber());

      bool alreadyVisitedNode = false;
      if (this->_nodesInCycle->ValueAt(nodeStructure->getNumber()))
         alreadyVisitedNode = true;

      (*(this->_nodesInCycle))[nodeStructure->getNumber()] = true;

      typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(nodeStructure->getStructure());

      initializeInSetInfo();

      TR::CFGEdgeList predList(node->getPredecessors());
      predList.insert(predList.end(), node->getExceptionPredecessors().begin(), node->getExceptionPredecessors().end());
      bool firstPred = true;
      bool stopAnalyzingThisNode = false;
      for (auto pred = predList.begin(); pred != predList.end(); ++pred)
         {
         TR_StructureSubGraphNode *predNode = (TR_StructureSubGraphNode *) (*pred)->getFrom();
         TR_Structure *predStructure = predNode->getStructure();
         if (pendingList.ValueAt(predStructure->getNumber()) && (!alreadyVisitedNode))
            {
            this->removeHeadFromAnalysisQueue();
            this->addToAnalysisQueue(predNode, 0);
            stopAnalyzingThisNode = true;
            break;
            }

         if (pendingList.ValueAt(predStructure->getNumber()) && !predStructure->hasBeenAnalyzedBefore()) //this->_firstIteration)
            {
            if (firstPred)
               initializeInfo(_currentInSetInfo);
            }
         else
            {
            typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *predInfo = this->getAnalysisInfo(predStructure);
            Container *predBitVector = predInfo->getContainer(predInfo->_outSetInfo, node->getNumber());
            compose(_currentInSetInfo, predBitVector);
            }
         firstPred = false;
         }

      if (stopAnalyzingThisNode)
         continue;

      if (node == regionStructure->getEntry())
         {
         if (regionStructure != this->_cfg->getStructure())
            compose(_currentInSetInfo, this->getAnalysisInfo(regionStructure)->_inSetInfo);
         else
            compose(_currentInSetInfo, _originalInSetInfo);
         }

     typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *regionAnalysisInfo = this->getAnalysisInfo(regionStructure);

    if (this->supportsGenAndKillSets() &&
        canGenAndKillForStructure(nodeStructure->getStructure()))
        {
        if (nodeStructure->getStructure()->asRegion() &&
            nodeStructure->getStructure()->asRegion()->isNaturalLoop())
           {
           Container *bitVector;

           *this->_temp = *_currentInSetInfo;

           bitVector = analysisInfo->getContainer(analysisInfo->_regularKillSetInfo, node->getNumber());
           if (bitVector)
              *this->_temp -= *bitVector;

           bitVector = analysisInfo->getContainer(analysisInfo->_exceptionKillSetInfo, node->getNumber());
           if (bitVector)
              *this->_temp -= *bitVector;

           bitVector = analysisInfo->getContainer(analysisInfo->_regularGenSetInfo, node->getNumber());
           if (bitVector)
              *this->_temp |= *bitVector;

           bitVector = analysisInfo->getContainer(analysisInfo->_exceptionGenSetInfo, node->getNumber());
           if (bitVector)
              *this->_temp |= *bitVector;
           compose(_currentInSetInfo, this->_temp);
           }
        }

     bool checkForChange  = !regionStructure->isAcyclic();
     bool outSetChanged = false;

     if (this->supportsGenAndKillSets() &&
         canGenAndKillForStructure(nodeStructure->getStructure()))
        {
        TR::CFGEdgeList succList (node->getSuccessors());
        succList.insert(succList.end(), node->getExceptionSuccessors().begin(), node->getExceptionSuccessors().end());
        int count = 0;
        for (auto succ = succList.begin(); succ != succList.end(); ++succ)
            {
            bool normalSucc = (++count <= node->getSuccessors().size());
            Container* _info = normalSucc ? this->_regularInfo : this->_exceptionInfo;
            TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>
               *_killSetInfo = normalSucc ? analysisInfo->_regularKillSetInfo : analysisInfo->_exceptionKillSetInfo,
               *_genSetInfo =  normalSucc ? analysisInfo->_regularGenSetInfo : analysisInfo->_exceptionGenSetInfo;
            TR::CFGNode *succNode = (*succ)->getTo();
            this->copyFromInto(_currentInSetInfo, _info);

            int32_t nodeNumber = nodeStructure->getStructure()->asRegion() ? succNode->getNumber() : node->getNumber();
            Container *bitVector = analysisInfo->getContainer(_killSetInfo, nodeNumber);
            if (bitVector)
               {
               *_info -= *bitVector;
               if (this->traceBVA())
                 {
                 traceMsg(this->comp(), "\n%sKill Set Info for Region or Block : %p numbered %d and exit %d is : \n",
                    (normalSucc ? "" : "E"),
                    nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber(), succNode->getNumber());
                 bitVector->print(this->comp());
                 traceMsg(this->comp(), "\n%s Info for Region or Block : %p numbered %d and exit %d is : \n",
                    (normalSucc ? "Normal" : "Exception"),
                    nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber(), succNode->getNumber());
                 _info->print(this->comp());
                 }
               }

            bitVector = analysisInfo->getContainer(_genSetInfo, nodeNumber);
            if (bitVector)
               {
               *_info |= *bitVector;
                if (this->traceBVA())
                 {
                 traceMsg(this->comp(), "\n%sGen Set Info for Region or Block : %p numbered %d and exit %d is : \n",
                    (normalSucc ? "" : "E"),
                    nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber(), succNode->getNumber());
                 bitVector->print(this->comp());
                 traceMsg(this->comp(), "\n1%s Info for Region or Block : %p numbered %d and exit %d is : \n",
                    (normalSucc ? "Normal" : "Exception"),
                    nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber(), succNode->getNumber());
                 _info->print(this->comp());
                 }
               }

            bitVector = analysisInfo->getContainer(analysisInfo->_outSetInfo, succNode->getNumber());
            // Don't copy vectors if they are the same since comparison is much cheaper and can save compile time
            if (*_info != *bitVector)
               {
               this->copyFromInto(_info, bitVector);
               if (checkForChange && !outSetChanged)
                  outSetChanged = true;
               }
            }
        }

      this->_nodesInCycle->Clear();
      bool b = nodeStructure->getStructure()->doDataFlowAnalysis(this, checkForChange);
      if (b || pendingList.ValueAt(node->getNumber()))
         outSetChanged = true;
      pendingList[nodeStructure->getStructure()->getNumber()] = false;
      this->removeHeadFromAnalysisQueue();

      if (this->traceBVA())
         {
         traceMsg(this->comp(), "\nIn Set Info for Region or Block : %p numbered %d is : \n", nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber());
         analysisInfo->_inSetInfo->print(this->comp());
         traceMsg(this->comp(), "\nOut Set Info for Region or Block : %p numbered %d is : \n", nodeStructure->getStructure(), nodeStructure->getStructure()->getNumber());
         typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
         for (pair = analysisInfo->_outSetInfo->getFirst(); pair; pair = pair->getNext())
            {
            traceMsg(this->comp(), "Exit or Succ numbered %d : ", pair->_nodeNumber);
            pair->_container->print(this->comp());
            traceMsg(this->comp(), "\n");
            }
         traceMsg(this->comp(), "\n");
         }

     bool needToIterate = false;
     if (!this->supportsGenAndKillSets() ||
         !canGenAndKillForStructure(regionStructure))
        needToIterate = true;

      bool changed = outSetChanged;
      for (auto succ = node->getSuccessors().begin(); succ != node->getSuccessors().end(); ++succ)
         {
         TR::CFGNode *succNode = (*succ)->getTo();
         if ((!regionStructure->isExitEdge(*succ)) && (needToIterate || (succNode != regionStructure->getEntry())) &&
             (pendingList.ValueAt(succNode->getNumber()) || outSetChanged))
            {
            this->_nodesInCycle->Clear();
            this->addToAnalysisQueue(toStructureSubGraphNode(succNode), outSetChanged ? 1 : 0);
            }
         }

      for (auto succ = node->getExceptionSuccessors().begin(); succ != node->getExceptionSuccessors().end(); ++succ)
         {
         TR::CFGNode *succNode = (*succ)->getTo();
         if ((!regionStructure->isExitEdge(*succ)) && (needToIterate || (succNode != regionStructure->getEntry())) &&
             (pendingList.ValueAt(succNode->getNumber()) || outSetChanged))
            {
            this->_nodesInCycle->Clear();
            this->addToAnalysisQueue(toStructureSubGraphNode(succNode), outSetChanged ? 1 : 0);
            }
         }

      if (changed)
         anyNodeChanged = true;
      }

   return anyNodeChanged;
   }

template<class Container>bool TR_ForwardDFSetAnalysis<Container *>::analyzeBlockStructure(TR_BlockStructure *blockStructure, bool checkForChange)
   {
   if (this->supportsGenAndKillSets() &&
       canGenAndKillForStructure(blockStructure))
      {
      blockStructure->setAnalyzedStatus(true);
      typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(blockStructure);

      if (!this->_blockAnalysisInfo[blockStructure->getNumber()])
         this->allocateBlockInfoContainer(&this->_blockAnalysisInfo[blockStructure->getNumber()], _currentInSetInfo);
      this->copyFromInto(_currentInSetInfo, this->_blockAnalysisInfo[blockStructure->getNumber()]);
      this->copyFromInto(_currentInSetInfo, analysisInfo->_inSetInfo);
      if (blockStructure->getNumber() == 0)
         {
         analyzeBlockZeroStructure(blockStructure);
         TR::Block *block = blockStructure->getBlock();
         Container *outSetInfo;
         bool changed = false;
         for (auto succ = block->getSuccessors().begin(); succ != block->getSuccessors().end(); ++succ)
            {
            outSetInfo = analysisInfo->getContainer(analysisInfo->_outSetInfo, (*succ)->getTo()->getNumber());
            if (checkForChange && !changed && !(*this->_regularInfo == *outSetInfo))
               changed = true;

            *outSetInfo = *this->_regularInfo;
            }
         }
      return false;
      }

   typename TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo *analysisInfo = this->getAnalysisInfo(blockStructure);

   initializeInfo(this->_regularInfo);
   initializeInfo(this->_exceptionInfo);
   //
   // Use information from last time we analyzed this structure; if
   // analyzing for the first time, initialize information
   //
   if (!blockStructure->hasBeenAnalyzedBefore())
      {
      blockStructure->setAnalyzedStatus(true);
      }
   else if (!this->supportsGenAndKillSets() ||
            !canGenAndKillForStructure(blockStructure))
      {
      bool tryEarlyExit = (*_currentInSetInfo == *analysisInfo->_inSetInfo);
      if (tryEarlyExit)
         {
         if (this->traceBVA())
            {
            traceMsg(this->comp(), "\nSkipping re-analysis of Block : %p numbered %d\n", blockStructure, blockStructure->getNumber());
            }
         return false;
         }
      }


   // Copy the current in set for comparison the next time we analyze this region
   //
   this->copyFromInto(_currentInSetInfo, analysisInfo->_inSetInfo);

   bool changed = false;
   if (blockStructure->getNumber() == 0)
      {
      analyzeBlockZeroStructure(blockStructure);
      }
   else
      {
      int32_t blockNum = blockStructure->getNumber();
      this->copyFromInto(_currentInSetInfo, this->_regularInfo);
      this->copyFromInto(_currentInSetInfo, this->_exceptionInfo);
      if (this->_regularGenSetInfo)
         {
         if (this->_regularKillSetInfo[blockNum])
            *this->_regularInfo -= *this->_regularKillSetInfo[blockNum];
         if (this->_regularGenSetInfo[blockNum])
            *this->_regularInfo |= *this->_regularGenSetInfo[blockNum];
         if (this->_exceptionKillSetInfo[blockNum])
            *this->_exceptionInfo -= *this->_exceptionKillSetInfo[blockNum];
         if (this->_exceptionGenSetInfo[blockNum])
            *this->_exceptionInfo |= *this->_exceptionGenSetInfo[blockNum];
         this->copyFromInto(analysisInfo->_inSetInfo, this->_blockAnalysisInfo[blockStructure->getNumber()]);
         }
      else
         {
         analyzeTreeTopsInBlockStructure(blockStructure);
         }
      }

   TR::Block *block = blockStructure->getBlock();
   TR::CFGEdgeList succList(block->getSuccessors());
   succList.insert(succList.end(), block->getExceptionSuccessors().begin(), block->getExceptionSuccessors().end());
   int count = 0;
   for (auto succ = succList.begin(); succ != succList.end(); ++succ)
      {
      bool normalSucc = (++count <= block->getSuccessors().size());
      Container* outSetInfo = analysisInfo->getContainer(analysisInfo->_outSetInfo, (*succ)->getTo()->getNumber());
      Container* _info = normalSucc ? this->_regularInfo : this->_exceptionInfo;

      if (checkForChange && !changed && !(*_info == *outSetInfo))
         changed = true;

      if (this->supportsGenAndKillSets() &&
          canGenAndKillForStructure(blockStructure))
         {
         if (!(*_info == *outSetInfo))
            TR_ASSERT(0, "This should not happen for a block\n");
         }

      *outSetInfo = *_info;
      }

  if (this->traceBVA())
      {
      traceMsg(this->comp(), "\nIn Set Info for Block : %p numbered %d is : \n", blockStructure, blockStructure->getNumber());
      analysisInfo->_inSetInfo->print(this->comp());
      traceMsg(this->comp(), "\nOut Set Info for Block : %p numbered %d is : \n", blockStructure, blockStructure->getNumber());
      typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
      for (pair = analysisInfo->_outSetInfo->getFirst(); pair; pair = pair->getNext())
         {
         traceMsg(this->comp(), "Exit or Succ numbered %d : ", pair->_nodeNumber);
         pair->_container->print(this->comp());
         traceMsg(this->comp(), "\n");
         }
      traceMsg(this->comp(), "\n");
      }

   return changed;
   }





template<class Container>void TR_ForwardDFSetAnalysis<Container *>::analyzeBlockZeroStructure(TR_BlockStructure *blockStructure)
   {
   analyzeTreeTopsInBlockStructure(blockStructure);
   }


template<class Container>void TR_ForwardDFSetAnalysis<Container *>::analyzeNode(TR::Node *node, vcount_t visitCount, TR_BlockStructure *blockStructure, Container *analysisInfo)
   {
   }




template<class Container>void TR_ForwardDFSetAnalysis<Container *>::analyzeTreeTopsInBlockStructure(TR_BlockStructure *blockStructure)
   {
   TR::Block *block = blockStructure->getBlock();
   TR::TreeTop *currentTree = block->getEntry();
   TR::TreeTop *exitTree = block->getExit();
   vcount_t visitCount = this->comp()->incVisitCount();
   this->copyFromInto(_currentInSetInfo, this->_regularInfo);
   this->copyFromInto(_currentInSetInfo, this->_exceptionInfo);

   while (!(currentTree == exitTree))
      {
      bool currentTreeHasChecks = this->treeHasChecks(currentTree);
      analyzeNode(currentTree->getNode(), visitCount, blockStructure, this->_regularInfo);
      if (currentTreeHasChecks)
         compose(this->_exceptionInfo, this->_regularInfo);
      if (!(currentTree == exitTree))
         currentTree = currentTree->getNextTreeTop();
      }
   }


template<class Container>void TR_ForwardDFSetAnalysis<Container *>::compose(Container *, Container *)
   {
   }

template<class Container>void TR_ForwardDFSetAnalysis<Container *>::inverseCompose(Container *, Container *)
   {
   }

template<class Container>TR_DataFlowAnalysis::Kind TR_ForwardDFSetAnalysis<Container *>::getKind()
   {
   return TR_DataFlowAnalysis::ForwardDFSetAnalysis;
   }


template<class Container>Container *
TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo::getContainer(TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>  *list,
                                                                    int32_t n)
   {
   typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
   for (pair = list->getFirst(); pair; pair = pair->getNext())
      {
      if (pair->_nodeNumber == n)
         return pair->_container;
      }

   return NULL;
   }

template<class Container>typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *TR_BasicDFSetAnalysis<Container *>::ExtraAnalysisInfo::getContainerNodeNumberPair(TR_LinkHead<typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair>  *list, int32_t n)
   {
   typename TR_BasicDFSetAnalysis<Container *>::TR_ContainerNodeNumberPair *pair;
   for (pair = list->getFirst(); pair; pair = pair->getNext())
      {
      if (pair->_nodeNumber == n)
         return pair;
      }

   return NULL;
   }

template class TR_BasicDFSetAnalysis<TR_BitVector *>;
template class TR_ForwardDFSetAnalysis<TR_BitVector *>;
template class TR_BasicDFSetAnalysis<TR_SingleBitContainer *>;
template class TR_ForwardDFSetAnalysis<TR_SingleBitContainer *>;
