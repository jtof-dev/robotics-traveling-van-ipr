import os
import math
import numpy as np

import matplotlib

# I added these specifically for making the plots appear on wayland with QT6
os.environ["QT_QPA_PLATFORM"] = "wayland"
matplotlib.use("QtAgg")

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# ----- Parameters -----
g = 9.81  # gravity
dt = 0.02  # time step (s)
M = 0.5  # Mass of the Cart (kg)
m = 0.2  # Mass of the pendulum (kg)
b = 0.1  # Coefficient of friction in the cart (N/m/s)
l = 0.3  # Length to pendulum center of mass
I = 0.006  # Moment of inertia of the pendulum

L = 2
q = I * (M + m) + M * m * l * l

Kp, Kd = 10.0, -0.25

# ----- Initial State -----
x = 0.0  # position of the cart (m)
xdot = 0.0  # ball velocity
theta = 0.01  # beam angle
thetadot = 0.0  # Angular velocity

# ----- Data storage for tracking -----
time_data = []
x_data = []
error_data = []

# ----- Figure Setup -----
fig, (ax_beam, ax_plot) = plt.subplots(2, 1, figsize=(6, 8))

# Beam axes
ax_beam.set_xlim(-L / 2 - 0.2, L / 2 + 0.2)
ax_beam.set_ylim(-1, 1)
ax_beam.set_aspect("equal")
ax_beam.set_title("Ball-on-Beam Simulation")
(beam_line,) = ax_beam.plot([], [], lw=4, color="black")
(ball,) = ax_beam.plot([], [], "ro", markersize=14)
(pend,) = ax_beam.plot([], [], "ro", markersize=14)

# Tracking plot axes
ax_plot.set_xlim(0, 8)  # will scroll as time progresses
ax_plot.set_ylim(-0.5, 0.5)
ax_plot.set_xlabel("Time (s)")
ax_plot.set_ylabel("Ball position / Error")
ax_plot.set_title("Ball Position and Error Over Time")
(pos_line,) = ax_plot.plot([], [], "b-", label="Position")
(error_line,) = ax_plot.plot([], [], "r--", label="Error")
ax_plot.legend()


# ----- Update Function -----
def update(frame):
    global x, xdot, theta, thetadot

    t = frame * dt

    # input force?
    error = 0 - theta
    u = Kp * error + Kd * xdot

    # Dynamics

    xdot = xdot
    xddot = (
        -(I * m * l * l) * b / q * xdot
        + (m * m * g * l * l / q) * theta
        + (I + m * l * l) / q * u
    )
    thetadot = thetadot
    thetaddot = (
        -m * l * b / q * xdot + m * g * l * (M + m) / q * theta + (m * l) / q * u
    )

    x = x + dt * xdot
    xdot = xdot + dt * xddot
    theta = theta + dt * thetadot
    thetadot = thetadot + dt * thetaddot

    # Cart Drawing
    ball_x = x
    ball_y = 0
    ball.set_data([ball_x], [ball_y])
    # Pendulum Drawing
    pend_x = l * math.sin(theta) + ball_x
    pend_y = l * math.cos(theta) + ball_y
    pend.set_data([pend_x], [pend_y])

    # Tracking data
    time_data.append(t)
    x_data.append(x)
    error_data.append(error)

    if t > ax_plot.get_xlim()[1]:
        ax_plot.set_xlim(t - 8, t)

    pos_line.set_data(time_data, x_data)
    error_line.set_data(time_data, error_data)

    return beam_line, ball, pos_line, error_line


# ----- Animate -----
ani = FuncAnimation(fig, update, frames=np.arange(0, 1000), interval=20, blit=False)
plt.tight_layout()
plt.show()
