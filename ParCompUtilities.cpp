#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <stack>
#include <bitset>
#include <cmath>
#include <unordered_set>

#include "FSM.h"
#include "ParCompUtilities.h"
#include "TRWSpecialUtilities.h"

using namespace std;

//Helper functions
void generateCompStates(const vector<vector<const State*> > & nStatesList, int index, vector<const State*> & tmp, vector<vector<const State *> > & newStates); //TODO: Optimize!!!!
CompositeState GetInitialCompositeState(const vector<FSM> & automata);
int CompareStateNames(const string & name1, const string & name2);///@TODO: Move this to FSM.h
bool IsCompositeStateProper(const vector<const State *> & states);
map<string, int> GetMasterEventCount(const vector<FSM> & automata);

struct BlockingTransition
{
  CompositeState origin;
  string event;
  CompositeState dest;
};

//TODO: Move these to smart wrapper class for bitset 
void BitSetToFile(bitset<MAX_NUMBER_OF_STATES_ALLOWED> & bits, uint32_t NumberOfValidBits, string & filename)
{
  ofstream outfile(filename);

  uint32_t counter = 0; 
  while(1)
  {

    uint8_t encoded = 0;
    for(int bitsLeft = 7; bitsLeft >= 0 && counter < NumberOfValidBits; bitsLeft--)
    {
      encoded |= (bits[counter++] << bitsLeft);
    }
    outfile << encoded;

    if(counter >= NumberOfValidBits)
      break;
  }
  outfile.close();
}

//TODO: Move these to smart wrapper class for bitset 
void FileToBitSet(bitset<MAX_NUMBER_OF_STATES_ALLOWED> & bits, uint32_t NumberOfValidBits, string & filename)
{
  ifstream infile(filename);
  uint32_t counter = 0;
  char e = infile.get();
  while(infile.good())
  {
    for(int i=0; i<8 && counter < NumberOfValidBits; i++)
      bits[counter++] = (e >> 7-i)&0x1;

    if(counter >= NumberOfValidBits)
      break;

    e = infile.get();
  } 
  cout << counter << " bits read in." << endl;
  infile.close();
}

/*
 * @brief Inverts the transitions in each automaton of an array of automata
 * @param FSMArray The original automata to be inverted
 * @param FSMArray The vector of automata to be populated with the inverted automata
 */
void InvertTransitions( const vector<FSM> & FSMArray, vector<FSM> & FSMArray_inv)
{ 
  //Loop through all automata, creating inverted versions of each 
  for(int i=0; i<FSMArray.size(); i++)
  {
    //Make the inverted automaton
    FSM inv;
    inv.Name = FSMArray[i].Name + "_inv";

    //Add all events to the inverted automaton
    vector<string> events = FSMArray[i].GetAllEvents();
    for(int j=0; j<events.size(); j++)
    {
      inv.AddEvent(events[j]);
    }

    //Loop through all the states, adding inverse transitions
    vector<const State*> states = FSMArray[i].GetAllStates();
    for(int j=0; j<states.size(); j++)
    {
      //Add the state
      inv.GetState(states[j]->stateName);

      //Loop through each set of transitions
      multimap<string, const State*>::const_iterator it = states[j]->transitions.begin();
      for(; it != states[j]->transitions.end(); it++)
      {
        //Add the reverse transition
        inv.AddTransition(it->second->stateName, states[j]->stateName, it->first);
      }

      //Update the marking of the state
      if(states[j]->marking) inv.MarkState(states[j]->stateName);
    }

    //Add the correct initial state
    inv.SetInitialState(FSMArray[i].GetInitialState()->stateName);
  
    //Push the inverted automaton onto the vector
    FSMArray_inv.push_back(inv);
  }
}

/*
 * @brief Calculates the worst-case state space of a parallel composition of these automata
 * @param automata The automata in question
 * @returns The product of the state spaces of each automata
 */
