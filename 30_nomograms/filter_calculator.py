import numpy as np
import matplotlib.pyplot as plt
import math

Z0 = 50.0
f_min=10e3
f_max=100e6

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

def low_pass_inductor(g, f):
    return Z0*g/(2*np.pi*f)

def low_pass_capacitor(g, f):
    return g/(2*np.pi*Z0*f)

def high_pass_inductor(g, f):
    return Z0/(2*np.pi*f*g)

def high_pass_capacitor(g, f):
    return 1/(2*np.pi*Z0*f*g)

def to_y(value, min_val, max_val):
    return (np.log(value) - np.log(min_val)) / (np.log(max_val) - np.log(min_val))


e6  = [j*10**i for i in range(-12, 0) for j in [1.0, 1.5, 2.2, 3.3, 4.7, 6.8]]
e12 = [j*10**i for i in range(-12, 0) for j in [1.2, 1.8, 2.7, 3.9, 5.6, 8.2]]
major = [10**i for i in range(3, 9)]
minor = [j*10**i for i in range(3, 9) for j in range(2, 10)]
minorminor = [j*10**i for i in range(3, 9) for j in np.arange(1.1, 10, 0.1)]

def plot_filter(order, ripple, filename):
    g = chebyshev_g_values(order, ripple)[:-1]

    fig, ax = plt.subplots(figsize=(8.27, 11.69))  # A4 portrait
    ax.set_title(
        "Chebychev Filter 50Ω %i-Pole %0.2fdB ripple"%(int(order), ripple),
        ha="center",
        va="bottom",
        fontsize=16,
        fontweight="bold"
    )

    ax.text(
        0.5,
        -0.015,
        "low-pass (series inductor first)",
        ha="center",
        va="top",
        fontsize=12,
        color = "darkred"
    )
    ax.text(
        0.5,
        -0.035,
        "high-pass (series capacitor first)",
        ha="center",
        va="top",
        fontsize=12,
        color = "darkblue"
    )
    ax.text(
        0.5,
        -0.06,
        "Copyright (C) Jonathan Dawson 2026",
        ha="center",
        va="top",
        fontsize=10,
        color = "black"
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

    num_cols = len(g)+1
    col_spacing = 1.0/len(g)

    #draw grid
    for f in major:
        y = to_y(f, f_min, f_max)
        if y < 0 or y > 1:
            continue
        ax.plot([0, 1], [y,y], color="lightgrey")
    for f in minor:
        y = to_y(f, f_min, f_max)
        if y < 0 or y > 1:
            continue
        ax.plot([0, 1], [y,y], color="lightgrey", linewidth=0.5)
    for f in minorminor:
        y = to_y(f, f_min, f_max)
        if y < 0 or y > 1:
            continue
        ax.plot([0, 1], [y,y], color="lightgrey", linewidth=0.3)

    for i in range(num_cols):
        x = i*col_spacing

        if i == 0:
            ax.plot([i*col_spacing, i*col_spacing], [0, 1], color="darkgreen")
            ax.text(x+0.01, 1+0.02, "Fc (Hz)", ha="center", va="bottom", fontsize=14, color="darkgreen")
            for f in major:
                y = to_y(f, f_min, f_max)
                if y < 0 or y > 1:
                    continue
                ax.plot([x, x+0.01], [y, y], color="darkgreen")
                ax.text(x+0.01, y, format_value(f), ha="left", va="center", fontsize=8, color="darkgreen")
            for f in minor:
                y = to_y(f, f_min, f_max)
                if y < 0 or y > 1:
                    continue
                ax.plot([x, x+0.006], [y, y], color="darkgreen")
                ax.text(x+0.006, y, format_value(f), ha="left", va="center", fontsize=8, color="darkgreen")

        else:
            low_pass_is_inductor = i%2==1

            ax.plot([x+0.002, x+0.002], [0, 1], color="darkred")
            ax.plot([x-0.002, x-0.002], [0, 1], color="darkblue")
            if low_pass_is_inductor:
                ax.text(x, 1+0.04, "Series", ha="center", va="bottom", fontsize=12, color="black")
                ax.text(x+0.005, 1+0.02, "L%i(H)"%i, ha="left", va="bottom", fontsize=11, color="darkred")
                ax.text(x-0.005, 1+0.02, "C%i(F)"%i, ha="right", va="bottom", fontsize=11, color="darkblue")
                lp_min_val = low_pass_inductor(g[i-1], f_min)
                lp_max_val = low_pass_inductor(g[i-1], f_max)
                hp_min_val = high_pass_capacitor(g[i-1], f_min)
                hp_max_val = high_pass_capacitor(g[i-1], f_max)
            else:
                ax.text(x, 1+0.04, "Shunt", ha="center", va="bottom", fontsize=12, color="black")
                ax.text(x+0.005, 1+0.02, "C%i(F)"%i, ha="left", va="bottom", fontsize=11, color="darkred")
                ax.text(x-0.005, 1+0.02, "L%i(H)"%i, ha="right", va="bottom", fontsize=11, color="darkblue")
                lp_min_val = low_pass_capacitor(g[i-1], f_min)
                lp_max_val = low_pass_capacitor(g[i-1], f_max)
                hp_min_val = high_pass_inductor(g[i-1], f_min)
                hp_max_val = high_pass_inductor(g[i-1], f_max)

            for value in e6:
                y = to_y(value, lp_min_val, lp_max_val)
                if y < 0 or y > 1:
                    continue
                ax.plot([x+0.002, x+0.02], [y, y], color="darkred")
                ax.text(x+0.02, y, format_value(value), ha="left", va="center", fontsize=8, color="darkred")
            for value in e12:
                y = to_y(value, lp_min_val, lp_max_val)
                if y < 0 or y > 1:
                    continue
                ax.plot([x+0.002, x+0.01], [y, y], color="darkred")
                ax.text(x+0.01, y, format_value(value), ha="left", va="center", fontsize=6, color="darkred")
            for value in e6:
                y = to_y(value, hp_min_val, hp_max_val)
                if y < 0 or y > 1:
                    continue
                ax.plot([x-0.002, x-0.02], [y, y], color="darkblue")
                ax.text(x-0.02, y, format_value(value), ha="right", va="center", fontsize=8, color="darkblue")
            for value in e12:
                y = to_y(value, hp_min_val, hp_max_val)
                if y < 0 or y > 1:
                    continue
                ax.plot([x-0.002, x-0.01], [y, y], color="darkblue")
                ax.text(x-0.01, y, format_value(value), ha="right", va="center", fontsize=6, color="darkblue")



    ax.set_xlim(-0.1,1.1)
    ax.set_ylim(-0.1,1.1)
    ax.axis("off")

    plt.tight_layout()
    plt.savefig(filename)
    plt.show()

def chebyshev_g_values(order, ripple_db):
    """ Calculate normalized g-values for a Chebyshev low-pass prototype.  """

    N = order

    if N < 1:
        raise ValueError("Order must be >= 1")

    if ripple_db <= 0:
        raise ValueError("Ripple must be > 0 dB")

    epsilon = math.sqrt(10**(ripple_db / 10) - 1)

    beta = math.asinh(1 / epsilon) / N
    gamma = math.sinh(beta)

    g = []

    # First element
    g.append(
        2 * math.sin(math.pi / (2*N)) / gamma
    )

    # Remaining elements
    for k in range(2, N + 1):

        a_prev = math.sin((2*k - 3) * math.pi / (2*N))
        a = math.sin((2*k - 1) * math.pi / (2*N))

        b_prev = (
            gamma**2 +
            math.sin((k-1)*math.pi/N)**2
        )

        g.append(
            (4 * a_prev * a) /
            (b_prev * g[-1])
        )

    # Termination
    if N % 2:
        g.append(1.0)
    else:
        g.append(
            (math.cosh(beta/2) / math.sinh(beta/2))**2
        )

    return g

plot_filter(3, 0.5, "chebychev_3_05.pdf")
plot_filter(5, 0.5, "chebychev_5_05.pdf")
plot_filter(7, 0.5, "chebychev_7_05.pdf")
