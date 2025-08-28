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

The way how I manually ran programs on this cpu was by doing the following. Please note that below I use the asembelly programs in the [`asm`](./asm/) folder, you can also write your own assembly files following the isa [`RiSC-isa`](./docs/RiSC-isa.pdf) and generate hex files from them using the assembler in the repo.<br/>

First compile the [`assembler`](./a.c) producing a binary let's call this produced binary `assembler`.<br/>
`gcc -o assemblerold a.c`<br/>
Second compile the [`cpu`](./RV-16.cpp) producing a binary let's call this produced binary `rv16`.<br/>
`g++ -Wall -g --std=c++2b RV-16.cpp  -o rv16` <br/>
Third generate the hex files the CPU reads.<br/>
`./assembler ./asm/basic_loop.s  main.hex`<br/>
Provide the hex file to the program and the CPU will begin running.<br/>
`./rv16 main.hex`

## Assembler
## Notes
Make sure when using assembler, each address is at an even address, there are 4 additonal instructions and all of these intructions take advantage of the byte addressed design of the cpu.<br/>
The reason each address must be even is because each address when coding in assembly for this cpu, points to the bottom byte of the data memory at that address, since 16-bit operands in memory occupy 2 bytes.
The instructions below allow the programmer to choose which one of these bytes within these halfword length memory locations are stored into or loaded from, taking advantage of the byte addressing allowed through my design.
###  sbl ra, rb imm
This instruction stores a byte (the least significant byte) from ra into the mem[rb+imm],into the lower byte of this address addr(1)-[ ,xxxxxxxx]-addr(0)
###  sbu ra, rb imm
This instruction stores a byte (the least significant byte) from ra into the mem[rb+imm],into the upper byte of this address addr(1)-[xxxxxxxx, ]-addr(0)
###  lbl ra, rb imm
This instruction loads the lower byte (the least significant byte) from mem[rb+imm] and stores the byte into ra
###  lbu ra, rb imm
This instruction loads the upper byte (the most significant byte) from mem[rb+imm] and stores the byte into ra


- [ ] Have bound correction for imm values and error returns
- [x] Actually implement the sb and lb instructions
- [ ] Clean up code and remove redundant parts
- [ ] provide psuedo instruction support
- [ ] work on allowing just a line with just a comment,currently this messes up my code,currently replacing the line with a noop, but that wastes a cycle for each comment 
- [ ] need some sort of way to keep track of how many movi instructions i have and make sure that the addresses of labels are adjusted correctly, sicne each movi takes 2 instructions and effects the labels
- [ ] jalr looks kinda cooked

E.g
Instead of
0:movi x,y
2: label1:
4: movi x,label1
=======
0: lui
2:addi
4: label1:
6: lui
addi 
so for each lui we hit the label must insrease by 2



## References
1) [RiSC-16 homepage](https://user.eng.umd.edu/~blj/RiSC/)
