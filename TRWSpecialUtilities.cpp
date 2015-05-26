#include "TRWSpecialUtilities.h"
#include "ParCompUtilities.h"

using namespace std;

/*
 * @brief Helper function to concisely get union of all events
 * @param automata All automata from which the union will be created
 * @returns The union of all the events in all automata
 */
vector<string> GetEventUnionOfAllAutomata(const vector<FSM> & automata)
{
  //Create a set of events to store the union
  vector<string> UnionSoFar;

  //Loop through all automata
  for(int i=0; i<automata.size(); i++)
  {
    //Get union of "union-so-far" and next automata
    vector<string> temp = EventUnion(UnionSoFar, automata[i].GetAllEvents());

    //Update union so far
    UnionSoFar = temp;
  }

  //Return the result
  return UnionSoFar;
}


/*
 * @brief Determines whether a DDC special automaton is needed to constrain a parallel composition
 * @param automata The automata to be composed
 * @returns True if a DDC spec automaton is needed to constrain the parcomp operation
 */
bool IsDDCSpecUseful(const vector<FSM> & automata)
{
  vector<string> Union = GetEventUnionOfAllAutomata(automata);
  vector<string>::iterator ddc = find(Union.begin(), Union.end(), "DDC");
  vector<string>::iterator dc = find(Union.begin(), Union.end(), "dc");
  return ( (ddc != Union.end()) && (dc != Union.end()) );
}


/*
 * @brief Determines whether a fault rule special automaton is needed to constrain a parallel composition
 * @param automata The automata to be composed
 * @returns True if a fault rule spec automaton is needed to constrain the parcomp operation
 */
bool IsFaultSpecUseful(const vector<FSM> & automata)
{
  vector<string> Union = GetEventUnionOfAllAutomata(automata);
  vector<string>::iterator fpdl = find(Union.begin(), Union.end(), "fpdl");
  vector<string>::iterator fddl = find(Union.begin(), Union.end(), "fddl");
  return ( (fpdl != Union.end()) || (fddl != Union.end()) );
}


/*
 * @brief Adds a "dc" transition wherever there exists a "DC" transition in any of the automata
 * @note  All "dc" transitions will have the same origin and destination states as the respective "DC" transitions
 * @note  The event set of any given automaton is updated if alias event is added to that automaton
 * @param automata The set of automata to be updated
 */
int AddDcAliasTransitions(FSM & automaton)
{
  //Keep track of how many transitions are added
  int TransitionsAddedCount = 0;
  
  //Loop through all states in this automaton
  vector<const State*> states = automaton.GetAllStates();
  for(int i=0; i<states.size(); i++)
  {   
    //Loop through all transitions at this state
    multimap<string,const State*>::const_iterator it = states[i]->transitions.begin();
    for(; it != states[i]->transitions.end(); it++)
    {     
      //Check if the event is a "DC" transition
      if(!(*it).first.compare("DC"))
      {
        //Add an alias transition
        automaton.AddTransition(states[i]->stateName, (*it).second->stateName, "dc");
        
        //Increment counter
        TransitionsAddedCount++;
      }      
    }     
  }
  return TransitionsAddedCount;
}



/*
 * @brief   Creates an automaton which is used to satisfy the DDC rule in a parallel composition of the given set of automata
 * @param   automata The set of automata to be composed
 * @note    These automata must have been updated with alias DC transitions for the result of this function to be meaningful
 * @returns The automaton which will be used to satisfy the DDC rule
 */
