import sys
import os

opcodes = { 'add': '000', 'addi': '001', 'nand': '010', 'lui': '011', 
           'sw': '100', 'sbl': 'SBL', 'sbu': 'SBU', 'lw': '101', 
           'lbl': 'LBL', 'lbu': 'LBU', 'movi': 'MOVI', 'beq': '110', 'jalr': '111' }

regcodes = { 'r0': '000', 'r1': '001', 'r2': '010', 'r3': '011', 
            'r4': '100', 'r5': '101', 'r6': '110', 'r7': '111' }

lines = []
labels = {}
movi_expansion_count = 0

def unsignextend(num, size, bit):
    return bit * (size - len(num)) + num if len(num) < size else num

def twos_comp(val, bits):
    """compute the 2's complement of int value val"""
    if val >= 0:
        return unsignextend(bin(val)[2:], bits, '0')
    else:
        mask = (1 << bits) - 1
        return bin(val & mask)[2:].zfill(bits)

def placeReg():
    """ Replaces register names in the code with their binary values. """
    for line in lines:
        for i, part in enumerate(line):
            if part in regcodes:
                line[i] = regcodes[part]

def placeAddr():
    """ Adjusts offsets based on labels and MOVI expansion. """
    global movi_expansion_count
    
    for i, line in enumerate(lines):
        for j, part in enumerate(line):
            if part in labels:
                # Calculate offset accounting for MOVI expansion
                offset = 2 * (labels[part] - (i + movi_expansion_count))
                
                if offset >= 0:
                    line[j] = unsignextend(bin(offset)[2:], 7, '0')
                else:
                    line[j] = twos_comp(offset, 7)

def switch(_type, num, bits):
    if _type == "unsigned":
        return unsignextend(bin(int(num))[2:], bits, '0')
    elif _type == "signed":
        return twos_comp(int(num), bits)
    elif _type == "hex":
        return unsignextend(bin(int(num, 16))[2:], bits, '0')
    elif _type == "oct":
        return unsignextend(bin(int(num, 8))[2:], bits, '0')

def sb_lb():
    for i in range(len(lines)):
        for j in range(len(lines[i])-1, -1, -1):
            if lines[i][j] == "SBL":
                lines[i][j] = "100"
                if j+3 < len(lines[i]):
                    lines[i][j+3] = unsignextend(bin(int(lines[i][j+3], 2) + 1)[2:], 7, '0')
            elif lines[i][j] == "SBU":
                lines[i][j] = "100"
                if j+3 < len(lines[i]):
                    lines[i][j+3] = unsignextend(bin(int(lines[i][j+3], 2) + 1)[2:], 7, '0')
            elif lines[i][j] == "LBL":
                lines[i][j] = "101"
                if j+3 < len(lines[i]):
                    lines[i][j+3] = unsignextend(bin(int(lines[i][j+3], 2) + 1)[2:], 7, '0')
            elif lines[i][j] == "LBU":
                lines[i][j] = "101"
                if j+3 < len(lines[i]):
                    lines[i][j+3] = unsignextend(bin(int(lines[i][j+3], 2) + 1)[2:], 7, '0')

def handleMOVI(line):
    """Handle MOVI instruction expansion to LUI and ADDI."""
    global movi_expansion_count
    movi_expansion_count += 1  # Track MOVI expansion
    
    if line[1] in regcodes:
        reg = regcodes[line[1]]
    else:
        reg = line[1]
    
    # Parse the immediate value
    imm_str = line[2]
    if imm_str in labels:
        imm_val = labels[imm_str] * 2  # Convert to byte address
    else:
        # Handle different number formats
        if imm_str.startswith('0x'):
            imm_val = int(imm_str, 16)
        elif imm_str.startswith('0o'):
            imm_val = int(imm_str, 8)
        elif imm_str.startswith('0b'):
            imm_val = int(imm_str, 2)
        else:
            imm_val = int(imm_str)
    
    # Split into upper and lower parts
    upper = (imm_val >> 6) & 0x3FF  # 10 bits
    lower = imm_val & 0x3F  # 6 bits
    
    # Generate LUI and ADDI instructions
    lui_instr = [opcodes['lui'], reg, unsignextend(bin(upper)[2:], 10, '0')]
    addi_instr = [opcodes['addi'], reg, reg, unsignextend(bin(lower)[2:], 7, '0')]
    
    return lui_instr + addi_instr

