#include <cstdint>
#include <fstream>
#include <iostream>

int main(){
    std::ifstream infile;
    infile.open("main.hex",std::fstream::in);
    std::uint8_t array[50];
    int index=0;
    if (!infile) {
        std::cerr << "Error opening file "<< std::endl;
        return 1;  // Exit with error code
    }
    
    unsigned int a;

    // Read and output each hex value until EOF
    while (infile >> std::hex >> a) {
        std::cout<< std::hex<< a <<std::endl;
        array[index]=(a & 0x00FF);
        array[index+1]=(a & 0xFF00);
        printf("Top bits: %.2X Bottom bits: %.2X\n ",array[index+1],array[index]);
        index+=2;
    }
    
    std::cout << "Finished reading hex file." << std::endl;
    
    return 0;
}

