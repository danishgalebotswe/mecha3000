import csv
import matplotlib.pyplot as plt
import numpy as np

t = [] # column 0
data1 = [] # column 1

with open('sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

# for i in range(len(t)):
#     # print the data to verify it was read
#     print(str(t[i]) +", " + str(data1[i]))

# data filtering
maf_data = [0] * len(t)
maf_sample_size = 250

for i in range(len(t)):
    if i >= maf_sample_size:
        data_sum = 0
        for j in range(maf_sample_size):
            data_sum += data1[i-j]
        # print(data_sum)
        data_avg = data_sum/maf_sample_size
        maf_data[i] = data_avg
        # print("data_avg = " + str(data_avg) + "data1 = " + str(data1[i]))


# Calculating the sample rate
sample_rate = len(data1)/ t[-1]
print('Sample rate = ' + str(sample_rate) + 'Hz.')

sample_interval = 1/sample_rate # getting the sampling interval

y = data1 # the data to make the fft from
z = maf_data
n = len(data1) # length of the signal
k = np.arange(n)
T = n/sample_rate
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(data1)/n # fft computing and normalization
Z = np.fft.fft(maf_data)/n
Y = Y[range(int(n/2))]
Z = Z[range(int(n/2))]

# plt.plot(t,data1)
# plt.xlabel('Time (sec)')
# plt.ylabel('Signal')
# plt.title('Signal vs Time')
# plt.show()

fig, (ax1, ax2) = plt.subplots(2,1)
ax1.plot(t,y, label="Actual Signal")
ax1.plot(t,z, label="Filtered Signal")
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.legend()
ax2.loglog(frq,abs(Y), label="Frequency of Actual Signal") # plotting the fft
ax2.loglog(frq,abs(Z), label="Frequency of Filtered Signal")
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq) & Z(freq)|')
ax2.legend()
plt.show()






