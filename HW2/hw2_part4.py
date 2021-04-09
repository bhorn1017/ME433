import csv
import matplotlib.pyplot as plt
import numpy as np


#create functions to read data and do MAF

def read_data():
    #read in the sample data (based on given python_csv.py code)

    t_list = [] # column 0
    data1_list = [] # column 1


    with open('sigA.csv') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows one by one
            t_list.append(float(row[0])) # leftmost column
            data1_list.append(float(row[1])) # second column
    return t_list, data1_list

  
t , data1 = read_data() #read the data
#determine the sampling rate
sample_rate = len(t) / t[-1] #sample rate = # of data points / total time of sampling
print('The sample rate is: ', sample_rate)





#plot fft (based on given python_fft.py code)


#dt = 1.0/sample_rate
#t = np.arange(0.0, 1.0, dt) # 10s
## a constant plus 100Hz and 1000Hz
#s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25
s = data1

Fs = sample_rate# sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = s # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]


fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'k-')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'k-') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()
