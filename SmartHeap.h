#ifndef _SMARTHEAP_H_
#define _SMARTHEAP_H_

#include <FSM.h>
#include <map>
#include <vector>
#include <bitset>
#include <queue>

#include "SmartStack.h" //For num bits allowed and StateIndexType
#include "DjikstraNode.h"

class SmartHeap
{
private:
  
  void ConstructorHelper(const std::vector<FSM> & automata);

  std::bitset<MAX_NUMBER_OF_STATES_ALLOWED> * accessed; 
  std::priority_queue<StateIndexWithPathCost> SearchHeap;

  ///@brief Structure for maintaining path history
  map<StateIndexType, DjikstraNode> parents;
  bool PathHistoryInvalid;
  StateIndexType InitialStatePushed;
  StateIndexWithPathCost LastStatePopped;

  ///@brief Parameters for determining encoded indexes
  int numberOfStateMachines;
  unsigned int actualStateSpace;
  std::map<const State*, int> indexMap;
  std::map< std::pair<int,int> , const State*> pointerMap;

  std::vector<int> numbits;
  std::vector<int> offset;
  std::vector<int> accumulator;


    
  ///@brief Convert state pointers to an encoded index
  //StateIndexType StatePointersToIndex(const std::vector<const State*> & states);

  ///@brief Convert an encoded index to state pointers
  //void IndexToStatePointers(std::vector<const State*> & states, StateIndexType encodedindex);

  ///@brief Set bit in search memory
  void SetBit(StateIndexType index);

  ///@brief Test bit in search memory
  bool GetBit(StateIndexType index);

public:
  /* 
   * @brief Constructor
   * @param automata Automata to be composed
   */
  SmartHeap(const std::vector<FSM> & automata);

  /* 
   * @brief Constructor
   * @param automata Automata to be composed
   * @param bits bitset to be used
   */
  SmartHeap(const std::vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits);

  /*
   * @brief Push a set of component states onto the heap
   * @param states Will encode these and push a compressed version on the stack
   * @param event The event of the current transition
   */
  void PushOnHeap(const std::vector<const State*> & states, const std::string & event);

  /*
   * @brief Pops a set of component states off the heap
   * @param states The vector that will be populated with the popped states
   */
  bool PopOffHeap(std::vector<const State*> & states);

  /*
   * @brief Get the heap size
   * @returns Size of heap
   */
  unsigned int GetSizeOfHeap(void);
 
  /*
   * @brief Test if the heap is empty
   * @returns true if heap is empty
   */
  bool IsHeapEmpty(void);

  /*
   * @brief Gets sets of state pointers for every composite state which has not been visited
   * @param states Vector of vector<State*> to be filled
   * @returns Number of states visited
   */
  uint32_t GetUnvisitedStates(std::vector<std::vector<const State*> > & CompositeStates);

  ///@brief Convert an encoded index to state pointers
  void IndexToStatePointers(std::vector<const State*> & states, StateIndexType encodedindex);

  ///@brief Convert state pointers to an encoded index
  StateIndexType StatePointersToIndex(const std::vector<const State*> & states);

  ///@brief Get state history
  bool GetPathHistory(const std::vector<const State*> & composite, std::vector<std::pair<std::string, std::vector<const State*> > > & path);
};

#endif
