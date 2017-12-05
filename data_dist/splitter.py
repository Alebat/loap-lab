import sys


def oo(fin, fout, num, den):
    with open(fin, "r") as f:
        with open(fout, "w") as fw:
            for index, line in enumerate(f):
                if index % den == num:
                    fw.write(line)


oo(sys.argv[1], sys.argv[1]+'0', 0, 3)
oo(sys.argv[1], sys.argv[1]+'1', 1, 3)
oo(sys.argv[1], sys.argv[1]+'2', 2, 3)


with open(sys.argv[1]+'2', "r") as f:
    with open(sys.argv[1]+'2f', "w") as fw:
        for index, line in enumerate(f):
            time = int(line.split(" ")[0])
            val = int(line.split(" ")[1].rstrip())
            if val > 0:
                fw.write(line.split(" ")[0] + " " + str(val) + "\n")
