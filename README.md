# OpenGL and GLFW implementation of a Smart Watch in 2d

<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/75681ed7-e529-4c74-8be0-946446983b6d" />
In the middle of the home screen, the time is displayed in HH:MM:SS format. Every second
increases the last value, until it exceeds 60. Then it increases the minute by 1 and the seconds are reset to 00. When the minute exceeds 60, it is reset to 00, and the hour is incremented by 1. When the hour exceeds 24, it is reset at 00. To the right of the clock there is an arrow pointing to the right which, when left-clicked mouse, changes the screen to heart rate measurement.

<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/9e170e2d-c7c5-4d1d-a7ca-6812bda22266" />
On the heart rate screen, there is a left-pointing and a left-pointing arrow on the left
clicking the mouse takes you back to the time screen. Heart rate measurement is located in the middle of the screen An ECG that is animated so that the graph moves to the left but so that the rectangle in which it is located stays in place. The graphic is represented by a single texture on which one down and one is drawn hill of the ECG graph and which is repeated several times. Above the graph is the number of beats per second minutes, which moves between 60 and 80 BPM with a random function. While holding the D key, running is simulated. The BPM slowly increases and the graphic texture tapers, so the effect will be created that there will be more and more beats that are visible on the monitor. While it is BPM over 200, a full-screen warning is displayed for the user to stop and rest.
To the right of the screen there is an arrow that points to the right and which, when clicked with the left click mouse, changes the screen to the battery view.

<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/cefdeb60-73ab-40d6-807e-2c10d7cc03bf" />
On the battery display screen, there is a battery symbol in the middle, which is at the beginning of the program 100% and is filled with green color, while the percentage is written above it. While the program lasts, on every 10 seconds the battery decreases by 1% and the green rectangle that fills the battery shrinks so that it reflects the appropriate percentage and so that it is always glued to the right edge of the battery.
If it exceeds 20%, the rectangle turns yellow, and if it exceeds 10%, it turns red. On the left on the side of the battery display screen there is an arrow pointing to the left and clicking takes you back to heart rate monitor.
The cursor should look like a heart.
