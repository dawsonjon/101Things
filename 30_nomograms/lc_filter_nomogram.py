import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import LogLocator, NullFormatter

def func(r, c):
    return 1/(2*np.pi*np.sqrt(r*c))

# Scale ranges
L_min, L_max = 1e-9, 100e-3 # Henrys
C_min, C_max = 1e-12, 1e-4 # Farads
f_min, f_max = func(L_max, C_max), func(L_min, C_min)  # Hz

# Logarithmic ranges in decades
l0 = np.log10(L_min)
l1 = np.log10(L_max)

c0 = np.log10(C_min)
c1 = np.log10(C_max)

f0 = np.log10(f_min)
f1 = np.log10(f_max)

# A straight line is represented by interpolation between
# the outer scales.

def L_to_y(R):
    return (np.log10(R) - l0) / (l1 - l0)

def C_to_y(C):
    return (np.log10(C) - c0) / (c1 - c0)

def F_to_y(f):
    return 1 - (np.log10(f) - f0) / (f1 - f0)


# Nomogram geometry
#plot a line between R and C and find point where it intersects the correct answer
#y = mx+c
m=1
c=0
y=F_to_y(func(L_min, C_max))
x=(y-c)/m

x_L = 0
x_F = x
x_C = 1


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
e6 = np.concatenate([decades*1.5, decades*2.2, decades*3.3, decades*4.7, decades*6.8])
minor = np.concatenate([decades*2, decades*3, decades*4, decades*5, decades*6, decades*7, decades*8, decades*9])

L_ticks = [i for i in decades if i <= L_max and i >= L_min]
C_ticks = [i for i in decades if i <= C_max and i >= C_min]
F_ticks = [i for i in decades if i <= f_max and i >= f_min]
L_minor_ticks = [i for i in e6 if i <= L_max and i >= L_min]
C_minor_ticks = [i for i in e6 if i <= C_max and i >= C_min]
F_minor_ticks = [i for i in minor if i <= f_max and i >= f_min]

draw_scale(x_L, "darkblue", "Inductance L (H)",         True, L_to_y, L_ticks, L_minor_ticks)
draw_scale(x_F, "darkred",  "Cutoff frequency fc (Hz)", False, F_to_y, F_ticks, F_minor_ticks, False)
draw_scale(x_C, "darkgreen","Capacitance C (F)",        False,  C_to_y, C_ticks, C_minor_ticks)


# ------------------------------------------------------------
# Formula
# ------------------------------------------------------------

ax.text(
    0.5,
    -0.025,
    r"$f_c = \frac{1}{2\pi \sqrt{LC}}$",
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
    "LC Filter Calculator",
    ha="center",
    va="bottom",
    fontsize=16,
    fontweight="bold"
)

# ------------------------------------------------------------
# Output
# ------------------------------------------------------------

plt.savefig(
    "lc_filter_nomogram.pdf",
    bbox_inches="tight"
)

plt.show()
