        movi    r1, 100          # Load immediate value 100 into r1 (value to manipulate)
        movi    r2, 50           # Load immediate value 50 into r2 (value to subtract)

        movi    r6, subroutine1  # Load address of subroutine1 into r6
        jalr    r7, r6            # Call subroutine1, store return address in r7

        halt                    # Stop execution here

subroutine1: movi    r4, 10           # Load immediate memory address 10 into r4 (for storing return address)
        sw      r7, r4, 0         # Store the return address in memory at r4

        movi    r6, subroutine2  # Load address of subroutine2 into r6
        jalr    r7, r6            # Call subroutine2, store return address in r7

        lw      r7, r4, 0         # Load the return address back from memory
        jalr    r0, r7            # Return to caller

subroutine2: add     r3, r1, r2         # Add r1 (100) and r2 (50), result in r3 (r3 = 150)

        movi    r4, 20           # Load immediate memory address 20 into r4 (for storing return address)
        sw      r7, r4, 0         # Store the return address in memory at r4

        movi    r6, subroutine3  # Load address of subroutine3 into r6
        jalr    r7, r6            # Call subroutine3, store return address in r7

        lw      r7, r4, 0         # Load the return address back from memory
        jalr    r0, r7            # Return to caller

subroutine3: addi    r3, r3, -30       # Subtract 30 from r3 (r3 = 120)
        jalr    r0, r7            # Return to caller
