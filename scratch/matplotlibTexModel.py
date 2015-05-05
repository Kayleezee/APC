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

import pylab as pl

"""
fig = pl.figure(figsize=(6,2)) # um die groesse des bildes zu aendern
fig.savefig("out.pgf",dpi = 100) # kann dann in einer latex datei eingebunden werden

# Die Einbindung in ein latex Document erfolgt dann durch

\documentclass{scrartcl}

\usepackage{pgf}

\begin{document}
\begin{figure}
    \input{out.pgf}
\end{figure}
\end{document}
"""
