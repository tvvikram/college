//===----- ScheduleDAGRRList.cpp - Reg pressure reduction list scheduler --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This implements bottom-up and top-down register pressure reduction list
// schedulers, using standard algorithms.  The basic approach uses a priority
// queue of available nodes to schedule.  One at a time, nodes are taken from
// the priority queue (thus in priority order), checked for legality to
// schedule, and emitted if legal.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "pre-RA-sched"
#include "ScheduleDAGSDNodes.h"
#include "llvm/InlineAsm.h"
#include "llvm/CodeGen/SchedulerRegistry.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <climits>
#include "llvm/CodeGen/Lineage.h"
#include <list>
using namespace llvm;

STATISTIC(NumBacktracks, "Number of times scheduler backtracked");
STATISTIC(NumUnfolds,    "Number of nodes unfolded");
STATISTIC(NumDups,       "Number of duplicated nodes");
STATISTIC(NumPRCopies,   "Number of physical register copies");

static RegisterScheduler
burrListDAGScheduler("list-burr",
                     "Bottom-up register reduction list scheduling",
                     createBURRListDAGScheduler);
static RegisterScheduler
tdrListrDAGScheduler("list-tdrr",
                     "Top-down register reduction list scheduling",
                     createTDRRListDAGScheduler);
static RegisterScheduler
sourceListDAGScheduler("source",
                       "Similar to list-burr but schedules in source "
                       "order when possible",
                       createSourceListDAGScheduler);

static RegisterScheduler
hybridListDAGScheduler("list-hybrid",
                       "Bottom-up register pressure aware list scheduling "
                       "which tries to balance latency and register pressure",
                       createHybridListDAGScheduler);

static RegisterScheduler
ILPListDAGScheduler("list-ilp",
                    "Bottom-up register pressure aware list scheduling "
                    "which tries to balance ILP and register pressure",
                    createILPListDAGScheduler);

namespace
{
//===----------------------------------------------------------------------===//
/// ScheduleDAGRRList - The actual register reduction list scheduler
/// implementation.  This supports both top-down and bottom-up scheduling.
///
class ScheduleDAGRRList : public ScheduleDAGSDNodes
{
private:
  /// isBottomUp - This is true if the scheduling problem is bottom-up, false if
  /// it is top-down.
  bool isBottomUp;

  /// NeedLatency - True if the scheduler will make use of latency information.
  ///
  bool NeedLatency;

  /// AvailableQueue - The priority queue to use for the available SUnits.
  SchedulingPriorityQueue *AvailableQueue;

  /// LiveRegDefs - A set of physical registers and their definition
  /// that are "live". These nodes must be scheduled before any other nodes that
  /// modifies the registers can be scheduled.
  unsigned NumLiveRegs;
  std::vector<SUnit*> LiveRegDefs;
  std::vector<unsigned> LiveRegCycles;

  // mycode here
  /// FIXME: Try using ilist instead of list
  /// Colors - Set of colors used to color the LIG
  std::list<int> Colors;
  typedef std::list<int>::iterator color_iterator;
  /// ColoredNodes - Set of colored nodes in the LIG
  std::list<int> ColoredNodes;
  typedef std::list<int>::iterator colorednodes_iterator;
  /// SUnitReg - SUnit and Register pair
  typedef std::pair < SUnit*, unsigned > SUnitReg;
  // mycode ends here

  /// Topo - A topological ordering for SUnits which permits fast IsReachable
  /// and similar queries.
  ScheduleDAGTopologicalSort Topo;

public:
  ScheduleDAGRRList(MachineFunction &mf,
                    bool isbottomup, bool needlatency,
                    SchedulingPriorityQueue *availqueue)
      : ScheduleDAGSDNodes(mf), isBottomUp(isbottomup), NeedLatency(needlatency),
      AvailableQueue(availqueue), Topo(SUnits)
  {
  }

  ~ScheduleDAGRRList()
  {
    delete AvailableQueue;
  }

  // MRISimp
  void Schedule();

  // mycode here
  /// dumpPreds - Dump the predecessors of the node
  void dumpPreds(SUnit *SU);

  /// dumpSuccs - Dump the successors of the node
  void dumpSuccs(SUnit *SU);

  /// getHighestNodeNotInAnyLineage - Returns the highest node not
  /// in any lineage
  SUnit *getHighestNodeNotInAnyLineage();

  /// VerifyLineages - Verify that all nodes were included in the lineage
  void VerifyLineages();

  /// LowestDescendant - Returns the lowest descendant of the node
  /// Currently, a very costly brute force algorithm
  /// OBSOLETE - This call currently not used and hence might not be required.
  SUnit *LowestDescendant(SUnit *SU);

  /// getLowestDescendant - Use LowestDescendant call to get the lowest
  /// descendant node giving least priority to load/store nodes. This is
  /// because, load/store nodes results in a lineage to halt abruptly.
  /// OBSOLETE - This call currently not used and hence might not be required.
  SUnit *getLowestDescendant(SUnit *SU);

  /// getLowestDescendantNode - Wrapper around SDNode::getDescendant().
  SUnit *getLowestDescendantNode(SUnit *SU);

  /// MRISLineageFormation - Forms lineages as required by the
  /// MRIS paper
  void MRISLineageFormation(SmallVectorImpl<Lineage> &LineageList);

  /// ModifyLineages - Build the reach matrix, fuse lineages,
  /// build and color the lineage interference graph
  int ModifyLineages(SmallVectorImpl<Lineage> &LineageList);

  /// PrintNodes - Print the SUnit nodes of the list
  void PrintNodes(SmallVectorImpl<SUnit*> &List);

  /// Reach - This returns true if there exists a path from Va to Vb, also
  /// through the sequencing edges in the DDG, otherwise returns false
  bool Reach(SDNode *Va, SDNode *Vb);

  /// PrintReachMatrix - Print the ReachMatrix
  void PrintMatrix(int Row, int Column, int **Matrix);

  /// ReachRelation - Calculate the reach relation matrix
  int **ReachRelation(int **ReachMatrix, SmallVectorImpl<SUnit*> &StartList,
                       SmallVectorImpl<SUnit*> &EndList);

  /// FuseLineages - Fusion of two lineages and updating the LineageList
  void FuseLineages(int UmEnd, int V1Start,
                     SmallVectorImpl<Lineage> &LineageList);

  /// BuildAndColorLIG - Build the Lineage Interference Graph using
  /// the ReachMatrix and color it to know the minimum number of
  /// registers required (HRB). Return HRB. NumRegs is maximum number of
  /// registers equal to the number of colors.  The LIG is colored using
  /// virtual registers with maximum registers equal to NumRegs.
  int BuildAndColorLIG(int Row, int Column, int **ReachMatrix,
                       SmallVectorImpl<Lineage> &LineageList);

  /// CheckInList - Check if SUnit is present in StartList/EndList
  bool CheckInList(SUnit* Value, std::list<SUnit*> &List);

  /// CheckInRegList - Check if Reg is present in RegAvailable
  bool CheckInRegList(unsigned Value, std::list<unsigned> &List);

  /// PrintReadyList - Prints the ReadyList
  void PrintReadyList(std::list<SUnitReg> &ReadyList);

  /// AddToReadyList - Find SUnit corresponding to SDNode and add to ReadyList
  void AddToReadyList(SDNode *N, std::list<SUnitReg> &ReadyList,
                      SmallVectorImpl<Lineage> &LineageList);

  /// AddSuccessors - Add nodes to the ReadyList all successors of Vi that have
  /// all its predeccessors listed and in decreasing height order.
  void AddSuccessors(SUnit *Vi, std::list<SUnitReg> &ReadyList,
                     SmallVectorImpl<Lineage> &LineageList);

  /// LineageInstructionSequence - Sequencing the nodes of the lineages using
  /// the registers allocated to each node during coloring of the
  /// Lineage Interference Graph
  void LineageInstructionSequence(int HRB,
                                   SmallVectorImpl<Lineage> &LineageList);
  // mycode ends here

  /// IsReachable - Checks if SU is reachable from TargetSU.
  bool IsReachable(const SUnit *SU, const SUnit *TargetSU)
  {
    return Topo.IsReachable(SU, TargetSU);
  }

  /// WillCreateCycle - Returns true if adding an edge from SU to TargetSU will
  /// create a cycle.
  bool WillCreateCycle(SUnit *SU, SUnit *TargetSU)
  {
    return Topo.WillCreateCycle(SU, TargetSU);
  }

  /// AddPred - adds a predecessor edge to SUnit SU.
  /// This returns true if this is a new predecessor.
  /// Updates the topological ordering if required.
  void AddPred(SUnit *SU, const SDep &D)
  {
    Topo.AddPred(SU, D.getSUnit());
    SU->addPred(D);
  }

  /// RemovePred - removes a predecessor edge from SUnit SU.
  /// This returns true if an edge was removed.
  /// Updates the topological ordering if required.
  void RemovePred(SUnit *SU, const SDep &D)
  {
    Topo.RemovePred(SU, D.getSUnit());
    SU->removePred(D);
  }

private:
  void ReleasePred(SUnit *SU, const SDep *PredEdge);
  void ReleasePredecessors(SUnit *SU, unsigned CurCycle);
  void ReleaseSucc(SUnit *SU, const SDep *SuccEdge);
  void ReleaseSuccessors(SUnit *SU);
  void CapturePred(SDep *PredEdge);
  void ScheduleNodeBottomUp(SUnit*, unsigned);
  void ScheduleNodeTopDown(SUnit*, unsigned);
  void UnscheduleNodeBottomUp(SUnit*);
  void BacktrackBottomUp(SUnit*, unsigned, unsigned&);
  SUnit *CopyAndMoveSuccessors(SUnit*);
  void InsertCopiesAndMoveSuccs(SUnit*, unsigned,
                                const TargetRegisterClass*,
                                const TargetRegisterClass*,
                                SmallVector<SUnit*, 2>&);
  bool DelayForLiveRegsBottomUp(SUnit*, SmallVector<unsigned, 4>&);
  void ListScheduleTopDown();
  void ListScheduleBottomUp();


  /// CreateNewSUnit - Creates a new SUnit and returns a pointer to it.
  /// Updates the topological ordering if required.
  SUnit *CreateNewSUnit(SDNode *N)
  {
    unsigned NumSUnits = SUnits.size();
    SUnit *NewNode = NewSUnit(N);
    // Update the topological ordering.
    if (NewNode->NodeNum >= NumSUnits)
      Topo.InitDAGTopologicalSorting();
    return NewNode;
  }

  /// CreateClone - Creates a new SUnit from an existing one.
  /// Updates the topological ordering if required.
  SUnit *CreateClone(SUnit *N)
  {
    unsigned NumSUnits = SUnits.size();
    SUnit *NewNode = Clone(N);
    // Update the topological ordering.
    if (NewNode->NodeNum >= NumSUnits)
      Topo.InitDAGTopologicalSorting();
    return NewNode;
  }

  /// ForceUnitLatencies - Register-pressure-reducing scheduling doesn't
  /// need actual latency information but the hybrid scheduler does.
  bool ForceUnitLatencies() const
  {
    return !NeedLatency;
  }
};
}  // end anonymous namespace

  //mycode here
void ScheduleDAGRRList::dumpPreds(SUnit *SU) {
  for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
        I != E; ++I) {
    // (I->getSUnit()->getNode()->getNodeType() == ISD::EntryToken)
    {
      SDNode *p = I->getSUnit()->getNode();
      p->dump();
    }
  }
}

void ScheduleDAGRRList::dumpSuccs(SUnit *SU) {
  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
        I != E; ++I) {
    //  if (I->getSUnit()->getNode()->getNodeType() == ISD::LOAD)
    {
      SDNode *p = I->getSUnit()->getNode();
      p->dump();
      //  dumpPreds(I->getSUnit());
    }
  }
}

/// getHighestNodeNotInAnyLineage - Returns the highest node that is not
/// in any lineage
SUnit *ScheduleDAGRRList::getHighestNodeNotInAnyLineage() {
  // As of now, neglect the FrameIndex nodes. Other nodes like TokenFactor
  // do not get selected as their height will be set to -1 by the
  // ComputeHeightOfEveryNode()
  SUnit *MaxNode = NULL;
  int MaxHeight = 0;
  for (unsigned su = 0, e = SUnits.size(); su != e; ++su) {
    SDNode *temp = SUnits[su].getNode();
    if (temp->getIsInLineage() == false && temp->getHeight() > MaxHeight) {
      MaxNode = &SUnits[su];
      MaxHeight = temp->getHeight();
    }
  }
  return MaxNode;
}

/// VerifyLineages - Verify that all nodes were included in the lineage
void ScheduleDAGRRList::VerifyLineages()
{
  std::cout << "VerifyLineages:\n";
  for (unsigned su = 0, e = SUnits.size(); su != e; ++su) {
    SDNode *temp = SUnits[su].getNode();
    if (temp->getIsInLineage() == false)
      temp->dump();
  }
}

/// LowestDescendant - Returns the lowest descendant of a node
/// Currently, a very costly brute force algorithm
/// OBSOLETE - This call currently not used and hence might not be required.
SUnit *ScheduleDAGRRList::LowestDescendant(SUnit *SU) {
  int h = 0, H;
  SDNode *ld = NULL;
  // Currently, get the lowest descendant SDNode.  Then perform a brute force
  // search to find the corresponding SUnit for the SDNode.
  // FIXME: Instead of brute force search, check if any index is available to
  // get the corresponding SUnit
  SDNode *N = SU->getNode();
  for (SDNode::use_iterator I = N->use_begin(), E = N->use_end(); I != E; ++I) {
    SDNode *temp = *I;
    if (I.getUse().getValueType() == MVT::Other)
      DEBUG(errs() << "LD: Blue edged node\n");
    else if (I.getUse().getValueType() == MVT::Flag)
      DEBUG(errs() << "LD: Red edged node\n");
    else {
      // Initialize 'h' using the first use
      if (h == 0) {
        h = temp->getHeight();
        ld = temp;
      }
      else {
        H = temp->getHeight();
        if (H < h) {
          h = H;
          ld = temp;
        }
      }
    }
  }
  // If N is load/store, then move to its FrameIndex and search among its
  // children, whose height is less than 'h'.
  if (N->getNodeType() == ISD::LOAD || N->getNodeType() == ISD::STORE) {
    for (unsigned j = 0; j < N->getNumOperands(); j++) {
      SDNode *fi = (N->getOperand(j)).getNode();
      if (SDNode::isLeafNode(fi->getNodeType()))
        for (SDNode::use_iterator X = fi->use_begin(), Y = fi->use_end();
              X != Y; ++X) {
          if (X.getUse().getValueType() == MVT::Other)
            DEBUG(errs() << "LDFI: Blue edged node.\n");
          else if (X.getUse().getValueType() == MVT::Flag)
            DEBUG(errs() << "LDFI: Red edged node.\n");
          else {
            SDNode *FIUses = *X;
            if (FIUses != N) {
              if (h == 0) {
                h = FIUses->getHeight();
                ld = FIUses;
              }
              else {
                H = FIUses->getHeight();
                if (H < h) {
                  h = H;
                  ld = FIUses;
                }
              }
            }
          }
        }
    }
  }

  // check for sequence edged nodes
  if (N->getSequencingEdge() != NULL) {
    if (N->getSequencingEdge()->getIsInLineage() == false)
      std::cout << "LD: ERROR! A sequence edged node not yet visited!\n";
  }

  if (ld != NULL && ld->getHeight() >= SU->getNode()->getHeight())
    return NULL;

  // Finally, perform the brute force check with SUnits to return the
  // corresponding SUnit for the ld node
  for (unsigned su = 0, e = SUnits.size(); su != e; ++su) {
    if (ld == SUnits[su].getNode())
      return &SUnits[su];
  }
  // If not found, then return NULL, as in case of a leafnode
  return NULL;
}

