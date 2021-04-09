import csv
import matplotlib.pyplot as plt
import numpy as np


#create functions to read data and do MAF

def read_data():
    #read in the sample data (based on given python_csv.py code)

    t_list = [] # column 0
    data1_list = [] # column 1


    with open('sigD.csv') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows one by one
            t_list.append(float(row[0])) # leftmost column
            data1_list.append(float(row[1])) # second column
    return t_list, data1_list

def moving_average(X,time_list,data_list):
    #filter using moving average 
    #averaging last X data points
    last_X = [] #stores the last X values (so that X values will be averaged)
    for k in range(X): #populate the initial list with X zeros
        last_X.append(0)
    X_averages = [] #list storing each averaged value 
    for i in range(len(time_list)): #loop over each time sampling point
        last_X.append(data_list[i]) #add the current data point to the list
        last_X.pop(0) #remove the zeroth element of the lit 
        total = 0 #sum for calculating average
        for j in last_X:
            total = total + j
        X_averages.append(total/X) #append the next value to X_averages
    return X_averages
   

t , data1 = read_data() #read the data
#determine the sampling rate
sample_rate = len(t) / t[-1] #sample rate = # of data points / total time of sampling
print('The sample rate is: ', sample_rate)
y1 = moving_average(50,t,data1) #use the MAF function on the data




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
Y1 = np.fft.fft(y1)/n # fft computing and normalization
Y = Y[range(int(n/2))]
Y1 = Y1[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'k-')
ax1.plot(t,y1,'r-')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('sigD, MAF, 50 averaged points')
ax2.loglog(frq,abs(Y),'k-') # plotting the fft
ax2.loglog(frq,abs(Y1),'r-')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()
