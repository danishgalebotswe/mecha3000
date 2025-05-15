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

# Calculating the sample rate
sample_rate = len(data1)/ t[-1]
print('Sample rate = ' + str(sample_rate) + 'Hz.')

plt.plot(t,data1)
plt.xlabel('Time (sec)')
plt.ylabel('Signal')
plt.title('Signal vs Time')
plt.show()





