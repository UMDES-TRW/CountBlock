
#include "gtest/gtest.h"
#include "FSM.h"

TEST(FsmTest, CreateASimpleFsm) {
	FSM fsm;
	fsm.SetInitialState("s0");
	fsm.AddTransition("s0", "s1", "a");
	fsm.AddTransition("s1", "s2", "b");
	fsm.AddEvent("c");

  // Check event set.
  auto events = fsm.GetAllEvents();
	ASSERT_EQ(3u, events.size());
	ASSERT_EQ("a", events[0]);
	ASSERT_EQ("b", events[1]);
	ASSERT_EQ("c", events[2]);

	// Check states.
	auto states = fsm.GetAllStates();
	ASSERT_EQ(3u, states.size());

	ASSERT_EQ("s0", states[0]->stateName);
	ASSERT_EQ(1u, states[0]->GetNumberOfTransitions());
	ASSERT_EQ(states[1], states[0]->transitions.find("a")->second);

	ASSERT_EQ("s1", states[1]->stateName);
	ASSERT_EQ(1u, states[1]->transitions.size());
	ASSERT_EQ(states[2], states[1]->transitions.find("b")->second);

	ASSERT_EQ("s2", states[2]->stateName);
	ASSERT_EQ(0u, states[2]->transitions.size());
}