#ifndef _SMARTSTACK_H_
#define _SMARTSTACK_H_

#include <FSM.h>
#include <map>
#include <vector>
#include <bitset>
#include <stack>

typedef uint32_t StateIndexType;

#define MAX_NUMBER_OF_STATES_ALLOWED 4294967296 //300000000

class SmartStack
{
private:
  
  void ConstructorHelper(const std::vector<FSM> & automata);

  std::bitset<MAX_NUMBER_OF_STATES_ALLOWED> * accessed; 
  bool deleteBitsOnExit;

  std::stack<StateIndexType> SearchStack;
 
  ///@brief Parameters for determining encoded indexes
  int numberOfStateMachines;
  unsigned int actualStateSpace;
  std::map<const State*, int> indexMap;
  std::map< std::pair<int,int> , const State*> pointerMap;

  std::vector<int> numbits;
  std::vector<int> offset;
  std::vector<int> accumulator;
    
  ///@brief Convert state pointers to an encoded index
  //StateIndexType StatePointersToIndex(const std::vector<State *> & states);

  ///@brief Convert an encoded index to state pointers
  //void IndexToStatePointers(std::vector<State *> & states, StateIndexType encodedindex);

  ///@brief Set bit in search memory
  void SetBit(StateIndexType index);

  ///@brief Test bit in search memory
  bool GetBit(StateIndexType index);

public:
  /* 
   * @brief Constructor
   * @param automata Automata to be composed
   */
  SmartStack(const std::vector<FSM> & automata);

  /* 
   * @brief Constructor
   * @param automata Automata to be composed
   * @param bits bitset to be used
   */
  SmartStack(const std::vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits);

  /*
   * @brief Push a set of component states onto the stack
   * @param states Will encode these and push a compressed version on the stack
   */
  void PushOnStack(const std::vector<const State*> & states);

  /*
   * @brief Pops a set of component states off the stack
   * @param states The vector that will be populated with the popped states
   */
  bool PopOffStack(std::vector<const State*> & states);

  /*
   * @brief Get the stack size
   * @returns Size of stack
   */
  unsigned int GetSizeOfStack(void);
 
  /*
   * @brief Test if the stack is empty
   * @returns true if stack is empty
   */
  bool IsStackEmpty(void);

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

  ///@brief Destructor
  ~SmartStack(void);

};

#endif
