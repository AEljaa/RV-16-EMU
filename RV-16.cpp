#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>  // Must have this!
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
using std::uint8_t;
using std::uint16_t;

struct Memory {
    static constexpr std::uint32_t MAX_MEM = 4096 * 16;
    std::uint8_t ROM[MAX_MEM];
    
    void initialise(const char* prog){
        std::cout << "Initialising memory" << std::endl;
        // Example initialization logic
        for(size_t i=0; i< MAX_MEM ; i++){
            ROM[i] = 0; // or any other initialization
        }
        std::ifstream infile(prog);
        std::string line;
        if (!infile) {
            std::cerr << "Error opening file "<< std::endl;
        }
        
        int index=0;
        unsigned int a;

        // Read and output each hex value until EOF
        while (getline(infile, line)) {
            std::istringstream(line) >> std::hex >> a;
            ROM[index]=(a & 0x00FF);
            ROM[index+1]=(a & 0xFF00) >> 8 ;
            index+=2;
        }
        
        std::cout << "Finished reading hex file." << std::endl;
    }

    void dump(const char* filename) {
        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Error opening file for memory dump: " << filename << std::endl;
            return;
        }

        outfile << std::hex << std::uppercase; 

        for (size_t i = 0; i < MAX_MEM; i += 2) {
            if (i != 0) {
                outfile << std::endl;
            }
            // 16-bit value (little endian)
            uint16_t val = static_cast<uint16_t>(ROM[i + 1] << 8 | ROM[i]);
            outfile << std::setw(4) << std::setfill('0') << val;
        }

        outfile.close();
        std::cout << "Memory dump written to " << filename << std::endl;
    }
//Memory map
//0-1023 16 bit instructions (max 1023 instructions a prog file can have)
//1024-4095 RAM
    std::uint8_t operator[] (std::uint16_t addr) const{
        return ROM[addr];
    }

    std::uint8_t& operator[] (std::uint16_t addr) {
        return ROM[addr];
    }
};


class CPU {

    public:
    CPU(const char* prog, int maxC) : maxCycles(maxC) {
        this->reset();
        //this->load(prog);
        this->mem.initialise(prog);
        this->instructionsExecuted("null");
        for(int i=0; i< this->maxCycles; i++ ){
            //std::cout<<"Fetching"<<std::endl;
            this->m_cinstruction=this->fetch();
            //printf("Instruction: %.4X\n",m_cinstruction);
            this->DecAndEx();
            //std::cout<<"Next Clock Cycle"<<std::endl;
            this->nextClk();
        }
        this->mem.dump("Finalmem.hex");
        this->Regdump("FinalReg.txt");
        this->reset();
    };

    void load(const char* path){
        std::ifstream infile(path);
        std::string line;
        if (!infile) {
            std::cerr << "Error opening file "<< std::endl;
        }
        
        int index=0;
        unsigned int a;

        // Read and output each hex value until EOF
        while (getline(infile, line)) {
            std::istringstream(line) >> std::hex >> a;
            this->mem[index]=(a & 0x00FF);
            this->mem[index+1]=(a & 0xFF00) >> 8 ;
            index+=2;
        }
        
        std::cout << "Finished reading hex file." << std::endl;
    }

    ~CPU(){
        std::cout << "All exectuted instructions in ExectutedInstructions.txt" << std::endl;
    }

    private:
    void reset(){
        this->PC=0;
        this->nextJumpOffset=2;
        this->JALR=0;
        this->m_cinstruction=0x0000;
        for(int i = 0 ; i<8; i++ ){
            reg[i]=0;
        }
    }

    std::uint16_t fetch(){
        return ((mem[PC+1] << 8 ) | mem[PC]);
    }

