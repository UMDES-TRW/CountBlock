#ifndef _TRW_SPECIAL_UTILITIES_H_
#define _TRW_SPECIAL_UTILITIES_H_

//STL includes
#include <vector>

//Implementation includes
#include "FSM.h"

/*
 * @brief Determines whether a DDC special automaton is needed to constrain a parallel composition
 * @param automata The automata to be composed
 * @returns True if a DDC spec automaton is needed to constrain the parcomp operation
 */
bool IsDDCSpecUseful(const std::vector<FSM> & automata);

/*
 * @brief Adds a "dc" transition wherever there exists a "DC" transition in any of the automata
 * @note  All "dc" transitions will have the same origin and destination states as the respective "DC" transitions
 * @note  The event set of any given automaton is updated if alias event is added to that automaton
 * @param automata The set of automata to be updated
 */
int AddDcAliasTransitions(FSM & automaton);

/*
 * @brief   Creates an automaton which is used to satisfy the DDC rule in a parallel composition of the given set of automata
 * @param   automata The set of automata to be composed
 * @note    These automata must have been updated with alias DC transitions for the result of this function to be meaningful
 * @returns The automaton which will be used to satisfy the DDC rule
 * TODO:    Param should be const! (set_union will throw errors)
 */
FSM CreateDDCSpecialAutomaton(std::vector<FSM> & automata);

/*
 * @brief Determines whether a fault rule special automaton is needed to constrain a parallel composition
 * @param automata The automata to be composed
 * @returns True if a fault rule spec automaton is needed to constrain the parcomp operation
 */
bool IsFaultSpecUseful(const std::vector<FSM> & automata);

/*
 * @brief   Creates an automaton which is used to satisfy the fault rule in a parallel composition of the given set of automata
 * @param   automata The set of automata to be composed
 * @returns The automaton which will be used to satisfy the fault rule
 * TODO:    Param should be const! (set_union will throw errors)
 */
FSM CreateFaultSpecialAutomaton(std::vector<FSM> & automata);

/*
 * @brief Helper function to concisely get union of all events
 * @param automata All automata from which the union will be created
 * @returns The union of all the events in all automata
 */
std::vector<std::string> GetEventUnionOfAllAutomata(const std::vector<FSM> & automata);

/*
 * @brief Determines whether or not an event is exogenous
 * @param event The event in question
 * @returns True if an event is exogenous
 */
bool IsEventExogenous(const std::string & event);

#endif
