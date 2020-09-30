lines = open("data.txt").read().split("\n")

program = []

program_starts = False

for line in lines:
    if program_starts:
        if line == "#END":
            break
        elif line.startswith("\t") and len(line) > 2:
            print(line)
            program.append(line)
    elif line == "#START:":
        program_starts = True
        

print(str(program_starts) + ", " + str(len(program)))
input("")