def preprocess():
    """First pass: process labels and basic instruction parsing."""
    global movi_expansion_count
    movi_expansion_count = 0
    
    # First, collect all labels
    instruction_count = 0
    for i, line in enumerate(lines):
        if ':' in line:
            label_part = line.split(':')[0].strip()
            labels[label_part] = instruction_count
            line = line.split(':', 1)[1].strip()
        
        if line and not line.startswith('#'):
            tokens = line.split()
            if tokens and tokens[0] == 'movi':
                instruction_count += 2  # MOVI expands to two instructions
            else:
                instruction_count += 1
    
    # Second pass: process instructions
    new_lines = []
    for i, line in enumerate(lines):
        # Remove comments
        if '#' in line:
            line = line[:line.find('#')].strip()
        
        # Handle labels
        if ':' in line:
            line = line.split(':', 1)[1].strip()
        
        if not line or line.isspace():
            continue
        
        # Replace commas with spaces and split
        line = line.replace(',', ' ').split()
        
        # Replace opcodes and registers
        for j, part in enumerate(line):
            if part in opcodes:
                line[j] = opcodes[part]
            elif part in regcodes:
                line[j] = regcodes[part]
        
        # Handle special cases
        if line[0] == opcodes['lui'] and len(line) > 2:
            # Process LUI immediate value
            imm = line[2]
            if imm.startswith('0x'):
                line[2] = unsignextend(bin(int(imm, 16))[2:], 10, '0')
            elif imm.startswith('0o'):
                line[2] = unsignextend(bin(int(imm, 8))[2:], 10, '0')
            elif imm.startswith('0b'):
                line[2] = unsignextend(imm[2:], 10, '0')
            else:
                line[2] = unsignextend(bin(int(imm))[2:], 10, '0')
        
        elif line[0] == 'MOVI':
            # Handle MOVI expansion
            expanded = handleMOVI(line)
            new_lines.extend([expanded[:4], expanded[4:]])
            continue
        
        elif line[0] in ['000', '010']:  # ADD or NAND
            if len(line) < 5:
                line.append('0000')
        
        new_lines.append(line)
    
    # Replace original lines with processed lines
    lines.clear()
    lines.extend(new_lines)

def write_hex(output_file):
    """Write the assembled code to a hex file."""
    try:
        if os.path.exists(output_file):
            os.remove(output_file)
    except:
        pass
    
    with open(output_file, 'w') as f:
        for line in lines:
            # Convert to binary string
            bin_str = ''.join(str(bit) for bit in line)
            
            # Pad to 16 bits if needed
            if len(bin_str) < 16:
                bin_str = bin_str.ljust(16, '0')
            elif len(bin_str) > 16:
                bin_str = bin_str[:16]  # Truncate to 16 bits
            
            # Convert to hex
            hex_val = format(int(bin_str, 2), '04X')
            
            # Write in little-endian format
            f.write(hex_val[2:4] + hex_val[0:2] + '\n')

if __name__ == "__main__":
    if len(sys.argv) < 5 or sys.argv[1] != '-s' or sys.argv[3] != '-o':
        print('Usage: python as16.py -s <source_assembly_file_name> -o <dest_hex_file_name>')
        exit(1)
    
    # Read input file
    with open(sys.argv[2], 'r') as file:
        lines = file.readlines()
    
    # Preprocess
    preprocess()
    
    # Place addresses
    placeAddr()
    
    # Place registers
    placeReg()
    
    # Handle special load/store instructions
    sb_lb()
    
    # Write output
    write_hex(sys.argv[4])
