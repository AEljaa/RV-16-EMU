
        movi    1, 100          # Load immediate value 100 into r1 (value to manipulate)
        movi    2, 50           # Load immediate value 50 into r2 (value to subtract)

        movi    6, subroutine1  # Load address of subroutine1 into r6
        jalr    7, 6            # Call subroutine1, store return address in r7

        halt                    # Stop execution here

# Subroutine 1: Calls subroutine 2, stores and loads return address from memory
subroutine1: movi    4, 10           # Load immediate memory address 10 into r4 (for storing return address)
        sw      7, 4, 0         # Store the return address in memory at r4

        movi    6, subroutine2  # Load address of subroutine2 into r6
        jalr    7, 6            # Call subroutine2, store return address in r7

        lw      7, 4, 0         # Load the return address back from memory
        jalr    0, 7            # Return to caller

# Subroutine 2: Adds and subtracts values, then stores the return address in memory
subroutine2: add     3, 1, 2         # Add r1 (100) and r2 (50), result in r3 (r3 = 150)

        movi    4, 20           # Load immediate memory address 20 into r4 (for storing return address)
        sw      7, 4, 0         # Store the return address in memory at r4

        movi    6, subroutine3  # Load address of subroutine3 into r6
        jalr    7, 6            # Call subroutine3, store return address in r7

        lw      7, 4, 0         # Load the return address back from memory
        jalr    0, 7            # Return to caller

# Subroutine 3: Subtracts a value and returns
subroutine3: addi    3, 3, -30       # Subtract 30 from r3 (r3 = 120)
        jalr    0, 7            # Return to caller

# Data section
.fill   0                      # Reserve memory (return address)
.fill   0                      # Reserve memory (return address)
.fill   0                      # Reserve memory (calculation storage)
