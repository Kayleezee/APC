import os
import csv

def giveVal(string, search):
    nr = string.find(search)
    if nr == -1: return "notAvail" 
    words = string[nr:].split()
    if len(words) > 1: return words[1]
    else: raise ValueError("giveValueToString: key not found")


f = open("data.csv","w")
writer = csv.writer(f, delimiter = " ")
writer.writerow(["ArraySize", "ThreadCount","Time[s]","Bandwidth[Bytes/s]"])
xArraySize = [2**x for x in range(16,25)]
xThreadCount = range(1,20)
measurements = 30


for arraySize in xArraySize:
    for threadCount in xThreadCount:
        command = "../mtlb " + str(arraySize) + " " + str(threadCount)
        print "COMMAND =", command
        time = 0.0
        bandwidth = 0.0
        for i in range(0,measurements):
            pipe = os.popen(command, 'r')
            output = pipe.read()
            pipe.close()
            time += float(giveVal(output, "time"))
            bandwidth += float(giveVal(output, "bandwidth"))
        time /= float(measurements)    
        bandwidth /= float(measurements)
        writer.writerow([arraySize, threadCount, time, bandwidth])

f.close()



