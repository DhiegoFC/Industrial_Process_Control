import serial
import matplotlib.pyplot as plt
import time

# Serial port configuration (adjust the port and baud rate as needed)
ser = serial.Serial('COM7', 115200, timeout=0.1)  # Adjust 'COM3' to the correct Arduino port

# Initialize lists to store data
time_data = []
actual_data = []
setpoint_data = []

# Total data collection duration (in seconds)
total_duration = 40

# Initial time
start_time = time.time()

print(f"Collecting data for {total_duration} seconds...")

try:
    while time.time() - start_time < total_duration:
        # Read data from the serial port
        line = ser.readline().decode('utf-8').strip()
        if line:
            # Assuming the data is sent in the format "time,setpoint,actual,error,output"
            data = line.split(',')
            if len(data) == 5:  # Ensure there are five values
                try:
                    # Collect time, setpoint, and actual values
                    current_time = float(data[0])  # First value is the time
                    setpoint = float(data[1])  # Second value is the setpoint
                    actual = float(data[2])  # Third value is the "actual"
                    
                    # Calculate elapsed time relative to the start of data collection
                    elapsed_time = current_time - start_time
                    
                    # Append the data to the lists
                    time_data.append(elapsed_time)
                    setpoint_data.append(setpoint)
                    actual_data.append(actual)
                    
                    # Plot the data at each iteration
                    plt.clf()  # Clear the previous plot to update
                    plt.plot(time_data, actual_data, label='Actual', color='b')
                    plt.plot(time_data, setpoint_data, label='Setpoint', color='black', linestyle='--')
                    plt.xlabel('Time (s)')
                    plt.ylabel('Values')
                    plt.title('System Output (Actual) and Setpoint vs Time')
                    plt.legend()
                    plt.grid(True)  # Add grid for better visualization
                    plt.pause(0.01)  # Small pause to update the plot
                
                except ValueError:
                    # Ignore type conversion errors
                    pass

except KeyboardInterrupt:
    print("Data collection interrupted by the user.")
finally:
    # Ensure the serial port is closed properly
    ser.close()
    plt.ioff()  # Turn off matplotlib's interactive mode
    plt.show()  # Show the final plot
