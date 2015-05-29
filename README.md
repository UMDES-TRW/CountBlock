# CountBlock
Modified version of Stephen Lanham's CountBlockingStates.

This version composes all automata one by one, with the first automaton input always being composed first.
The rest are composed based on largest event set size to smallest.

This program is intended for usage with Gt, an FSM representing a path that should be enforced.

To build, use "make" or "make all".

To build Stephen Lanham's original version, use "make original".
**WARNING: Input FSMs are hardcoded in the original. Verify that the desired FSMs are imported before compiling.**
