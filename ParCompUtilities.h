
#ifndef new_branch_ParCompUtilities_h
#define new_branch_ParCompUtilities_h

#define LANHAM

#ifdef LANHAM
  #include <SmartStack.h>
#endif

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
#include "SmartStack.h"
#include "SmartHeap.h"

using namespace std;

typedef vector<const State*> CompositeState;

struct CompositeTransition
{
  ///@brief The event for this transition
  string event;

  ///@brief The destination composite state
  CompositeState destination;

  ///@brief Constructor
  CompositeTransition(string event, CompositeState dest)
    :event(event), destination(dest) {};
};

/*
 * @brief Creates copies of the FSMs in which the direction of every transition is reversed
 * @param automata The FSMs to be inverted
 * @param inverted The copy of each FSM with reversed transitions
 * @TODO Should the function signature of this be FSM GetInvertedAutomaton(const FSM&); ?
 */
void InvertTransitions( const vector<FSM> & automata, vector<FSM> & inverted);

//TODO: Move these to smart wrapper class for bitset 
void BitSetToFile(bitset<MAX_NUMBER_OF_STATES_ALLOWED> & bits, uint32_t NumberOfValidBits, string & filename);
void FileToBitSet(bitset<MAX_NUMBER_OF_STATES_ALLOWED> & bits, uint32_t NumberOfValidBits, string & filename);

/*
 * @brief Performs a depth-first search of the accessible part of the parallel composition of the referenced automata
 * @param bits Pointer to the memory structure which keeps track of visited states.
 *             When passing in this argument, the search space will be constrained to unmarked states 
 *             After this function returns, bits corresponding to composite states visited will be marked
 *             If this information is not desired, pass in NULL. 
 * @param ProperRuleEnforced Set this to true if exogenous events should be prevented from occurring at unmarked states
 * @param out This FSM will be populated with the result of the ParComp. Pass in NULL if no output is desired.
 * @param verbose Set this to true if printed updates to the terminal are desired
 * @note Populating an output FSM will slow the algorithm and increase the memory footprint.
 * @TODO Should the bitset be wrapped in a smarter class (that checks how it is encoded so no mistakes occur?
 */
uint32_t ParCompAcc(const vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits, const bool ProperRuleEnforced, FSM * out, bool verbose);

/*
 * @brief Performs a depth-first search of the coaccessible part of the parallel composition of the referenced automata
 * @param bits Pointer to the memory structure which keeps track of visited states.
 *             When passing in this argument, the search space will be constrained to unmarked states 
 *             After this function returns, bits corresponding to composite states visited will be marked
 *             If this information is not desired, pass in NULL. 
 * @param ProperRuleEnforced Set this to true if exogenous events should be prevented from occurring at unmarked states
 * @TODO Should the bitset be wrapped in a smarter class (that checks how it is encoded so no mistakes occur?
 */
uint32_t ParCompCoAcc(const vector<FSM> & automata, bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits, const bool ProperRuleEnforced);

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
uint32_t FindBlockingTransitions(const vector<FSM> & fsm, const bitset<MAX_NUMBER_OF_STATES_ALLOWED> * blocking, 
                                 const bool ProperRuleEnforced, FSM * out, ofstream & blockfile);

/*
 * @brief Calculates the worst-case state space of a parallel composition of these automata
 * @param automata The automata in question
 * @returns The product of the state spaces of each automata
 */
uint64_t GetCartesianStateSpace(const vector<FSM> & automata);

//TODO: Move this to FSM.h
bool AreAutomataEqual(const FSM & G1, const FSM & G2);

/*
 * @brief Generates and returns a state name for the associated composite state
 * @param state The composite state in question
 * @returns The name of the state
 */
string GetStateName( const CompositeState & state);

/*
 * @brief Generates and returns a name for a composite FSM name
 * @param automata The automata being composed
 * @returns The name of the composed automata
 */
string GetCompositeFsmName(const vector<FSM> & automata);

//TODO: Optimize this and replace it with GetEventUnion(const vector<FSM> &)
vector<string> EventUnion(const vector<string> &, const vector<string> &);


#endif