/// getLowestDescendant - Use LowestDescendant call to get the lowest descendant
/// node giving least priority to load/store nodes.  This is because, load/store
/// nodes results in a lineage to halt abruptly.
/// OBSOLETE - This call currently not used and hence might not be required.
SUnit *ScheduleDAGRRList::getLowestDescendant(SUnit *SU) {
  bool Flag;	// Flag used for looping
  // If a NULL node, other than load/store node is found
  bool FoundNullNode = false;
  // Temporary location for load/store nodes
  SmallVector< std::pair< SUnit *, int >, 2 > TempLS;
  SUnit *RetSU;
  // Set VisitedHeight of every node to false
  DAG->setVisitedHeightToFalse();
  do {
    Flag = false;
    RetSU = LowestDescendant(SU);
    if (RetSU == NULL) { 	// leafNode
      FoundNullNode = true;
    }
    else {
      SDNode *RetSD = RetSU->getNode();
      if (RetSD->getNodeType() == ISD::LOAD
            || RetSD->getNodeType() == ISD::STORE) {
        if (RetSD->getVisitedHeight() == false) {
          Flag = true;
          TempLS.push_back(std::make_pair(RetSU, RetSD->getHeight()));
          RetSD->setHeight(SU->getNode()->getHeight());
          RetSD->setVisitedHeight(true);
        }
      }
    }
  } while (Flag == true);
  for (int i = 0, e = TempLS.size(); i != e; ++i) {
    TempLS[i].first->getNode()->setHeight(TempLS[i].second);
  }
  if (FoundNullNode == true) {
    // return the first load/store node in the list, otherwise return NULL
    if (TempLS.size() >= 1)
      return TempLS[0].first;
  }
  return RetSU;
}

/// getLowestDescendantNode - Wrapper around SDNode::getDescendant().
SUnit* ScheduleDAGRRList::getLowestDescendantNode(SUnit *SU) {
  SDNode *SD = SU->getNode()->getDescendant();
  if (SD)
    for (int i = 0, e = SUnits.size(); i != e; ++i) {
      if (SUnits[i].getNode() == SD)
        return &SUnits[i];
    }
  return NULL;
}

// MRIS Lineage formation
void ScheduleDAGRRList::
     MRISLineageFormation(SmallVectorImpl<Lineage> &LineageList) {
  DEBUG(errs() << "===== MRIS Lineage Formation begins:\n");
  DAG->MarkAllNodesAsNotInAnyLineage();
  // VerifyLineages();
  DAG->ComputeHeightOfEveryNode();

  SUnit *ViSU, *VjSU;
  SDNode *Vi, *Vj;
  int s = 0;
  while ((ViSU = getHighestNodeNotInAnyLineage()) != NULL) {
    bool RecomputeHeight = false;
    // Create a lineage and add it to the list
    Lineage r;
    LineageList.push_back(r);
    Lineage *l = &LineageList[s]; s++;
    // Start a new lineage containing Vi by adding it to the LineageList
    // Also mark Vi as in lineage
    l->addNodeToLineage(ViSU);
    ViSU->getNode()->setIsInLineage(true);

    VjSU = getLowestDescendantNode(ViSU);

    while (VjSU != NULL) {
      Vi = ViSU->getNode();
      Vj = VjSU->getNode();
      // If Vi has multiple descendants
      if (Vi->getNumDescendants() > 1) {
        RecomputeHeight = true;
        for (SDNode::descendant_iterator I = Vi->descendant_begin(),
              E = Vi->descendant_end(); I != E; ++I) {
          SDNode *Vk = *I;
          if (Vk != Vj) {
            //std::cout << "Sequence edge:\n";
            //Vk->dump();
            //Vj->dump();
            Vk->setSequencingEdge(Vj);
          }
        }
      }

      // Add VjSU to lineage
      l->addNodeToLineage(VjSU);

       // If VjSU is already in lineage, then break the lineage
       if (Vj->getIsInLineage() == true)
         break;

       Vj->setIsInLineage(true);
       ViSU = VjSU;

       VjSU = getLowestDescendantNode(ViSU);
     }

     // Print the lineage
     // l->printLineage();

     // Recompute height if necessary
     if (RecomputeHeight == true)
       DAG->ComputeHeightOfEveryNode();

     // DAG->PrintAllNodes();
  }
  DEBUG(errs() << "===== MRIS Lineage Formation ends.\n");
}

/// PrintNodes - Print the SUnit nodes of the list
void ScheduleDAGRRList::PrintNodes(SmallVectorImpl < SUnit * > &List) {
  for (int i = 0, j = List.size(); i != j; i++)
    List[i]->getNode()->dump();
}

/// Reach - This returns true if there exists a path from Va to Vb, also
/// through the sequencing edges in the DDG, otherwise returns false
bool ScheduleDAGRRList::Reach(SDNode *Va, SDNode *Vb) {
  if (Va == Vb)	return true;
  // Goto each of the uses and finally visit the sequencing edged node too
  for (SDNode::use_iterator I = Va->use_begin(),E = Vb->use_end();I != E; ++I) {
    SDNode *X = *I;
    if (X == Va)
      return true;
    // otherwise check if this use reaches Vb
    if (I.getUse().getValueType() == MVT::Other)
      DEBUG(errs() << "RCH: Blue edged node\n");
    else if (I.getUse().getValueType() == MVT::Flag)
      DEBUG(errs() << "RCH: Red edged node\n");
    else if (X->getVisitedHeight() == false) {
      X->setVisitedHeight(true);
      if (Reach(X, Vb))
        return true;
    }
  }
  // Now check through the FrameIndex
  if (Va->getNodeType() == ISD::LOAD || Va->getNodeType() == ISD::STORE) {
    for (unsigned j = 0; j < Va->getNumOperands(); j++) {
      SDNode *temp = (Va->getOperand(j)).getNode();
      // If FrameIndex node is found then call search through each of its uses
      // except Va, otherwise it results in recursion
      if (SDNode::isLeafNode(temp->getNodeType())
            && temp->getVisitedHeight() == false) {
        temp->setVisitedHeight(true);
        // This is a costly algorithm as per the current understanding of DAG.
        // This maybe in order of cube complexity, which results in more
        // execution time with increase in load/store operations.
        for (SDNode::use_iterator X = temp->use_begin(), Y = temp->use_end();
              X != Y;) {
          if (X.getUse().getValueType() == MVT::Other) {
            DEBUG (errs() << "RCHFI: Blue edged node\n");
            ++X;
          }
          else if (X.getUse().getValueType() == MVT::Flag) {
            DEBUG (errs() << "RCHFI: Red edged node\n");
            ++X;
          }
          else {
            SDNode *FIUses = *X;
            if (FIUses != Va && FIUses->getVisitedHeight() == false) {
              DEBUG (errs() << FIUses->dump());
              FIUses->setVisitedHeight(true);
              if (Reach(FIUses, Vb))
                return true;
            }
            ++X;
          }
        }
      }
    }
  }

  if (Va->getSequencingEdge() != NULL)
    if (Reach(Va->getSequencingEdge(), Vb))
      return true;

  // Return false if neither the uses nor sequencing edge reaches Vb
  return false;
}

/// PrintReachMatrix - Print the ReachMatrix
void ScheduleDAGRRList::PrintMatrix(int Row, int Column, int **Matrix) {
  for (int i = 0; i < Row; i++) {
    for (int j = 0; j < Column; j++)
      std::cout << Matrix[i][j];
    std::cout << "\n";
  }
}

/// ReachRelation - Calculate the reach relation matrix
int ** ScheduleDAGRRList::ReachRelation(int **ReachMatrix,
                                        SmallVectorImpl< SUnit * > &StartList,
                                        SmallVectorImpl< SUnit * > &EndList) {
  ReachMatrix = new int*[StartList.size()];
  for (unsigned i = 0; i < StartList.size(); i++)
    ReachMatrix[i] = new int[EndList.size()];
  // If Va is in StartList, Vb is in EndList, then R(Va, Vb) = 1,
  // if there is a path in the augmented DDG from Va to Vb, else
  // R(Va, Vb) = 0
  for (unsigned i = 0; i < StartList.size(); i++)
    for (unsigned j = 0; j < EndList.size(); j++) {
    // std::cout << "Reach:\n";
    // StartList[i]->getNode()->dump();
    // EndList[j]->getNode()->dump();
    DAG->setVisitedHeightToFalse();
    if (Reach(StartList[i]->getNode(), EndList[j]->getNode()))
      ReachMatrix[i][j] = 1;
    else
      ReachMatrix[i][j] = 0;
  }
  //  PrintReachMatrix(StartList.size(), EndList.size(), ReachMatrix);
  return ReachMatrix;
}

/// FuseLineages - Fusion of two lineages and updating the LineageList
void ScheduleDAGRRList::FuseLineages(int UmEnd, int V1Start,
                                     SmallVectorImpl < Lineage > &LineageList) {
  // Add a sequencing edge from UmEnd to V1Start
  SDNode *start, *end;
  start = LineageList[UmEnd].getEndNode()->getNode();
  end = LineageList[V1Start].getStartNode()->getNode();
  if (LineageList[UmEnd].getStartNode()->getNode()->getHeight()
        < end->getHeight())
    std::cout << "FL: False lineage fusions: Height1: " 
              << LineageList[UmEnd].getStartNode()->getNode()->getHeight()
              << "Height2: " << end->getHeight() << "\n";
  start->setSequencingEdge(end);
  // LineageList[UmEnd].getEndNode()->getNode()->
  //   setSequencingEdge(LineageList[V1Start].getStartNode()->getNode());

  // Now add the nodes starting from V1 to the end of Um,
  // and delete the lineage starting with V1
  LineageList[UmEnd].AppendNodes(LineageList[V1Start]);
  SmallVectorImpl<Lineage>::iterator I = LineageList.begin(),
                                     E = LineageList.end();
  for ( ; I != E; ++I) {
    Lineage *X = I;
    if (X == &LineageList[V1Start]) {
      // std::cout << "Erase:\n";
      // X->printLineage();
      LineageList.erase(I);
      break;
    }
  }
  if (I == E)
    std::cout << "FL: Unknown lineage!\n";
  /*
  std::cout << "After erase in Fuse Lineages:\n";
  for (unsigned i = 0, e = LineageList.size(); i != e; ++i) {
    std::cout << "Lineage " << i << "\n";
    LineageList[i].printLineage();
  }
  */
}

/// BuildAndColorLIG - Build the Lineage Interference Graph using
/// the ReachMatrix and color it to know the minimum number of
/// registers required (HRB). Return HRB. NumRegs is maximum number of
/// registers equal to the number of colors.  The LIG is colored using
/// virtual registers with maximum registers equal to NumRegs.
int ScheduleDAGRRList::BuildAndColorLIG(int Row, int Column, int **ReachMatrix,
                                        SmallVectorImpl<Lineage> &LineageList) {
  // Construct LIG.
  int **LIG = new int*[Row];
  for (int i = 0; i < Row; i++) {
    LIG[i] = new int[Column];
    for (int j = 0; j < Column; j++)
      LIG[i][j] = 0;
  }
  // std::cout << "Contents of LIG before:\n";
  // PrintMatrix(Row, Column, LIG);

  for (int i = 0; i < Row; i++)
    for (int j = i + 1; j < Column; j++)
      if (ReachMatrix[i][j] && ReachMatrix[j][i])
        LIG[i][j] = LIG[j][i] = 1;

  // std::cout << "Contents of LIG after:\n";
  // PrintMatrix(Row, Column, LIG);

  // Color LIG
  int NumRegs = LineageList.size();
  int NumColors = 0;	// Equal to current HRB
  ColoredNodes.clear();
  // All nodes of LIG are implicitly pushed to stack.
  // Now start popping each node and color it.
  for (int j = NumRegs - 1; j >= 0; j--) {
    Colors.clear();
    for (int i = 0; i < NumColors; i++)
      Colors.push_back(i);
    // Check if there is an edge from popped node to colored nodes and remove
    // the 'i'th color from the list
    for (colorednodes_iterator I = ColoredNodes.begin(), E = ColoredNodes.end();
          I != E; ++I) {
      if (LIG[j][*I])
        Colors.remove(LineageList[*I].getLineageReg());
      }
    // Now color the popped node
    // Reuse the existing color
    if (Colors.size() > 0)
      LineageList[j].setRegToLineage(Colors.front());
    // else get a new color and assign
    else if (Colors.size() == 0) {
      // FIXME: Assign Virtual Registers, maybe from createVirtualRegister().
      LineageList[j].setRegToLineage(NumColors);
      NumColors++;
    }
    ColoredNodes.push_back(j);
  }
  // std::cout << "NumColors: " << NumColors << "\n";
  return NumColors;
}

