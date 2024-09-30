#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>  // Must have this!
using std::uint8_t;
using std::uint16_t;

struct Memory {
    static constexpr std::uint32_t MAX_MEM = 4096 * 16;
    std::uint8_t ROM[MAX_MEM];
    
    void initialise(){
        std::cout << "Initialising memory" << std::endl;
        // Example initialization logic
        //for(size_t i=0; i< MAX_MEM ; i++){
        //    ROM[i] = 0; // or any other initialization
        //}
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
    CPU(){
        this->reset();
        this->load();
        while(PC<20){
            std::cout<<"Program Counter : "<< PC <<std::endl;
            std::cout<<"Fetching"<<std::endl;
            this->m_cinstruction=this->fetch();
            printf("Instruction: %.4X\n",m_cinstruction);
            this->DecAndEx();
            std::cout<<"Next Clock Cycle"<<std::endl;
            this->nextClk();
        }
        this->reset();
    };

    void load(){
        //load path
        // for
        mem[0]=0x03;
        mem[1]=0x00;
        mem[2]=0x04;
        mem[3]=0x20;
        mem[4]=0x05;
        mem[5]=0x40;
        mem[6]=0x06;
        mem[7]=0x60;
        mem[8]=0x07;
        mem[9]=0x80;
        mem[10]=0x08;
        mem[11]=0xA0;
        mem[12]=0x09;
        mem[13]=0xC0;
        mem[14]=0x00;
        mem[15]=0xE0;
    }

    private:
    std::uint16_t reg[8];
    std::uint16_t PC = 0;
    std::uint16_t nextJumpOffset = 2;
    std::uint16_t m_cinstruction=0x0000;
    Memory mem;

    void reset(){
        this->PC=0;
        for(int i = 0 ; i<100; i++ ){
            reg[i]=0;
        }
    }

    std::uint16_t fetch(){
        return ((mem[PC+1] << 8 ) | mem[PC]);
    }

    void DecAndEx(){
        std::uint8_t opcode= (m_cinstruction >> 13);
        int a = ((m_cinstruction >> 10) & 0x7 );
        std::uint16_t jump;
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
                std::cout<<"======== reg c: "<<c<<std::endl;
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
                std::cout<<"BEQ instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                std::uint16_t imm = (m_cinstruction & 0x7F );
                this->nextJumpOffset = (this->reg[a] == this->reg[b]) ? (2+imm) : 2 ;
             } break;
            case 0x7:{
                std::cout<<"JALR instruction"<<std::endl;
                int b = ((m_cinstruction >> 7) & 0x7 );
                jump = this->reg[b] ;
                this->reg[a] = (a != 0) ? (this->PC+2) : 0 ; 
             } break;
        }
    }

    void nextClk(){
        this->PC+=this->nextJumpOffset;
        this->nextJumpOffset=2;
    }

    //mem.initialise();

};

int main(){
    CPU cpu;
    //cpu.load("../test/prog.hex");
}
