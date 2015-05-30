###########################################
#
#   Purpose: this script provides easy data
#            collection of other programs
#            and their output.
#
#   Author: Christoph Klein
#
#   Change: 2015-05-11
#
###########################################

import sys
import csv
import os
import operator
import string
import math

def printHelp():
    print """
data_creation.py
Script simplifies the collection of a program's output
usage: python data_creation.py -b binary_path -o output_path

Main options:
-ba "[(start end count), ... , log(start, end, count), parameter, ...]"
    A list with arguments for the binary. You can
    give numeric intervals in round brackets, which will
    be expanded numpy linspace like, and/or a constant parameters.
    With the log keyword you can generate log like expansion.
-k  "[key1, key2, ...]"
    Grep this keywords from the binary's output and
    write for each key the following string to the output file.
-v  be verbose 
-h  show this help message
"""    

argvString = string.join(sys.argv) + " "
for arg in sys.argv:
    if arg == "-h":
        printHelp()
        sys.exit()

if not ("-o" in sys.argv and "-b" in sys.argv) or len(sys.argv) < 5:
    raise IOError("To few command line arguments. Use -h for help.")

def giveVal(string, search):
    nr = string.find(search)
    if nr == -1: return "notAvail" 
    words = string[nr:].split()
    if len(words) > 1: return words[1]
    else: raise ValueError("giveValueToString: key not found")

## BINARY AND OUTPUT FILE HANDLING ##
opt_v   = "-v" in sys.argv
pth_bin = sys.argv[sys.argv.index("-b")+1]
pth_out = sys.argv[sys.argv.index("-o")+1]
if not os.path.exists(pth_bin):
    raise EnvironmentError("Binary " + pth_bin + " does not exist.")
if not pth_out[-4:] == ".csv":
    pth_out += ".csv"
    if opt_v: print "added .csv extension to output file"

## BINARY ARGUMENT HANDLING ##
strListBinArgs = ['']
if "-ba" in sys.argv:
    ## create the parameter string and remove whitespaces ##
    for i, arg in list(enumerate(sys.argv)):
        if arg == "-ba":
            strBinArgs = sys.argv[i+1]
    if ",," in strBinArgs:
        raise IOError("Binary parameter list has wrong format. Use -h for help.")
    ## sratch binary arguments out of the string ##
    strListBinArgs = strBinArgs[1:-1].split(',')
    strListBinArgs = [arg.strip() for arg in strListBinArgs]
    for i,arg in list(enumerate(strListBinArgs)):
        ## handle the expansion of the numeric parameters ##
        if arg[0] == '(' or arg[:4] == "log(":
            if arg[:4] == "log(":
                log = True
                arg = arg[3:]
            else:
                log = False
                
            start = arg[1:-1].split()[0]
            end = arg[1:-1].split()[1]
            count = arg[1:-1].split()[2]
            if len(start) == 0 or len(end) == 0 or len(count) ==  0 or not arg[-1] == ')':
                raise IOError("Argument " + arg + " in binary parameter\
                        list has wrong format. Use -h for help.")
             
            start = float(start)
            end = float(end)
            count = float(count)
            if start > end:
                raise ValueError("For numeric parameter expansion: start < end!")
            
            space = []
            if log:
                end_exp = math.log10(end)/math.log10(2.0)
                beg_exp = math.log10(start)/math.log10(2.0)
                inc_exp = (end_exp - beg_exp) / count
                act_exp = beg_exp
                x = 2**beg_exp
            else:
                increment = (end - start)/(count - 1.0)
                x = start
            for ii in xrange(int(count)):
                space.append(x)
                if log:
                    act_exp += inc_exp
                    x = 2**act_exp
                else:
                    x +=  increment
            if opt_v: print "Numeric parameter expansion of argument", i, "resulted in", space
            strListBinArgs[i] = space

## for strings ##
def append_to_all_elements(lis, el):
    for i, l in list(enumerate(lis)):
        lis[i] = l + " " + el

## for strings ##
def array_blower(lis, array):
    add = []
    for l in lis:
        for a in array:
            add.append(l + " " + str(a))
    return add 

strCommandList = [pth_bin + " "]

for arg in strListBinArgs:
    if type(arg) == list:
        strCommandList = array_blower(strCommandList, arg)
    else:
        append_to_all_elements(strCommandList, arg)
        
print "I am going to execute the following commands:"
for com in strCommandList:
    print "\t" + com
answer = raw_input("Do you want to resume[Y/n]?") 
if "No" in answer or "n" in answer or "Nein" in answer or "N" in answer:
    print "Aborting ..."
    sys.exit()
    
## KEYWORD HANDLING ## 
if "-k" in sys.argv:
    f_out = open(pth_out, 'w')
    strKeyArgs = sys.argv[sys.argv.index("-k") + 1]
    if not strKeyArgs[0] == '[' or not strKeyArgs[-1] == ']':
        raise IOError("Keyword list has wrong format.")
    strListKeyArgs = strKeyArgs[1:-1].split(",")
    for i, arg in list(enumerate(strListKeyArgs)):
        strListKeyArgs[i] = arg.strip()
    if opt_v:
        print "I got the following keywords from your input:"
        for arg in strListKeyArgs:
            print "\t" + arg
elif opt_v:
    print "I did not get any keyword argument list."
    print "No output file will be generated." 

## HELP FUNCTION TO GREP THE CORRECT UNITS ##
def get_units(out, keys):
    units = []
    for k in keys:
        index = out.find(k)
        if index == -1:
            units.append('')
            break
        sub_out = out[index:] 
        newline = sub_out.find('\n')
        if newline == -1: newline = len(sub_out)
        line = sub_out[:newline]
        words = line.split()
        keypos = words.index(k)
        if keypos + 2 >= len(words):
            units.append('')
        else:
            units.append(words[keypos + 2])
    return units 

## EXECUTE BINARY ##    
f_initialized = False
for i, command in list(enumerate(strCommandList)):
    pipe = os.popen(command, 'r')
    output = pipe.read()
    pipe.close()
    if "-k" in sys.argv and not f_initialized:
        strListUnits = get_units(output, strListKeyArgs)
        key2Fieldname = dict()
        fieldnames = []
        if opt_v: print "Determined units for keys like:"
        for key, unit in zip(strListKeyArgs, strListUnits):
            if opt_v: print key + "\t[" + unit + ']'
            key2Fieldname[key] = key + '[' + unit + ']'
            fieldnames.append(key + '[' + unit + ']')
        writer = csv.DictWriter(f_out, delimiter=' ', fieldnames=fieldnames)
#        writer.writeheader() # does not work on moore. Therefore we do the hacky solution:
        headerWriter = csv.writer(f_out, delimiter=' ')
        headerWriter.writerow(fieldnames)

        f_initialized = True
    if opt_v: 
        print "\n\n"
        print "EXECUTED COMMAND",command 
        print "BELONGING OUTPUT"
        for li in output.split('\n'):
            print "\t" + li
    ## scratch every value for every key ## 
    if "-k" in sys.argv:
        toBeWritten = dict()
        for key in strListKeyArgs:
            val = giveVal(output, key) 
            toBeWritten[key2Fieldname[key]] = val
        if opt_v:
            print "I scratched the following key value pairs from the output:"
            for field, v in toBeWritten.iteritems():
                print field + "\t" + v
        writer.writerow(toBeWritten)

if "-k" in sys.argv:
    f_out.close()