/// ModifyLineages - Build the reach matrix, fuse lineages,
/// build and color the lineage interference graph
// modlin
int ScheduleDAGRRList::ModifyLineages(SmallVectorImpl< Lineage > &LineageList) {
  SmallVector < SUnit *, 4> StartList;
  SmallVector < SUnit *, 4> EndList;
  int V1Start, UmEnd; // ReachMatrix indices that contain 0, R(v1, um) = 0
  bool LoopFlag = false; // Flag indicating to loop if ReachMatrix has a 0
  int **ReachMatrix;	// The Reach relation matrix
  // int HRB, OldHRB = 0;	// Hueristic Register Bounds

  // Add nodes to StartList and EndList
  for (unsigned i = 0, j = LineageList.size(); i != j; i++) {
    StartList.push_back(LineageList[i].getStartNode());
    EndList.push_back(LineageList[i].getEndNode());
  }
  // std::cout << "Start Nodes:\n";PrintNodes(StartList);
  // std::cout << "End Nodes:\n";PrintNodes(EndList);

  // Build the reach matrix
  ReachMatrix = ReachRelation(ReachMatrix, StartList, EndList);
  // std::cout << "After Reach Relation.\n";
  PrintMatrix(StartList.size(), EndList.size(), ReachMatrix);

  // Check for 0 in ReachMatrix and set LoopFlag to true if there is a 0
  // V1Start and UmEnd are set to the corresponding lineages, R(v1, um) = 0
  for (unsigned i = 0; i < StartList.size(); i++) {
    for (unsigned j = 0; j < EndList.size(); j++)
      if (!ReachMatrix[i][j]) {
        V1Start = i;
        UmEnd = j;
        LoopFlag = true;
        break;
      }
      if (LoopFlag) break;
    }
  // int vik;
  // std::cin >> vik;
  // std::cout << "\n\nEnter while loop NOW:\n";

  while (LoopFlag) {
    LoopFlag = false;
    // std::cout << "R(U1, Vn) = " << ReachMatrix[UmEnd][V1Start] 
    //   << " R(V1, Um) = " << ReachMatrix[V1Start][UmEnd] << "\n";
    if (ReachMatrix[UmEnd][V1Start]) {
      //std::cout << "Fuse Lineages: " << UmEnd << " " << V1Start << "\n";
      // Lineage fusion
      /*
      std::cout << "UmEnd: " << UmEnd << " V1Start: " << V1Start << "\n";
      std::cout << "Before fusion:\n";
      std::cout << "UmEnd: " << UmEnd << "\n";
      LineageList[UmEnd].printLineage();
      std::cout << "V1Start: " << V1Start << "\n";
      LineageList[V1Start].printLineage();
      */
      FuseLineages(UmEnd, V1Start, LineageList);
      /*
      std::cout << "After fusion:\n";
      std::cout << "UmEnd: " << UmEnd << "\n";
      LineageList[UmEnd].printLineage();
      std::cout << "V1Start: " << V1Start << "\n";
      LineageList[V1Start].printLineage();
      */

      // Delete the contents of StartList, EndList; also delete and build
      // a new ReachMatrix
      for (unsigned i = 0, e = StartList.size(); i != e; ++i) {
        StartList.pop_back();
        delete ReachMatrix[i];
      }
      delete ReachMatrix;
      for (unsigned i = 0, e = EndList.size(); i != e; ++i)
        EndList.pop_back();
 
      // Add nodes to StartList and EndList
      for (unsigned i = 0, j = LineageList.size(); i != j; i++) {
        StartList.push_back(LineageList[i].getStartNode());
        EndList.push_back(LineageList[i].getEndNode());
      }
      /*
      std::cout << "Start Nodes:\n";PrintNodes(StartList);
      std::cout << "End Nodes:\n";PrintNodes(EndList);
      std::cout << "After ReachMatrix deletion:\n";
      PrintReachMatrix(StartList.size(), EndList.size(), ReachMatrix);
      */
    
      // Build the reach matrix
      ReachMatrix = ReachRelation(ReachMatrix, StartList, EndList);
      // std::cout << "After ReachRelation in while:\n";
      PrintMatrix(StartList.size(), EndList.size(), ReachMatrix);

      /*
      // Register allocate to each node of the lineage by building the
      // LineageInterferenceGraph
      HRB = BuildAndColorLIG(StartList.size(), EndList.size(),
                              ReachMatrix, LineageList);
      // HRB should always decrease using the lineage fusion, otherwise error
      std::cout << "HRB: " << HRB << "\n";
      if (OldHRB < HRB)
        std::cout << "MODL:HRB increased!\n";
      OldHRB = HRB;*/
      V1Start = 0;
      UmEnd = -1;
    }
    // Check for 0 in ReachMatrix and set LoopFlag to true if there is a 0
    // V1Start and UmEnd are set to the corresponding lineages
    for (unsigned i = V1Start; i < StartList.size(); i++) {
      for (unsigned j = UmEnd + 1; j < EndList.size(); j++)
        if (!ReachMatrix[i][j]) {
          V1Start = i;
          UmEnd = j;
          LoopFlag = true;
          break;
        }
      if (LoopFlag)
        break;
      UmEnd = -1;
    }
    // std::cout << "UmEnd: " << UmEnd << " V1Start: " << V1Start << "\n";
    // std::cin >> vik;
    // std::cout << "\n\nEnter while loop AGAIN:\n";
  }
  // Register allocate to each node of the lineage by building the
  // LineageInterferenceGraph
  return BuildAndColorLIG(StartList.size(), EndList.size(),
                          ReachMatrix, LineageList);
}

/// CheckInList - Check if SUnit is present in StartList/EndList
bool ScheduleDAGRRList::CheckInList(SUnit* Value, std::list<SUnit*> &List) {
  for (std::list<SUnit*>::iterator I = List.begin(), E = List.end();
        I != E; ++I) {
    SUnit* temp = *I;
    if (Value->getNode() == temp->getNode())
      return true;
  }
  return false;
}

/// CheckInRegList - Check if Reg is present in RegAvailable.
bool ScheduleDAGRRList::CheckInRegList(unsigned Value,
                                       std::list<unsigned> &List) {
  for (std::list<unsigned>::iterator I = List.begin(), E = List.end();
        I != E; ++I) {
    unsigned temp = *I;
    if (Value == temp)
      return true;
    }
  return false;
}

/// AddToReadyList - Find SUnit corresponding to SDNode and add to ReadyList
void ScheduleDAGRRList::AddToReadyList(SDNode *N,std::list<SUnitReg> &ReadyList,
                                       SmallVectorImpl<Lineage> &LineageList) {
  SUnit *SU = NULL;
  // Get the SUnit corresponding to SDNode
  for (int i = 0, e = SUnits.size(); i != e; ++i) {
    SUnit *su = &SUnits[i];
    if (su->getNode() == N) {
      SU = su;
      break;
    }
  }

  if (SU == NULL) {
    return;
  }

  bool inserted = false;
  unsigned Rj ;
  // Now add the SUnit to ReadyList by decreasing order by height
  for (std::list<SUnitReg>::iterator I = ReadyList.begin(), E = ReadyList.end();
        I != E; ++I) {
    SUnitReg SR = *I;
    if (N->getHeight() > (SR.first)->getNode()->getHeight()) {
      // find SU in lineages and get the corresponding register
      for (int i = 0, e = LineageList.size(); i != e; ++i) {
        int reg = LineageList[i].findRegInLineage(SU);
        if (reg >= 0) {
          Rj = reg;
          inserted = true;
          break;
        }
      }
      // check if <SU, Rj> pair is already in ReadyList
      bool foundSU = false;
      for (std::list<SUnitReg>::iterator X = ReadyList.begin(),
            Y = ReadyList.end(); X != Y; ++X) {
        SUnitReg sr = *X;
        if (SU->getNode() == (sr.first)->getNode() && Rj == sr.second) {
          foundSU = true;
          break;
        }
      }
      if (!foundSU) {
        // std::cout << "Before:\n";
        // PrintReadyList(ReadyList);
        ReadyList.insert(I, std::make_pair(SU, Rj));
        // std::cout << "After:\n";
        // PrintReadyList(ReadyList);
      }
    }
    if (inserted)
      break;
  }
  if (!inserted) {
    for (int i = 0, e = LineageList.size(); i != e; ++i) {
      int reg = LineageList[i].findRegInLineage(SU);
      if (reg >= 0) {
        Rj = reg;
        break;
      }
    }
    // check if <SU, Rj> pair is already in ReadyList
    bool foundSU = false;
    for (std::list<SUnitReg>::iterator X = ReadyList.begin(),
          Y = ReadyList.end(); X != Y; ++X) {
      SUnitReg sr = *X;
      if (SU->getNode() == (sr.first)->getNode() && Rj == sr.second) {
        foundSU = true;
        break;
      }
    }
    if (!foundSU)
      ReadyList.push_back(std::make_pair(SU, Rj));
  }
}

/// AddSuccessors - Add nodes to the ReadyList all successors of Vi that have
/// all its predeccessors listed and in decreasing height order.
void ScheduleDAGRRList::AddSuccessors(SUnit *Vi, std::list<SUnitReg> &ReadyList,
                                      SmallVectorImpl <Lineage> &LineageList) {
  SDNode *N = Vi->getNode();
  N->setIsScheduled(true);
  // succ_iterator cannot be used as it does not give the successors
  // across FrameIndex.
  for (SDNode::use_iterator I = N->use_begin(), E = N->use_end(); I != E; ) {
    SDNode *curNode = *I;
    if (I.getUse().getValueType() == MVT::Other) {
      DEBUG (errs() << "AddSuccs: Blue edged node\n");
      ++I;
    }
    else if (I.getUse().getValueType() == MVT::Flag) {
      DEBUG (errs() << "AddSuccs: Red edged node\n");
      ++I;
    }
    else if (curNode->getIsScheduled()) {
      std::cout << "AddSuccs: Child listed before parent!\n";
      ++I;
    }
    else {
      DEBUG (errs() << "Normal node\n");
      DEBUG (errs() << curNode->dump());
      // If all predecessors of the node are scheduled then this node is ready to be scheduled
      // add it to ReadyList
      if (DAG->CheckForPredsSched(curNode))
        AddToReadyList(curNode, ReadyList, LineageList);
      ++I;
    }
  }

  // Sequence edged node
  SDNode *se;
  if ((se = N->getSequencingEdge()) && DAG->CheckForPredsSched(se))
    AddToReadyList(se, ReadyList, LineageList);

  // If N is load/store, get its children via FrameIndex
  if (N->getNodeType() == ISD::LOAD || N->getNodeType() == ISD::STORE) {
    for (unsigned j = 0; j < N->getNumOperands(); j++) {
      SDNode *temp = (N->getOperand(j)).getNode();
      // Find FrameIndex node
      if (SDNode::isLeafNode(temp->getNodeType())) {
        for (SDNode::use_iterator X = temp->use_begin(), Y = temp->use_end();
              X != Y; ) {
          if (X.getUse().getValueType() == MVT::Other) {
            DEBUG (errs() << "AddSuccsFI: Blue edged node\n");
            ++X;
          }
          else if (X.getUse().getValueType() == MVT::Flag) {
            DEBUG (errs() << "AddSuccsFI: Red edged node\n");
            ++X;
          }
          else {
            SDNode *FIUses = *X;
            if (FIUses != N && FIUses->getIsScheduled() == false) {
              DEBUG (errs() << FIUses->dump());
              // Generally, FrameIndex has only load/store
              if (DAG->CheckForPredsSched(FIUses))
                AddToReadyList(FIUses, ReadyList, LineageList);
            }
            ++X;
          }
        }
      }
    }
  }
}

/// PrintReadyList - Prints the ReadyList
void ScheduleDAGRRList::PrintReadyList(std::list<SUnitReg> &ReadyList) {
  std::cout << "ReadyList:\n";
  for (std::list<SUnitReg>::iterator I = ReadyList.begin(), E = ReadyList.end();
        I != E; ++I) {
    SUnitReg SR = *I;
    SDNode *N = (SR.first)->getNode();
    N->dump();
    std::cout << N->getHeight() << " " << SR.second << "   ";
  }
}

/// LineageInstructionSequence - Sequencing the nodes of the lineages using
/// the registers allocated to each node during coloring of the
/// Lineage Interference Graph
void ScheduleDAGRRList::LineageInstructionSequence(int HRB,
                          SmallVectorImpl <Lineage> &LineageList) {
  /// ReadyList - Used for Sequencing
  std::list<SUnitReg> ReadyList;
  /// RegAvailable - Set of available registers
  std::list<unsigned> RegAvailable;
  std::list<SUnit*> StartList;
  std::list<SUnit*> EndList;
  SUnit *Vi = NULL;
  unsigned Rj = NULL;

  // Add nodes to StartList and EndList
  for (unsigned i = 0, j = LineageList.size(); i != j; i++) {
    StartList.push_back(LineageList[i].getStartNode());
    EndList.push_back(LineageList[i].getEndNode());
  }

  ReadyList.clear();
  // Add the node that has no predeccessors - this is same as adding
  // the children of EntryToken.
  SDNode *ET = DAG->getEntryNode().getNode();
  for (SDNode::descendant_iterator I = ET->descendant_begin(),
        E = ET->descendant_end(); I != E; ++I) {
    SDNode *temp = *I;
    AddToReadyList(temp, ReadyList, LineageList);
  }

  for (int i = 0, e = LineageList.size(); i != e; ++i) {
    unsigned reg = LineageList[i].getLineageReg();
    bool inAvailable = false;
    for (std::list<unsigned>::iterator I = RegAvailable.begin(),
          E = RegAvailable.end(); I != E; ++I)
      if (reg == *I) {
        inAvailable = true;
         break;
      }
    if (!inAvailable)
      RegAvailable.push_back(reg);
  }
  std::cout << "RegAvailable:\n";
  for (std::list<unsigned>::iterator I = RegAvailable.begin(),
        E = RegAvailable.end(); I != E; ++I)
    std::cout << *I << " ";
  std::cout << "\n";

  DAG->setIsScheduledToFalse();
  // Sequence.reserve(50);

  std::cout << "Sequence:\n";
  while (!ReadyList.empty()) {
    // The ReadyList will have nodes in decreasing height order
    for (std::list<SUnitReg>::iterator I = ReadyList.begin(), E = ReadyList.end();
          I != E; ++I) {
      SUnitReg SR = *I;
      Vi = SR.first;
      Rj = SR.second;
      if (!CheckInList(Vi, StartList) || CheckInRegList(Rj, RegAvailable)) {
        RegAvailable.remove(Rj);
        ReadyList.erase(I);
        Sequence.push_back(Vi);
        Vi->isScheduled = true;
        Vi->getNode()->dump();
        std::cout << Rj << "  ";
        // Add nodes to the ReadyList all successors of Vi that have all its
        // predeccessors listed and in decreasing height order.
        AddSuccessors(Vi, ReadyList, LineageList);
        // FIXME: Returning of registers has problems at the meet of two lineages.
        if (CheckInList(Vi, EndList)) {
          bool inAvailable = false;
          for (std::list<unsigned>::iterator I = RegAvailable.begin(),
                E = RegAvailable.end(); I != E; ++I)
          if (Rj == *I) {
            inAvailable = true;
            break;
          }
          if (!inAvailable)
            RegAvailable.push_back(Rj);
        }
        /*
        SUnitReg s = *(ReadyList.begin());
        (&s.first)->getNode()->dump(); std::cout << s.second << "\n";
        PrintReadyList(ReadyList);
        */
        break;
      }
    }
    /*
    // Check if there are unscheduled nodes. Every node should belong to
    // one of the lineage and if the start of a lineage is scheduled then
    // the entire lineage is scheduled. Therefore check for unscheduled
    // start nodes of all lineages and schedule the entire lineage by adding
    // the start node to ReadyList.
    // FIXME: Enable this once returning of register is correct.
    if (ReadyList.size() == 0) {
      for (int i = 0, e = LineageList.size(); i != e; ++i) {
        SUnit *Start = (LineageList[i].getStartNode());
        if (!Start->isScheduled) {
          ReadyList.push_back(std::
                              make_pair(Start, LineageList[i].getLineageReg()));
          break;
        }
      }
    }
    */
  }
}
  //mycode ends here


