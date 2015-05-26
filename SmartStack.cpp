#include <SmartStack.h>

using namespace std;

void SmartStack::ConstructorHelper(const std::vector<FSM> & automata)
{
 //Resize all vectors to have proper size
  numberOfStateMachines = automata.size();
  numbits.resize(numberOfStateMachines);
  offset.resize(numberOfStateMachines);
  accumulator.resize(numberOfStateMachines);
 
  //Populate parameter vectors
 	for( int i=(numberOfStateMachines - 1); i>=0; i-- )
 	{
 	  if( i == (numberOfStateMachines - 1) )
 	  {
 	    offset[i] = 0;
 	    accumulator[i] = 1;
 	  }
 	  else
 	  {
 	    offset[i] = offset[i+1] + numbits[i+1];
 	    this->accumulator[i] = this->accumulator[i+1] * (automata[i+1].GetNumberOfStates());
 	  }
 		
 		numbits[i] = ceil( log2( automata[i].GetNumberOfStates() ) );
 	}
 	
 	actualStateSpace = accumulator[0]*automata[0].GetNumberOfStates();

  //Populate the pointer-to-index and index-to-pointer maps
  for(int i=0; i<numberOfStateMachines; i++)
  {
    vector<const State*> states = automata[i].GetAllStates();
    for(int j=0; j<states.size(); j++)
    {
      indexMap.insert(make_pair(states[j], j));
      pointerMap.insert(make_pair(make_pair(i,j), states[j]));
    }
  }
}

/* 
 * @brief Constructor
 * @param automata Automata to be composed
 */
SmartStack::SmartStack(const std::vector<FSM> & automata)
{
  //Allocate a bitset on the heap
  this->accessed = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;
  this->deleteBitsOnExit = true;
  this->accessed->reset();

  //Call a helper function which will configure class for use
  this->ConstructorHelper(automata);
}

/* 
 * @brief Constructor
 * @param automata Automata to be composed
 * @param bits bitset to be used
 */
SmartStack::SmartStack(const std::vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits)
{
  if(bits)
  {
    this->accessed = bits;
    this->deleteBitsOnExit = false;
  }
  else
  {
    this->accessed = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;
    this->deleteBitsOnExit = true;
    this->accessed->reset();
  }
  this->ConstructorHelper(automata);
}


/*
 * @brief Push a set of component states onto the stack
 * @param states Will encode these and push a compressed version on the stack
 */
void SmartStack::PushOnStack(const std::vector<const State*> & states)
{
  //Convert state pointers to encoded index
  StateIndexType index = StatePointersToIndex(states);

  if(!GetBit(index))
  {
    SearchStack.push(index);
    SetBit(index);
  }
}

/*
 * @brief Pops a set of component states off the stack
 * @param states The vector that will be populated with the popped states
 */
bool SmartStack::PopOffStack(std::vector<const State*> & states)
{
  //If search stack is empty
  if(SearchStack.size() <= 0)
  {
    states.clear();
    return false;
  }

  //Pop event off top
  StateIndexType NextStateIndex = SearchStack.top();
  SearchStack.pop();

  //Convert encoded index into state pointers
  IndexToStatePointers(states, NextStateIndex);
  return true;
}

 /*
  * @brief Get the stack size
  * @returns Size of stack
  */
 unsigned int SmartStack::GetSizeOfStack(void)
 {
   return (unsigned int)SearchStack.size();
 }
 
 /*
  * @brief Test if the stack is empty
  * @returns true if stack is empty
  */
 bool SmartStack::IsStackEmpty(void)
 {
   return SearchStack.empty();
 }

 /*
  * @brief Calculates the encoded state value for a given bitset index
  */
 void SmartStack::IndexToStatePointers(std::vector<const State*> & states, StateIndexType encodedindex)
 {	
 	unsigned int stateIndex; 	
 	for(unsigned int i=0; i < numberOfStateMachines; i++)
 	{
 		stateIndex = encodedindex/accumulator[i];
 		states.push_back(pointerMap[make_pair(i,stateIndex)]);
 		
 		encodedindex -= (stateIndex * accumulator[i]);	
 	}
 	return;
 }

///@brief Convert state pointers to an encoded index
StateIndexType SmartStack::StatePointersToIndex(const std::vector<const State*> & states)
{
	StateIndexType index = 0;
 	unsigned int scalar;

 	for(unsigned int i=0; i < numberOfStateMachines; i++)
 	{
 	  scalar = (indexMap[states[i]]) & ((1 << numbits[i]) - 1);
 	  index += scalar * accumulator[i];
 	}
 	return index;
}

/*
 * @brief Gets sets of state pointers for every composite state which has not been visited
 * @param states Vector of vector<State*> to be filled
 * @returns Number of states visited
 */
uint32_t SmartStack::GetUnvisitedStates(std::vector<std::vector<const State*> > & CompositeStates)
{
  for(StateIndexType i=0; i<MAX_NUMBER_OF_STATES_ALLOWED; i++)
  {
    if(!GetBit(i))
    {
      vector<const State*> pointers;
      IndexToStatePointers(pointers, i);
      CompositeStates.push_back(pointers);
    }
  }
  return CompositeStates.size();
}
 /*
  * @brief Determine the value of a state's bitset entry
  * @param index A composite state encoding
  * @returns bit entry in bitset
  */
 bool  SmartStack::GetBit(StateIndexType index)
 {
   try
   {
     return accessed->test(index);
   }
   catch(out_of_range)
   {
     cerr << "Bitset out of range when testing state " << index << ". Exiting." << endl;
     exit(1);
   }
 }
 
 /*
  * @brief Set a state's bitset entry to true
  * @param encodedState A composite state encoding
  * @returns bit entry in bitset
  */
 void SmartStack::SetBit(StateIndexType index)
 {
   try
   {
     accessed->set(index);
   }
   catch(out_of_range)
   {
     cerr << "Bitset out of range when setting state " << index << ". Exiting." << endl;
     exit(1);
   }
 }

/*
 * @brief Destructor
 */  
SmartStack::~SmartStack(void)
{
  //Free the bits if a valid pointer was not passed in 
  if(this->deleteBitsOnExit)
  {
    delete this->accessed;
  }

  //Clear the stack
  while(!this->SearchStack.empty())
  {
    this->SearchStack.pop();
  }

  //Clear the encoding memory devices
  this->indexMap.clear();
  this->pointerMap.clear();
  this->numbits.clear();
  this->offset.clear();
  this->accumulator.clear();
}

