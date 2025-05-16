import csv
import matplotlib.pyplot as plt
import numpy as np

t = [] # column 0
data1 = [] # column 1

with open('sigD.csv') as f:
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

# *********Moving Average*********
# maf_data = [0] * len(t)
# maf_sample_size = 250


# for i in range(len(t)):
#     if i >= maf_sample_size:
#         data_sum = 0
#         for j in range(maf_sample_size):
#             data_sum += data1[i-j]
#         # print(data_sum)
#         data_avg = data_sum/maf_sample_size
#         maf_data[i] = data_avg
#         # print("data_avg = " + str(data_avg) + "data1 = " + str(data1[i]))

# *********Infinite Impulse Response (IIR)*********
# new_average = [0] * len(t)
# A = 0.95
# B = 0.05

# for i in range(len(t)):
#     if i > 1:
        # new_average[i] = A * new_average[i-1] + B * data1[i]

# **********Finite Impulse Response (FIR)***********
new_average = [0] * len(t)
h = [
    0.000000000000000000,
    0.000008471711691010,
    0.000034031005548846,
    0.000077106988255137,
    0.000138403662837941,
    0.000218883366724262,
    0.000319743982133028,
    0.000442390308259176,
    0.000588400085152093,
    0.000759485251766625,
    0.000957449103869573,
    0.001184140089975318,
    0.001441403044086650,
    0.001731028701745491,
    0.002054702379971361,
    0.002413952721518276,
    0.002810101409173712,
    0.003244214746448159,
    0.003717057977084964,
    0.004229053177713048,
    0.004780241506250726,
    0.005370250524144369,
    0.005998267234192322,
    0.006663017388751364,
    0.007362751526909165,
    0.008095238095238093,
    0.008857763896654724,
    0.009647141997425276,
    0.010459727105281005,
    0.011291438313783771,
    0.012137788991368519,
    0.012993923479729190,
    0.013854660157205743,
    0.014714540320298524,
    0.015567882242006970,
    0.016408839680862878,
    0.017231464040654450,
    0.018029769319095220,
    0.018797798935072635,
    0.019529693489398869,
    0.020219758493749275,
    0.020862531097052692,
    0.021452844848101230,
    0.021985891557450114,
    0.022457279360424725,
    0.022863086135656491,
    0.023199907499233917,
    0.023464898672271538,
    0.023655809608274510,
    0.023771012864745100,
    0.023809523809523808,
    0.023771012864745100,
    0.023655809608274510,
    0.023464898672271532,
    0.023199907499233920,
    0.022863086135656491,
    0.022457279360424732,
    0.021985891557450125,
    0.021452844848101227,
    0.020862531097052703,
    0.020219758493749282,
    0.019529693489398876,
    0.018797798935072642,
    0.018029769319095220,
    0.017231464040654450,
    0.016408839680862878,
    0.015567882242006961,
    0.014714540320298522,
    0.013854660157205732,
    0.012993923479729190,
    0.012137788991368524,
    0.011291438313783765,
    0.010459727105281005,
    0.009647141997425281,
    0.008857763896654724,
    0.008095238095238098,
    0.007362751526909175,
    0.006663017388751366,
    0.005998267234192327,
    0.005370250524144368,
    0.004780241506250728,
    0.004229053177713054,
    0.003717057977084971,
    0.003244214746448154,
    0.002810101409173709,
    0.002413952721518279,
    0.002054702379971364,
    0.001731028701745495,
    0.001441403044086652,
    0.001184140089975317,
    0.000957449103869573,
    0.000759485251766627,
    0.000588400085152096,
    0.000442390308259177,
    0.000319743982133028,
    0.000218883366724262,
    0.000138403662837942,
    0.000077106988255137,
    0.000034031005548847,
    0.000008471711691010,
    0.000000000000000000,
]

for i in range(len(t)):
    if i >= len(h):
        for j in range(len(h)):
            new_average[i] += h[j] * data1[i-j]
        # new_average[i] = h[0] * new_average[i-2] + h[1] * new_average[i-1] + h[2] * data1[i]


# # Calculating the sample rate
sample_rate = len(data1)/ t[-1]
print('Sample rate = ' + str(sample_rate) + 'Hz.')

sample_interval = 1/sample_rate # getting the sampling interval

y = data1 # the data to make the fft from
# z = maf_data
z = new_average
n = len(data1) # length of the signal
k = np.arange(n)
T = n/sample_rate
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(data1)/n # fft computing and normalization
# Z = np.fft.fft(maf_data)/n
Z = np.fft.fft(new_average)/n
Y = Y[range(int(n/2))]
Z = Z[range(int(n/2))]

# plt.plot(t,data1)
# plt.xlabel('Time (sec)')
# plt.ylabel('Signal')
# plt.title('Signal vs Time')
# plt.show()

fig, (ax1, ax2) = plt.subplots(2,1)
ax1.plot(t,y, color='black', label="Actual Signal")
ax1.plot(t,z, color='red', label="Filtered Signal")
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.legend()
ax2.loglog(frq,abs(Y), color='black', label="Frequency of Actual Signal") # plotting the fft
ax2.loglog(frq,abs(Z), color='red',label="Frequency of Filtered Signal")
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Magnitude|')
ax2.legend()
plt.show()