/// Schedule - Schedule the DAG using list scheduling.MRIS
void ScheduleDAGRRList::Schedule()
{
  DEBUG(dbgs()
        << "********** List Scheduling BB#" << BB->getNumber()
        << " **********\n");
  NumLiveRegs = 0;
  LiveRegDefs.resize(TRI->getNumRegs(), NULL);
  LiveRegCycles.resize(TRI->getNumRegs(), 0);

  // Build the scheduling graph.
  BuildSchedGraph(NULL);

  // mycode here
  SmallVector< Lineage , 4> LineageList;
  MRISLineageFormation(LineageList);
  DAG->PrintSequenceNodes();

  // Print the lineages
  ///*
  std::cout << "After Lineage formation:\n";
  for (unsigned i = 0, e = LineageList.size(); i != e; ++i) {
  std::cout << "Lineage " << i << "\n";
  LineageList[i].printLineage();
  }
  //*/
  VerifyLineages();

  /*
  for (SmallVectorImpl< Lineage >::iterator I = LineageList.begin(),
        E = LineageList.end(); I != E; ++I) {
    LineageList.erase(I);
  }
  */

  // Lineage Fusion and Lineage Interference Graph
  int HRB = ModifyLineages(LineageList);
  // Print the lineages
  ///*
  std::cout << "After fusions:\n";
  for (unsigned i = 0, e = LineageList.size(); i != e; ++i) {
    std::cout << "Lineage " << i << "\n";
    LineageList[i].printLineage();
  }
  //*/
  VerifyLineages();

  // Recompute height of all nodes as sequencing edges are adding during
  // lineage fusion
  DAG->ComputeHeightOfEveryNode();
  DAG->PrintSequenceNodes();

  // Instruction Sequencing
  LineageInstructionSequence(HRB, LineageList);
  
  // Instructions in sequence
  // dumpSchedule();
  /*
  std::cout << "Sequence:\n";
  for (int i = 0, e = Sequence.size(); i != e; ++i) {
    SUnit *N = Sequence[i];
    N->getNode()->dump();
  }
  */
 // mycode ends here



  DEBUG(for (unsigned su = 0, e = SUnits.size(); su != e; ++su)
        SUnits[su].dumpAll(this));
  Topo.InitDAGTopologicalSorting();

  AvailableQueue->initNodes(SUnits);

  // mycode here
  /*
  std::cout << "Sequence:\n";
  for (int i = 0, e = SUnits.size(); i != e; ++i) {
    SUnit *su = &SUnits[i];
    //if (su->getNode()->getNodeType() != ISD::UNDEF)
      SDNode *N = su->getNode();
    N->dump();
    const TargetInstrDesc &TID = TII->get(N->getMachineOpcode());
    std::cout << "ImpDef:\n";
    const unsigned *ImpDef = TID.getImplicitDefs();
    for (int i = 0, e = TID.getNumImplicitDefs(); i != e; ++ImpDef, ++i) {
      std::cout << *ImpDef << " ";
    }
    std::cout << "\n";
    }
  */

  /*
  std::cout << "Before scheduling:\n";
  SUnit *su = &SUnits[0];
  SDNode *N = su->getNode();
  std::cout << "Node:\n";
  N->dump();
  std::cout << "Succs:\n";
  for (SUnit::succ_iterator I = su->Succs.begin(), E = su->Succs.end();
        I != E; ++I) {
    SUnit *child = I->getSUnit();
  }
  */
  // mycode ends here

  // Execute the actual scheduling loop Top-Down or Bottom-Up as appropriate.
  /*
	if (isBottomUp)
    ListScheduleBottomUp();
  else
    ListScheduleTopDown();
  */

  // mycode here
  /*
  std::cout << "Sequence:\n";
  for (int i = 0, e = SUnits.size(); i != e; ++i) {
    SUnit *su = &SUnits[i];
    // if (su->getNode()->getNodeType() != ISD::UNDEF)
         SDNode *N = su->getNode();
    N->dump();
    const TargetInstrDesc &TID = TII->get(N->getMachineOpcode());
    std::cout << "ImpDef:\n";
    const unsigned *ImpDef = TID.getImplicitDefs();
    for (int i = 0, e = TID.getNumImplicitDefs(); i != e; ++ImpDef, ++i) {
      std::cout << *ImpDef << " ";
    }
    std::cout << "\n";
  }
  */
  // mycode ends here

  AvailableQueue->releaseState();
}

//===----------------------------------------------------------------------===//
//  Bottom-Up Scheduling
//===----------------------------------------------------------------------===//

/// ReleasePred - Decrement the NumSuccsLeft count of a predecessor. Add it to
/// the AvailableQueue if the count reaches zero. Also update its cycle bound.
void ScheduleDAGRRList::ReleasePred(SUnit *SU, const SDep *PredEdge)
{
  SUnit *PredSU = PredEdge->getSUnit();

#ifndef NDEBUG
  if (PredSU->NumSuccsLeft == 0)
    {
      dbgs() << "*** Scheduling failed! ***\n";
      PredSU->dump(this);
      dbgs() << " has been released too many times!\n";
      llvm_unreachable(0);
    }
#endif
  --PredSU->NumSuccsLeft;

  if (!ForceUnitLatencies())
    {
      // Updating predecessor's height. This is now the cycle when the
      // predecessor can be scheduled without causing a pipeline stall.
      PredSU->setHeightToAtLeast(SU->getHeight() + PredEdge->getLatency());
    }

  // If all the node's successors are scheduled, this node is ready
  // to be scheduled. Ignore the special EntrySU node.
  if (PredSU->NumSuccsLeft == 0 && PredSU != &EntrySU)
    {
      PredSU->isAvailable = true;
      AvailableQueue->push(PredSU);
    }
}

void ScheduleDAGRRList::ReleasePredecessors(SUnit *SU, unsigned CurCycle)
{
  // Bottom up: release predecessors
  for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    {
      ReleasePred(SU, &*I);
      if (I->isAssignedRegDep())
        {
          // This is a physical register dependency and it's impossible or
          // expensive to copy the register. Make sure nothing that can
          // clobber the register is scheduled between the predecessor and
          // this node.
          if (!LiveRegDefs[I->getReg()])
            {
              ++NumLiveRegs;
              LiveRegDefs[I->getReg()] = I->getSUnit();
              LiveRegCycles[I->getReg()] = CurCycle;
            }
        }
    }
}

/// ScheduleNodeBottomUp - Add the node to the schedule. Decrement the pending
/// count of its predecessors. If a predecessor pending count is zero, add it to
/// the Available queue.
void ScheduleDAGRRList::ScheduleNodeBottomUp(SUnit *SU, unsigned CurCycle)
{
  DEBUG(dbgs() << "\n*** Scheduling [" << CurCycle << "]: ");
  DEBUG(SU->dump(this));

#ifndef NDEBUG
  if (CurCycle < SU->getHeight())
    DEBUG(dbgs() << "   Height [" << SU->getHeight() << "] pipeline stall!\n");
#endif

  // FIXME: Handle noop hazard.
  SU->setHeightToAtLeast(CurCycle);
  Sequence.push_back(SU);

  AvailableQueue->ScheduledNode(SU);

  ReleasePredecessors(SU, CurCycle);

  // Release all the implicit physical register defs that are live.
  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isAssignedRegDep())
        {
          if (LiveRegCycles[I->getReg()] == I->getSUnit()->getHeight())
            {
              assert(NumLiveRegs > 0 && "NumLiveRegs is already zero!");
              assert(LiveRegDefs[I->getReg()] == SU &&
                     "Physical register dependency violated?");
              --NumLiveRegs;
              LiveRegDefs[I->getReg()] = NULL;
              LiveRegCycles[I->getReg()] = 0;
            }
        }
    }

  SU->isScheduled = true;
}

/// CapturePred - This does the opposite of ReleasePred. Since SU is being
/// unscheduled, incrcease the succ left count of its predecessors. Remove
/// them from AvailableQueue if necessary.
void ScheduleDAGRRList::CapturePred(SDep *PredEdge)
{
  SUnit *PredSU = PredEdge->getSUnit();
  if (PredSU->isAvailable)
    {
      PredSU->isAvailable = false;
      if (!PredSU->isPending)
        AvailableQueue->remove(PredSU);
    }

  assert(PredSU->NumSuccsLeft < UINT_MAX && "NumSuccsLeft will overflow!");
  ++PredSU->NumSuccsLeft;
}

/// UnscheduleNodeBottomUp - Remove the node from the schedule, update its and
/// its predecessor states to reflect the change.
void ScheduleDAGRRList::UnscheduleNodeBottomUp(SUnit *SU)
{
  DEBUG(dbgs() << "*** Unscheduling [" << SU->getHeight() << "]: ");
  DEBUG(SU->dump(this));

  for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    {
      CapturePred(&*I);
      if (I->isAssignedRegDep() && SU->getHeight() == LiveRegCycles[I->getReg()])
        {
          assert(NumLiveRegs > 0 && "NumLiveRegs is already zero!");
          assert(LiveRegDefs[I->getReg()] == I->getSUnit() &&
                 "Physical register dependency violated?");
          --NumLiveRegs;
          LiveRegDefs[I->getReg()] = NULL;
          LiveRegCycles[I->getReg()] = 0;
        }
    }

  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isAssignedRegDep())
        {
          if (!LiveRegDefs[I->getReg()])
            {
              LiveRegDefs[I->getReg()] = SU;
              ++NumLiveRegs;
            }
          if (I->getSUnit()->getHeight() < LiveRegCycles[I->getReg()])
            LiveRegCycles[I->getReg()] = I->getSUnit()->getHeight();
        }
    }

  SU->setHeightDirty();
  SU->isScheduled = false;
  SU->isAvailable = true;
  AvailableQueue->push(SU);
  AvailableQueue->UnscheduledNode(SU);
}

/// BacktrackBottomUp - Backtrack scheduling to a previous cycle specified in
/// BTCycle in order to schedule a specific node.
void ScheduleDAGRRList::BacktrackBottomUp(SUnit *SU, unsigned BtCycle,
    unsigned &CurCycle)
{
  SUnit *OldSU = NULL;
  while (CurCycle > BtCycle)
    {
      OldSU = Sequence.back();
      Sequence.pop_back();
      if (SU->isSucc(OldSU))
        // Don't try to remove SU from AvailableQueue.
        SU->isAvailable = false;
      UnscheduleNodeBottomUp(OldSU);
      --CurCycle;
      AvailableQueue->setCurCycle(CurCycle);
    }

  assert(!SU->isSucc(OldSU) && "Something is wrong!");

  ++NumBacktracks;
}

static bool isOperandOf(const SUnit *SU, SDNode *N)
{
  for (const SDNode *SUNode = SU->getNode(); SUNode;
       SUNode = SUNode->getFlaggedNode())
    {
      if (SUNode->isOperandOf(N))
        return true;
    }
  return false;
}

/// CopyAndMoveSuccessors - Clone the specified node and move its scheduled
/// successors to the newly created node.
SUnit *ScheduleDAGRRList::CopyAndMoveSuccessors(SUnit *SU)
{
  if (SU->getNode()->getFlaggedNode())
    return NULL;

  SDNode *N = SU->getNode();
  if (!N)
    return NULL;

  SUnit *NewSU;
  bool TryUnfold = false;
  for (unsigned i = 0, e = N->getNumValues(); i != e; ++i)
    {
      EVT VT = N->getValueType(i);
      if (VT == MVT::Flag)
        return NULL;
      else if (VT == MVT::Other)
        TryUnfold = true;
    }
  for (unsigned i = 0, e = N->getNumOperands(); i != e; ++i)
    {
      const SDValue &Op = N->getOperand(i);
      EVT VT = Op.getNode()->getValueType(Op.getResNo());
      if (VT == MVT::Flag)
        return NULL;
    }

  if (TryUnfold)
    {
      SmallVector<SDNode*, 2> NewNodes;
      if (!TII->unfoldMemoryOperand(*DAG, N, NewNodes))
        return NULL;

      DEBUG(dbgs() << "Unfolding SU #" << SU->NodeNum << "\n");
      assert(NewNodes.size() == 2 && "Expected a load folding node!");

      N = NewNodes[1];
      SDNode *LoadNode = NewNodes[0];
      unsigned NumVals = N->getNumValues();
      unsigned OldNumVals = SU->getNode()->getNumValues();
      for (unsigned i = 0; i != NumVals; ++i)
        DAG->ReplaceAllUsesOfValueWith(SDValue(SU->getNode(), i), SDValue(N, i));
      DAG->ReplaceAllUsesOfValueWith(SDValue(SU->getNode(), OldNumVals-1),
                                     SDValue(LoadNode, 1));

      // LoadNode may already exist. This can happen when there is another
      // load from the same location and producing the same type of value
      // but it has different alignment or volatileness.
      bool isNewLoad = true;
      SUnit *LoadSU;
      if (LoadNode->getNodeId() != -1)
        {
          LoadSU = &SUnits[LoadNode->getNodeId()];
          isNewLoad = false;
        }
      else
        {
          LoadSU = CreateNewSUnit(LoadNode);
          LoadNode->setNodeId(LoadSU->NodeNum);
          ComputeLatency(LoadSU);
        }

      SUnit *NewSU = CreateNewSUnit(N);
      assert(N->getNodeId() == -1 && "Node already inserted!");
      N->setNodeId(NewSU->NodeNum);

      const TargetInstrDesc &TID = TII->get(N->getMachineOpcode());
      for (unsigned i = 0; i != TID.getNumOperands(); ++i)
        {
          if (TID.getOperandConstraint(i, TOI::TIED_TO) != -1)
            {
              NewSU->isTwoAddress = true;
              break;
            }
        }
      if (TID.isCommutable())
        NewSU->isCommutable = true;
      ComputeLatency(NewSU);

      // Record all the edges to and from the old SU, by category.
      SmallVector<SDep, 4> ChainPreds;
      SmallVector<SDep, 4> ChainSuccs;
      SmallVector<SDep, 4> LoadPreds;
      SmallVector<SDep, 4> NodePreds;
      SmallVector<SDep, 4> NodeSuccs;
      for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
           I != E; ++I)
        {
          if (I->isCtrl())
            ChainPreds.push_back(*I);
          else if (isOperandOf(I->getSUnit(), LoadNode))
            LoadPreds.push_back(*I);
          else
            NodePreds.push_back(*I);
        }
      for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
           I != E; ++I)
        {
          if (I->isCtrl())
            ChainSuccs.push_back(*I);
          else
            NodeSuccs.push_back(*I);
        }

      // Now assign edges to the newly-created nodes.
      for (unsigned i = 0, e = ChainPreds.size(); i != e; ++i)
        {
          const SDep &Pred = ChainPreds[i];
          RemovePred(SU, Pred);
          if (isNewLoad)
            AddPred(LoadSU, Pred);
        }
      for (unsigned i = 0, e = LoadPreds.size(); i != e; ++i)
        {
          const SDep &Pred = LoadPreds[i];
          RemovePred(SU, Pred);
          if (isNewLoad)
            AddPred(LoadSU, Pred);
        }
      for (unsigned i = 0, e = NodePreds.size(); i != e; ++i)
        {
          const SDep &Pred = NodePreds[i];
          RemovePred(SU, Pred);
          AddPred(NewSU, Pred);
        }
      for (unsigned i = 0, e = NodeSuccs.size(); i != e; ++i)
        {
          SDep D = NodeSuccs[i];
          SUnit *SuccDep = D.getSUnit();
          D.setSUnit(SU);
          RemovePred(SuccDep, D);
          D.setSUnit(NewSU);
          AddPred(SuccDep, D);
        }
      for (unsigned i = 0, e = ChainSuccs.size(); i != e; ++i)
        {
          SDep D = ChainSuccs[i];
          SUnit *SuccDep = D.getSUnit();
          D.setSUnit(SU);
          RemovePred(SuccDep, D);
          if (isNewLoad)
            {
              D.setSUnit(LoadSU);
              AddPred(SuccDep, D);
            }
        }

      // Add a data dependency to reflect that NewSU reads the value defined
      // by LoadSU.
      AddPred(NewSU, SDep(LoadSU, SDep::Data, LoadSU->Latency));

      if (isNewLoad)
        AvailableQueue->addNode(LoadSU);
      AvailableQueue->addNode(NewSU);

      ++NumUnfolds;

      if (NewSU->NumSuccsLeft == 0)
        {
          NewSU->isAvailable = true;
          return NewSU;
        }
      SU = NewSU;
    }

  DEBUG(dbgs() << "    Duplicating SU #" << SU->NodeNum << "\n");
  NewSU = CreateClone(SU);

  // New SUnit has the exact same predecessors.
  for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    if (!I->isArtificial())
      AddPred(NewSU, *I);

  // Only copy scheduled successors. Cut them from old node's successor
  // list and move them over.
  SmallVector<std::pair<SUnit *, SDep>, 4> DelDeps;
  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isArtificial())
        continue;
      SUnit *SuccSU = I->getSUnit();
      if (SuccSU->isScheduled)
        {
          SDep D = *I;
          D.setSUnit(NewSU);
          AddPred(SuccSU, D);
          D.setSUnit(SU);
          DelDeps.push_back(std::make_pair(SuccSU, D));
        }
    }
  for (unsigned i = 0, e = DelDeps.size(); i != e; ++i)
    RemovePred(DelDeps[i].first, DelDeps[i].second);

  AvailableQueue->updateNode(SU);
  AvailableQueue->addNode(NewSU);

  ++NumDups;
  return NewSU;
}

