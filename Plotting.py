import json as js
import matplotlib.pyplot as plt
import numpy as np

with open("Time(num of streams).json", "r") as read_file:
    plotting_data = js.load(read_file)

x_axis = []
y_axis = []
print(plotting_data[5][1])
for i in range(len(plotting_data)):
    x_axis.append(plotting_data[i][0])
    y_axis.append(plotting_data[i][1])
print(x_axis)
print(y_axis)

plt.plot(x_axis, y_axis, 'ro')
plt.xlabel("number of streams")
plt.ylabel("time, mks")
plt.show()