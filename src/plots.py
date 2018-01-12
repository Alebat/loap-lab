import matplotlib.pyplot as plt
import sys

import os, shutil
folder = 'plots'
for the_file in os.listdir(folder):
    file_path = os.path.join(folder, the_file)
    try:
        if os.path.isfile(file_path):
            os.unlink(file_path)
    except Exception as e:
        print(e)

last = None
count = 1
with open("data.txt", "r") as f:
    for l in f:
        p = l.split(" ")
        if last is None:
            last = p[0]
        elif last == p[0]:
            count += 1
        else:
            break

data = []
time = []

ind = 0
with open("data.txt", "r") as f:
    for l in f:
        col = ind % count
        p = l.replace("\n", "").split(" ")

        if ind < count:
            data.append([])
        data[col].append(p[1])

        if col == 0:
            time.append(p[0])
        ind += 1

for i in range(count):
    plt.plot(time, data[i])
    fname="plots/test_%s.png" % i
    plt.savefig(fname)
    plt.clf()