/// InsertCopiesAndMoveSuccs - Insert register copies and move all
/// scheduled successors of the given SUnit to the last copy.
void ScheduleDAGRRList::InsertCopiesAndMoveSuccs(SUnit *SU, unsigned Reg,
    const TargetRegisterClass *DestRC,
    const TargetRegisterClass *SrcRC,
    SmallVector<SUnit*, 2> &Copies)
{
  SUnit *CopyFromSU = CreateNewSUnit(NULL);
  CopyFromSU->CopySrcRC = SrcRC;
  CopyFromSU->CopyDstRC = DestRC;

  SUnit *CopyToSU = CreateNewSUnit(NULL);
  CopyToSU->CopySrcRC = DestRC;
  CopyToSU->CopyDstRC = SrcRC;

  // Only copy scheduled successors. Cut them from old node's successor
  // list and move them over.
  SmallVector<std::pair<SUnit *, SDep>, 4> DelDeps;
  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isArtificial())
        continue;
      SUnit *SuccSU = I->getSUnit();
      if (SuccSU->isScheduled)
        {
          SDep D = *I;
          D.setSUnit(CopyToSU);
          AddPred(SuccSU, D);
          DelDeps.push_back(std::make_pair(SuccSU, *I));
        }
    }
  for (unsigned i = 0, e = DelDeps.size(); i != e; ++i)
    RemovePred(DelDeps[i].first, DelDeps[i].second);

  AddPred(CopyFromSU, SDep(SU, SDep::Data, SU->Latency, Reg));
  AddPred(CopyToSU, SDep(CopyFromSU, SDep::Data, CopyFromSU->Latency, 0));

  AvailableQueue->updateNode(SU);
  AvailableQueue->addNode(CopyFromSU);
  AvailableQueue->addNode(CopyToSU);
  Copies.push_back(CopyFromSU);
  Copies.push_back(CopyToSU);

  ++NumPRCopies;
}

/// getPhysicalRegisterVT - Returns the ValueType of the physical register
/// definition of the specified node.
/// FIXME: Move to SelectionDAG?
static EVT getPhysicalRegisterVT(SDNode *N, unsigned Reg,
                                 const TargetInstrInfo *TII)
{
  const TargetInstrDesc &TID = TII->get(N->getMachineOpcode());
  assert(TID.ImplicitDefs && "Physical reg def must be in implicit def list!");
  unsigned NumRes = TID.getNumDefs();
  for (const unsigned *ImpDef = TID.getImplicitDefs(); *ImpDef; ++ImpDef)
    {
      if (Reg == *ImpDef)
        break;
      ++NumRes;
    }
  return N->getValueType(NumRes);
}

/// CheckForLiveRegDef - Return true and update live register vector if the
/// specified register def of the specified SUnit clobbers any "live" registers.
static bool CheckForLiveRegDef(SUnit *SU, unsigned Reg,
                               std::vector<SUnit*> &LiveRegDefs,
                               SmallSet<unsigned, 4> &RegAdded,
                               SmallVector<unsigned, 4> &LRegs,
                               const TargetRegisterInfo *TRI)
{
  bool Added = false;
  if (LiveRegDefs[Reg] && LiveRegDefs[Reg] != SU)
    {
      if (RegAdded.insert(Reg))
        {
          LRegs.push_back(Reg);
          Added = true;
        }
    }
  for (const unsigned *Alias = TRI->getAliasSet(Reg); *Alias; ++Alias)
    if (LiveRegDefs[*Alias] && LiveRegDefs[*Alias] != SU)
      {
        if (RegAdded.insert(*Alias))
          {
            LRegs.push_back(*Alias);
            Added = true;
          }
      }
  return Added;
}

/// DelayForLiveRegsBottomUp - Returns true if it is necessary to delay
/// scheduling of the given node to satisfy live physical register dependencies.
/// If the specific node is the last one that's available to schedule, do
/// whatever is necessary (i.e. backtracking or cloning) to make it possible.
bool ScheduleDAGRRList::DelayForLiveRegsBottomUp(SUnit *SU,
    SmallVector<unsigned, 4> &LRegs)
{
  if (NumLiveRegs == 0)
    return false;

  SmallSet<unsigned, 4> RegAdded;
  // If this node would clobber any "live" register, then it's not ready.
  for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    {
      if (I->isAssignedRegDep())
        CheckForLiveRegDef(I->getSUnit(), I->getReg(), LiveRegDefs,
                           RegAdded, LRegs, TRI);
    }

  for (SDNode *Node = SU->getNode(); Node; Node = Node->getFlaggedNode())
    {
      if (Node->getOpcode() == ISD::INLINEASM)
        {
          // Inline asm can clobber physical defs.
          unsigned NumOps = Node->getNumOperands();
          if (Node->getOperand(NumOps-1).getValueType() == MVT::Flag)
            --NumOps;  // Ignore the flag operand.

          for (unsigned i = InlineAsm::Op_FirstOperand; i != NumOps;)
            {
              unsigned Flags =
                cast<ConstantSDNode>(Node->getOperand(i))->getZExtValue();
              unsigned NumVals = InlineAsm::getNumOperandRegisters(Flags);

              ++i; // Skip the ID value.
              if (InlineAsm::isRegDefKind(Flags) ||
                  InlineAsm::isRegDefEarlyClobberKind(Flags))
                {
                  // Check for def of register or earlyclobber register.
                  for (; NumVals; --NumVals, ++i)
                    {
                      unsigned Reg = cast<RegisterSDNode>(Node->getOperand(i))->getReg();
                      if (TargetRegisterInfo::isPhysicalRegister(Reg))
                        CheckForLiveRegDef(SU, Reg, LiveRegDefs, RegAdded, LRegs, TRI);
                    }
                }
              else
                i += NumVals;
            }
          continue;
        }

      if (!Node->isMachineOpcode())
        continue;
      const TargetInstrDesc &TID = TII->get(Node->getMachineOpcode());
      if (!TID.ImplicitDefs)
        continue;
      for (const unsigned *Reg = TID.ImplicitDefs; *Reg; ++Reg)
        CheckForLiveRegDef(SU, *Reg, LiveRegDefs, RegAdded, LRegs, TRI);
    }
  return !LRegs.empty();
}


/// ListScheduleBottomUp - The main loop of list scheduling for bottom-up
/// schedulers.
void ScheduleDAGRRList::ListScheduleBottomUp()
{
  unsigned CurCycle = 0;

  // Release any predecessors of the special Exit node.
  ReleasePredecessors(&ExitSU, CurCycle);

  // Add root to Available queue.
  if (!SUnits.empty())
    {
      SUnit *RootSU = &SUnits[DAG->getRoot().getNode()->getNodeId()];
      assert(RootSU->Succs.empty() && "Graph root shouldn't have successors!");
      RootSU->isAvailable = true;
      AvailableQueue->push(RootSU);
    }

  // While Available queue is not empty, grab the node with the highest
  // priority. If it is not ready put it back.  Schedule the node.
  SmallVector<SUnit*, 4> NotReady;
  DenseMap<SUnit*, SmallVector<unsigned, 4> > LRegsMap;
  Sequence.reserve(SUnits.size());
  while (!AvailableQueue->empty())
    {
      bool Delayed = false;
      LRegsMap.clear();
      SUnit *CurSU = AvailableQueue->pop();
      while (CurSU)
        {
          SmallVector<unsigned, 4> LRegs;
          if (!DelayForLiveRegsBottomUp(CurSU, LRegs))
            break;
          Delayed = true;
          LRegsMap.insert(std::make_pair(CurSU, LRegs));

          CurSU->isPending = true;  // This SU is not in AvailableQueue right now.
          NotReady.push_back(CurSU);
          CurSU = AvailableQueue->pop();
        }

      // All candidates are delayed due to live physical reg dependencies.
      // Try backtracking, code duplication, or inserting cross class copies
      // to resolve it.
      if (Delayed && !CurSU)
        {
          for (unsigned i = 0, e = NotReady.size(); i != e; ++i)
            {
              SUnit *TrySU = NotReady[i];
              SmallVector<unsigned, 4> &LRegs = LRegsMap[TrySU];

              // Try unscheduling up to the point where it's safe to schedule
              // this node.
              unsigned LiveCycle = CurCycle;
              for (unsigned j = 0, ee = LRegs.size(); j != ee; ++j)
                {
                  unsigned Reg = LRegs[j];
                  unsigned LCycle = LiveRegCycles[Reg];
                  LiveCycle = std::min(LiveCycle, LCycle);
                }
              SUnit *OldSU = Sequence[LiveCycle];
              if (!WillCreateCycle(TrySU, OldSU))
                {
                  BacktrackBottomUp(TrySU, LiveCycle, CurCycle);
                  // Force the current node to be scheduled before the node that
                  // requires the physical reg dep.
                  if (OldSU->isAvailable)
                    {
                      OldSU->isAvailable = false;
                      AvailableQueue->remove(OldSU);
                    }
                  AddPred(TrySU, SDep(OldSU, SDep::Order, /*Latency=*/1,
                                      /*Reg=*/0, /*isNormalMemory=*/false,
                                      /*isMustAlias=*/false, /*isArtificial=*/true));
                  // If one or more successors has been unscheduled, then the current
                  // node is no longer avaialable. Schedule a successor that's now
                  // available instead.
                  if (!TrySU->isAvailable)
                    CurSU = AvailableQueue->pop();
                  else
                    {
                      CurSU = TrySU;
                      TrySU->isPending = false;
                      NotReady.erase(NotReady.begin()+i);
                    }
                  break;
                }
            }

          if (!CurSU)
            {
              // Can't backtrack. If it's too expensive to copy the value, then try
              // duplicate the nodes that produces these "too expensive to copy"
              // values to break the dependency. In case even that doesn't work,
              // insert cross class copies.
              // If it's not too expensive, i.e. cost != -1, issue copies.
              SUnit *TrySU = NotReady[0];
              SmallVector<unsigned, 4> &LRegs = LRegsMap[TrySU];
              assert(LRegs.size() == 1 && "Can't handle this yet!");
              unsigned Reg = LRegs[0];
              SUnit *LRDef = LiveRegDefs[Reg];
              EVT VT = getPhysicalRegisterVT(LRDef->getNode(), Reg, TII);
              const TargetRegisterClass *RC =
                TRI->getMinimalPhysRegClass(Reg, VT);
              const TargetRegisterClass *DestRC = TRI->getCrossCopyRegClass(RC);

              // If cross copy register class is null, then it must be possible copy
              // the value directly. Do not try duplicate the def.
              SUnit *NewDef = 0;
              if (DestRC)
                NewDef = CopyAndMoveSuccessors(LRDef);
              else
                DestRC = RC;
              if (!NewDef)
                {
                  // Issue copies, these can be expensive cross register class copies.
                  SmallVector<SUnit*, 2> Copies;
                  InsertCopiesAndMoveSuccs(LRDef, Reg, DestRC, RC, Copies);
                  DEBUG(dbgs() << "    Adding an edge from SU #" << TrySU->NodeNum
                        << " to SU #" << Copies.front()->NodeNum << "\n");
                  AddPred(TrySU, SDep(Copies.front(), SDep::Order, /*Latency=*/1,
                                      /*Reg=*/0, /*isNormalMemory=*/false,
                                      /*isMustAlias=*/false,
                                      /*isArtificial=*/true));
                  NewDef = Copies.back();
                }

              DEBUG(dbgs() << "    Adding an edge from SU #" << NewDef->NodeNum
                    << " to SU #" << TrySU->NodeNum << "\n");
              LiveRegDefs[Reg] = NewDef;
              AddPred(NewDef, SDep(TrySU, SDep::Order, /*Latency=*/1,
                                   /*Reg=*/0, /*isNormalMemory=*/false,
                                   /*isMustAlias=*/false,
                                   /*isArtificial=*/true));
              TrySU->isAvailable = false;
              CurSU = NewDef;
            }

          assert(CurSU && "Unable to resolve live physical register dependencies!");
        }

      // Add the nodes that aren't ready back onto the available list.
      for (unsigned i = 0, e = NotReady.size(); i != e; ++i)
        {
          NotReady[i]->isPending = false;
          // May no longer be available due to backtracking.
          if (NotReady[i]->isAvailable)
            AvailableQueue->push(NotReady[i]);
        }
      NotReady.clear();

      if (CurSU)
        ScheduleNodeBottomUp(CurSU, CurCycle);
      ++CurCycle;
      AvailableQueue->setCurCycle(CurCycle);
    }

  // Reverse the order if it is bottom up.
  std::reverse(Sequence.begin(), Sequence.end());

#ifndef NDEBUG
  VerifySchedule(isBottomUp);
#endif
}

//===----------------------------------------------------------------------===//
//  Top-Down Scheduling
//===----------------------------------------------------------------------===//

/// ReleaseSucc - Decrement the NumPredsLeft count of a successor. Add it to
/// the AvailableQueue if the count reaches zero. Also update its cycle bound.
void ScheduleDAGRRList::ReleaseSucc(SUnit *SU, const SDep *SuccEdge)
{
  SUnit *SuccSU = SuccEdge->getSUnit();

#ifndef NDEBUG
  if (SuccSU->NumPredsLeft == 0)
    {
      dbgs() << "*** Scheduling failed! ***\n";
      SuccSU->dump(this);
      dbgs() << " has been released too many times!\n";
      llvm_unreachable(0);
    }
#endif
  --SuccSU->NumPredsLeft;

  // If all the node's predecessors are scheduled, this node is ready
  // to be scheduled. Ignore the special ExitSU node.
  if (SuccSU->NumPredsLeft == 0 && SuccSU != &ExitSU)
    {
      SuccSU->isAvailable = true;
      AvailableQueue->push(SuccSU);
    }
}

