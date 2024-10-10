const int INPUT_PIN = A0;  // Analog input pin (sensor)
const int OUTPUT_PIN = 3;  // PWM output pin (controller)
const int POT_PIN = A1;    // Pin where the potentiometer is connected

double dt, last_time;
double integral = 0, previous_error = 0, previous_filtered_derivative = 0, output = 0;
double kp, ki, kd;
double setpoint = 200;

unsigned long startTime;   // To store the start time
unsigned long currentTime; // To calculate the current time

bool use_derivative_filter = true;  // Option to enable or disable derivative filtering
bool use_anti_windup = false;  // Option to enable or disable anti-windup
bool use_reference_weighting = false;  // Option to enable or disable reference weighting

double Kbc;  // Reference anti-windup back-calculation gain 
double beta;  // Reference Proportional weighting factor 
double alpha; // Reference filter coefficient
void setup()
{
  kp = 0.67;
  ki = 0.3;
  kd = 0.2;
  last_time = 0;
  Kbc = ki; // Anti-windup back-calculation gain formula 
  //Kbc = 0.5; // Anti-windup back-calculation gain if 'ki' is not well tunned. 
  beta = 0.5;  // Proportional weighting factor
  alpha = 0.02;  // Filter coefficient for derivative action (between 0 and 1)

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
  //setpoint = map(analogRead(POT_PIN), 0, 1023, 0, 255);  // Adjusts the setpoint between 0 and 255 (adjust as necessary)

  double now = millis();
  dt = (now - last_time) / 1000.00;  // Calculates the time interval (dt)
  last_time = now;

  // Reads the sensor value (from 0 to 1023) and maps it to the range of 0 to 255
  double actual = map(analogRead(INPUT_PIN), 0, 1023, 0, 255);
  double error = setpoint - actual;

  // Use the error in the PID calculation
  output = pid_isa(error, actual, setpoint);  // Pass actual and setpoint to the PID function

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

double pid_isa(double error, double actual, double setpoint)
{
  // Proportional term with reference weighting
  double proportional;
  if (use_reference_weighting) {
    proportional = beta * (setpoint - actual);  // Weighted setpoint influence
  } else {
    proportional = error;  // Standard proportional without reference weighting
  }

  // Integral term
  integral += error * dt;

  // Derivative term
  double derivative;
  if (use_derivative_filter) {
    // Apply the derivative filter
    double raw_derivative = (error - previous_error) / dt;
    derivative = alpha * raw_derivative + (1 - alpha) * previous_filtered_derivative;
    previous_filtered_derivative = derivative;
  } else {
    // No filtering, use raw derivative
    derivative = (error - previous_error) / dt;
  }
  
  previous_error = error;

  // Calculate the PID output with or without filtered derivative
  double output = kp * (proportional + ki * integral + kd * derivative);

  // Limit the output to PWM range (0 to 255)
  double output_saturated = output;

  if (output > 255) output_saturated = 255;
  if (output < 0) output_saturated = 0;

  // Apply anti-windup with back-calculation if enabled
  if (use_anti_windup) {
    integral += Kbc * (output_saturated - output);  // Adjust the integral term based on saturation
  }

  // Apply anti-windup with conditional integration if enabled
  /*if (use_anti_windup) {
    if ((output_saturated < 255 && output_saturated > 0) || (error * output_saturated < 0)) {
      // Only integrate if the output is not saturated or if error helps reduce saturation
      integral += error * dt;  // Update the integral term
    }
  } else {
    // Regular integration without anti-windup
    integral += error * dt;
  }*/

  return output_saturated;  // Return the saturated output
}
