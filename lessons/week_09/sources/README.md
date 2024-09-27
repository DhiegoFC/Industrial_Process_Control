# PID Controller on Arduino UNO

Create the following circuit with two capacitors and two resistors. It is possible to tune the Arduino's PWM output according to the setpoint. You can use either a fixed setpoint or a variable one by using a potentiometer.

<img src="./lesson_images/arduino_uno_pid.jpg" alt="Arduino UNO PID Components" width="1000"/>

<img src="./lesson_images/arduino_uno_pid_circuit.jpg" alt="Arduino UNO PID Circuit" width="1000"/>

[With this code](./pid_controller_arduino_uno/pid_controller_arduino_uno.ino), upload it to the Arduino Uno's flash memory using the Arduino IDE platform. Observe how the system behaves by using the Arduino IDE's Serial Plotter, adjusting the PID controller variables, as well as the delay in the loop() function.