void ScheduleDAGRRList::ReleaseSuccessors(SUnit *SU)
{
  // Top down: release successors
  for (SUnit::succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      assert(!I->isAssignedRegDep() &&
             "The list-tdrr scheduler doesn't yet support physreg dependencies!");

      ReleaseSucc(SU, &*I);
    }
}

/// ScheduleNodeTopDown - Add the node to the schedule. Decrement the pending
/// count of its successors. If a successor pending count is zero, add it to
/// the Available queue.
void ScheduleDAGRRList::ScheduleNodeTopDown(SUnit *SU, unsigned CurCycle)
{
  DEBUG(dbgs() << "*** Scheduling [" << CurCycle << "]: ");
  DEBUG(SU->dump(this));

  assert(CurCycle >= SU->getDepth() && "Node scheduled above its depth!");
  SU->setDepthToAtLeast(CurCycle);
  Sequence.push_back(SU);

  ReleaseSuccessors(SU);
  SU->isScheduled = true;
  AvailableQueue->ScheduledNode(SU);
}

/// ListScheduleTopDown - The main loop of list scheduling for top-down
/// schedulers.
void ScheduleDAGRRList::ListScheduleTopDown()
{
  unsigned CurCycle = 0;
  AvailableQueue->setCurCycle(CurCycle);

  // Release any successors of the special Entry node.
  ReleaseSuccessors(&EntrySU);

  // All leaves to Available queue.
  for (unsigned i = 0, e = SUnits.size(); i != e; ++i)
    {
      // It is available if it has no predecessors.
      if (SUnits[i].Preds.empty())
        {
          AvailableQueue->push(&SUnits[i]);
          SUnits[i].isAvailable = true;
        }
    }

  // While Available queue is not empty, grab the node with the highest
  // priority. If it is not ready put it back.  Schedule the node.
  Sequence.reserve(SUnits.size());
  while (!AvailableQueue->empty())
    {
      SUnit *CurSU = AvailableQueue->pop();

      if (CurSU)
        ScheduleNodeTopDown(CurSU, CurCycle);
      ++CurCycle;
      AvailableQueue->setCurCycle(CurCycle);
    }

#ifndef NDEBUG
  VerifySchedule(isBottomUp);
#endif
}


//===----------------------------------------------------------------------===//
//                RegReductionPriorityQueue Implementation
//===----------------------------------------------------------------------===//
//
// This is a SchedulingPriorityQueue that schedules using Sethi Ullman numbers
// to reduce register pressure.
//
namespace
{
template<class SF>
class RegReductionPriorityQueue;

/// bu_ls_rr_sort - Priority function for bottom up register pressure
// reduction scheduler.
struct bu_ls_rr_sort : public std::binary_function<SUnit*, SUnit*, bool>
{
  RegReductionPriorityQueue<bu_ls_rr_sort> *SPQ;
  bu_ls_rr_sort(RegReductionPriorityQueue<bu_ls_rr_sort> *spq) : SPQ(spq) {}
  bu_ls_rr_sort(const bu_ls_rr_sort &RHS) : SPQ(RHS.SPQ) {}

  bool operator()(const SUnit* left, const SUnit* right) const;
};

// td_ls_rr_sort - Priority function for top down register pressure reduction
// scheduler.
struct td_ls_rr_sort : public std::binary_function<SUnit*, SUnit*, bool>
{
  RegReductionPriorityQueue<td_ls_rr_sort> *SPQ;
  td_ls_rr_sort(RegReductionPriorityQueue<td_ls_rr_sort> *spq) : SPQ(spq) {}
  td_ls_rr_sort(const td_ls_rr_sort &RHS) : SPQ(RHS.SPQ) {}

  bool operator()(const SUnit* left, const SUnit* right) const;
};

// src_ls_rr_sort - Priority function for source order scheduler.
struct src_ls_rr_sort : public std::binary_function<SUnit*, SUnit*, bool>
{
  RegReductionPriorityQueue<src_ls_rr_sort> *SPQ;
  src_ls_rr_sort(RegReductionPriorityQueue<src_ls_rr_sort> *spq)
      : SPQ(spq) {}
  src_ls_rr_sort(const src_ls_rr_sort &RHS)
      : SPQ(RHS.SPQ) {}

  bool operator()(const SUnit* left, const SUnit* right) const;
};

// hybrid_ls_rr_sort - Priority function for hybrid scheduler.
struct hybrid_ls_rr_sort : public std::binary_function<SUnit*, SUnit*, bool>
{
  RegReductionPriorityQueue<hybrid_ls_rr_sort> *SPQ;
  hybrid_ls_rr_sort(RegReductionPriorityQueue<hybrid_ls_rr_sort> *spq)
      : SPQ(spq) {}
  hybrid_ls_rr_sort(const hybrid_ls_rr_sort &RHS)
      : SPQ(RHS.SPQ) {}

  bool operator()(const SUnit* left, const SUnit* right) const;
};

// ilp_ls_rr_sort - Priority function for ILP (instruction level parallelism)
// scheduler.
struct ilp_ls_rr_sort : public std::binary_function<SUnit*, SUnit*, bool>
{
  RegReductionPriorityQueue<ilp_ls_rr_sort> *SPQ;
  ilp_ls_rr_sort(RegReductionPriorityQueue<ilp_ls_rr_sort> *spq)
      : SPQ(spq) {}
  ilp_ls_rr_sort(const ilp_ls_rr_sort &RHS)
      : SPQ(RHS.SPQ) {}

  bool operator()(const SUnit* left, const SUnit* right) const;
};
}  // end anonymous namespace

/// CalcNodeSethiUllmanNumber - Compute Sethi Ullman number.
/// Smaller number is the higher priority.
static unsigned
CalcNodeSethiUllmanNumber(const SUnit *SU, std::vector<unsigned> &SUNumbers)
{
  unsigned &SethiUllmanNumber = SUNumbers[SU->NodeNum];
  if (SethiUllmanNumber != 0)
    return SethiUllmanNumber;

  unsigned Extra = 0;
  for (SUnit::const_pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    {
      if (I->isCtrl()) continue;  // ignore chain preds
      SUnit *PredSU = I->getSUnit();
      unsigned PredSethiUllman = CalcNodeSethiUllmanNumber(PredSU, SUNumbers);
      if (PredSethiUllman > SethiUllmanNumber)
        {
          SethiUllmanNumber = PredSethiUllman;
          Extra = 0;
        }
      else if (PredSethiUllman == SethiUllmanNumber)
        ++Extra;
    }

  SethiUllmanNumber += Extra;

  if (SethiUllmanNumber == 0)
    SethiUllmanNumber = 1;

  return SethiUllmanNumber;
}

namespace
{
template<class SF>
class RegReductionPriorityQueue : public SchedulingPriorityQueue
{
  std::vector<SUnit*> Queue;
  SF Picker;
  unsigned CurQueueId;
  bool TracksRegPressure;

protected:
  // SUnits - The SUnits for the current graph.
  std::vector<SUnit> *SUnits;

  MachineFunction &MF;
  const TargetInstrInfo *TII;
  const TargetRegisterInfo *TRI;
  const TargetLowering *TLI;
  ScheduleDAGRRList *scheduleDAG;

  // SethiUllmanNumbers - The SethiUllman number for each node.
  std::vector<unsigned> SethiUllmanNumbers;

  /// RegPressure - Tracking current reg pressure per register class.
  ///
  std::vector<unsigned> RegPressure;

  /// RegLimit - Tracking the number of allocatable registers per register
  /// class.
  std::vector<unsigned> RegLimit;

public:
  RegReductionPriorityQueue(MachineFunction &mf,
                            bool tracksrp,
                            const TargetInstrInfo *tii,
                            const TargetRegisterInfo *tri,
                            const TargetLowering *tli)
      : Picker(this), CurQueueId(0), TracksRegPressure(tracksrp),
      MF(mf), TII(tii), TRI(tri), TLI(tli), scheduleDAG(NULL)
  {
    if (TracksRegPressure)
      {
        unsigned NumRC = TRI->getNumRegClasses();
        RegLimit.resize(NumRC);
        RegPressure.resize(NumRC);
        std::fill(RegLimit.begin(), RegLimit.end(), 0);
        std::fill(RegPressure.begin(), RegPressure.end(), 0);
        for (TargetRegisterInfo::regclass_iterator I = TRI->regclass_begin(),
             E = TRI->regclass_end(); I != E; ++I)
          RegLimit[(*I)->getID()] = tli->getRegPressureLimit(*I, MF);
      }
  }

  void initNodes(std::vector<SUnit> &sunits)
  {
    SUnits = &sunits;
    // Add pseudo dependency edges for two-address nodes.
    AddPseudoTwoAddrDeps();
    // Reroute edges to nodes with multiple uses.
    PrescheduleNodesWithMultipleUses();
    // Calculate node priorities.
    CalculateSethiUllmanNumbers();
  }

  void addNode(const SUnit *SU)
  {
    unsigned SUSize = SethiUllmanNumbers.size();
    if (SUnits->size() > SUSize)
      SethiUllmanNumbers.resize(SUSize*2, 0);
    CalcNodeSethiUllmanNumber(SU, SethiUllmanNumbers);
  }

  void updateNode(const SUnit *SU)
  {
    SethiUllmanNumbers[SU->NodeNum] = 0;
    CalcNodeSethiUllmanNumber(SU, SethiUllmanNumbers);
  }

  void releaseState()
  {
    SUnits = 0;
    SethiUllmanNumbers.clear();
    std::fill(RegPressure.begin(), RegPressure.end(), 0);
  }

  unsigned getNodePriority(const SUnit *SU) const
  {
    assert(SU->NodeNum < SethiUllmanNumbers.size());
    unsigned Opc = SU->getNode() ? SU->getNode()->getOpcode() : 0;
    if (Opc == ISD::TokenFactor || Opc == ISD::CopyToReg)
      // CopyToReg should be close to its uses to facilitate coalescing and
      // avoid spilling.
      return 0;
    if (Opc == TargetOpcode::EXTRACT_SUBREG ||
        Opc == TargetOpcode::SUBREG_TO_REG ||
        Opc == TargetOpcode::INSERT_SUBREG)
      // EXTRACT_SUBREG, INSERT_SUBREG, and SUBREG_TO_REG nodes should be
      // close to their uses to facilitate coalescing.
      return 0;
    if (SU->NumSuccs == 0 && SU->NumPreds != 0)
      // If SU does not have a register use, i.e. it doesn't produce a value
      // that would be consumed (e.g. store), then it terminates a chain of
      // computation.  Give it a large SethiUllman number so it will be
      // scheduled right before its predecessors that it doesn't lengthen
      // their live ranges.
      return 0xffff;
    if (SU->NumPreds == 0 && SU->NumSuccs != 0)
      // If SU does not have a register def, schedule it close to its uses
      // because it does not lengthen any live ranges.
      return 0;
    return SethiUllmanNumbers[SU->NodeNum];
  }

  unsigned getNodeOrdering(const SUnit *SU) const
  {
    return scheduleDAG->DAG->GetOrdering(SU->getNode());
  }

  bool empty() const
  {
    return Queue.empty();
  }

  void push(SUnit *U)
  {
    assert(!U->NodeQueueId && "Node in the queue already");
    U->NodeQueueId = ++CurQueueId;
    Queue.push_back(U);
  }

  SUnit *pop()
  {
    if (empty()) return NULL;
    std::vector<SUnit *>::iterator Best = Queue.begin();
    for (std::vector<SUnit *>::iterator I = llvm::next(Queue.begin()),
         E = Queue.end(); I != E; ++I)
      if (Picker(*Best, *I))
        Best = I;
    SUnit *V = *Best;
    if (Best != prior(Queue.end()))
      std::swap(*Best, Queue.back());
    Queue.pop_back();
    V->NodeQueueId = 0;
    return V;
  }

  void remove(SUnit *SU)
  {
    assert(!Queue.empty() && "Queue is empty!");
    assert(SU->NodeQueueId != 0 && "Not in queue!");
    std::vector<SUnit *>::iterator I = std::find(Queue.begin(), Queue.end(),
                                       SU);
    if (I != prior(Queue.end()))
      std::swap(*I, Queue.back());
    Queue.pop_back();
    SU->NodeQueueId = 0;
  }

  bool HighRegPressure(const SUnit *SU) const
  {
    if (!TLI)
      return false;

    for (SUnit::const_pred_iterator I = SU->Preds.begin(),E = SU->Preds.end();
         I != E; ++I)
      {
        if (I->isCtrl())
          continue;
        SUnit *PredSU = I->getSUnit();
        const SDNode *PN = PredSU->getNode();
        if (!PN->isMachineOpcode())
          {
            if (PN->getOpcode() == ISD::CopyFromReg)
              {
                EVT VT = PN->getValueType(0);
                unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
                unsigned Cost = TLI->getRepRegClassCostFor(VT);
                if ((RegPressure[RCId] + Cost) >= RegLimit[RCId])
                  return true;
              }
            continue;
          }
        unsigned POpc = PN->getMachineOpcode();
        if (POpc == TargetOpcode::IMPLICIT_DEF)
          continue;
        if (POpc == TargetOpcode::EXTRACT_SUBREG)
          {
            EVT VT = PN->getOperand(0).getValueType();
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            unsigned Cost = TLI->getRepRegClassCostFor(VT);
            // Check if this increases register pressure of the specific register
            // class to the point where it would cause spills.
            if ((RegPressure[RCId] + Cost) >= RegLimit[RCId])
              return true;
            continue;
          }
        else if (POpc == TargetOpcode::INSERT_SUBREG ||
                 POpc == TargetOpcode::SUBREG_TO_REG)
          {
            EVT VT = PN->getValueType(0);
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            unsigned Cost = TLI->getRepRegClassCostFor(VT);
            // Check if this increases register pressure of the specific register
            // class to the point where it would cause spills.
            if ((RegPressure[RCId] + Cost) >= RegLimit[RCId])
              return true;
            continue;
          }
        unsigned NumDefs = TII->get(PN->getMachineOpcode()).getNumDefs();
        for (unsigned i = 0; i != NumDefs; ++i)
          {
            EVT VT = PN->getValueType(i);
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            if (RegPressure[RCId] >= RegLimit[RCId])
              return true; // Reg pressure already high.
            unsigned Cost = TLI->getRepRegClassCostFor(VT);
            if (!PN->hasAnyUseOfValue(i))
              continue;
            // Check if this increases register pressure of the specific register
            // class to the point where it would cause spills.
            if ((RegPressure[RCId] + Cost) >= RegLimit[RCId])
              return true;
          }
      }

    return false;
  }

  void ScheduledNode(SUnit *SU)
  {
    if (!TracksRegPressure)
      return;

    const SDNode *N = SU->getNode();
    if (!N->isMachineOpcode())
      {
        if (N->getOpcode() != ISD::CopyToReg)
          return;
      }
    else
      {
        unsigned Opc = N->getMachineOpcode();
        if (Opc == TargetOpcode::EXTRACT_SUBREG ||
            Opc == TargetOpcode::INSERT_SUBREG ||
            Opc == TargetOpcode::SUBREG_TO_REG ||
            Opc == TargetOpcode::REG_SEQUENCE ||
            Opc == TargetOpcode::IMPLICIT_DEF)
          return;
      }

    for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
         I != E; ++I)
      {
        if (I->isCtrl())
          continue;
        SUnit *PredSU = I->getSUnit();
        if (PredSU->NumSuccsLeft != PredSU->NumSuccs)
          continue;
        const SDNode *PN = PredSU->getNode();
        if (!PN->isMachineOpcode())
          {
            if (PN->getOpcode() == ISD::CopyFromReg)
              {
                EVT VT = PN->getValueType(0);
                unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
                RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
              }
            continue;
          }
        unsigned POpc = PN->getMachineOpcode();
        if (POpc == TargetOpcode::IMPLICIT_DEF)
          continue;
        if (POpc == TargetOpcode::EXTRACT_SUBREG)
          {
            EVT VT = PN->getOperand(0).getValueType();
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
            continue;
          }
        else if (POpc == TargetOpcode::INSERT_SUBREG ||
                 POpc == TargetOpcode::SUBREG_TO_REG)
          {
            EVT VT = PN->getValueType(0);
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
            continue;
          }
        unsigned NumDefs = TII->get(PN->getMachineOpcode()).getNumDefs();
        for (unsigned i = 0; i != NumDefs; ++i)
          {
            EVT VT = PN->getValueType(i);
            if (!PN->hasAnyUseOfValue(i))
              continue;
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
          }
      }

    // Check for isMachineOpcode() as PrescheduleNodesWithMultipleUses()
    // may transfer data dependencies to CopyToReg.
    if (SU->NumSuccs && N->isMachineOpcode())
      {
        unsigned NumDefs = TII->get(N->getMachineOpcode()).getNumDefs();
        for (unsigned i = 0; i != NumDefs; ++i)
          {
            EVT VT = N->getValueType(i);
            if (!N->hasAnyUseOfValue(i))
              continue;
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            if (RegPressure[RCId] < TLI->getRepRegClassCostFor(VT))
              // Register pressure tracking is imprecise. This can happen.
              RegPressure[RCId] = 0;
            else
              RegPressure[RCId] -= TLI->getRepRegClassCostFor(VT);
          }
      }

    dumpRegPressure();
  }

  void UnscheduledNode(SUnit *SU)
  {
    if (!TracksRegPressure)
      return;

    const SDNode *N = SU->getNode();
    if (!N->isMachineOpcode())
      {
        if (N->getOpcode() != ISD::CopyToReg)
          return;
      }
    else
      {
        unsigned Opc = N->getMachineOpcode();
        if (Opc == TargetOpcode::EXTRACT_SUBREG ||
            Opc == TargetOpcode::INSERT_SUBREG ||
            Opc == TargetOpcode::SUBREG_TO_REG ||
            Opc == TargetOpcode::REG_SEQUENCE ||
            Opc == TargetOpcode::IMPLICIT_DEF)
          return;
      }

    for (SUnit::pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
         I != E; ++I)
      {
        if (I->isCtrl())
          continue;
        SUnit *PredSU = I->getSUnit();
        if (PredSU->NumSuccsLeft != PredSU->NumSuccs)
          continue;
        const SDNode *PN = PredSU->getNode();
        if (!PN->isMachineOpcode())
          {
            if (PN->getOpcode() == ISD::CopyFromReg)
              {
                EVT VT = PN->getValueType(0);
                unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
                RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
              }
            continue;
          }
        unsigned POpc = PN->getMachineOpcode();
        if (POpc == TargetOpcode::IMPLICIT_DEF)
          continue;
        if (POpc == TargetOpcode::EXTRACT_SUBREG)
          {
            EVT VT = PN->getOperand(0).getValueType();
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
            continue;
          }
        else if (POpc == TargetOpcode::INSERT_SUBREG ||
                 POpc == TargetOpcode::SUBREG_TO_REG)
          {
            EVT VT = PN->getValueType(0);
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
            continue;
          }
        unsigned NumDefs = TII->get(PN->getMachineOpcode()).getNumDefs();
        for (unsigned i = 0; i != NumDefs; ++i)
          {
            EVT VT = PN->getValueType(i);
            if (!PN->hasAnyUseOfValue(i))
              continue;
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            if (RegPressure[RCId] < TLI->getRepRegClassCostFor(VT))
              // Register pressure tracking is imprecise. This can happen.
              RegPressure[RCId] = 0;
            else
              RegPressure[RCId] -= TLI->getRepRegClassCostFor(VT);
          }
      }

    // Check for isMachineOpcode() as PrescheduleNodesWithMultipleUses()
    // may transfer data dependencies to CopyToReg.
    if (SU->NumSuccs && N->isMachineOpcode())
      {
        unsigned NumDefs = TII->get(N->getMachineOpcode()).getNumDefs();
        for (unsigned i = NumDefs, e = N->getNumValues(); i != e; ++i)
          {
            EVT VT = N->getValueType(i);
            if (VT == MVT::Flag || VT == MVT::Other)
              continue;
            if (!N->hasAnyUseOfValue(i))
              continue;
            unsigned RCId = TLI->getRepRegClassFor(VT)->getID();
            RegPressure[RCId] += TLI->getRepRegClassCostFor(VT);
          }
      }

    dumpRegPressure();
  }

  void setScheduleDAG(ScheduleDAGRRList *scheduleDag)
  {
    scheduleDAG = scheduleDag;
  }

  void dumpRegPressure() const
  {
    for (TargetRegisterInfo::regclass_iterator I = TRI->regclass_begin(),
         E = TRI->regclass_end(); I != E; ++I)
      {
        const TargetRegisterClass *RC = *I;
        unsigned Id = RC->getID();
        unsigned RP = RegPressure[Id];
        if (!RP) continue;
        DEBUG(dbgs() << RC->getName() << ": " << RP << " / " << RegLimit[Id]
              << '\n');
      }
  }

protected:
  bool canClobber(const SUnit *SU, const SUnit *Op);
  void AddPseudoTwoAddrDeps();
  void PrescheduleNodesWithMultipleUses();
  void CalculateSethiUllmanNumbers();
};

typedef RegReductionPriorityQueue<bu_ls_rr_sort>
BURegReductionPriorityQueue;

typedef RegReductionPriorityQueue<td_ls_rr_sort>
TDRegReductionPriorityQueue;

typedef RegReductionPriorityQueue<src_ls_rr_sort>
SrcRegReductionPriorityQueue;

typedef RegReductionPriorityQueue<hybrid_ls_rr_sort>
HybridBURRPriorityQueue;

typedef RegReductionPriorityQueue<ilp_ls_rr_sort>
ILPBURRPriorityQueue;
}

