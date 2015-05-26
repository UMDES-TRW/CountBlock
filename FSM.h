#ifndef new_branch_FSM_h
#define new_branch_FSM_h


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
#include <set>

using namespace std;

//~~~~~~~~~~~~ structs used for fsm ~~~~~~~~~~~~~~~~//

/*
 * typedef transition to be pair<string, State*>
 */
//struct State;

//typedef pair<string, const State*> Tran;


/*
 * A struct to store a state
 * Each state has a multimap for transitions
 * a multimap can map one key to multiple values
 * use equal_range to get all the values for a given key
 * <itlow, ithigh> = equal_range(key)
 */

struct State
{
public:
	string stateName;
    bool marking;
	  multimap<string, const State*> transitions; //each <string, unsigned int > is a transition
    bool examined = false;
	
    State(const string str): stateName(str), marking(false), examined(false){};
    
    int GetNumberOfTransitions(void) const
	{
        return (unsigned int)transitions.size();
	};
    
    void addTran(const string & ev, const State* ns)
    {
        transitions.insert(make_pair(ev, ns));
    };
    void printState(void)
    {
        cout << "\"" << stateName << "\"";
    };
    
    ~State()
    {
      stateName.clear();
      transitions.clear();
    };
	
};

///@TODO: Make copy constructor which allocates new memory for each state

///@brief A class to represent an automaton
class FSM
{
private:
  ///@brief Maps state name to state pointer
  map<string, State*> states;

  ///@brief Set of events
  set<string> events;

  ///@brief Initial state - defaults to first state added
  State * initial;

  ///@brief Adds a transition from origin to destination 
  void generateTransitionPtr(State* origin, const State* dest, const string & event);

  ///@brief Gets a pointer to the given state - creates one if necessary
  State* getStatePtr(const string & statename);

public:
  ///@brief Default constructor
  FSM(void):initial(NULL){};

  /*
   * @brief Constructor - Reads an FSM from a .fsm file at @param filepath
   * @param filepath The location of the file to be imported
   */
  FSM(const string & filepath);

  ///@brief Copy constructor
  FSM(const FSM &);

  ///@brief Name of the fsm, to be written and read freely
  string Name;

  /*
   * @brief Sets the initial state to the state with indicated name
   * @param statename The name of the desired state
   * @note Creates the referenced state if not found
   */
  void SetInitialState(const string & statename);

  /*
   * @brief Marks the state with indicated name
   * @param statename The name of the desired state
   * @note Creates the referenced state if not found
   */
  void MarkState(const string & statename);

  /*
   * @brief Adds a transition to the automaton from origin to dest with indicated event
   * @param origin The state at which the transition starts. 
   * @param dest The state at which the transition ends. 
   * @param event The event associated with the transition (is added to the event set).
   * @note If either state does not yet exist, it will be created
   */
  void AddTransition(const string & origin, const string & dest, const string & event);

  /*
   * @brief Adds an event to the event set. Does not create a transition
   * @param event The event to be added
   */
  void AddEvent(const string & event);

  /*
   * @brief Adds all events to the event set. Does not create any transition
   * @param newevents The events to be added
   */
  void AddEvents(const vector<string> & newevents);

  /*
   * @brief Returns a constant pointer to the state of the indicated name
   * @param statename The name of the referenced state
   * @note If a state of this name does not already exist, it will be created
   * @returns A constant pointer to the existing or newly-created state
   */
  const State* GetState(const string & statename);

  /*
   * @brief Returns a copied vector of all state pointers, sorted by state name
   * @note Does not modify internal data
   * @returns Pointers to all the states
   */
  vector<const State *> GetAllStates(void) const;

  /*
   * @brief Returns a copied vector of all events in alphanumeric order
   * @note Does not modify internal data
   * @returns All events
   * @TODO: Should to be changed to "const set<string> & GetAllEvents(void) const" 
   */
  vector<string> GetAllEvents(void) const;

  ///@brief Returns the number of states
  int GetNumberOfStates(void) const;

  ///@brief Returns the number of events
  int GetNumberOfEvents(void) const;

  ///@brief Returns true if an FSM contains the given event
  bool HasEvent(const string & event) const;

  ///@brief Returns the pointer to the initial state
  const State* GetInitialState(void) const;

  ///@brief Export the automaton in .fsm format to filepath
  void Export(const string & filepath) const;

  ///@brief Clear all information about the automaton
  void Clear(void);

  ///@brief Assignment operator
  FSM& operator=(const FSM & original);

  ///@brief Destructor
  ~FSM(void);
};

#endif
