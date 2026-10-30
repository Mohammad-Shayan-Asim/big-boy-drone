import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def read_last_lines(file_path, n):
    with open(file_path, "rb") as f:
        lines = f.readlines()
        return lines[-n:]

def preprocess(lines):
    sensor_data = []
    for i, data in enumerate(lines):
        data = data.decode('ascii')
        data = data.strip().split(',')
        try:
            data = [int(x) for x in data]
            sensor_data.append(data)
        except:
            # print(data, i)
            pass

    accel = [x[:3] for x in sensor_data]
    gyro = [x[4:] for x in sensor_data]
    temp = [x[3] for x in sensor_data]

    g = 9.81
    gyro_m = 1 / 131  
    accel_m = g / 16384
    temp_m = 1 / 333.87 

    accel = [ [v*accel_m for v in x] for x in accel]
    gyro = [[v*gyro_m for v in x] for x in gyro]
    temp = [t*temp_m + 21 for t in temp]
    return accel, gyro, temp

FILE_PATH = "serial.log"
REFRESH_MS = 5
MAX_POINTS = 100


fig, (ax_acc, ax_gyr, ax_tmp) = plt.subplots(3, 1, sharex=True, figsize=(8, 8))
fig.suptitle(f"Real-Time Data (Last {MAX_POINTS} Readings)")

acc_x, = ax_acc.plot([], [], label="x")
acc_y, = ax_acc.plot([], [], label="y")
acc_z, = ax_acc.plot([], [], label="z")

gyr_x, = ax_gyr.plot([], [], label="x")
gyr_y, = ax_gyr.plot([], [], label="y")
gyr_z, = ax_gyr.plot([], [], label="z")

temp_line, = ax_tmp.plot([], [], color="r")

ax_acc.set_ylabel("Accel")
ax_gyr.set_ylabel("Gyro")
ax_tmp.set_ylabel("Temp")
ax_tmp.set_xlabel("Index")
ax_acc.legend()
ax_gyr.legend()
ax_acc.grid(True)
ax_gyr.grid(True)
ax_tmp.grid(True)

def update(frame):
    lines = read_last_lines(FILE_PATH, MAX_POINTS)
    accel, gyro, temp = preprocess(lines)

    accel = np.array(accel)  
    gyro = np.array(gyro)   
    temp = np.array(temp)   

    if len(temp) == 0:
        return

    x = np.arange(len(temp))

    acc_x.set_data(x, accel[:, 0])
    acc_y.set_data(x, accel[:, 1])
    acc_z.set_data(x, accel[:, 2])

    gyr_x.set_data(x, gyro[:, 0])
    gyr_y.set_data(x, gyro[:, 1])
    gyr_z.set_data(x, gyro[:, 2])

    temp_line.set_data(x, temp)

    ax_acc.relim()
    ax_acc.autoscale_view()
    ax_gyr.relim()
    ax_gyr.autoscale_view()
    ax_tmp.relim()
    ax_tmp.autoscale_view()


ani = FuncAnimation(fig, update, interval=REFRESH_MS, cache_frame_data=False)
plt.tight_layout()
plt.show()