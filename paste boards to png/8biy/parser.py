lines = open("data.txt").read().split("\n")

debug = True

program = []

program_starts = False

asm = []
macros = []

for line in lines:
    line = line.split("//")[0].rstrip()
    
    if line.startswith("0b"):
        txt = line.split("\t")
        asm.append((txt[0], txt[1].split(" ")))
    elif line.startswith("def "):
        txt = line.split(" ")
        macros.append((txt[1], txt[2]))
    elif program_starts:
        if line == "#END":
            break
        elif line.startswith("\t") and len(line) > 2:
            program.append(line[1:])
        elif line.endswith(":"):
            macros.append(("{0:b}".format(len(program)).zfill(6), line[:-1]))
            
    elif line == "#START:":
        macros.append(("000000", "START"))
        program_starts = True
        
if debug:    
    for m in macros:
        print(m[0] + " -> " + m[1])
        
    print("\n")

    for inst in asm:
        line = inst[0] + ">"
        for word in inst[1]:
            line += word + "|"
        print(line)

    print("\n")

def find(matrix, target):
    i = 0
    for e in matrix:
        if e[1] == target:
            return (i, e[0])
        i += 1
    return (-1, "")
    
errors = not program_starts

line_i = 0

for str_inst in program:
    
    start_line = str(line_i).zfill(2) + "|"
    line_i += 1
    inst = []
    
    for param in str_inst.split(" "):
        for m in macros:
            if (param == m[1]):
                inst.append(m[0])
                break
        else:
            inst.append(param)
    if len(inst) == 1 and inst[0].startswith("0b"):
        if debug:
            print(start_line + inst[0][2:].zfill(6) + "> " + str_inst)
        else:
            print(start_line + inst[0][2:].zfill(6))
    elif len(inst) == 1 and (inst[0].startswith("0") or inst[0].startswith("1")):
        if debug:
            print(start_line + inst[0] + "> " + str_inst)
        else:
            print(start_line + inst[0])
    else:
        inst_i = find(asm, inst)
        if (inst_i[0] == -1):
            errors = True
            print(start_line + "EEEEEE> " + str_inst + " - doesn't exist")
            continue
        elif debug:
            print(start_line + inst_i[1][2:] + "> " + str_inst)
        else:
            print(start_line + inst_i[1][2:])
    
print(str(errors) + ", " + str(len(program)))
input("")