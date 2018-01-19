from __future__ import print_function, division
import numpy as np; import pandas as pd; import matplotlib.pyplot as plt

#
# Load and average data, get the mean speed
#

NFILES = 10
INTERVAL = 0.005
raw_data = []

for i in range(NFILES):
    raw_data.append(np.loadtxt("data2/data_l" + str(i) + ".txt"))

for i in range(NFILES):
    raw_data.append(np.loadtxt("data2/data_r" + str(i) + ".txt"))

# axis (channel, file, point, variable) 
raw_data = np.reshape(raw_data, (2, NFILES, -1, 2))

# diff along angle variable, transforming to radiants
speed = np.diff(raw_data[:,:,:,1] / 180. * 3.141592653, axis=2) / INTERVAL
speed_time = raw_data[:,:,:-1,0] / 1000

# mean along file axis
average_speed = np.mean(speed, axis=1)
average_time = np.mean(speed_time, axis=1)

plt.plot(average_time[0], average_speed[0])
plt.savefig('raw_left_average.pdf', bbox_inches='tight')
plt.close()

plt.plot(average_time[1], average_speed[1])
plt.savefig('raw_right_average.pdf', bbox_inches='tight')
plt.close()

#
# Get STDEV of speed
#

# rmse = sqrt(avg(single - mean))
# rmse = sqrt(avg(speed[:,:,:] - average_speed[:], axis=point))

average_speed = np.reshape(average_speed, (2,1,-1))
err = speed[:,:,:] - average_speed
merr = np.mean(np.mean(np.power(err, 2), axis=2), axis=1)

print("STDev [left, right] motor:", merr)