    void DecAndEx(){
        std::uint8_t opcode= (m_cinstruction >> 13);
        int a = ((m_cinstruction >> 10) & 0x7 );
        switch(opcode){
            case 0x0:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                int c = (m_cinstruction & 0x7);
                this->reg[a]= (a != 0) ? this->reg[b]+this->reg[c] : 0;
                std::string instr = "ADD Reg " + std::to_string(b) + " (" + std::to_string(this->reg[b]) + ")" +
                    " and Reg " + std::to_string(c) + " (" + std::to_string(this->reg[c]) + ")" +
                    " into Reg " + std::to_string(a) + " and now curr val: (" + std::to_string(this->reg[a]) + ")";
                instructionsExecuted(instr.c_str()); 
            } break;
            case 0x1:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = this->SignExtention(m_cinstruction & 0x7F );
                this->reg[a]= (a != 0) ? this->reg[b]+imm : 0;
                std::string instr = "ADDI Reg " + std::to_string(b) + " (" + std::to_string(this->reg[b]) + ")" +
                    " and 7bit imm " + std::to_string(imm) + " (" + std::to_string(imm) + ")" +
                    " into Reg " + std::to_string(a) + " and now curr val: (" + std::to_string(this->reg[a]) + ")";
                instructionsExecuted(instr.c_str()); 
             } break;
            case 0x2:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                int c = (m_cinstruction & 0x7);
                this->reg[a]= (a != 0) ? ~(this->reg[b] & this->reg[c]) : 0 ;
                std::string instr = "NAND Reg " + std::to_string(b) + " (" + std::to_string(this->reg[b]) + ")" +
                    " and Reg " + std::to_string(c) + " (" + std::to_string(this->reg[c]) + ")" +
                    " into Reg " + std::to_string(a) + " and now curr val: (" + std::to_string(this->reg[a]) + ")";
                instructionsExecuted(instr.c_str()); 
             } break;
            case 0x3:{
                std::uint16_t imm = (m_cinstruction << 6);
                this->reg[a]= (a != 0) ? imm : 0 ; //Place the 10 ten bits of the 16-bit imm into the 10 ten bits of regA, setting the bottom 6 bits of regA to zero
                std::string instr = "LUI 10 bit " + std::to_string(imm) +
                    " into Reg " + std::to_string(a) + " and now curr val 16bit: (" + std::to_string(this->reg[a]) + ")";
                instructionsExecuted(instr.c_str()); 
             } break;
            case 0x4:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = this->SignExtention(m_cinstruction & 0x7F );
                std::uint16_t addr = this->reg[b] + 2*imm + 0x0400;//RAM starts from a offset of 1024
                addr = (addr % 2 == 0) ? addr : addr - 1; //byte addressing,forget last bit doesn't matter
                mem[addr]= (this->reg[a] & 0x00FF);//little endian so lsbs gets stored at lower mem address (store first byte at offset 0)
                mem[addr+1]= (this->reg[a] & 0xFF00) >> 8 ;//store second byte at offset 1
                                                           //
                std::string instr = "SW (16bits) Value at Register " + std::to_string(a) + " ("  + std::to_string(this->reg[a]) + ") " +
                    " into Mem[" + std::to_string(addr) + "] calculated by value Reg "+ std::to_string(b)+ "(" + std::to_string(this->reg[b])+ ") + imm " + std::to_string(imm)+ " and now curr val: (" + std::to_string((mem[addr+1] << 8 ) | mem[addr]) + ")";
                instructionsExecuted(instr.c_str()); 
             } break;
            case 0x5:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = this->SignExtention(m_cinstruction & 0x7F );
                std::uint16_t addr = this->reg[b] + 2*imm + 0x0400;//RAM starts from a offset of 1024
                addr = (addr % 2 == 0) ? addr : addr - 1; //byte addressing,forget last bit doesn't matter could just do &FFFE

                this->reg[a] = ((mem[addr+1] << 8 ) | mem[addr]);
                std::string instr = "LW (16bits) Value at Mem[" + std::to_string(addr) + "] (" + std::to_string((mem[addr+1] << 8 ) | mem[addr]) + ")" +
                    " into Register " + std::to_string(a) + " ("  + std::to_string(this->reg[a]) + ") " +
                    " and now curr val: (" + std::to_string(this->reg[a]) + ")";
                instructionsExecuted(instr.c_str()); 
             } break;
            case 0x6:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = this->SignExtention(m_cinstruction & 0x7F );
                this->nextJumpOffset = (this->reg[a] == this->reg[b]) ? (2*(imm+1)) : 2 ;
                std::string instr = "BEQ Reg " + std::to_string(a) + " (" + std::to_string(this->reg[a]) + ")" +
                    " Reg " + std::to_string(b) + " (" + std::to_string(this->reg[b]) + ") Jumpoffset = "+ std::to_string(this->nextJumpOffset);
                instructionsExecuted(instr.c_str());
             } break;
            case 0x7:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                this->nextJumpOffset = 2*(this->reg[b]) ; //need pc to know it should chane its value to this 
                this->reg[a] = (a != 0) ? (0.5*(this->PC+2)) : 0 ; 
                this->JALR=1;
                std::string instr = "JALR set PC with " + std::to_string(this->nextJumpOffset) + " and store Register " + std::to_string(a)+ " with "+ std::to_string(this->PC+2) + " if not 0 else reg0 stays 0" +
                    " JALR flag =  " + std::to_string(this->JALR);
                instructionsExecuted(instr.c_str());
             } break;
        }
    }

    void instructionsExecuted(const char* instruction){
        if(this->PC==0 && !this->IfileDeleted) {
            if (std::remove("ExectutedInstructions.txt") != 0)
                std::perror("Error deleting ExectutedInstructions.txt");
            else
                std::puts("ExectutedInstructions.txt successfully deleted");
            
            this->IfileDeleted = true;
        }

        // Append the instruction to the file
        if(std::string(instruction) != "null"){
            std::ofstream file("ExectutedInstructions.txt", std::ios_base::app);
            file << "PC:"<<this->PC <<" "<<instruction << std::endl;  // Write the instruction with a newline for better readability
        }
    }

    void nextClk(){
        this->PC= this->JALR ? this->nextJumpOffset : this->PC + this->nextJumpOffset ;
        this->nextJumpOffset=2;
        this->JALR=0;
    }

    uint16_t SignExtention(uint16_t imm){
        uint16_t retimm= (imm>>6) ? ( imm |0xFF80) : imm;
        return retimm;
    }

     void Regdump(const char* filename) {
        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Error opening file for Reg dump: " << filename << std::endl;
            return;
        }
        for (size_t i = 0; i < 8; i++) {
            outfile << "Register " + std::to_string(i)+ " " + std::to_string(this->reg[i]) << "\n" ;
        }
        outfile.close();
        std::cout << "Register dump written to " << filename << std::endl;
    }   

    //mem.initialise();
    std::uint16_t reg[8];
    std::uint16_t PC = 0;
    std::uint16_t nextJumpOffset = 2;
    std::uint16_t m_cinstruction;
    int maxCycles=10000;
    bool IfileDeleted = 0;
    bool JALR=0;
    Memory mem;

};

int main(int argc, char* argv[]){
    //enter path
    CPU cpu(argv[1],200000);
    //cpu.load("../test/prog.hex");
}
