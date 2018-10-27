# Assem
Linking meta-assembler for 8 bit CPUs with processor definitions for Z80 and 8051 CPUs

Written as the existing assemblers were a bit limited and didn't encourage modularity. This assembler, and its linker allows private labels and symbols to be defined with global symbols being specifically exported and imported between modules.

The processor definition language consists of 2 parts for each opcode:  parse rules and production rules.  The parse rules pattern match the source and in doing so push values (such as register codes) onto an evaluation stack.  The production rule is forth-like and uses a Reverse Polish language to evaluate and emit the resulting machine code. 
