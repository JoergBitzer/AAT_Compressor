import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf

datapath = "C:/AudioDev/build/AAT_Compressor/Tester/ConsoleAppExample_artefacts/Debug"
filein = "testin.wav"
fileout = "testout.wav"

# build the path to the input and output files
inputfile = datapath + "/" + filein
outputfile = datapath + "/" + fileout

# read the input file
datain = sf.read(inputfile)
datain = datain[0]

# read the output file
dataout = sf.read(outputfile)
dataout = dataout[0]

# plot the input and output signals
fig, ax = plt.subplots()
ax.plot(datain)
ax.plot(dataout)
ax.set(xlabel='Sample', ylabel='Amplitude',
       title='Input and Output Signals')
ax.grid()
plt.show()
