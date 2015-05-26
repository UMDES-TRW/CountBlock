== This is the Readme file for folder TestCase ==

Here contains the test case files for the new par_comp rules, denoted by ||tp. 

TC0: 
very simple test case that tests parcomp associativity
G123 = G1 ||tp G2 ||tp G3



TC1:
- G1_t-tc.fsm and G2_t-tc.fsm are sub automata of G1_t and G2_t. 
- We have modified the DC part of G2_t.fsm in order to take care of the DDC-DC rule using spec automaton.
- spec.fsm is the spec automaton that enforces “dc” to occur right after DDC


G1_t-tc || G2_t-tc || spec = G12spec.fsm (composed with the standard p.c)
G1_t-tc ||tp G2_t-tc ||tp spec = G12-tp-DDC.fsm (composed with ||tp)

*This test case tests ||tp



TC2:
G1_t-tc is a sub automata of G1_t 
G1_t-tc || pdl_v6 || ddl_v6 = 
 