# -*- coding: utf-8 -*-
PGF = True

if PGF:
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

import reader as r
import matplotlib.pyplot as plt
import numpy as np
import myplot as mplt


rlist = [] # reader list

## append (reader label, reader object) ##
rlist.append(("Default", r.Reader("dataDefault.csv", ' ')))
rlist.append(("Membind 0,1",r.Reader("dataMembind.csv", ' ')))
rlist.append(("Interleave=all",r.Reader("dataInterleave.csv", ' ')))
rlist.append(("Membind 0",r.Reader("dataMembind0.csv", ' ')))

dlist = [] # dictionary list
for l, r in rlist:
    dlist.append((l, r.get_dict("thread_count[]")))
    r.close()

fig = plt.figure(figsize=(6.3,3))
ax = fig.add_subplot(111)

for l, d in dlist:
    ax.plot(d["thread_count[]"], d["prefix_sum_time_parallel[s]"], label=l, marker='.',markersize=5)

ax.set_xlabel("Thread Count")
ax.set_ylabel("Mean Parallel Prefix Summation Time [s]")
mplt.legend_top(ax)
fig.subplots_adjust(bottom=0.15,top=0.78)

if PGF:
    fig.savefig("plot.pgf")
else:
    plt.show()
