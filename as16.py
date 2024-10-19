import sys
opcodes= { 'add' : '000', 'addi' : '001', 'nand' : '010', 'lui' : '011', 'sw' : '100', 'sb' : 'SB','lw' : '101', 'lb' : 'LB', 'beq' : '110', 'jalr' : '111' }
regcodes= { 'r0' : '000', 'r1' : '001', 'r2' : '010', 'r3' : '011', 'r4' : '100', 'r5' : '101','r6' : '110', 'r7' : '111'}
lines=[]
lineaddr=[]
labels={}

def placeReg():
    for i in range(0,len(lines)):
        for j in range(len(lines[i])-1,-1,-1):
            try:
                lines[i][j]=regcodes[lines[i][j]] #replace the opcode with the key value if we are on it
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
                    lines[i][j]=int(lines[i][j],16)
                elif(lines[i][j].find('0o') != -1): #oct values 0oaaa
                    lines[i][j]=int(lines[i][j],8)
                elif(lines[i][j].find('0b') != -1): #binary values
                    lines[i][j]=lines[i][j][2:]
                elif(int(lines[i][j] != -1)):
                    print("NUMBER : ",lines[i][j])

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
    print(lines)


