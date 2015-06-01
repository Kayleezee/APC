##################################################################
#
# FILENAME    - myplot.py
#
# DESCRIPTION - This file provides functions for common used
#               matplotlib.pyplot layouts.
#
# AUTHOR      - Christoph Klein
#
# LAST CHANGE - 2015-05-31
#
##################################################################

import matplotlib.pyplot as plt

def legend_top(ax):
    ax.legend(bbox_to_anchor=(0,1.02,1,.102), loc=3, ncol=2, mode="expand", borderaxespad=0)

def legend_right(ax):
    ax.legend(bbox_to_anchor=(1.05,1), loc=2, borderaxespad=0)
