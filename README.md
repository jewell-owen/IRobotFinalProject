This was the final project for Embedded Systems I.
The task was to locate the 4 skinniest objects in the test field navigate to within 10 cm of them and play a sound to indicate that we located a target object.
While navigating the robot must avoid all tall objects and back up and navigate around if the robot bumped a short object (below the sensor height).

The driver and navigator can not see the field and must rely on scan data from the IR sensor and Ping sensor to locate objects.
Our raw scan data was sent to a Python GUI which showed us the distance and angle of the object realtive to the robot.

Our main navigation code is written in C and is in main.c in the final folder.
Our GUI was written in python and is located in app.py in the final folder.

![image](https://github.com/user-attachments/assets/e6dbc032-5b6c-4b6e-aad5-e814d03c6115)
![image](https://github.com/user-attachments/assets/0c3dbdcb-ce26-4a96-8a6c-e4c826dcf2c6)

