cmake .
make 
./assembler ./asm/"$1".s ./asm/"$1".hex
./cpu ./asm/"$1".hex

