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

import csv
import numpy as np
import collections
import matplotlib as mp
mp.rcParams.update(pdf_with_custom_preamble)
mp.use("pgf")
import matplotlib.pyplot as plt

f = open("./data_remote.csv", "r")
reader = csv.reader(f, skipinitialspace=True, delimiter="\t")
reader.next() ## skip header ##


## GET PLOTDATA ##
plotdata = dict()
for row in reader:
    csize = int(row[0])
    stride = int(row[2])
    loadtime = float(row[4])

    if csize in plotdata: 
        plotdata[csize].append([stride, loadtime])
    else:
        plotdata[csize] = [[stride, loadtime]]




fig = plt.figure(figsize=(6,4.5))

ax = fig.add_subplot(111)
plotdata = collections.OrderedDict(sorted(plotdata.items()))

for i,csize in list(enumerate(plotdata)):
    plotdata[csize] = np.transpose(plotdata[csize])
    RGBValue = float(i)/float(len(plotdata))
    col = (RGBValue, RGBValue, RGBValue)
    ax.plot(4*plotdata[csize][0], plotdata[csize][1],\
            label="$2^{"+str(int(np.log2(csize*4)))+"}$ Bytes",\
            color = col, lw = 2)

ax.set_xlabel("stride [Bytes]")
ax.set_ylabel("loadtime [ns]")
ax.set_xscale("log", basex=2)
ax.grid()
ax.legend(bbox_to_anchor=(1.05, 1.1), loc=2, borderaxespad=0.)
fig.subplots_adjust(right= 0.8)
fig.savefig("pointer_chasing_remote.pgf")
