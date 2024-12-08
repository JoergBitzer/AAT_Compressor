import numpy as np
import matplotlib.pyplot as plt

def draw_compressor_curve(threshold, ratio, knee_width, makeup_gain):
    x = np.linspace(-80, 2, 100)
    y = np.zeros_like(x)
    for i in range(len(x)):
        if (x[i] - threshold < -knee_width/2):
            y[i] = x[i]
        elif (x[i] - threshold > knee_width/2):
            y[i] = threshold + (x[i] - threshold) / ratio
        else:
            y[i] = x[i] + (1/ratio - 1) * (x[i] - threshold + knee_width/2)**2 / (2*knee_width)
    y += makeup_gain
    fig, ax = plt.subplots()
    ax.plot(x, y)
    ax.set(xlabel='Input (dB)', ylabel='Output (dB)',
           title='Compressor Curve')
    ax.grid()
    plt.show()

def compute_automakeupgain(threshold, ratio):
    return -threshold*(1- 1 / ratio)

threshold = -40
ratio = 2
knee_width = 20
makeup_gain = compute_automakeupgain(threshold, ratio)

draw_compressor_curve(threshold, ratio, knee_width, makeup_gain)

def compute_gain(input, threshold, ratio, knee_width, makeup_gain):
    output = 0
    if (input - threshold < -knee_width/2):
        output = input
    elif (input - threshold > knee_width/2):
        output = threshold + (input - threshold) / ratio
    else:
        output =  input + (1/ratio - 1) * (input - threshold + knee_width/2)**2 / (2*knee_width) 
    output += makeup_gain
    gain = output - input
    return gain

# draw gain curve
x = np.linspace(-80, 2, 100)
y = np.zeros_like(x)
for i in range(len(x)):
    y[i] = compute_gain(x[i], threshold, ratio, knee_width, makeup_gain)

fig, ax = plt.subplots()
ax.plot(x, y)
ax.set(xlabel='Input (dB)', ylabel='Gain (dB)',
       title='Gain Curve')
ax.grid()
plt.show()

def compute_rms_recursive(input, tau, fs):
    alpha = np.exp(-1/(tau*fs))
    rms = np.zeros_like(input)
    # rms[0] = input[0]**2
    rms[0] = 0
    for i in range(1, len(input)):
        rms[i] = alpha * rms[i-1] + (1 - alpha) * input[i]**2
    rms = np.sqrt(rms)
    return rms

def compute_att_rel_dynamics(input, tau_attack, tau_release, fs):
    alpha_attack =  np.exp(-1/(tau_attack*fs))
    alpha_release = np.exp(-1/(tau_release*fs))
    att_rel_dynamics = np.zeros_like(input)
    att_rel_dynamics[0] = 0
    for i in range(1, len(input)):
        if input[i] < att_rel_dynamics[i-1]:
            att_rel_dynamics[i] = alpha_attack * att_rel_dynamics[i-1] + (1 - alpha_attack) * input[i]
        if input[i] > att_rel_dynamics[i-1]:
            att_rel_dynamics[i] = alpha_release * att_rel_dynamics[i-1] + (1 - alpha_release) * input[i]
    return att_rel_dynamics

fs = 44100
len_sec = 1
datain = np.ones(fs*len_sec) * 0.01 #linear
datain[fs//2:3*fs//4] = 0.5 

#fig, ax = plt.subplots()
#ax.plot(datain)
#plt.show()

tau_rms = 0.005
tau_attack = 0.01
tau_release = 0.1

# compute rms in linear scale
rms = compute_rms_recursive(datain, tau_rms, fs)
rms_log = 20*np.log10(rms + 0.00000001)

#fig, ax = plt.subplots()
#ax.plot(rms_log)
#plt.show()
# loop for compressor curve
gain_log = np.zeros_like(datain)
for i in range(len(datain)):
    gain_log[i] = compute_gain(rms_log[i], threshold, ratio, knee_width, makeup_gain)
    
# gain smoothing 
gain_smoothed_log = compute_att_rel_dynamics(gain_log, tau_attack, tau_release, fs)

# apply gain
dataout = datain * 10**(gain_smoothed_log/20)

fig, ax = plt.subplots(2,1)
ax[0].plot(datain)
ax[0].plot(dataout)
ax[0].set(xlabel='time', ylabel='Amplitude',
       title='Input and Output')
ax[0].grid()
ax[1].plot(gain_log)
ax[1].plot(gain_smoothed_log)
ax[1].set(xlabel='time', ylabel='Gain (dB)',
       title='Gain Curve')

plt.show()


