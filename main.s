
        addi    r1, r0, 10    # Load immediate value 10 into register r1
        addi    r2, r0, 20    # Load immediate value 20 into register r2
        add     r3, r1, r2    # Add r1 and r2, store result in r3 (r3 = 10 + 20 = 30)
        
        nand    r4, r1, r2    # NAND r1 and r2, store result in r4 (used for testing NAND)
        
        sw      r3, r0, 0     # Store r3 (30) into memory at address 0
        lw      r5, r0, 0     # Load value from memory address 0 into r5 (r5 = 30)
        
check:  beq     r5, r0, done  # If r5 == 0, branch to 'done'
        addi    r5, r5, -1    # Decrement r5 by 1
        beq     r0, r0, check # Unconditional branch to 'check'

done:   halt                  # End of the program
