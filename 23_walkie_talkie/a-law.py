import numpy as np
import matplotlib.pyplot as plt
import os

# A-law constant
A = 87.6

def a_law_compress(x, bits_in=12, bits_out=8):
    x_norm = (x / (2**bits_in - 1)) * 2 - 1
    abs_x = np.abs(x_norm)
    compressed = np.where(
        abs_x < 1/A,
        (A * abs_x) / (1 + np.log(A)),
        (1 + np.log(A * abs_x)) / (1 + np.log(A))
    )
    compressed = np.sign(x_norm) * compressed
    compressed_scaled = ((compressed + 1) / 2 * (2**bits_out - 1)).astype(int)
    return compressed_scaled

def a_law_expand(y, bits_in=8, bits_out=12):
    y_norm = (y / (2**bits_in - 1)) * 2 - 1
    abs_y = np.abs(y_norm)
    expanded = np.where(
        abs_y < (1 / (1 + np.log(A))),
        abs_y * (1 + np.log(A)) / A,
        np.exp(abs_y * (1 + np.log(A)) - 1) / A
    )
    expanded = np.sign(y_norm) * expanded
    pcm_out = ((expanded + 1) / 2 * (2**bits_out - 1)).astype(int)
    return pcm_out

# Create output folder
output_dir = "plots"
os.makedirs(output_dir, exist_ok=True)

# Generate 12-bit PCM input
pcm_in = np.arange(0, 2**12)  # 0..4095

# Compress and expand
a_law_out = a_law_compress(pcm_in, bits_in=12, bits_out=8)
pcm_expanded = a_law_expand(a_law_out, bits_in=8, bits_out=12)

# Plot and export compression curve
plt.figure(figsize=(10,5))
plt.plot(pcm_in, a_law_out, color='blue', label='A-law Compression')
plt.title("12-bit PCM -> 8-bit A-law Compression Curve")
plt.xlabel("12-bit PCM Input")
plt.ylabel("8-bit A-law Output")
plt.grid(True)
plt.legend()
compression_file = os.path.join(output_dir, "a_law_compression.png")
plt.savefig(compression_file, dpi=300)
plt.show()
print(f"Compression curve saved to {compression_file}")

# Plot and export expansion curve
plt.figure(figsize=(10,5))
plt.plot(a_law_out, pcm_expanded, color='red', label='A-law Expansion')
plt.title("8-bit A-law -> 12-bit PCM Expansion Curve")
plt.xlabel("8-bit A-law Input")
plt.ylabel("12-bit PCM Output")
plt.grid(True)
plt.legend()
expansion_file = os.path.join(output_dir, "a_law_expansion.png")
plt.savefig(expansion_file, dpi=300)
plt.show()
print(f"Expansion curve saved to {expansion_file}")
