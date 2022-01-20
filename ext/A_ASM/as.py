'''
The assembler for AwkOS
''' 
from cProfile import label
from sys import argv

labs = list[str]

instructions = [
    "nop", "deflabel"
]
global outp
outp = str()

def ParseFile(filelines, filename):
    
    print("Now parsing file: " + filename)
    for i in range(len(filelines)):
        filelines[i] = filelines[i].strip()
        # is label?
        if filelines[i].endswith(':'):
            labs.append(filelines[i].trim(':'))
            outp += chr(0x01) # instruction deflabel
            
            
        
    pass    

for i in range(len(argv)):
    if i == 0: 
        pass
    else:
        if argv[i] == "--ctype":
            # output written array to paste into C kernel
            pass
        in_file = open(argv[i], 'r')
        if not in_file.closed:
            ls = in_file.readlines()
            ParseFile(ls, argv[i])