FSM CreateDDCSpecialAutomaton(std::vector<FSM> & automata)
{
  //Create the special FSM
  FSM special;
  
  //Add the default state
  string State0Name = "0";
  special.GetState(State0Name);
  special.MarkState(State0Name);

  //Add the constraint state
  string State1Name = "1";
  special.GetState(State1Name);
  special.MarkState(State1Name);
  
  //Calculate the union of all events
  vector<string> Union = GetEventUnionOfAllAutomata(automata);

  //Loop through union of events
  for(int i=0; i<Union.size(); i++)
  {
    //Add the event to the special automaton's alphabet
    special.AddEvent(Union[i]);

    //Switch on event to determine appropriate transition
    if(!Union[i].compare("DDC"))
    {
      //Add the transition from State0 to State1
      special.AddTransition(State0Name, State1Name, "DDC");
    }
    else if(!Union[i].compare("dc"))
    {
      //Add the transition from State1 to State 0
      special.AddTransition(State1Name, State0Name, "dc");
    }
    else
    {
      //Add the transition that loops on State0
      special.AddTransition(State0Name, State0Name, Union[i]);
    }
  }

  return special;
}


/*
 * @brief   Creates an automaton which is used to satisfy the fault rule in a parallel composition of the given set of automata
 * @param   automata The set of automata to be composed
 * @returns The automaton which will be used to satisfy the fault rule
 */
FSM CreateFaultSpecialAutomaton(vector<FSM> & automata)
{
  //Create the special FSM
  FSM special;

  //Add the default state
  string State0Name = "Default";
  special.GetState(State0Name);
  special.MarkState(State0Name);

  //Add the driver-side fault state
  string State1Name = "DriverSideFault";
  special.GetState(State1Name);
  special.MarkState(State1Name);

  //Add the passenger-side fault state
  string State2Name = "PassengerSideFault";
  special.GetState(State2Name);
  special.MarkState(State2Name);

  //Calculate the union of all events
  vector<string> Union = GetEventUnionOfAllAutomata(automata);

  //Loop through union of events
  for(int i=0; i<Union.size(); i++)
  {
    //Add the event to the special automaton's alphabet
    special.AddEvent(Union[i]);

    //Switch on event to determine appropriate transition
    if(!Union[i].compare("nddl1_f") || !Union[i].compare("nddl2_f") || 
       !Union[i].compare("ddl1_f") || !Union[i].compare("ddl2_f") )
    {
      //Add the transition from the default state
      special.AddTransition(State0Name, State1Name, Union[i]);

      //Add the self-loop transition from the driver-side fault state
      special.AddTransition(State1Name, State1Name, Union[i]);

      //Add the transition from the passenger-side fault state
      special.AddTransition(State2Name, State1Name, Union[i]);
    }
    else if(!Union[i].compare("npdl1_f") || !Union[i].compare("npdl2_f") || 
            !Union[i].compare("pdl1_f") || !Union[i].compare("pdl2_f") )
    {
      //Add the transition from the default state
      special.AddTransition(State0Name, State2Name, Union[i]);

      //Add the transition from the driver-side fault state
      special.AddTransition(State1Name, State2Name, Union[i]);

      //Add the self-loop transition from the passenger-side fault state
      special.AddTransition(State2Name, State2Name, Union[i]);
    }
    else if(!Union[i].compare("fddl"))
    {
      //Add the transition from the driver-side fault state
      special.AddTransition(State1Name, State0Name, "fddl");
    }
    else if(!Union[i].compare("fpdl"))
    {
      //Add the transition from the driver-side fault state
      special.AddTransition(State2Name, State0Name, "fpdl");
    }
    else
    {
      //Add the self-loop transition from the default state
      special.AddTransition(State0Name, State0Name, Union[i]);

      //Add the transition from the driver-side fault state
      special.AddTransition(State1Name, State0Name, Union[i]);

      //Add the transition from the passenger-side fault state
      special.AddTransition(State2Name, State0Name, Union[i]);
    }
  }

  //Return the special automaton
  return special;
}

/*
 * @brief Determines whether or not an event is exogenous
 * @param event The event in question
 * @returns True if an event is exogenous
 */
bool IsEventExogenous(const string & event)
{
  for (int i = 0; i < event.size(); i++) 
  {
    //Allow 'n' in the 0th slot
    char c = event[i];
    if( i == 0 && c == 'n')
      continue;

    //Allow numbers and capital letters
    else if (isupper(c) || isdigit(c)) 
      continue;

    //Allow underscores
    else if (c == '_')
      continue;

    else
      return false;
  }
  return true;
}
