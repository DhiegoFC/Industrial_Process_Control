import serial
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets  # Importing QtWidgets for QApplication
import time

# Serial port configuration (adjust as needed)
ser = serial.Serial('COM3', 115200, timeout=0.1)  # Adjust 'COM3' as per your Arduino's port

# Plot window configuration using PyQtGraph
pg.setConfigOption('background', 'w')
app = QtWidgets.QApplication([])  # Corrected to QtWidgets.QApplication
win = pg.GraphicsLayoutWidget(show=True, title="Real-Time Serial Data Reading")
win.resize(800, 600)
win.setWindowTitle('PID Controller - Arduino UNO')


# Creating the plots
plot = win.addPlot(title="System Output (Actual) and Setpoint vs Time")
curve_actual = plot.plot(pen='b', name='Actual')  # Curve for actual values
curve_setpoint = plot.plot(pen='black', name='Setpoint', linestyle='--')  # Curve for setpoint values

# Lists for storing data
time_data = []
actual_data = []
setpoint_data = []

# Total data collection duration (in seconds)
total_duration = 100
start_time = time.time()
time_window = 30  # Time window in seconds to display on the plot
moving_average_window = 5  # Define the size of the moving average window

print(f"Collecting data for {total_duration} seconds...")

def update():
    """Update function called on each iteration."""
    global time_data, actual_data, setpoint_data
    
    # Reading data from the serial port
    line = ser.readline().decode('utf-8').strip()
    if line:
        data = line.split(',')
        if len(data) == 5:  # Ensuring there are 5 values
            try:
                # Capturing time, setpoint, and actual values
                current_time = float(data[0])  # First value is the time
                setpoint = float(data[1])  # Second value is the setpoint
                actual = float(data[2])  # Third value is the "actual"
                
                # Calculate elapsed time since the start of data collection
                #elapsed_time = (current_time - start_time) 
              
                time_data.append(current_time)
                setpoint_data.append(setpoint)
                actual_data.append(actual)
                
                # Update the graph curves
                curve_actual.setData(time_data, actual_data)
                curve_setpoint.setData(time_data, setpoint_data)
                
                # Adjust the X-axis range to show only the last `time_window` seconds
                if current_time > time_window:
                    plot.setXRange(current_time - time_window, current_time)
                
                # Update the plot title to show current Setpoint and Actual values
                plot.setTitle(f"System Output (Actual = {actual:.2f}, Setpoint = {setpoint:.2f}) vs Time")
                              
            except ValueError:
                pass  # Ignore conversion errors

# Configuring the timer to update the graphs in real-time
timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(1)  # Updates every 1 ms

# Running the PyQtGraph application
if __name__ == '__main__':
    try:
        app.exec()  # Corrected to app.exec()
    except KeyboardInterrupt:
        print("Data collection interrupted by user.")
    finally:
        ser.close()  # Properly close the serial port
