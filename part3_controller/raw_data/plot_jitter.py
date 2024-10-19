import matplotlib.pyplot as plt

# Load jitter data from file
with open("jitter_data.txt", "r") as f:
    jitter_data = [float(line.strip()) for line in f]

# Plot histogram
plt.hist(jitter_data, bins=50, edgecolor='black')
plt.title("Jitter Distribution (Control Loop using usleep)")
plt.xlabel("Jitter (ms)")
plt.ylabel("Frequency")
plt.grid(True)

# Show plot
plt.show()