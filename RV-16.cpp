#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>  // Must have this!
#include <fstream>
#include <_stdio.h>
#include <sstream>
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
        for (size_t i = 0; i < MAX_MEM; i++) {
            if ((i % 2 == 0) && (i !=0 )) outfile << std::endl; // 2 bytes per line (16 bits)
            outfile << std::hex << (int)ROM[i] << " ";
        }
        outfile.close();
        std::cout << "Memory dump written to " << filename << std::endl;
    }

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
        while( PC < this->maxCycles ){
            std::cout<<"Program Counter : "<< PC <<std::endl;
            std::cout<<"Fetching"<<std::endl;
            this->m_cinstruction=this->fetch();
            printf("Instruction: %.4X\n",m_cinstruction);
            this->DecAndEx();
            std::cout<<"Next Clock Cycle"<<std::endl;
            this->nextClk();
        }
        this->mem.dump("Finalmem.hex");
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

    private:
    void reset(){
        this->PC=0;
        this->nextJumpOffset=2;
        this->JALR=0;
        std::uint16_t m_cinstruction=0x0000;
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
                printf("ADD %.1X %.1X into %.1X\n",b,c,a);
            } break;
            case 0x1:{
                std::cout<<"ADDI instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = (m_cinstruction & 0x7F );
                this->reg[a]= (a != 0) ? this->reg[b]+imm : 0;
             } break;
            case 0x2:{
                std::cout<<"NAND instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                int c = (m_cinstruction & 0x7);
                this->reg[a]= (a != 0) ? !(this->reg[b] & this->reg[c]) : 0 ;
             } break;
            case 0x3:{
                std::cout<<"LUI instruction"<<std::endl;
                std::uint16_t imm = (m_cinstruction & 0x2FF );
                this->reg[a]= (a != 0) ? (imm << 6) : 0 ; //Place the 10 ten bits of the 16-bit imm into the 10 ten bits of regA, setting the bottom 6 bits of regA to zero
             } break;
            case 0x4:{
                std::cout<<"SW instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = (m_cinstruction & 0x7F );
                std::uint16_t addr = this->reg[b] + imm;
                mem[addr]= (this->reg[a] & 0xFF);//little endian so lsbs gets stored at lower mem address (store first byte at offset 0)
                mem[addr+1]= (this->reg[a] << 8  & 0xFF00);//store second byte at offset 1
             } break;
            case 0x5:{
                std::cout<<"LW instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = (m_cinstruction & 0x7F );
                std::uint16_t addr = this->reg[b] + imm;
                this->reg[a] = ((mem[addr+1] << 8 ) | mem[addr]);
             } break;
            case 0x6:{
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = (m_cinstruction & 0x7F );
                this->nextJumpOffset = (this->reg[a] == this->reg[b]) ? (2+imm) : 2 ;
                std::cout<<"BEQ instruction, Offset : "<<this->nextJumpOffset<<std::endl;

             } break;
            case 0x7:{
                std::cout<<"JALR instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                this->nextJumpOffset = this->reg[b] ; //need pc to knwo it should chane its value to this 
                this->reg[a] = (a != 0) ? (this->PC+2) : 0 ; 
                this->JALR=1;
             } break;
        }
    }

    void nextClk(){
        this->PC= this->JALR ? this->nextJumpOffset : this->PC + this->nextJumpOffset ;
        this->nextJumpOffset=2;
        this->JALR=0;
    }

    //mem.initialise();
    std::uint16_t reg[8];
    std::uint16_t PC = 0;
    std::uint16_t nextJumpOffset = 2;
    std::uint16_t m_cinstruction=0x0000;
    int maxCycles;
    bool JALR=0;
    Memory mem;

};

int main(){
    //enter path
    CPU cpu("main.hex",2000);
    //cpu.load("../test/prog.hex");
}
