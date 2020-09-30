file = open("data.txt")
lines = file.readlines()
result = []

instruction_counter = 0
param_counter = 0

for line in lines:
    if line.startswith("0b"):
        code = "0b" + str((0b10 & param_counter) >> 1) + str(0b1 & param_counter) + str((0b1000 & instruction_counter) >> 3) + str((0b100 & instruction_counter) >> 2) + str((0b10 & instruction_counter) >> 1) + str(0b1 & instruction_counter)
        result.append(code + line[8:])
        if param_counter < 3:
            param_counter += 1
        else:
            param_counter = 0
            instruction_counter += 1
    else:
        result.append(line)
file.close()
filew = open("data.txt", "w")
filew.writelines(result)
filew.close()