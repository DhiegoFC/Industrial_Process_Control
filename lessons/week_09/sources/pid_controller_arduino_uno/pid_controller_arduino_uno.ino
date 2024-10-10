const int INPUT_PIN = A0;  // Analog input pin (sensor)
const int OUTPUT_PIN = 3;  // PWM output pin (controller)
const int POT_PIN = A1;    // Pin where the potentiometer is connected

double dt, last_time;
double integral, previous, output = 0;
double kp, ki, kd;
double setpoint = 200.00;  // If using a variable setpoint with the potentiometer, comment out this line
//double setpoint;
unsigned long startTime;   // To store the start time
unsigned long currentTime; // To calculate the current time

void setup()
{
  kp = 2.0;
  ki = 1.7;
  kd = 0.003;
  last_time = 0;
  Serial.begin(115200);
  analogWrite(OUTPUT_PIN, 0);  // Initializes the output to 0
  startTime = millis();  // Store the start time for time tracking

  // Initial loop to print zeros for system setup phase
  for (int i = 0; i < 50; i++)
  {
    currentTime = (millis() - startTime) / 1000.0;  // Elapsed time in seconds
    Serial.print(currentTime);  // Print elapsed time
    Serial.print(",");
    Serial.println(0);  // Print initial output (0) to establish a baseline
    delay(100);
  }
  delay(100);  // Small delay to allow the system to stabilize
}

void loop()
{
  // Reads the value of the potentiometer (between 0 and 1023) and adjusts the setpoint dynamically. 
  // Comment out the line below if you want to use a fixed setpoint value
  //setpoint = map(analogRead(POT_PIN), 0, 1023, 0, 255);  // Adjusts the setpoint between 0 and 255 (adjust as necessary)

  double now = millis();
  dt = (now - last_time) / 1000.00;  // Calculates the time interval (dt)
  last_time = now;

  // Reads the sensor value (from 0 to 1023) and maps it to the range of 0 to 255
  double actual = map(analogRead(INPUT_PIN), 0, 1023, 0, 255);
  double error = setpoint - actual;

  // Use the error in the PID calculation
  output = pid(error);  // Calculates the PID output value

  analogWrite(OUTPUT_PIN, output);  // Applies the calculated output

  // Send the elapsed time, setpoint, actual, error, and output to the Serial Plotter
  currentTime = millis() - startTime;  // Calculate the time elapsed since the start
  Serial.print(currentTime / 1000.0);  // Print the elapsed time in seconds
  Serial.print(",");                   // Comma separator
  Serial.print(setpoint);              // Print the setpoint value
  Serial.print(",");
  Serial.print(actual);                // Print the actual sensor value
  Serial.print(",");
  Serial.print(error);                 // Print the error
  Serial.print(",");
  Serial.println(output);              // Print the PID output value

  delay(12);  // Insert delay in the circuit (optional, but usually set to a small value)
}

double pid(double error)
{
  double proportional = error;
  integral += error * dt;  // Updates the integral term
  
  double derivative = (error - previous) / dt;  // Calculates the derivative term
  previous = error;
  
  double output = kp * ((proportional) + (ki * integral) + (kd * derivative));  // PID Ideal (ISA) summation

  return output;
}