uint64_t GetCartesianStateSpace(const vector<FSM> & automata)
{
  if(automata.size() == 0)
    return 0;

  //Multiply the size of the automata
  uint64_t product = 1;
  for(int i=0; i<automata.size(); i++)
  {
    product *= automata[i].GetNumberOfStates();
  }
  
  return product;
}

///@TODO: Move this to FSM.h
int CompareStateNames(const string & name1, const string & name2)
{
  if(name1.empty() || name2.empty())
    return false;

  //Remove all parentheses
  string temp1(name1), temp2(name2);
  size_t found = temp1.find("(");
  while( found != std::string::npos )
  {
    temp1.erase(temp1.begin()+found);
    found = temp1.find("(");
  }
  found = temp1.find(")");
  while( found != std::string::npos )
  {
    temp1.erase(temp1.begin()+found);
    found = temp1.find(")");
  }
  found = temp2.find("(");
  while( found != std::string::npos )
  {
    temp2.erase(temp2.begin()+found);
    found = temp2.find("(");
  }
  found = temp2.find(")");
  while( found != std::string::npos )
  {
    temp2.erase(temp2.begin()+found);
    found = temp2.find(")");
  }

  return temp1.compare(temp2);
}

///@TODO: Move this to FSM.h
bool AreStateNamesEqual(const string & name1, const string & name2)
{
  return !CompareStateNames(name1, name2);
}

