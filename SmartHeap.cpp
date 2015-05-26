#include <SmartHeap.h>

using namespace std;

void SmartHeap::ConstructorHelper(const std::vector<FSM> & automata)
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
SmartHeap::SmartHeap(const std::vector<FSM> & automata)
  :LastStatePopped((StateIndexType)0, 0)
{
  this->accessed = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;
  this->accessed->reset();
  this->ConstructorHelper(automata);
  
  //Djikstra-specific
  this->PathHistoryInvalid = true;
}

/* 
 * @brief Constructor
 * @param automata Automata to be composed
 * @param bits bitset to be used
 */
SmartHeap::SmartHeap(const std::vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits)
  :LastStatePopped((StateIndexType)0, 0)
{
  this->accessed = bits;
  this->ConstructorHelper(automata);

  //Djikstra-specific
  this->PathHistoryInvalid = true;
}


/*
 * @brief Push a set of component states onto the heap
 * @param states Will encode these and push a compressed version on the heap
 */
void SmartHeap::PushOnHeap(const std::vector<const State*> & states, const std::string & event)
{
  //Convert state pointers to encoded index
  StateIndexType index = StatePointersToIndex(states);

  //Only push if it has not been visited
  if(!GetBit(index))
  {   
    //Check to see if the state pushed is the first
    if(this->PathHistoryInvalid == true)
    {
      this->InitialStatePushed = index;
      this->PathHistoryInvalid = false;
      this->LastStatePopped.state = index;
      this->LastStatePopped.pathlength = -1; 
    }
    
    //Enter the state in the parents map
    this->parents.insert(make_pair(index, DjikstraNode(this->LastStatePopped.state, event)));

    //Push the state
    SearchHeap.push(StateIndexWithPathCost(index, this->LastStatePopped.pathlength+1));
    SetBit(index);
/*
  map<StateIndexType, DjikstraNode>::iterator it = parents.find(index);
  cout <<  "State " << index << " pushed\n\tPath length: " << this->LastStatePopped.pathlength+1 << endl;
  cout << "\tParent: " << it->second.parent << "\n\tEvent: " << it->second.event << endl;
  char c; cin>>c;
*/
  }
}

/*
 * @brief Pops a set of component states off the heap
 * @param states The vector that will be populated with the popped states
 */
bool SmartHeap::PopOffHeap(std::vector<const State*> & states)
{
  //If search heap is empty
  if(SearchHeap.size() <= 0)
  {
    states.clear();
    return false;
  }

  //Pop event off top
  StateIndexWithPathCost NextState = SearchHeap.top();
  SearchHeap.pop();

  //Update Djikstra information
  this->LastStatePopped = NextState;
/*
  cout << "Last state popped off: " << this->LastStatePopped.state << " " << this->LastStatePopped.pathlength << endl;
  char c; cin >> c;
*/
  //Convert encoded index into state pointers
  IndexToStatePointers(states, NextState.state);
  return true;
}

 /*
  * @brief Get the heap size
  * @returns Size of heap
  */
 unsigned int SmartHeap::GetSizeOfHeap(void)
 {
   return (unsigned int)SearchHeap.size();
 }
 
 /*
  * @brief Test if the heap is empty
  * @returns true if heap is empty
  */
 bool SmartHeap::IsHeapEmpty(void)
 {
   return SearchHeap.empty();
 }

 /*
  * @brief Calculates the encoded state value for a given bitset index
  */
 void SmartHeap::IndexToStatePointers(std::vector<const State*> & states, StateIndexType encodedindex)
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
StateIndexType SmartHeap::StatePointersToIndex(const std::vector<const State*> & states)
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
uint32_t SmartHeap::GetUnvisitedStates(std::vector<std::vector<const State*> > & CompositeStates)
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
 bool  SmartHeap::GetBit(StateIndexType index)
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
 void SmartHeap::SetBit(StateIndexType index)
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

  ///@brief Get state history
bool SmartHeap::GetPathHistory(const std::vector<const State*> & composite, std::vector<std::pair<std::string, std::vector<const State*> > > & path)
{
  //Check that at least one state has been pushed on
  if(this->PathHistoryInvalid)
  {
    //No state has been pushed
    path.clear();
    return false;
  }

  //Get index
  StateIndexType currentindex = StatePointersToIndex(composite);

  //Trace path backward until initial state is reached
  while(currentindex != this->InitialStatePushed)
  {
    map<StateIndexType, DjikstraNode>::iterator it = this->parents.find(currentindex);
    if(it == this->parents.end())
    {
      //State does not have history - state was never pushed
      path.clear();
      return false;
    }
    
    //Put the composite event in the path
    vector<const State*> destination;
    IndexToStatePointers(destination, currentindex);
    path.insert(path.begin(),make_pair(it->second.event, destination));
  
    //Step backward
    currentindex = it->second.parent;
  }

  //Return successfully
  return true;

}







