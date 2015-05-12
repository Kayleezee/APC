# -*- coding: utf-8 -*-
import matplotlib as mp
mp.use("pgf")

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


mp.rcParams.update(pdf_with_custom_preamble)

import matplotlib.pyplot as plt
import numpy as np
import csv

f = open("./data_local_big.csv", 'r')
f_r = open("./databig.csv", 'r')

reader = csv.reader(f, delimiter=' ')
reader_r = csv.reader(f_r, delimiter=' ')
plotData = dict()
plotData_r = dict()

for row in reader:
    if not row[0] in plotData:
        plotData[row[0]] = []
    numbers = [float(x) for x in row[2:]]
    plotData[row[0]].append(numbers)
    
for row in reader_r:
    if not row[0] in plotData_r:
        plotData_r[row[0]] = []
    numbers = [float(x) for x in row[2:]]
    plotData_r[row[0]].append(numbers)
    
for key in plotData:
    plotData[key] = np.transpose(plotData[key])
for key in plotData_r:
    plotData_r[key] = np.transpose(plotData_r[key])
    
fig = plt.figure(figsize=(6,5))
ax = fig.add_subplot(111)

cols = ['black','red', 'blue']
for key,c in zip(plotData,cols):
    ax.plot(plotData[key][3], plotData[key][1]/10**6, label=key+ " local", color=c,ls="--")
for key,c in zip(plotData, cols):    
    ax.plot(plotData_r[key][3], plotData_r[key][1]/10**6, label=key+" moore",color=c)


ax.set_xlabel("Thread Count")    
ax.set_ylabel("Increments $[10^6/s]$")

ax.legend(bbox_to_anchor=(0,1.02,1,.102),loc=3,ncol=2,mode="expand",borderaxespad=0)
ax.grid()
fig.savefig("plotbig.pgf")

fig = plt.figure(figsize=(6,5))
ax = fig.add_subplot(111)

cols = ['black','red', 'blue']
for key,c in zip(plotData,cols):
    ax.plot(plotData[key][3], plotData[key][0], label=key+ " local", color=c,ls="--")
for key,c in zip(plotData, cols):    
    ax.plot(plotData_r[key][3], plotData_r[key][0], label=key+" moore",color=c)

ax.set_yscale("log")
ax.set_xlabel("Thread Count")    
ax.set_ylabel("Total Time $[s]$")

ax.legend(bbox_to_anchor=(0,1.02,1,.102),loc=3,ncol=2,mode="expand",borderaxespad=0)
ax.grid()
fig.savefig("plotbigtotal.pgf")
f.close()
