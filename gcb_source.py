import cv2
import mediapipe as mp
import socket

# Connect to the ESP32 access point
esp32_ip = '192.168.4.1'  # Default IP for ESP32 access point
esp32_port = 12345  # Changed port number
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((esp32_ip, esp32_port))
print('Connected to ESP32.')

# Initialize Mediapipe Hand model
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)
mp_drawing = mp.solutions.drawing_utils

# Initialize webcam
cap = cv2.VideoCapture(0)

# Finger tip landmarks in Mediapipe
finger_tips = [8, 12, 16, 20]
thumb_tip = 4

commands = {
    0: '0-Stop',
    1: '1-Forward',
    2: '2-Reverse',
    3: '3-Right',
    4: '4-Left'
}

while True:
    success, frame = cap.read()
    if not success:
        break

    # Convert the frame to RGB
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Process the RGB frame to detect hand landmarks
    results = hands.process(rgb_frame)

    # If hand landmarks are detected
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Draw landmarks on the frame
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # Get landmarks coordinates
            landmarks = []
            for lm in hand_landmarks.landmark:
                h, w, c = frame.shape
                cx, cy = int(lm.x * w), int(lm.y * h)
                landmarks.append((cx, cy))

            # Count fingers
            finger_count = 0
            # Check for each finger (excluding thumb)
            for tip in finger_tips:
                if landmarks[tip][1] < landmarks[tip - 2][1]:
                    finger_count += 1
            # Check for thumb
            if landmarks[thumb_tip][0] > landmarks[thumb_tip - 2][0]:
                finger_count += 1

            command = commands.get(finger_count, '5-Unknown')

            # Display the command on the frame
            cv2.putText(frame, command, (50, 150), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 5)

            # Print the count in the terminal
            print(f"Fingers detected: {finger_count}")

            # Send the finger count to the ESP32
            message = str(finger_count) + '\n'
            s.send(message.encode())

            # Receive response from ESP32
            response = s.recv(1024).decode().strip().split(',')
            distance = float(response[0])
            obstacle_detected = bool(int(response[1]))

            # Display distance and obstacle status on the frame
            cv2.putText(frame, f"Distance: {distance:.2f} cm", (50, 200), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            obstacle_text = "Obstacle Detected" if obstacle_detected else "No Obstacle"
            cv2.putText(frame, obstacle_text, (50, 250), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255) if obstacle_detected else (0, 255, 0), 2)

    # Display the frame
    cv2.imshow("Gesture Controlled Bot", frame)

    # Break the loop if 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Close the socket and release the webcam
s.close()
cap.release()
cv2.destroyAllWindows()