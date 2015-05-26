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

#include "FSM.h"

using namespace std;

//Helper function declarations
string GetNameFromPath (const std::string& filepath);


/*
 * @brief Constructor - Reads an FSM from a .fsm file at @param filepath
 * @param filepath The location of the file to be imported
 */
FSM::FSM(const string & filepath)
  :initial(NULL)
{
  //Open desired file
  ifstream fin(filepath);
  if(!fin)
  {
    cout << "Opening file " <<  filepath << " was unsuccessful! Exiting!" << endl;
    exit(1);
  }

  //Assign the name based on the name of the file
  this->Name = GetNameFromPath(filepath);

  //Get the number of states
  int numstates;
  fin >> numstates;
  if(!fin)
  {
    cout << "Error in reading " << filepath << ". Exiting!" << endl;
    exit(1);
  }

  //~~~~ Loop through all states in file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  for (int i = 0; i < numstates; i++)
  {
    //Read in state information
    string statename;
    int marking, numtransitions;
    fin >> statename >> marking >> numtransitions;
    if(!fin)
    {
      cout << "Error in reading " << filepath;
      cout << " after " << i << " states. Exiting!" << endl;
      exit(1);
    }

    //Add the state to the automaton
    this->GetState(statename);

    //Set initial state if it is the first state
    if(i==0) this->SetInitialState(statename);

    //Mark it if relevant
    if(marking) this->MarkState(statename);
   

    //Loop through the transitions, adding them to the automaton
    for (int j = 0; j < numtransitions; j++)
    {
      string event, dest;
      char controllable, observable;
      fin >> event >> dest >> controllable >> observable;
      if(!fin)
      {
        cout << "Error in reading " << filepath;
        cout << " at state " << statename << ". Exiting!" << endl;
        exit(1);
      }
      this->AddTransition(statename, dest, event);
    }
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //Check for any events at the end of the file
  int numberevents;
  string junk;
  fin >> numberevents >> junk;
  if(!fin)
  {
    //There are no events to add
    return;
  }

  //Get the events
  for(int i=0; i< numberevents; i++)
  {
    string event;
    char controllable, observable;
    fin >> event >> controllable >> observable;
    if(!fin || controllable != 'c' || observable != 'o')
    {
      cout << "Error in reading extra events in " << filepath << ". Exiting!" << endl;
      exit(1);
    }
    this->AddEvent(event);
  }
}

/*
 * @brief Copy constructor -  allocates entirely new heap memory
 * @param original The FSM to be copied
 */
FSM::FSM(const FSM & original)
  :initial(NULL)
{
  //Copy name
  this->Name = original.Name;

  //Copy event set
  this->AddEvents( original.GetAllEvents() );

  //Loop through states
  vector<const State*> OriginalStates = original.GetAllStates();
  for(int i=0; i<OriginalStates.size(); i++)
  {
    //Make state (in case of no transitions from or to)
    this->GetState(OriginalStates[i]->stateName);

    //Loop through transitions
    multimap<string,const State*>::const_iterator it = OriginalStates[i]->transitions.begin();
    for(; it != OriginalStates[i]->transitions.end(); it++)
    {
      //Add the same transition
      this->AddTransition(OriginalStates[i]->stateName, it->second->stateName, it->first);
    }

    //Mark the state 
    if(OriginalStates[i]->marking) this->MarkState(OriginalStates[i]->stateName);
  }

  //Copy initial state
  this->SetInitialState(original.GetInitialState()->stateName);
}

/*
 * @brief Assignment operator -  allocates entirely new heap memory
 * @param original The FSM to be copied
 */
FSM& FSM::operator=(const FSM & original)
{
  //Clear the FSM
  this->Clear();

  //Copy name
  this->Name = original.Name;

  //Copy event set
  this->AddEvents( original.GetAllEvents() );

  //Loop through states
  vector<const State*> OriginalStates = original.GetAllStates();
  for(int i=0; i<OriginalStates.size(); i++)
  {
    //Make state (in case of no transitions from or to)
    this->GetState(OriginalStates[i]->stateName);

    //Loop through transitions
    multimap<string,const State*>::const_iterator it = OriginalStates[i]->transitions.begin();
    for(; it != OriginalStates[i]->transitions.end(); it++)
    {
      //Add the same transition
      this->AddTransition(OriginalStates[i]->stateName, it->second->stateName, it->first);
    }

    //Mark the state 
    if(OriginalStates[i]->marking) this->MarkState(OriginalStates[i]->stateName);
  }

  //Copy initial state
  this->SetInitialState(original.GetInitialState()->stateName);
}

/*
 * @brief Sets the initial state to the state with indicated name
 * @param statename The name of the desired state
 * @note Creates the referenced state if not found
 */
void FSM::SetInitialState(const string & statename)
{
  //Set the initial state pointer
  initial = this->getStatePtr(statename);
}

/*
 * @brief Marks the state with indicated name
 * @param statename The name of the desired state
 * @note Creates the referenced state if not found
 */
void FSM::MarkState(const string & statename)
{
  //Find the state
  State* s = this->getStatePtr(statename);
  
  //Mark the state
  s->marking = true;
}

/*
 * @brief Adds a transition to the automaton from origin to dest with indicated event
 * @param origin The state at which the transition starts. 
 * @param dest The state at which the transition ends. 
 * @param event The event associated with the transition (is added to the event set).
 * @note If either state does not yet exist, it will be created
 */
void FSM::AddTransition(const string & origin, const string & dest, const string & event)
{
    //Get the state pointers for each of the states
    State* originPtr = this->getStatePtr(origin);
    State* destPtr = this->getStatePtr(dest);
    
    //Add a transition to the automaton
    this->generateTransitionPtr(originPtr, (const State*)destPtr, event);
}

/*
 * @brief Adds an event to the event set. Does not create a transition
 * @param event The event to be added
 */
void FSM::AddEvent(const string & event)
{
  //Insert the event
  this->events.insert(event);  
}

/*
 * @brief Adds all events to the event set. Does not create any transition
 * @param newevents The events to be added
 */
void FSM::AddEvents(const vector<string> & newevents)
{
  //Loop through events, inserting each into the event set
  for(int i=0; i<newevents.size(); i++)
  {
    this->events.insert(newevents[i]);
  }
}

/*
 * @brief Returns a constant pointer to the state of the indicated name
 * @param statename The name of the referenced state
 * @note If a state of this name does not already exist, it will be created
 * @returns A pointer to the existing or newly-created state
 */
const State* FSM::GetState(const string & statename)
{
  //Make the pointer const
  return (const State*)this->getStatePtr(statename);
}

/*
 * @brief Returns a copied vector of all state pointers, sorted by state name
 * @note Does not modify internal data
 * @returns Pointers to all the states
 */
vector<const State*> FSM::GetAllStates(void) const
{
  //Vector to be returned
  vector<const State*> allstates;

  //Populate vector
  map<string,State*>::const_iterator it = this->states.begin();
  for(; it != this->states.end(); it++)
  {
    allstates.push_back((const State*)it->second);
  }
  
  //Return the vector
  return allstates;
}

/*
 * @brief Returns a copied vector of all events in alphanumeric order
 * @note Does not modify internal data
 * @returns All events
 * @TODO: Should to be changed to "const set<string> & GetAllEvents(void) const" 
 */
vector<string> FSM::GetAllEvents(void) const
{
  //Vector to be returned
  vector<string> allevents;

  //Populate vector
  set<string>::const_iterator it = this->events.begin();
  for(; it != this->events.end(); it++)
  {
    allevents.push_back(*it);
  }
  
  //Return the vector
  return allevents; 
}

///@brief Returns the number of states
int FSM::GetNumberOfStates(void) const
{
  //Return the number of states
  return this->states.size();
}

///@brief Returns the number of events
int FSM::GetNumberOfEvents(void) const
{
  //Return the number of events
  return this->events.size();
}

///@brief Returns true if an FSM contains the given event
bool FSM::HasEvent(const string & event) const
{
  return (this->events.find(event) != this->events.end());
}

///@brief Returns the pointer to the initial state
const State* FSM::GetInitialState(void) const
{
  return initial;
}

void FSM::Export(const string & filepath) const
{
  //Open the file to be written
  ofstream fout(filepath);
  if(!fout)
  {
    cout << "Write to " << filepath << " failed! Aborting write." << endl;
    return;
  }

  //Print the number of states
  fout << this->states.size() << endl;

  //~~~ Print the initial state first to maintain the convention ~~~~//
  State* s = this->initial;

  //Print name, marking, num transitions
  fout << s->stateName << "\t";
  fout << s->marking << "\t";
  fout << s->GetNumberOfTransitions() << endl;

  //Print all transitions
  multimap<string, const State*>::const_iterator it2 = s->transitions.begin();
  for (; it2 != s->transitions.end(); it2++)
  {
    //Print event and destination
    fout << it2->first << "\t" << it2->second->stateName << "\t";
    fout << 'c' << "\t" << 'o' << endl;
  }
  fout << endl;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


  //~~~~~~~~ Loop through states, printing each one ~~~~~~~~~~//
  map<string,State*>::const_iterator it1 = this->states.begin();
  for (; it1 != this->states.end(); it1++)
  {            
    State* s = it1->second;

    if(s == this->initial) continue;

    //Print name, marking, num transitions
    fout << s->stateName << "\t";
    fout << s->marking << "\t";
    fout << s->GetNumberOfTransitions() << endl;

    //Print all transitions
    multimap<string, const State*>::const_iterator it2 = s->transitions.begin();
    for (; it2 != s->transitions.end(); it2++)
    {
        //Print event and destination
        fout << it2->first << "\t" << it2->second->stateName << "\t";
        fout << 'c' << "\t" << 'o' << endl;
    }
    fout << endl;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  
  //Print number of events
  fout << this->events.size() << " events" << endl;

  //Print all events
  set<string>::const_iterator it = this->events.begin();
  for(; it != this->events.end(); it++)
  {
    fout << *it << "\tc\to" << endl;
  }
}

///@brief Clear all information about the automaton
void FSM::Clear(void)
{
  //Clear the event set
  this->events.clear();

  //Clear the initial state
  this->initial = NULL;

  //Clear the states
  map<string, State*>::const_iterator it  = this->states.begin();
  for(; it != this->states.end(); it++)
  { 
    delete it->second;
  }
  this->states.clear();

  //Clear the automaton's name
  this->Name.clear();
}

///@brief Adds a transition from origin to destination
void FSM::generateTransitionPtr(State* origin, const State* dest, const string & event)
{
  //Add the transition to the origin state
  origin->addTran(event, dest);

  //Add the event to the event set
  this->AddEvent(event);
}

///@brief Gets a pointer to the given state - creates one if necessary
State* FSM::getStatePtr(const string & statename)
{
  //Search the map for state pointer
  map<string,State*>::iterator state = this->states.find(statename);

  if(state != this->states.end())
  {
    //If it already exists, return the pointer
    return state->second;
  }
  else
  {
    //Otherwise, create one on the heap
    State* s = new State(statename);

    //Add it to the map
    states.insert(make_pair(statename, s));
    
    //If it is the first state in, make it the initial by default
    if(!this->initial) this->initial = s;

    //Return the pointer
    return s;
  }
}

/*
 * @brief Helper function - gets the name of an automaton from a filepath
 * @param filepath The file path
 * @returns The name of the automaton
 */
string GetNameFromPath (const std::string& filepath)
{
  unsigned slashPos = filepath.find_last_of("/\\");
  unsigned extensionPos = filepath.find_last_of(".");
  
  return filepath.substr(slashPos+1, extensionPos-slashPos - 1);
}

/*
 * @brief Destructor
 */
FSM::~FSM(void)
{
  this->Clear();
}

