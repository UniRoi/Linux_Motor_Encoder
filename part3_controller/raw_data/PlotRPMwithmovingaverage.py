import numpy as np
import matplotlib.pyplot as plt
import os


# Function to read RPM data from the text file
def read_data(filename):
    rpm_values = []
    pwm_values = []

    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) >= 1:
                try:
                    rpm = float(parts[0])  # First value: RPM
                    pwm = (float(parts[1]) /255) *100
                    rpm_values.append(rpm)
                    pwm_values.append(pwm)
                except ValueError:
                    print(f"Invalid data encountered: {line.strip()}")

    return rpm_values, pwm_values


# Moving average function
def moving_average(data, window_size):
    """Calculate the moving average of a given data list."""
    return np.convolve(data, np.ones(window_size) / window_size, mode='valid')


# Function to plot the RPM data
def plot_rpm(rpm_values, filtered_values, window_size):
    time_values = [i * 0.1 for i in range(len(rpm_values))]  # Time in seconds (100ms intervals)

    plt.figure(figsize=(10, 5))
    plt.plot(time_values, rpm_values, label='Original RPM', color='red', alpha=0.5)
    plt.plot(time_values[window_size - 1:], filtered_values,
             label='Moving Average (window size: {})'.format(window_size), color='blue')
    plt.title('RPM Over Time with Moving Average')
    plt.ylabel('RPM')
    plt.xlabel('Time (s)')
    plt.grid()
    plt.legend()
    plt.tight_layout()  # Adjusts the layout to fit in the figure area.
    plt.show()


def plot_rpm_pwm(rpm_val, pwm_val):
    time_values = [i * 0.1 for i in range(len(rpm_val))]  # Time in seconds (100ms intervals)

    fig, ax1 = plt.subplots()
    line1, = ax1.plot(time_values, rpm_val, label='RPM', color='red')
    ax1.set_ylabel('RPM')
    ax1.set_xlabel('Time (s)')
    ax2 = ax1.twinx()
    line2, = ax2.plot(time_values, pwm_val, label='duty cycle', color='blue')
    ax2.set_ylabel('duty cycle')
    lines = [line1, line2]
    labels = [line.get_label() for line in lines]
    ax1.legend(lines, labels, loc='upper left')
    # ax1.legend()
    plt.title('RPM and duty cycle Over Time')
    plt.grid()
    # plt.legend()
    # plt.tight_layout()  # Adjusts the layout to fit in the figure area.
    plt.show()

# Main function
if __name__ == "__main__":
    cwd = os.getcwd()
    print(cwd)
    filename = cwd + '/part3_controller/raw_data/speed_duty_10000rpm.txt'  # Replace with your actual file name
    rpm_values, pwm_values = read_data(filename)

    # Define the window size for the moving average
    window_size = 4  # You can change this value for more or less smoothing

    # Apply the moving average
    filtered_values = moving_average(rpm_values, window_size)

    # Plot the original and filtered RPM values
    # plot_rpm(rpm_values, window_size)
    plot_rpm_pwm(rpm_values, pwm_values)
