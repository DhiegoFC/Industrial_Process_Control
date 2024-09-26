const int INPUT_PIN = A0;  // Analog input pin (sensor)
const int OUTPUT_PIN = 3;  // PWM output pin (controller)
const int POT_PIN = A1;    // Pin where the potentiometer is connected

double dt, last_time;
double integral, previous, output = 0;
double kp, ki, kd;
double setpoint = 75.00; // If using a variable setpoint with the potentiometer, comment out this line
//double setpoint;
void setup()
{
  kp = 1.98;
  ki = 1.45;
  kd = 0.002;
  last_time = 0;
  Serial.begin(9600);
  analogWrite(OUTPUT_PIN, 0);  // Initializes the output to 0
  for(int i = 0; i < 50; i++)
  {
    Serial.print(setpoint);
    Serial.print(",");
    Serial.println(0);
    delay(100);
  }
  delay(100);
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
  output = pid(error);  // Calculates the PID output value

  analogWrite(OUTPUT_PIN, output);  // Applies the calculated output

  // Sends the setpoint and the actual value to the Serial Plotter, separated by commas
  Serial.print(setpoint);
  Serial.print(",");
  Serial.print(actual);
  Serial.print(",");
  Serial.print(error);
  Serial.print(",");
  Serial.println(output);

  delay(0);  // Insert delay in the circuit
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
