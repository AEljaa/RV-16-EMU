# RV-16-EMU

## Design Choices
For this implementation of the RISC 16 bit cpu, I decided to have 1024 word addresses for instruction memory (ROM) and 3072 word addresses for data memory (RAM).<br/>
I did this since 1024 instructions in a assembly file to execute is more than enough instructions for all my use cases and I had to create a distinction between read only memory (like instructions) and data memory (like array values), to prevent the programmer from writing into instruction memory (prvent memory corruption).<br/>
The CPU is little endian and byte addressed.

## Capabilities
- Full RISC16 instruction set support following the [`RiSC-isa`](./docs/RiSC-isa.pdf) developed by Bruce Jacob

## Build instructions
### Using [`cpu.sh`](./cpu.sh)

In order to run a program, you can use the run the bash script [`cpu.sh`](./cpu.sh) that calls cmake and make to build the assebler and cpu binaries and assemble a program specified in the asm folder, loading the hexfile generated into the intruction memory of the RV-16 cpu (ROM).
https://github.com/AEljaa/RV-16-EMU/blob/5007dbb34ffd4cae0a3c4a8fa817eaaf6f3af7a7/cpu.sh#L1-L4

### Manually building

The way how I manually ran programs on this cpu was by doing the following. Please note that below I use the asembelly programs in the [`asm`](./asm/) folder, you can also write your own assembly files following the isa [`RiSC-isa`](./docs/RiSC-isa.pdf) and generate hex files from them using the assembler in the repo.
First compile the [`assembler`](./a.c) producing a binary let's call this produced binary `assembler`
`gcc -o assemblerold a.c`
Second compile the [`cpu`](./RV-16.cpp) producing a binary let's call this produced binary `rv16`
`g++ -Wall -g --std=c++2b RV-16.cpp  -o rv16`
Third generate the hex files the CPU reads 
`./assembler ./asm/basic_loop.s  main.hex`
Provide the hex file to the program and the CPU will begin running.
`./rv16 main.hex`

## References
1) [RiSC-16 homepage](https://user.eng.umd.edu/~blj/RiSC/)
