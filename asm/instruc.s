addi r3,r3,0x7F
addi r4,r4,10
sbl   r3,r4,2
sbu   r3,r4,2
lbl   r5,r4,2
lbl   r6,r4,2

main:   

check:  beq     r5, r0, done  # If r5 == 0, branch to 'done'
        addi    r5, r5, -1    # Decrement r5 by 1
        beq     r0, r0, check # Unconditional branch to 'check'

done:   halt                  # End of the program
