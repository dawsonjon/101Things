import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import LogLocator, NullFormatter


#r1 = z0 * (a+1)/(a+2)
#log(r1) = log(z0) + log((a+1)/(a-1))

def func(a):
    return (a+1)/(a-1)

# Scale ranges
Z_min, Z_max = 20, 100
A_min, A_max = 1.4, 100
r_min, r_max = func(A_max)*Z_min, func(A_min)*Z_max

# Logarithmic ranges in decades
z0 = np.log10(Z_min)
z1 = np.log10(Z_max)

a0 = np.log10(func(A_max))
a1 = np.log10(func(A_min))
print(func(A_min), func(A_max))

r0 = np.log10(r_min)
r1 = np.log10(r_max)

# A straight line is represented by interpolation between
# the outer scales.

def Z_to_y(R):
    return (np.log10(R) - z0) / (z1 - z0)

def A_to_y(A):
    return (np.log10(func(A)) - a0) / (a1 - a0)

def dB_to_y(dB):
    return A_to_y(np.power(10.0, dB/20.0, ))

def R_to_y(f):
    return (np.log10(f) - r0) / (r1 - r0)


# Nomogram geometry
#plot a line between R and C and find point where it intersects the correct answer
#y = mx+c
m=1
c=0

y=R_to_y(func(A_min)*Z_min)
x=(y-c)/m

x_Z = 0
x_R = x
x_A = 1


# ------------------------------------------------------------
# Plot
# ------------------------------------------------------------

fig, ax = plt.subplots(figsize=(8.27, 11.69))  # A4 portrait

ax.set_xlim(-0.1, 1.1)
ax.set_ylim(-0.1, 1.08)
ax.axis("off")

# ------------------------------------------------------------
# Draw scales
# ------------------------------------------------------------

def draw_scale(x, colour, label, right, transform, major_values, minor_values, minor_labels=True):

    # Main scale line
    ax.plot(
        [x, x],
        [0, 1],
        linewidth=1.2,
        color=colour
    )

    # Label
    ax.text(
        x,
        1 + 0.035,
        label,
        ha="center",
        va="bottom",
        fontsize=13,
        fontweight="bold",
        color=colour
    )

    for value in major_values:
        y = transform(value)

        # Tick
        ax.plot(
            [x - 0.025 if right else x + 0.025, x],
            [y, y],
            linewidth=0.8,
            color=colour
        )

        # Label
        ax.text(
            x - 0.045 if right else x + 0.045,
            y,
            format_value(value),
            ha="right" if right else "left",
            va="center",
            fontsize=8,
            color=colour
        )

    for value in minor_values:
        y = transform(value)

        # Tick
        ax.plot(
            [x - 0.015 if right else x + 0.015, x],
            [y, y],
            linewidth=0.8,
            color=colour
        )

        if minor_labels:

            # Label
            ax.text(
                x - 0.045 if right else x + 0.045,
                y,
                format_value(value),
                ha="right" if right else "left",
                va="center",
                fontsize=8,
                color=colour
            )


def format_value(value):

    if value >= 1e9:
        return f"{value/1e9:g} G"
    elif value >= 1e6:
        return f"{value/1e6:g} M"
    elif value >= 1e3:
        return f"{value/1e3:g} k"
    elif value >= 1:
        return f"{value:g}"
    elif value >= 1e-3:
        return f"{value*1e3:g} m"
    elif value >= 1e-6:
        return f"{value*1e6:g} µ"
    elif value >= 1e-9:
        return f"{value*1e9:g} n"
    elif value >= 1e-12:
        return f"{value*1e12:g} p"
    else:
        return f"{value:g}"


# ------------------------------------------------------------
# Major ticks
# ------------------------------------------------------------

decades = np.arange(-12, 12)
decades = np.power(10.0*np.ones_like(decades), decades)
e6 = np.concatenate([decades*1.0, decades*1.5, decades*2.2, decades*3.3, decades*4.7, decades*6.8])
e24 = np.concatenate([i * np.array([1.1, 1.2, 1.3, 1.6, 1.8, 2.0, 2.4, 2.7, 3.0, 3.6, 3.9, 4.3, 5.1, 5.6, 6.2, 7.5, 8.2, 9.1]) for i in decades])
minor = np.concatenate([decades*2, decades*3, decades*4, decades*5, decades*6, decades*7, decades*8, decades*9])

Z_ticks = [i for i in np.arange(Z_min, Z_max, 10) if i <= Z_max and i >= Z_min]
A_ticks = [i for i in [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 100] if i <= A_max and i >= A_min]
R_ticks = [i for i in e6 if i <= r_max and i >= r_min]
Z_minor_ticks = [i for i in np.arange(Z_min, Z_max) if i <= Z_max and i >= Z_min]
A_minor_ticks = [i for i in [1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9] if i <= A_max and i >= A_min]
R_minor_ticks = [i for i in e24 if i <= r_max and i >= r_min]

dB_ticks = [i for i in np.concatenate([[3, 6], np.arange(0, 100, 10)]) if np.power(10, i/20) <= A_max and np.power(10, i/20) >= A_min]
dB_minor_ticks = [i for i in np.arange(0, 100) if np.power(10, i/20) <= A_max and np.power(10, i/20) >= A_min]

draw_scale(x_Z, "darkblue", "Z0 (Ω)",      True,  Z_to_y, Z_ticks, Z_minor_ticks, False)
draw_scale(x_R, "darkred",  "$R_{shunt}$ (Ω)",  False, R_to_y, R_ticks, R_minor_ticks)
draw_scale(x_A, "darkgreen","Gain lin/dB", True,  A_to_y, A_ticks, A_minor_ticks)
draw_scale(x_A, "darkgreen","",            False, dB_to_y, dB_ticks, dB_minor_ticks, False)

ax.plot([x_Z, x_A], [Z_to_y(50), A_to_y(10)])


# ------------------------------------------------------------
# Formula
# ------------------------------------------------------------

ax.text(
    0.25,
    -0.025,
    r"$R_{shunt} = Z_0 \frac{A+1}{A-1}$",
    ha="center",
    va="top",
    fontsize=14
)

ax.text(
    0.75,
    -0.025,
    r"$A = 10^{\frac{dB}{20}}$",
    ha="center",
    va="top",
    fontsize=14
)

ax.text(
    0.5,
    -0.08,
    "101things.readthedocs.io",
    ha="center",
    va="top",
    fontsize=10,
    color = "blue"
)

ax.set_title(
    "Pi Attenuator Calculator $R_{shunt}$",
    ha="center",
    va="bottom",
    fontsize=16,
    fontweight="bold"
)

# ------------------------------------------------------------
# Output
# ------------------------------------------------------------

plt.savefig(
    "pi_attenuator_shunt_nomogram.pdf",
    bbox_inches="tight"
)

plt.show()
