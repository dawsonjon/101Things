import numpy as np
import scipy as sc
import matplotlib.pyplot as plt
from matplotlib.ticker import LogLocator, NullFormatter

def to_wavelength(F):
    return F/sc.constants.c

# Scale ranges
F_min, F_max = 1e5, 10e9 # Hertz
lambda_min, labmda_max = to_wavelength(F_min), to_wavelength(F_max)

def lin_to_y(P, _min, _max):
    p0 = np.log10(_min)
    p1 = np.log10(_max)
    return (np.log10(P) - p0) / (p1 - p0)

# ------------------------------------------------------------
# Plot
# ------------------------------------------------------------

fig, ax = plt.subplots(figsize=(8.27, 11.69))  # A4 portrait

ax.set_xlim(-0.1, 1.1)
ax.set_ylim(-0.1, 1.08)
ax.axis("off")

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
    elif value >= 1e-15:
        return f"{value*1e15:g} f"
    elif value >= 1e-18:
        return f"{value*1e18:g} a"
    else:
        return f"{value:g}"

def format_number(value):
    return f"{value:g}"

# ------------------------------------------------------------
# Draw scales
# ------------------------------------------------------------

def draw_scale(x, colour, label, right, transform, _min, _max, major_values, minor_values, minor_labels=True, lim=None, _format=format_value):

    if lim is None:
        lim=[_min, _max]

    # Main scale line
    ax.plot(
        [x, x],
        [transform(lim[0], _min, _max), transform(lim[1], _min, _max)],
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
        y = transform(value, _min, _max)

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
            _format(value),
            ha="right" if right else "left",
            va="center",
            fontsize=8,
            color=colour
        )

    for value in minor_values:
        y = transform(value, _min, _max)

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
                _format(value),
                ha="right" if right else "left",
                va="center",
                fontsize=8,
                color=colour
            )




# ------------------------------------------------------------
# Major ticks
# ------------------------------------------------------------

decades = np.arange(-19, 12)
decades = np.power(10.0*np.ones_like(decades), decades)
minor = np.concatenate([decades*2, decades*3, decades*4, decades*5, decades*6, decades*7, decades*8, decades*9])

log_decades = np.arange(-160, 120, 10)
log_minor = np.arange(-160, 120)


F_ticks = [i for i in decades if i <= F_max and i >= F_min]
F_minor_ticks = [i for i in minor if i <= F_max and i >= F_min]
lambda_ticks = [i for i in decades if i <= labmda_max and i >= lambda_min]
lambda_minor_ticks = [i for i in minor if i <= labmda_max and i >= lambda_min]

#draw grid
for y in F_ticks:
    ax.plot(
        [0, 1],
        [lin_to_y(y, F_min, F_max), lin_to_y(y, F_min, F_max)],
        linewidth=0.8,
        color="lightgrey"
    )

for y in F_minor_ticks:
    ax.plot(
        [0, 1],
        [lin_to_y(y, F_min, F_max), lin_to_y(y, F_min, F_max)],
        linewidth=0.2,
        color="lightgrey"
    )

draw_scale(0,   "darkblue", "Frequency (Hz)",  True, lin_to_y, F_min, F_max, F_ticks, F_minor_ticks, False)
draw_scale(1.0, "darkgreen", "Wavelength (m)", False, lin_to_y, lambda_min, labmda_max, lambda_ticks, lambda_minor_ticks, False)


# ------------------------------------------------------------
# Formula
# ------------------------------------------------------------

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
    "Frequency and Wavelength Calculator",
    ha="center",
    va="bottom",
    fontsize=16,
    fontweight="bold"
)

# ------------------------------------------------------------
# Output
# ------------------------------------------------------------

plt.savefig(
    "frequency_wavelength_calculator.pdf",
    bbox_inches="tight"
)

plt.show()
