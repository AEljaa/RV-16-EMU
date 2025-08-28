        movi    r1, 100          # Load immediate value 100 into r1 (value to manipulate)
        movi    r2, 50           # Load immediate value 50 into r2 (value to subtract)
        movi    r6, subroutine1  # Load address of subroutine1 into r6

subroutine1: add r0,r0,r0           # Load immediate memory address 10 into r4 (for storing return address)
