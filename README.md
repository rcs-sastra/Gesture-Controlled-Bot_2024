# Gesture-Controlled-Bot_2024
This repository contains the source code for the Gesture Controlled Bot developed as part of the "Gesture Controlled Bot: Wave to Drive" workshop

This README file will walk you through the entire code, explaining each part in simple terms. It connects a laptop's camera to track hand gestures using MediaPipe, sends commands based on the detected gesture to an ESP32, and receives real-time feedback such as obstacle detection and distance.

**Prerequisites:**

- Install the necessary libraries before running the code:
(for both macOS and windows)
pip install opencv-python mediapipe

- Ensure you have an ESP32 device configured as an access point, and you know its IP address and port number.

**Code Explanation:**



#### 1. **Connecting to the ESP32 via TCP/IP Socket**


esp32_ip = '192.168.4.1'  # IP address of ESP32 access point
esp32_port = 12345  # Port number for communication
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a TCP/IP socket
s.connect((esp32_ip, esp32_port))  # Connect to the ESP32


- **IP Address (`192.168.4.1`)**: This is the unique address of your ESP32 device. Think of it as the "house address" of your ESP32 in the network.
- **Port Number (`12345`)**: A port acts like a specific door on the device where data is sent and received. Each device can have multiple ports.
- **Socket (`socket.socket`)**: This establishes a connection between your computer and the ESP32 so they can talk to each other.



#### 2. **Setting Up MediaPipe Hand Tracking**


mp_hands = mp.solutions.hands  # Load the hand-tracking model from MediaPipe
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)  # Configure model parameters
mp_drawing = mp.solutions.drawing_utils  # Tool to draw hand landmarks on the image

- **`mp_hands`**: MediaPipe provides a pre-trained hand-tracking model that can identify hand landmarks (finger joints, palm, etc.).
- **`Hands(max_num_hands=1, min_detection_confidence=0.7)`**: 
  - `max_num_hands`: Specifies that only one hand will be detected at a time.
  - `min_detection_confidence`: The model only accepts frames with a confidence level of 70% or higher.
- **`mp_drawing`**: A helper utility to draw the detected landmarks on the webcam frame.


#### 3. **Initializing the Webcam**

cap = cv2.VideoCapture(0)  # Start capturing video from the default webcam

- **`cv2.VideoCapture(0)`**: This opens the first webcam connected to your system (the number `0` represents the default camera).


#### 4. **Defining Finger Tip and Thumb Tip Landmarks**


finger_tips = [8, 12, 16, 20]  # Indexes for finger tip landmarks
thumb_tip = 4  # Index for thumb tip landmark


- These numbers represent specific landmarks in MediaPipe's hand model that correspond to the tips of each finger.


#### 5. **Gesture-Based Commands**


commands = {
    0: '0-Stop',
    1: '1-Forward',
    2: '2-Reverse',
    3: '3-Right',
    4: '4-Left'
}


- **`commands`**: Maps the number of detected fingers to specific robot actions.
  - For example, 0 fingers raised means "Stop," 1 finger means "Move Forward," and so on.


#### 6. **Processing the Video Feed**


success, frame = cap.read()  # Read a frame from the webcam
if not success:
    break


- **`cap.read()`**: Reads one frame from the webcam at a time. `success` returns `True` if the frame is captured successfully, and `frame` holds the image data.



#### 7. **Converting Frame to RGB**


rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert the BGR frame to RGB


 **`cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)`**: OpenCV uses BGR color format by default, but MediaPipe requires RGB, so this converts the image format.



#### 8. **Detecting Hand Landmarks**

results = hands.process(rgb_frame)  # Detect hand landmarks in the RGB frame


- **`hands.process(rgb_frame)`**: This function processes the frame and detects hand landmarks (if any).



#### 9. **Drawing Hand Landmarks**


mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)


- **`mp_drawing.draw_landmarks()`**: Draws lines connecting the joints of the hand and marks key points (like fingertips) on the frame.


#### 10. **Counting Fingers**


for tip in finger_tips:
    if landmarks[tip][1] < landmarks[tip - 2][1]:
        finger_count += 1
if landmarks[thumb_tip][0] > landmarks[thumb_tip - 2][0]:
    finger_count += 1


- **Checking Finger Tips**: For each finger, it compares the y-coordinate of the tip with that of the lower joint to determine whether the finger is raised.
- **Thumb Check**: The thumb uses the x-coordinate to check whether it's extended outward (to the right).


#### 11. **Sending Commands to ESP32**


message = str(finger_count) + '\n'
s.send(message.encode())  # Send the finger count to the ESP32


- **`s.send(message.encode())`**: Converts the finger count to a string and sends it as a message to the ESP32 over the socket connection.


#### 12. **Receiving Data from ESP32**

response = s.recv(1024).decode().strip().split(',')
distance = float(response[0])
obstacle_detected = bool(int(response[1]))


- **`s.recv(1024)`**: Receives data from the ESP32. The number `1024` represents the buffer size (max bytes to receive).
- **`response.split(',')`**: The data received is split into two parts:
  1. **Distance**: The distance between the robot and an obstacle (in cm).
  2. **Obstacle Detection**: Whether an obstacle is present or not (True/False).



#### 13. **Displaying Results on the Screen**


cv2.putText(frame, command, (50, 150), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 5)
cv2.putText(frame, f"Distance: {distance:.2f} cm", (50, 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)


- **`cv2.putText()`**: This function displays text on the video feed, such as the current command and distance from obstacles.


#### 14. **Stopping the Program**


if cv2.waitKey(1) & 0xFF == ord('q'):
    break


- **`cv2.waitKey(1)`**: Waits for 1 millisecond for a key press. If the `q` key is pressed, the program breaks the loop and stops.


#### 15. **Releasing Resources**

s.close()  # Close the socket connection
cap.release()  # Release the webcam
cv2.destroyAllWindows()  # Close all OpenCV windows


- **`s.close()`**: Closes the connection to the ESP32.
- **`cap.release()`**: Stops the webcam.
- **`cv2.destroyAllWindows()`**: Closes any OpenCV windows.


### Summary

This project captures real-time hand gestures using a webcam, interprets them as commands for a robot, and sends these commands to an ESP32 device over a wireless connection. The ESP32 also sends back information about the distance to obstacles and whether an obstacle is detected.
