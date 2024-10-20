import sys
opcodes= { 'add' : '000', 'addi' : '001', 'nand' : '010', 'lui' : '011', 'sw' : '100', 'sb' : 'SB','lw' : '101', 'lb' : 'LB', 'beq' : '110', 'jalr' : '111' }
regcodes= { 'r0' : '000', 'r1' : '001', 'r2' : '010', 'r3' : '011', 'r4' : '100', 'r5' : '101','r6' : '110', 'r7' : '111'}
lines=[]
lineaddr=[]
labels={}

def unsignextend(num,size,bit):
    print(len(num)," bit number ",num)
    if(len(num)<size):
        num= bit * (size - len(num)) + num
    return num

def twos_comp(val, bits):
    """compute the 2's complement of int value val"""
    return bin(val - (1 << bits))[3:]        # compute negative value

def placeReg():
    for i in range(0,len(lines)):
        for j in range(len(lines[i])-1,-1,-1):
            try:
                lines[i][j]=regcodes[lines[i][j]] #replace the opcode with the key value if we are on it
            except:
                print("can't do: ",lines[i][j])
def placeAddr():
    for i in range(0,len(lines)):
        for j in range(len(lines[i])-1,-1,-1):
            try:
                offset=(labels[lines[i][j]]-i) #replace the opcode with the key value if we are on it
                print("Address offset" ,offset)
                if(offset>0):
                    lines[i][j]=unsignextend(bin(offset)[2:],7,'0')
                else:
                    lines[i][j]=twos_comp(int(bin(int(offset))[3:],2),7)
            except:
                print("can't do: ",lines[i][j])

def preprocess(lables):
    for i,line in enumerate(lines):
        lines[i]=line.strip()
        if(lines[i].find('#') != -1):
            lines[i]=line[0:lines[i].find('#')].strip() #remove the comments
        lines[i]=lines[i].replace(',',' ') #remove commas, since we will spilt opcodes up
        if(line.find(':') != -1):
            labels[lines[i][0:lines[i].find(':')]]=i #assign the line number (after removing empty spaces) as the adress
            lines[i] = lines[i][lines[i].find(':')+1:].strip()
       # nop and .fill can be done here 
        lines[i]=lines[i].split() 
        for j in range(len(lines[i])-1,-1,-1):
            try:
                lines[i][j]=opcodes[lines[i][j]] #replace the opcode with the key value if we are on it
            except:
                if(lines[i][j].find('0x') != -1): #hex values 0xaaa
                    lines[i][j]=unsignextend(bin(int(lines[i][j],16))[2:],7,'0')
                elif(lines[i][j].find('0o') != -1): #oct values 0oaaa
                    lines[i][j]=unsignextend(bin(int(lines[i][j],8))[2:],7,'0')
                elif(lines[i][j].find('0b') != -1): #binary values
                    lines[i][j]=unsignextend(lines[i][j][2:],7,'0')
                elif(lines[i][j]=='lui'): #10 bit lui imm values
                    print("LUI ",bin(int(lines[i][j+2])))
                    lines[i][j+2]=unsignextend(bin(int(lines[i][j+2]))[2:],10,'0')
                elif((opcodes.get(lines[i][j],0) or regcodes.get(lines[i][j],0)) == 0):
                    if(labels.get(lines[i][j])):
                        print("Label : ",lines[i][j])
                        continue
                    elif(lines[i][j]=="halt"):
                        print("halt : ",lines[i][j])
                    elif(lines[i][j].isdigit()):#positive numbers
                        print("NUMBER : ",lines[i][j])
                        lines[i][j]=unsignextend(bin(int(lines[i][j]))[2:],7,'0')
                    elif(lines[i][j][0]=='-' and lines[i][j][1:].isdigit()):#negative numbers
                        print("negative number",bin(int(lines[i][j]))[3:])
                        lines[i][j]=twos_comp(int(bin(int(lines[i][j]))[3:],2),7)
                        print("negative 7 bit binary number",lines[i][j])


if __name__ == "__main__":
    if(len(sys.argv)!=2):
        print('Usage: python as16.py <assembly_file_name> -o <hex_file_name>')
        exit(1)
    try:
        file = open(sys.argv[1],'r')
    except:
        print(f"File ./{sys.argv[1]} does not exist")
        exit(1)
    while True:
        line= file.readline()
        if not line:
            break #if at eof then exit loop
        if(line.strip()==""): #if line is empty then remove
            continue
        lines.append(line.strip())
    file.close()
    # pass 1
    preprocess(labels)
    # pass 2
    placeReg()
    # PASS 3
    placeAddr()
    print(lines)


