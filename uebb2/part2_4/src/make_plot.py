# -*- coding: utf-8 -*-
pdf_with_custom_preamble = {
    "font.family":"serif",
    "font.serif":[],
    "font.size":11,
    "pgf.rcfonts":False,
    "text.usetex":True,
    "pgf.texsystem":"pdflatex",
    "pgf.preamble": [
        r"\usepackage[T1]{fontenc}",
        r"\usepackage{lmodern}",
        ]
}
import matplotlib as mp
mp.rcParams.update(pdf_with_custom_preamble)
mp.use("pgf")
import csv
import collections
import numpy as np
import matplotlib.pyplot as plt

f = open("data_remote.csv", 'r')
reader = csv.reader(f, delimiter=' ')
reader.next() ## skip header ##

plotdata = dict()

for row in reader:
    arraySize = int(row[0])
    threadCount = int(row[1])
    time = float(row[2])
    bandwidth = float(row[3])

    if arraySize in plotdata:
        plotdata[arraySize].append([threadCount, time, bandwidth])
    else:
        plotdata[arraySize] = [[threadCount, time, bandwidth]]

for arraySize in plotdata:
    plotdata[arraySize] = np.transpose(plotdata[arraySize])

plotdata = collections.OrderedDict(sorted(plotdata.items()))

fig = plt.figure(figsize=(6,5))
ax = fig.add_subplot(111)

for i,csize in list(enumerate(plotdata)):
    RGBValue = float(i)/float(len(plotdata))
    col = (RGBValue, RGBValue, RGBValue)
    ax.plot(plotdata[csize][0], plotdata[csize][2],\
            label="$2^{"+str(int(np.log2(csize*4)))+"}$ Elements",\
            color = col, lw = 2)

ax.set_ylabel("Bandwidth [Bytes/s]")    
ax.set_xlabel("Thread Count")
ax.grid()
ax.legend(bbox_to_anchor=(1.05, 1.0), loc=2, borderaxespad=0.)
fig.subplots_adjust(right= 0.8)
fig.savefig("./threads_remote.pgf")