/// closestSucc - Returns the scheduled cycle of the successor which is
/// closest to the current cycle.
static unsigned closestSucc(const SUnit *SU)
{
  unsigned MaxHeight = 0;
  for (SUnit::const_succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isCtrl()) continue;  // ignore chain succs
      unsigned Height = I->getSUnit()->getHeight();
      // If there are bunch of CopyToRegs stacked up, they should be considered
      // to be at the same position.
      if (I->getSUnit()->getNode() &&
          I->getSUnit()->getNode()->getOpcode() == ISD::CopyToReg)
        Height = closestSucc(I->getSUnit())+1;
      if (Height > MaxHeight)
        MaxHeight = Height;
    }
  return MaxHeight;
}

/// calcMaxScratches - Returns an cost estimate of the worse case requirement
/// for scratch registers, i.e. number of data dependencies.
static unsigned calcMaxScratches(const SUnit *SU)
{
  unsigned Scratches = 0;
  for (SUnit::const_pred_iterator I = SU->Preds.begin(), E = SU->Preds.end();
       I != E; ++I)
    {
      if (I->isCtrl()) continue;  // ignore chain preds
      Scratches++;
    }
  return Scratches;
}

template <typename RRSort>
static bool BURRSort(const SUnit *left, const SUnit *right,
                     const RegReductionPriorityQueue<RRSort> *SPQ)
{
  unsigned LPriority = SPQ->getNodePriority(left);
  unsigned RPriority = SPQ->getNodePriority(right);
  if (LPriority != RPriority)
    return LPriority > RPriority;

  // Try schedule def + use closer when Sethi-Ullman numbers are the same.
  // e.g.
  // t1 = op t2, c1
  // t3 = op t4, c2
  //
  // and the following instructions are both ready.
  // t2 = op c3
  // t4 = op c4
  //
  // Then schedule t2 = op first.
  // i.e.
  // t4 = op c4
  // t2 = op c3
  // t1 = op t2, c1
  // t3 = op t4, c2
  //
  // This creates more short live intervals.
  unsigned LDist = closestSucc(left);
  unsigned RDist = closestSucc(right);
  if (LDist != RDist)
    return LDist < RDist;

  // How many registers becomes live when the node is scheduled.
  unsigned LScratch = calcMaxScratches(left);
  unsigned RScratch = calcMaxScratches(right);
  if (LScratch != RScratch)
    return LScratch > RScratch;

  if (left->getHeight() != right->getHeight())
    return left->getHeight() > right->getHeight();

  if (left->getDepth() != right->getDepth())
    return left->getDepth() < right->getDepth();

  assert(left->NodeQueueId && right->NodeQueueId &&
         "NodeQueueId cannot be zero");
  return (left->NodeQueueId > right->NodeQueueId);
}

// Bottom up
bool bu_ls_rr_sort::operator()(const SUnit *left, const SUnit *right) const
{
  return BURRSort(left, right, SPQ);
}

// Source order, otherwise bottom up.
bool src_ls_rr_sort::operator()(const SUnit *left, const SUnit *right) const
{
  unsigned LOrder = SPQ->getNodeOrdering(left);
  unsigned ROrder = SPQ->getNodeOrdering(right);

  // Prefer an ordering where the lower the non-zero order number, the higher
  // the preference.
  if ((LOrder || ROrder) && LOrder != ROrder)
    return LOrder != 0 && (LOrder < ROrder || ROrder == 0);

  return BURRSort(left, right, SPQ);
}

bool hybrid_ls_rr_sort::operator()(const SUnit *left, const SUnit *right) const
{
  bool LHigh = SPQ->HighRegPressure(left);
  bool RHigh = SPQ->HighRegPressure(right);
  // Avoid causing spills. If register pressure is high, schedule for
  // register pressure reduction.
  if (LHigh && !RHigh)
    return true;
  else if (!LHigh && RHigh)
    return false;
  else if (!LHigh && !RHigh)
    {
      // Low register pressure situation, schedule for latency if possible.
      bool LStall = left->SchedulingPref == Sched::Latency &&
                    SPQ->getCurCycle() < left->getHeight();
      bool RStall = right->SchedulingPref == Sched::Latency &&
                    SPQ->getCurCycle() < right->getHeight();
      // If scheduling one of the node will cause a pipeline stall, delay it.
      // If scheduling either one of the node will cause a pipeline stall, sort
      // them according to their height.
      // If neither will cause a pipeline stall, try to reduce register pressure.
      if (LStall)
        {
          if (!RStall)
            return true;
          if (left->getHeight() != right->getHeight())
            return left->getHeight() > right->getHeight();
        }
      else if (RStall)
        return false;

      // If either node is scheduling for latency, sort them by height and latency
      // first.
      if (left->SchedulingPref == Sched::Latency ||
          right->SchedulingPref == Sched::Latency)
        {
          if (left->getHeight() != right->getHeight())
            return left->getHeight() > right->getHeight();
          if (left->Latency != right->Latency)
            return left->Latency > right->Latency;
        }
    }

  return BURRSort(left, right, SPQ);
}

bool ilp_ls_rr_sort::operator()(const SUnit *left,
                                const SUnit *right) const
{
  bool LHigh = SPQ->HighRegPressure(left);
  bool RHigh = SPQ->HighRegPressure(right);
  // Avoid causing spills. If register pressure is high, schedule for
  // register pressure reduction.
  if (LHigh && !RHigh)
    return true;
  else if (!LHigh && RHigh)
    return false;
  else if (!LHigh && !RHigh)
    {
      // Low register pressure situation, schedule to maximize instruction level
      // parallelism.
      if (left->NumPreds > right->NumPreds)
        return false;
      else if (left->NumPreds < right->NumPreds)
        return false;
    }

  return BURRSort(left, right, SPQ);
}

template<class SF>
bool
RegReductionPriorityQueue<SF>::canClobber(const SUnit *SU, const SUnit *Op)
{
  if (SU->isTwoAddress)
    {
      unsigned Opc = SU->getNode()->getMachineOpcode();
      const TargetInstrDesc &TID = TII->get(Opc);
      unsigned NumRes = TID.getNumDefs();
      unsigned NumOps = TID.getNumOperands() - NumRes;
      for (unsigned i = 0; i != NumOps; ++i)
        {
          if (TID.getOperandConstraint(i+NumRes, TOI::TIED_TO) != -1)
            {
              SDNode *DU = SU->getNode()->getOperand(i).getNode();
              if (DU->getNodeId() != -1 &&
                  Op->OrigNode == &(*SUnits)[DU->getNodeId()])
                return true;
            }
        }
    }
  return false;
}

/// hasCopyToRegUse - Return true if SU has a value successor that is a
/// CopyToReg node.
static bool hasCopyToRegUse(const SUnit *SU)
{
  for (SUnit::const_succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      if (I->isCtrl()) continue;
      const SUnit *SuccSU = I->getSUnit();
      if (SuccSU->getNode() && SuccSU->getNode()->getOpcode() == ISD::CopyToReg)
        return true;
    }
  return false;
}

/// canClobberPhysRegDefs - True if SU would clobber one of SuccSU's
/// physical register defs.
static bool canClobberPhysRegDefs(const SUnit *SuccSU, const SUnit *SU,
                                  const TargetInstrInfo *TII,
                                  const TargetRegisterInfo *TRI)
{
  SDNode *N = SuccSU->getNode();
  unsigned NumDefs = TII->get(N->getMachineOpcode()).getNumDefs();
  const unsigned *ImpDefs = TII->get(N->getMachineOpcode()).getImplicitDefs();
  assert(ImpDefs && "Caller should check hasPhysRegDefs");
  for (const SDNode *SUNode = SU->getNode(); SUNode;
       SUNode = SUNode->getFlaggedNode())
    {
      if (!SUNode->isMachineOpcode())
        continue;
      const unsigned *SUImpDefs =
        TII->get(SUNode->getMachineOpcode()).getImplicitDefs();
      if (!SUImpDefs)
        return false;
      for (unsigned i = NumDefs, e = N->getNumValues(); i != e; ++i)
        {
          EVT VT = N->getValueType(i);
          if (VT == MVT::Flag || VT == MVT::Other)
            continue;
          if (!N->hasAnyUseOfValue(i))
            continue;
          unsigned Reg = ImpDefs[i - NumDefs];
          for (; *SUImpDefs; ++SUImpDefs)
            {
              unsigned SUReg = *SUImpDefs;
              if (TRI->regsOverlap(Reg, SUReg))
                return true;
            }
        }
    }
  return false;
}

