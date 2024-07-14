# uncomment the next line if running in a notebook
#%matplotlib inline
import numpy as np
import matplotlib.pyplot as plt

# mass, spring constant, initial position and velocity
m = 1
m_mars = 6.42 * 10**23
r_mars = 3390000
grav_const = 6.674 * 10**(-11)
position = np.array([0,0,4000000])
velocity = np.array([4629,0,0])
position_v = position
velocity_v = velocity

# simulation time, timestep and time
t_max = 100000
dt = 1
t_array = np.arange(0, t_max, dt)

# initialise empty lists to record trajectories
x_list = []
v_list = []
x_vertlet = []
v_vertlet = []

# Euler integration
for t in t_array:

    # append current state to trajectories
    x_list.append(position)
    v_list.append(velocity)
    x_vertlet.append(position_v)
    v_vertlet.append(velocity_v)
    
    # calculate new position and velocity
    pos_mag = np.linalg.norm(position)
    if pos_mag <= r_mars:
        break
    a = -grav_const * m_mars * position / pos_mag**3
    #print(position)
    #print(pos_mag)
    position = position + dt * velocity
    velocity = velocity + dt * a
    #print(velocity)

    if t >= t_array[1]:
        pos_mag_v = np.linalg.norm(position_v)
        if pos_mag_v <= r_mars:
            break
        position_v = 2*x_vertlet[-1] - x_vertlet[-2] - dt**2 * grav_const * m_mars *x_vertlet[-1]/pos_mag_v**3
        velocity_v = 1/dt * (position_v - x_vertlet[-1])
        #print(velocity_v)
    else:
        position_v = position
        velocity_v = velocity

# convert trajectory lists into arrays, so they can be sliced (useful for Assignment 2)
x_array = np.array(x_list)
v_array = np.array(v_list)
xvert_array = np.array(x_vertlet)
vvert_array = np.array(v_vertlet)

# plot the position-time graph

#plt.figure(1)
#plt.clf()
#plt.xlabel('time (s)')
#plt.grid()
#plt.plot(t_array, x_array, label='x (m)')
#plt.plot(t_array, v_array, label='v (m/s)')
#plt.legend()
#plt.show()

#plt.figure(2)
#plt.clf()
#plt.xlabel('time (s)')
#plt.grid()
#plt.plot(t_array, xvert_array, label='x (m)')
#plt.plot(t_array, vvert_array, label='v (m/s)')
#plt.legend()
#plt.show()

#ASSIGNMENT 2 - (1)

#plt.figure(1)
#plt.clf()
#plt.xlabel('time (s)')
#plt.grid()
#plt.plot(t_array, x_array[:,2], label='z (m)')
#plt.plot(t_array, v_array, label='v (m/s)')
#plt.legend()
#plt.show()

plt.figure(3)
plt.clf()
plt.xlabel('time (s)')
plt.grid()
plt.plot(t_array, xvert_array[:,0], label='z (m)')
#plt.plot(t_array, vvert_array, label='v (m/s)')
plt.legend()
plt.show()

#ASSIGNMENT 2 - (2)

plt.figure(1)
plt.clf()
plt.xlabel('x (m)')
plt.grid()
plt.plot(x_array[:,0], x_array[:,2], label='z (m)')
#plt.plot(t_array, v_array, label='v (m/s)')
plt.legend()
plt.show()

plt.figure(2)
plt.clf()
plt.xlabel('x (m)')
plt.grid()
plt.plot(xvert_array[:,0], xvert_array[:,2], label='z (m)')
#plt.plot(t_array, vvert_array, label='v (m/s)')
plt.legend()
plt.show()