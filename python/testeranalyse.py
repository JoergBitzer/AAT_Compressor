import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf   # nachinstallieren mit pip install soundfile

path_to_files = 'C:/AudioDev/build/AAT_Compressor/Tester/ConsoleAppExample_artefacts/Debug'
filename_in = 'CompTestIn.wav'
filename_out = 'CompTestOut.wav'

fullfilename_in = path_to_files + '/' + filename_in
fullfilename_out = path_to_files + '/' + filename_out

data_in,fs = sf.read(fullfilename_in)
data_out,fs = sf.read(fullfilename_out)

fig,ax = plt.subplots()
ax.plot(data_in[:,0])
ax.plot(data_out[:,0])

plt.show()