///@TODO: Move this to FSM.h
bool AreAutomataEqual(const FSM & G1, const FSM & G2)
{ 
  //~~~~~~~~~~~~~~~~~~~~~ Compare Event Sets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //Get the sorted event set from each automaton
  vector<string> events1 = G1.GetAllEvents();
  vector<string> events2 = G2.GetAllEvents(); 

  //Check for inconsistencies in event sets
  bool eventfail = false;
  if(events1.size() != events2.size())
  {
    cout << "G1 has " << events1.size() << " events | G2 has " << events2.size() << " events" << endl;
    eventfail = true;
  }
  else
  {
    //Compare each set of events
    for(int i=0; i<events1.size(); i++)
    {
      //Check that strings match
      if(events1[i].compare(events2[i]))
      {
        eventfail = true;
        break;
      }
    }
  } 
  if(eventfail)
  {
    for(int i=0; (i<events1.size() || i<events2.size()); i++)
    {
      cout << ((i<events1.size())?events1[i]:"\t") << " | ";
      cout << ((i<events2.size())?events2[i]:"\t") << endl;
    }
    cout << "Mismatch in event sets" << endl;
    return false;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~ Check initial state ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  if( !AreStateNamesEqual(G1.GetInitialState()->stateName, G2.GetInitialState()->stateName) )
  {
    cout << "G1 initial state: " << G1.GetInitialState()->stateName << endl;
    cout << "G2 initial state: " << G2.GetInitialState()->stateName << endl;
    cout << "Mismatch in initial states." << endl;
    return false;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~~ Compare State Sets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //Get the sorted event set from each automaton
  vector<const State*> states1 = G1.GetAllStates();
  vector<const State*> states2 = G2.GetAllStates(); 
  int i=0, j=0;
  for(; i<states1.size() || j<states2.size();)
  {
    //Check for end of lists
    if(i == states1.size())
    { 
      cout << "G2 has unique state " << states2[j]->stateName << " " << states2[j]->marking << endl;
      multimap<string, const State*>::const_iterator it2 = states2[j]->transitions.begin();
      for(;it2 != states2[j]->transitions.end(); it2++)
        cout << "\t" << it2->first << " " << it2->second->stateName << endl;
      j++;
      continue;
    }
    if(j == states2.size())
    {
      cout << "G1 has unique state " << states1[i]->stateName << " " << states1[i]->marking << endl;
      multimap<string, const State*>::const_iterator it1 = states1[i]->transitions.begin();
      for(;it1 != states1[i]->transitions.end(); it1++)
        cout << "\t" << it1->first << " " << it1->second->stateName << ((it1->second == G1.GetInitialState())?"*":"") << endl;
      i++;
      continue;
    }

    //Compare states
    if(CompareStateNames(states1[i]->stateName, states2[j]->stateName) > 0)
    {
      cout << "G2 has unique state " << states2[j]->stateName << " " << states2[j]->marking << endl;
      multimap<string, const State*>::const_iterator it2 = states2[j]->transitions.begin();
      for(;it2 != states2[j]->transitions.end(); it2++)
        cout << "\t" << it2->first << " " << it2->second->stateName << endl;
      j++;
      continue;      
    }
    else if(CompareStateNames(states1[i]->stateName, states2[j]->stateName) < 0)
    {
      cout << "G1 has unique state " << states1[i]->stateName << " " << states1[i]->marking << endl;
      multimap<string, const State*>::const_iterator it1 = states1[i]->transitions.begin();
      for(;it1 != states1[i]->transitions.end(); it1++)
        cout << "\t" << it1->first << " " << it1->second->stateName << endl;
      i++;
      continue;
    }
    else
    {
      //Check marking 
      if(states1[i]->marking != states2[j]->marking)
      {
        cout << "State " << states1[i]->stateName << " is marked incorrectly." << endl;
        cout << "G1: " << states1[i]->marking << " | G2: " << states2[j]->marking << endl;
        return false;
      }

      //Check transitions
      bool transitionfail = false;
      if(states1[i]->transitions.size() != states2[j]->transitions.size() )
      {
        cout << states1[i]->transitions.size() << " transitions | " << states2[j]->transitions.size() << " transtions" << endl;
        transitionfail = true;
      }
      else
      {
        //Verify events and destinations
        multimap<string, const State*>::const_iterator it1 = states1[i]->transitions.begin();
        multimap<string, const State*>::const_iterator it2 = states2[j]->transitions.begin();
        while( it1 != states1[i]->transitions.end())
        {
          //Check events
          if( it1->first.compare(it2->first) || !AreStateNamesEqual(it1->second->stateName, it2->second->stateName) )
          {
            transitionfail = true;
            break;
          }     
          it1++; 
          it2++;
        }
      }
      if(transitionfail)
      {
        multimap<string, const State*>::const_iterator it1 = states1[i]->transitions.begin();
        multimap<string, const State*>::const_iterator it2 = states2[j]->transitions.begin();

        for(;it1 != states1[i]->transitions.end() || it2 != states2[j]->transitions.end();)
        {
          if(it1 != states1[i]->transitions.end())
          {
            cout << (*it1).first << " " << (*it1).second->stateName;
            it1++;
          }
          else
            cout << "\t";
          cout << " | ";
          if(it2 != states2[j]->transitions.end())
          {
            cout << (*it2).first << " " << (*it2).second->stateName;
            it2++;
          }
          cout << endl;
        }
        cout << "Mismatch in transitions at state " << states1[i]->stateName << endl;
        return false;
      }
    }
    i++; j++;
  }
  return true;
}

bool IsCompositeStateProper(const vector<const State *> & states)
{
  for(int i=0; i<states.size(); i++)
  {   
    if(states[i]->marking == false)
      return false; 
  }
  return true;
}

///@brief Gets the automata count for each event
map<string, int> GetMasterEventCount(const vector<FSM> & automata)
{
  //Get the event union of all the automata
  vector<string> Union = GetEventUnionOfAllAutomata(automata);

  //For each event, count the number of automata containing it
  map<string, int> MasterEventCount;
  for(int i=0; i<Union.size(); i++)
  {
    //Check each automaton for this event
    for(int j=0; j<automata.size(); j++)
    {    
      if(automata[j].HasEvent(Union[i]))
      {
        MasterEventCount[Union[i]]++;
      }
    }
  }

  return MasterEventCount;
}

CompositeState GetInitialCompositeState(const vector<FSM> & automata)
{
  vector<const State *> z0vec;
  for (int i = 0; i < automata.size(); i++)
  {
    z0vec.push_back(automata[i].GetInitialState());
  }
  return z0vec;  
}

void GetAllValidTransitions(const CompositeState & CurrentState, const map<string,int> & MasterEventCount, vector<CompositeTransition> & Transitions)
{
  //Loop through each component state, tallying events up
  map<string,int> LocalEventCount;
  for(int i=0; i<CurrentState.size(); i++)
  {
    //Loop through all the unique events at this state
    multimap<string, const State*>::const_iterator it = CurrentState[i]->transitions.begin();
    for(; it != CurrentState[i]->transitions.end(); it = CurrentState[i]->transitions.upper_bound(it->first))
    {
      //Increment the local event count
      LocalEventCount[it->first]++;
    }
  }

  //Loop through local event union
  map<string,int>::iterator it = LocalEventCount.begin();
  for(; it != LocalEventCount.end(); it++)
  {
    //Skip the event if it is not allowed to occur by rules of parallel composition
    if(it->second != MasterEventCount.find(it->first)->second ) continue;

    //Populate the destination components for valid transitions
    vector<vector<const State*> > nStatesList(CurrentState.size(), vector<const State*>());
    for(int i=0; i<CurrentState.size(); i++)
    {
      //Get the range of local destinations 
      pair<multimap<string, const State*>::const_iterator, multimap<string, const State*>::const_iterator> local_dest;
      local_dest = CurrentState[i]->transitions.equal_range(it->first);

      //If the state does not allow this event to occur
      if(local_dest.first == local_dest.second)
        nStatesList[i].push_back(CurrentState[i]); //Add self loop
      else
      {
        for(; local_dest.first != local_dest.second; local_dest.first++)
          nStatesList[i].push_back(local_dest.first->second); //Add all destinations
      }
    }  
      
    //Add all generated transtions to the results list
    vector<const State*> tmp;
    vector<CompositeState> results;
    generateCompStates(nStatesList, 0, tmp, results);
    for(int k=0; k<results.size(); k++)
    {
      Transitions.push_back(CompositeTransition(it->first, results[k]));
    }  
  }
}

/*
 * @brief Performs a depth-first search of the accessible part of the parallel composition of the referenced automata
 * @param bits Pointer to the memory structure which keeps track of visited states.
 *             When passing in this argument, the search space will be constrained to unmarked states 
 *             After this function returns, bits corresponding to composite states visited will be marked
 *             If this information is not desired, pass in NULL. 
 * @param ProperRuleEnforced Set this to true if exogenous events should be prevented from occurring at unmarked states
 * @param out This FSM will be populated with the result of the ParComp. Pass in NULL if no output is desired.
 * @param verbose Set this to true if output to the terminal is desired
 * @note Populating an output FSM will slow the algorithm and increase the memory footprint.
 * @TODO Should the bitset be wrapped in a smarter class (that checks how it is encoded so no mistakes occur?
 */
uint32_t ParCompAcc(const vector<FSM> & fsm, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits, const bool ProperRuleEnforced, FSM * out, bool verbose)
{
  //Get event union and unique FSM counts 
  map<string, int> MasterEventCount = GetMasterEventCount(fsm);
  if(out)
  {
    out->Name = GetCompositeFsmName(fsm);
    vector<string> Union = GetEventUnionOfAllAutomata(fsm);
    out->AddEvents(Union);
  }

  //Create search memory structure
  SmartStack stack(fsm, bits);
  stack.PushOnStack( GetInitialCompositeState(fsm) );

  uint64_t count = 0;
  while(!stack.IsStackEmpty())
  {
    //Get next state to be explored
    CompositeState CurrentState;
    stack.PopOffStack(CurrentState);
        
    //Update the output FSM
    if(out)
    {
      string statename = GetStateName(CurrentState);
      out->GetState(statename);
      if( IsCompositeStateProper(CurrentState) ) 
        out->MarkState(statename);
    }

    //Determine all transitions allowed by the rules of parallel composition
    vector<CompositeTransition> Transitions;
    GetAllValidTransitions(CurrentState, MasterEventCount, Transitions);

    //Check if state is proper
    bool stateIsProper = IsCompositeStateProper(CurrentState);

    //Loop through all generated transitions
    for(int i=0; i<Transitions.size(); i++)
    {
      //Skip this transition if it is not allowed by the TRW rules
      if (ProperRuleEnforced && !stateIsProper && IsEventExogenous(Transitions[i].event)) 
        continue; 

      //Add the destination state to the stack
      stack.PushOnStack(Transitions[i].destination);

      //Add the transition to the resulting automaton
      if(out)
      {
        string origState = GetStateName(CurrentState);
        string destState = GetStateName(Transitions[i].destination);
        out->AddTransition(origState, destState, Transitions[i].event);
      }
    }
    
    //Print update
    count++;
    if(verbose && !(count % 100)) cout <<"\r" << (count-1) << " states have been explored.    \r"; cout.flush();
  }

  //Print final update
  if(verbose)  cout <<"\r" << count << " states have been explored.    "; cout.flush();
  return count;
}

/*
 * @brief Performs a depth-first search of the coaccessible part of the parallel composition of the referenced automata
 * @param bits Pointer to the memory structure which keeps track of visited states.
 *             When passing in this argument, the search space will be constrained to unmarked states 
 *             After this function returns, bits corresponding to composite states visited will be marked
 *             If this information is not desired, pass in NULL. 
 * @param ProperRuleEnforced Set this to true if exogenous events should be prevented from occurring at unmarked states
 * @TODO Should the bitset be wrapped in a smarter class (that checks how it is encoded so no mistakes occur?
 */
uint32_t ParCompCoAcc(const vector<FSM> & fsm, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits, const bool ProperRuleEnforced)
{
  //~~~~~~~~ Invert the transitions ~~~~~~~~~~~
  vector<FSM> inverted;
  InvertTransitions(fsm, inverted);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //Get unique FSM counts 
  map<string, int> MasterEventCount = GetMasterEventCount(inverted);

  //Create search memory structure
  SmartStack stack(inverted, bits);
  stack.PushOnStack( GetInitialCompositeState(inverted) );

  uint32_t count = 0;
  while(!stack.IsStackEmpty())
  {
    //Get next state to be explored
    CompositeState CurrentState;
    stack.PopOffStack(CurrentState);

    //Determine all transitions allowed by the rules of parallel composition
    vector<CompositeTransition> Transitions;
    GetAllValidTransitions(CurrentState, MasterEventCount, Transitions);

    //Loop through all generated transitions
    for(int i=0; i<Transitions.size(); i++)
    {
      //Skip this transition if it is not allowed by the TRW rules
      if (ProperRuleEnforced && !IsCompositeStateProper(Transitions[i].destination)
         && IsEventExogenous(Transitions[i].event)) 
        continue; 

      //Add the destination state to the stack
      stack.PushOnStack(Transitions[i].destination);
    }
    
    //Print update
    if(!(count++ % 100)) cout <<"\r" << (count-1) << " states have been explored.    \r"; cout.flush();
  }

  //Print final update
  cout <<"\r" << count << " states have been explored.    " << endl;
  return count;
}

/*
 * @brief Performs a depth-first search of the trim part of the parallel composition of the referenced automata
 * @param blocking Pointer to the memory structure which keeps track of blocked states.
 *                 If a blocked state is encountered, certain information about it will be printed to a log
 * @param ProperRuleEnforced Set this to true if exogenous events should be prevented from occurring at unmarked states
 * @param out This FSM will be populated with the result of the search (trim parallel composition of the original automata)
 * @param blockfile The file to which the blocking information will be written
 * @note Populating an output FSM will slow the algorithm and increase the memory footprint.
 * @TODO Should the bitset be wrapped in a smarter class (that checks how it is encoded so no mistakes occur?
 */
uint32_t FindBlockingTransitions(const vector<FSM> & fsm, const bitset<MAX_NUMBER_OF_STATES_ALLOWED> * blockingbits, 
                                 const bool ProperRuleEnforced, FSM * out, ofstream & blockfile)
{
  //This will be used to capture stats on blocking states and transitions
  vector<BlockingTransition> BlockingTransitionSet;

  //Get event union and unique FSM counts 
  map<string, int> MasterEventCount = GetMasterEventCount(fsm);
  if(out)
  {
    vector<string> Union = GetEventUnionOfAllAutomata(fsm);
    out->AddEvents(Union);
  }

  //Create search memory structure
  SmartHeap heap(fsm);
  heap.PushOnHeap(GetInitialCompositeState(fsm), "");

  uint64_t count = 0;
  uint64_t EventCount = 0;
  while(!heap.IsHeapEmpty())
  {
    //Get next state to be explored
    vector<const State*> zvec;
    heap.PopOffHeap(zvec);
    
    //Determine if the state is proper
    bool proper = IsCompositeStateProper(zvec);

    //If a blocking state is found
    if(blockingbits->test(heap.StatePointersToIndex(zvec)))
    {
      blockfile << "Blocking state explored" << endl;
      blockfile << "State name: " << GetStateName(zvec) << endl;
      blockfile << "Marked? " << (proper?"YES":"NO") << endl;
    }

    //Loop through each component state, tallying events up
    map<string,int> LocalEventCount;
    for(int i=0; i<zvec.size(); i++)
    {
      //Loop through all the unique events at this state
      multimap<string, const State*>::const_iterator it = zvec[i]->transitions.begin();
      for(; it != zvec[i]->transitions.end(); it = zvec[i]->transitions.upper_bound(it->first))
      {
        //Increment the local event count
        LocalEventCount[it->first]++;
      }
    }

    //Loop through local event union
    map<string,int>::iterator it = LocalEventCount.begin();
    for(; it != LocalEventCount.end(); it++)
    {
      //Skip the event if it is not allowed to occur by rules of parallel composition
      if(it->second != MasterEventCount[it->first]) continue;

      //Skip the event if it is not allowed to occur to TRW constraints
      if (ProperRuleEnforced && !proper && IsEventExogenous(it->first)) continue; 

      //Populate the destination components for valid transitions
      vector<vector<const State*> > nStatesList(zvec.size(), vector<const State*>());
      for(int i=0; i<zvec.size(); i++)
      {
        //Get the range of local destinations 
        pair<multimap<string, const State*>::const_iterator, multimap<string, const State*>::const_iterator> local_dest;
        local_dest = zvec[i]->transitions.equal_range(it->first);
  
        //If the state does not allow this event to occur
        if(local_dest.first == local_dest.second)
          nStatesList[i].push_back(zvec[i]); //Add self loop
        else
        {
          for(; local_dest.first != local_dest.second; local_dest.first++)
            nStatesList[i].push_back(local_dest.first->second); //Add all destinations
        }
      }  
      
      //Add all generated transtions to the search heap
      vector<const State*> tmp;
      vector<vector<const State*> > results;
      generateCompStates(nStatesList, 0, tmp, results);

      //If a blocking state is found
      if(blockingbits->test(heap.StatePointersToIndex(zvec)))
      {
        for(int k=0; k<results.size(); k++)
        {
          blockfile << "\t" << it->first << "\t" << GetStateName(results[k]) << endl;
        }
      }

      for(int k=0; k<results.size(); k++)
      {
        //Push all transitions on heap to explore blocking states.
        heap.PushOnHeap(results[k], it->first);

        if(blockingbits->test(heap.StatePointersToIndex(results[k])) &&
          !blockingbits->test(heap.StatePointersToIndex(zvec)))
        {
          //Log this transition
          BlockingTransition b;
          b.origin = zvec;
          b.event = it->first;
          b.dest = results[k];
          BlockingTransitionSet.push_back(b);

          //This represents a transition out of the non-blocking space"
          blockfile << "State found which transitions out of non-blocking space" << endl;        
          blockfile << "Blocking state name: " << GetStateName(results[k]) << endl;
          blockfile << "Transition History: " << endl; 
          vector<pair<string, vector<const State*> > > transitionHistory;
          if(heap.GetPathHistory(results[k], transitionHistory))
          {
            for(int l=0; l<transitionHistory.size(); l++)
            {
              blockfile << "\t" << transitionHistory[l].first << "\t" << GetStateName(transitionHistory[l].second) << endl;
            }
            blockfile << endl;
          }
          else
          {
            blockfile << "No history found. Error!!!" << endl;
            return 0;
          }
          EventCount++;
        }
        else if(!blockingbits->test(heap.StatePointersToIndex(results[k])) &&
                blockingbits->test(heap.StatePointersToIndex(zvec)))
        {
          //This is a transition from blocking space to non-blocking. Should never occur
          cout << "PROBLEM!" << endl;
          exit(1);
        }
        else if(!blockingbits->test(heap.StatePointersToIndex(results[k])) &&
                !blockingbits->test(heap.StatePointersToIndex(zvec)))
        {
          //This is a transition within non-blocking space. Only write these transitions to file
          if(out)
          { 
            out->AddTransition(GetStateName(zvec), GetStateName(results[k]), it->first);
            if(proper) out->MarkState(GetStateName(zvec));
          }
        }        
      }  
    }

    if(!(count++ % 100))
    {
      cout <<"\r" << count << " states have been explored. " << EventCount << " blocking transitions have been found    \r"; cout.flush();
    }
  }
  cout <<"\r" << count << " states were explored. " << EventCount << " blocking transitions were found." << endl;

//__________________________________________________
  ofstream transfile("TRANS.txt");
  transfile << BlockingTransitionSet.size() << " blocking transitions" << endl;
  
  for(vector<BlockingTransition>::iterator blockit = BlockingTransitionSet.begin();
      blockit != BlockingTransitionSet.end(); blockit++)
  {
    CompositeState orig = blockit->origin, dest = blockit->dest;    
    for(int i=0; i<orig.size(); i++)
    {
      transfile << orig[i]->stateName;
      if(i != orig.size()-1)
        transfile << "|";
    }
    transfile << endl << blockit->event << endl;
    for(int i=0; i<orig.size(); i++)
    {
      transfile << dest[i]->stateName;
      if(i != dest.size()-1)
        transfile << "|";
    }
    transfile << endl;
  }
  transfile.close();
//__________________________________________________________
  return EventCount;
}

//TODO: Optimize!!!!
void generateCompStates(const vector<vector<const State*> > & nStatesList, int index, vector<const State*> & tmp, vector<vector<const State *> > & newStates)
{
    for (int i = 0; i < nStatesList[index].size(); i++)
    {
        vector<const State*> tmp_i = tmp;
        tmp_i.push_back(nStatesList[index][i]);
        
        if (index == nStatesList.size() - 1)
        {
            newStates.push_back(tmp_i);
        }
        else generateCompStates(nStatesList, index + 1, tmp_i, newStates);
    }
}

/*
 * @brief Generates and returns a state name for the associated composite state
 * @param state The composite state in question
 * @returns The name of the state
 */
string GetStateName( const CompositeState & state)
{
    string name("(");
    int i = 0;
    while (i < state.size() - 1)
    {
        name = name + state[i]-> stateName + ",";
        i++;
    }
    name = name + state[i]-> stateName + ")";
    return name;
}

/*
 * @brief Generates and returns a name for a composite FSM name
 * @param automata The automata being composed
 * @returns The name of the composed automata
 */
string GetCompositeFsmName(const vector<FSM> & automata)
{
  string name;
  for(int i=0; i<automata.size(); i++)
  {
    name += automata[i].Name;
    if(i < automata.size() - 1) name += ",";
  }
  return name;
}

//TODO: Optimize this and replace it with GetEventUnion(const vector<FSM> &)
vector<string> EventUnion(const vector<string> & E1, const vector<string> & E2)
{
    vector<string> ret = E1;
    for (int i = 0; i < E2.size(); i++)
    {
        bool existed = false;
        for (int j = 0; j < E1.size(); j++)
        {
            if (E1[j].compare(E2[i]) == 0)//already exists
            {
                existed = true;
                break;
            }
        }
        if (!existed) ret.push_back(E2[i]);
    }
    return ret;
}























