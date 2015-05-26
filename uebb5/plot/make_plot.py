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
import csv
import numpy as np

f = open("./data.csv", 'r')
reader = csv.reader(f, delimiter=' ')

data = np.zeros((4,48))

labels = ["pthread","central","dissemination","tree"]

for i,row in list(enumerate(reader)):
    for j in range(4):
        data[j][i] = float(row[j])

f.close()

fig = plt.figure(figsize=(5,4))
ax = fig.add_subplot(111)

for j in range(4):
    ax.plot(range(1,49), 1./data[j], label=labels[j])

ax.set_xlabel("Thread Count")
ax.set_ylabel("Average Time per Barrier[s]")
ax.legend(loc=4,ncol=2)
ax.set_yscale("log")
fig.subplots_adjust(bottom=0.15, left=0.15)

fig.savefig("barrier.pgf")

f = open("./table.tex",'w')
table = r"""\begin{tabular}{lllll}
& \textbf{Pthread} & \textbf{Central} & \textbf{Dissemination} & \textbf{Tree} \\
\textbf{Thread}"""
for i in xrange(4):
    table += " & Average"
table += r"\\"+"\n" + r"\textbf{count}"
for i in xrange(4):
    table += " & barrier"
table += r"\\"+"\n" + " "
for i in xrange(4):
    table += " & latency[s]"
table += r"\\\hline"+"\n" + " "

for i in [1,2,4,8,12,16,24,32,40,48]:
    table += str(i)
    row = ""
    for j in range(4):
        row += " & " + "{:.2e}".format(1./data[j][i-1])
    row += r" \\" 
    row += "\n"
    table += row
table += r"\end{tabular}"
f.write(table)
f.close()
