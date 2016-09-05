// lineage.h - contains list of SUnits
//===-- llvm/CodeGen/Lineage.h - InstSelection DAG ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Lineage class, which holds list of SUnits in the
// lineage used in Minimum Register Instruction Sequencing.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_LINEAGE_H
#define LLVM_CODEGEN_LINEAGE_H

#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/RecyclingAllocator.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <iterator>

namespace llvm {
  class SelectionDAG;
  class SUnit;
  
  class Lineage {
  public:
    /// SUnitReg - Pair which represents an SUnit with the corresponding
    /// virtual register.
    typedef std::pair < SUnit*, int > SUnitReg;
    /// LineageNodes - The linked list of nodes in the current lineage.
    SmallVector< SUnitReg , 4> LineageNodes;

    /// Start, End - The start and end nodes of the lineage.
    SUnit *Start, *End;

    /// LineageSize - The size of the lineage.
    int LineageSize;

    // void operator=(const Lineage&);  // Do not implement.
    // Lineage(const Lineage&);  // Do not implement.

  public:
    Lineage() {
      setLineageSize(0);
    }
    
    ~Lineage() {
    }

    /// getStartNode - Returns the start node of the lineage.
    SUnit *getStartNode() {
      return Start;
    }

    /// setStartNode - Sets the start node of the lineage.
    void setStartNode(SUnit *N) {
      Start = N;
    }

    /// getEndNode - Returns the last node of the lineage.
    SUnit *getEndNode() {
      return End;
    }

    /// setEndNode - Sets the last node of the lineage.
    void setEndNode(SUnit *N) {
      End = N;
    }

    /// getLineageSize - Returns the number of nodes in the lineage.
    int getLineageSize() const {
      return LineageSize;
    }

    /// setLineageSize - Set the number of nodes in the lineage.
    void setLineageSize(int value) {
      LineageSize = value;
    }

    /// getLineageReg - Return the register assigned to the lineage.
    int getLineageReg() {
      if (LineageNodes.size() > 0)
        return LineageNodes[0].second;
      return -1;
    }

    /// setRegToLineage - Sets the register to the entire nodes of the lineage.
    void setRegToLineage(int Reg) {
      for (int i = 0, e = getLineageSize(); i != e; i++)
        if (LineageNodes[i].second == -1)
          LineageNodes[i].second = Reg;
    }

    /// findRegInLineage - Return the register assigned to the SUnit.
    int findRegInLineage(SUnit *N) {
      // std::cout << "findRegInLineage:\n";
      // N->getNode()->dump();
      for (int i = 0, e = LineageNodes.size(); i != e; ++i) {
        // LineageNodes[i].first.getNode()->dump();
        SUnit *SU = LineageNodes[i].first;
        if (SU->getNode() == N->getNode())
          return LineageNodes[i].second;
      }
      return -1;
    }

    /// addNodeToLineage - Add the SUnitReg pair to the lineage.
    void addNodeToLineage(SUnit *N, unsigned Reg = -1) {
      if (LineageNodes.size() == 0)
        setStartNode(N);
      setEndNode(N);
      SUnitReg SR(N, Reg);
      LineageNodes.push_back(SR);
      // LineageNodes[0]->getNode()->dump();
      setLineageSize(getLineageSize() + 1);
      // std::cout << "LineageSize: " << getLineageSize() << "\n";
    }

    /// AppenNodes - Add nodes of lineage 'end' to the end of the lineage.
    void AppendNodes(Lineage end) {
      /*
      std::cout << "FuseLineages::AppendNodes:\n";
      printLineage();
      std::cout << "Nodes being added are:\n";
      */
      for (int i = 0, e = end.getLineageSize(); i != e; ++i) {
        // end.LineageNodes[i].first.getNode()->dump();
        addNodeToLineage(end.LineageNodes[i].first, end.LineageNodes[i].second);
      }
      /*
      std::cout << "After appending:\n";
      printLineage();
      */
    }

    /// printLineage - Print the SUnitReg pair of nodes in the lineage.
    void printLineage() {
      std::cout << "Print Lineage: size: " << getLineageSize() << "\n";
      for (int i = 0, e = getLineageSize(); i != e; i++) {
          LineageNodes[i].first->getNode()->dump();
          std::cout << LineageNodes[i].second << " ";
      }
    }
  };
}
#endif