/// PrescheduleNodesWithMultipleUses - Nodes with multiple uses
/// are not handled well by the general register pressure reduction
/// heuristics. When presented with code like this:
///
///      N
///    / |
///   /  |
///  U  store
///  |
/// ...
///
/// the heuristics tend to push the store up, but since the
/// operand of the store has another use (U), this would increase
/// the length of that other use (the U->N edge).
///
/// This function transforms code like the above to route U's
/// dependence through the store when possible, like this:
///
///      N
///      ||
///      ||
///     store
///       |
///       U
///       |
///      ...
///
/// This results in the store being scheduled immediately
/// after N, which shortens the U->N live range, reducing
/// register pressure.
///
template<class SF>
void RegReductionPriorityQueue<SF>::PrescheduleNodesWithMultipleUses()
{
  // Visit all the nodes in topological order, working top-down.
  for (unsigned i = 0, e = SUnits->size(); i != e; ++i)
    {
      SUnit *SU = &(*SUnits)[i];
      // For now, only look at nodes with no data successors, such as stores.
      // These are especially important, due to the heuristics in
      // getNodePriority for nodes with no data successors.
      if (SU->NumSuccs != 0)
        continue;
      // For now, only look at nodes with exactly one data predecessor.
      if (SU->NumPreds != 1)
        continue;
      // Avoid prescheduling copies to virtual registers, which don't behave
      // like other nodes from the perspective of scheduling heuristics.
      if (SDNode *N = SU->getNode())
        if (N->getOpcode() == ISD::CopyToReg &&
            TargetRegisterInfo::isVirtualRegister
            (cast<RegisterSDNode>(N->getOperand(1))->getReg()))
          continue;

      // Locate the single data predecessor.
      SUnit *PredSU = 0;
      for (SUnit::const_pred_iterator II = SU->Preds.begin(),
           EE = SU->Preds.end(); II != EE; ++II)
        if (!II->isCtrl())
          {
            PredSU = II->getSUnit();
            break;
          }
      assert(PredSU);

      // Don't rewrite edges that carry physregs, because that requires additional
      // support infrastructure.
      if (PredSU->hasPhysRegDefs)
        continue;
      // Short-circuit the case where SU is PredSU's only data successor.
      if (PredSU->NumSuccs == 1)
        continue;
      // Avoid prescheduling to copies from virtual registers, which don't behave
      // like other nodes from the perspective of scheduling // heuristics.
      if (SDNode *N = SU->getNode())
        if (N->getOpcode() == ISD::CopyFromReg &&
            TargetRegisterInfo::isVirtualRegister
            (cast<RegisterSDNode>(N->getOperand(1))->getReg()))
          continue;

      // Perform checks on the successors of PredSU.
      for (SUnit::const_succ_iterator II = PredSU->Succs.begin(),
           EE = PredSU->Succs.end(); II != EE; ++II)
        {
          SUnit *PredSuccSU = II->getSUnit();
          if (PredSuccSU == SU) continue;
          // If PredSU has another successor with no data successors, for
          // now don't attempt to choose either over the other.
          if (PredSuccSU->NumSuccs == 0)
            goto outer_loop_continue;
          // Don't break physical register dependencies.
          if (SU->hasPhysRegClobbers && PredSuccSU->hasPhysRegDefs)
            if (canClobberPhysRegDefs(PredSuccSU, SU, TII, TRI))
              goto outer_loop_continue;
          // Don't introduce graph cycles.
          if (scheduleDAG->IsReachable(SU, PredSuccSU))
            goto outer_loop_continue;
        }

      // Ok, the transformation is safe and the heuristics suggest it is
      // profitable. Update the graph.
      DEBUG(dbgs() << "    Prescheduling SU #" << SU->NodeNum
            << " next to PredSU #" << PredSU->NodeNum
            << " to guide scheduling in the presence of multiple uses\n");
      for (unsigned i = 0; i != PredSU->Succs.size(); ++i)
        {
          SDep Edge = PredSU->Succs[i];
          assert(!Edge.isAssignedRegDep());
          SUnit *SuccSU = Edge.getSUnit();
          if (SuccSU != SU)
            {
              Edge.setSUnit(PredSU);
              scheduleDAG->RemovePred(SuccSU, Edge);
              scheduleDAG->AddPred(SU, Edge);
              Edge.setSUnit(SU);
              scheduleDAG->AddPred(SuccSU, Edge);
              --i;
            }
        }
outer_loop_continue:
      ;
    }
}

/// AddPseudoTwoAddrDeps - If two nodes share an operand and one of them uses
/// it as a def&use operand. Add a pseudo control edge from it to the other
/// node (if it won't create a cycle) so the two-address one will be scheduled
/// first (lower in the schedule). If both nodes are two-address, favor the
/// one that has a CopyToReg use (more likely to be a loop induction update).
/// If both are two-address, but one is commutable while the other is not
/// commutable, favor the one that's not commutable.
template<class SF>
void RegReductionPriorityQueue<SF>::AddPseudoTwoAddrDeps()
{
  for (unsigned i = 0, e = SUnits->size(); i != e; ++i)
    {
      SUnit *SU = &(*SUnits)[i];
      if (!SU->isTwoAddress)
        continue;

      SDNode *Node = SU->getNode();
      if (!Node || !Node->isMachineOpcode() || SU->getNode()->getFlaggedNode())
        continue;

      unsigned Opc = Node->getMachineOpcode();
      const TargetInstrDesc &TID = TII->get(Opc);
      unsigned NumRes = TID.getNumDefs();
      unsigned NumOps = TID.getNumOperands() - NumRes;
      for (unsigned j = 0; j != NumOps; ++j)
        {
          if (TID.getOperandConstraint(j+NumRes, TOI::TIED_TO) == -1)
            continue;
          SDNode *DU = SU->getNode()->getOperand(j).getNode();
          if (DU->getNodeId() == -1)
            continue;
          const SUnit *DUSU = &(*SUnits)[DU->getNodeId()];
          if (!DUSU) continue;
          for (SUnit::const_succ_iterator I = DUSU->Succs.begin(),
               E = DUSU->Succs.end(); I != E; ++I)
            {
              if (I->isCtrl()) continue;
              SUnit *SuccSU = I->getSUnit();
              if (SuccSU == SU)
                continue;
              // Be conservative. Ignore if nodes aren't at roughly the same
              // depth and height.
              if (SuccSU->getHeight() < SU->getHeight() &&
                  (SU->getHeight() - SuccSU->getHeight()) > 1)
                continue;
              // Skip past COPY_TO_REGCLASS nodes, so that the pseudo edge
              // constrains whatever is using the copy, instead of the copy
              // itself. In the case that the copy is coalesced, this
              // preserves the intent of the pseudo two-address heurietics.
              while (SuccSU->Succs.size() == 1 &&
                     SuccSU->getNode()->isMachineOpcode() &&
                     SuccSU->getNode()->getMachineOpcode() ==
                     TargetOpcode::COPY_TO_REGCLASS)
                SuccSU = SuccSU->Succs.front().getSUnit();
              // Don't constrain non-instruction nodes.
              if (!SuccSU->getNode() || !SuccSU->getNode()->isMachineOpcode())
                continue;
              // Don't constrain nodes with physical register defs if the
              // predecessor can clobber them.
              if (SuccSU->hasPhysRegDefs && SU->hasPhysRegClobbers)
                {
                  if (canClobberPhysRegDefs(SuccSU, SU, TII, TRI))
                    continue;
                }
              // Don't constrain EXTRACT_SUBREG, INSERT_SUBREG, and SUBREG_TO_REG;
              // these may be coalesced away. We want them close to their uses.
              unsigned SuccOpc = SuccSU->getNode()->getMachineOpcode();
              if (SuccOpc == TargetOpcode::EXTRACT_SUBREG ||
                  SuccOpc == TargetOpcode::INSERT_SUBREG ||
                  SuccOpc == TargetOpcode::SUBREG_TO_REG)
                continue;
              if ((!canClobber(SuccSU, DUSU) ||
                   (hasCopyToRegUse(SU) && !hasCopyToRegUse(SuccSU)) ||
                   (!SU->isCommutable && SuccSU->isCommutable)) &&
                  !scheduleDAG->IsReachable(SuccSU, SU))
                {
                  DEBUG(dbgs() << "    Adding a pseudo-two-addr edge from SU #"
                        << SU->NodeNum << " to SU #" << SuccSU->NodeNum << "\n");
                  scheduleDAG->AddPred(SU, SDep(SuccSU, SDep::Order, /*Latency=*/0,
                                                /*Reg=*/0, /*isNormalMemory=*/false,
                                                /*isMustAlias=*/false,
                                                /*isArtificial=*/true));
                }
            }
        }
    }
}

/// CalculateSethiUllmanNumbers - Calculate Sethi-Ullman numbers of all
/// scheduling units.
template<class SF>
void RegReductionPriorityQueue<SF>::CalculateSethiUllmanNumbers()
{
  SethiUllmanNumbers.assign(SUnits->size(), 0);

  for (unsigned i = 0, e = SUnits->size(); i != e; ++i)
    CalcNodeSethiUllmanNumber(&(*SUnits)[i], SethiUllmanNumbers);
}

/// LimitedSumOfUnscheduledPredsOfSuccs - Compute the sum of the unscheduled
/// predecessors of the successors of the SUnit SU. Stop when the provided
/// limit is exceeded.
static unsigned LimitedSumOfUnscheduledPredsOfSuccs(const SUnit *SU,
    unsigned Limit)
{
  unsigned Sum = 0;
  for (SUnit::const_succ_iterator I = SU->Succs.begin(), E = SU->Succs.end();
       I != E; ++I)
    {
      const SUnit *SuccSU = I->getSUnit();
      for (SUnit::const_pred_iterator II = SuccSU->Preds.begin(),
           EE = SuccSU->Preds.end(); II != EE; ++II)
        {
          SUnit *PredSU = II->getSUnit();
          if (!PredSU->isScheduled)
            if (++Sum > Limit)
              return Sum;
        }
    }
  return Sum;
}


// Top down
bool td_ls_rr_sort::operator()(const SUnit *left, const SUnit *right) const
{
  unsigned LPriority = SPQ->getNodePriority(left);
  unsigned RPriority = SPQ->getNodePriority(right);
  bool LIsTarget = left->getNode() && left->getNode()->isMachineOpcode();
  bool RIsTarget = right->getNode() && right->getNode()->isMachineOpcode();
  bool LIsFloater = LIsTarget && left->NumPreds == 0;
  bool RIsFloater = RIsTarget && right->NumPreds == 0;
  unsigned LBonus = (LimitedSumOfUnscheduledPredsOfSuccs(left,1) == 1) ? 2 : 0;
  unsigned RBonus = (LimitedSumOfUnscheduledPredsOfSuccs(right,1) == 1) ? 2 : 0;

  if (left->NumSuccs == 0 && right->NumSuccs != 0)
    return false;
  else if (left->NumSuccs != 0 && right->NumSuccs == 0)
    return true;

  if (LIsFloater)
    LBonus -= 2;
  if (RIsFloater)
    RBonus -= 2;
  if (left->NumSuccs == 1)
    LBonus += 2;
  if (right->NumSuccs == 1)
    RBonus += 2;

  if (LPriority+LBonus != RPriority+RBonus)
    return LPriority+LBonus < RPriority+RBonus;

  if (left->getDepth() != right->getDepth())
    return left->getDepth() < right->getDepth();

  if (left->NumSuccsLeft != right->NumSuccsLeft)
    return left->NumSuccsLeft > right->NumSuccsLeft;

  assert(left->NodeQueueId && right->NodeQueueId &&
         "NodeQueueId cannot be zero");
  return (left->NodeQueueId > right->NodeQueueId);
}

//===----------------------------------------------------------------------===//
//                         Public Constructor Functions
//===----------------------------------------------------------------------===//

llvm::ScheduleDAGSDNodes *
llvm::createBURRListDAGScheduler(SelectionDAGISel *IS, CodeGenOpt::Level)
{
  const TargetMachine &TM = IS->TM;
  const TargetInstrInfo *TII = TM.getInstrInfo();
  const TargetRegisterInfo *TRI = TM.getRegisterInfo();

  BURegReductionPriorityQueue *PQ =
    new BURegReductionPriorityQueue(*IS->MF, false, TII, TRI, 0);
  ScheduleDAGRRList *SD = new ScheduleDAGRRList(*IS->MF, true, false, PQ);
  PQ->setScheduleDAG(SD);
  return SD;
}

llvm::ScheduleDAGSDNodes *
llvm::createTDRRListDAGScheduler(SelectionDAGISel *IS, CodeGenOpt::Level)
{
  const TargetMachine &TM = IS->TM;
  const TargetInstrInfo *TII = TM.getInstrInfo();
  const TargetRegisterInfo *TRI = TM.getRegisterInfo();

  TDRegReductionPriorityQueue *PQ =
    new TDRegReductionPriorityQueue(*IS->MF, false, TII, TRI, 0);
  ScheduleDAGRRList *SD = new ScheduleDAGRRList(*IS->MF, false, false, PQ);
  PQ->setScheduleDAG(SD);
  return SD;
}

llvm::ScheduleDAGSDNodes *
llvm::createSourceListDAGScheduler(SelectionDAGISel *IS, CodeGenOpt::Level)
{
  const TargetMachine &TM = IS->TM;
  const TargetInstrInfo *TII = TM.getInstrInfo();
  const TargetRegisterInfo *TRI = TM.getRegisterInfo();

  SrcRegReductionPriorityQueue *PQ =
    new SrcRegReductionPriorityQueue(*IS->MF, false, TII, TRI, 0);
  ScheduleDAGRRList *SD = new ScheduleDAGRRList(*IS->MF, true, false, PQ);
  PQ->setScheduleDAG(SD);
  return SD;
}

llvm::ScheduleDAGSDNodes *
llvm::createHybridListDAGScheduler(SelectionDAGISel *IS, CodeGenOpt::Level)
{
  const TargetMachine &TM = IS->TM;
  const TargetInstrInfo *TII = TM.getInstrInfo();
  const TargetRegisterInfo *TRI = TM.getRegisterInfo();
  const TargetLowering *TLI = &IS->getTargetLowering();

  HybridBURRPriorityQueue *PQ =
    new HybridBURRPriorityQueue(*IS->MF, true, TII, TRI, TLI);
  ScheduleDAGRRList *SD = new ScheduleDAGRRList(*IS->MF, true, true, PQ);
  PQ->setScheduleDAG(SD);
  return SD;
}

llvm::ScheduleDAGSDNodes *
llvm::createILPListDAGScheduler(SelectionDAGISel *IS, CodeGenOpt::Level)
{
  const TargetMachine &TM = IS->TM;
  const TargetInstrInfo *TII = TM.getInstrInfo();
  const TargetRegisterInfo *TRI = TM.getRegisterInfo();
  const TargetLowering *TLI = &IS->getTargetLowering();

  ILPBURRPriorityQueue *PQ =
    new ILPBURRPriorityQueue(*IS->MF, true, TII, TRI, TLI);
  ScheduleDAGRRList *SD = new ScheduleDAGRRList(*IS->MF, true, true, PQ);
  PQ->setScheduleDAG(SD);
  return SD;
}
