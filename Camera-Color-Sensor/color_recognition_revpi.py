from http import server

import cv2
import numpy as np
import joblib
import os
from pyModbusTCP.server import ModbusServer, DataBank
from camera_utils import select_camera

# Color to Integer Mapping for CODESYS
# Ensure these match your Ladder Logic constants
COLOR_MAP = {
    "red": 1,
    "green": 2,
    "blue": 3,
    "yellow": 4,
    "orange": 5,
    "violet": 6,
    "black": 7,
    "white": 8,
    "grey": 9,
    "brown": 10
}

def extract_features(img):
    """Advanced color features: BGR Mean/Std, HSV Mean/Std."""
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2Lab)
    mean_bgr = cv2.mean(img)[:3]
    mean_hsv = cv2.mean(hsv_img)[:3]
    std_bgr = np.std(img, axis=(0, 1))
    std_hsv = np.std(hsv_img, axis=(0, 1))
    return np.concatenate([mean_bgr, mean_hsv, std_bgr, std_hsv])

def main():
    # 1. Start Modbus Server (PC IP, Port 502)
    # Note: On Windows, you might need to run as Admin to use port 502, 
    # or use a higher port like 1502 if 502 is blocked.
    MODBUS_PORT = 1502
    server = ModbusServer("0.0.0.0", port=MODBUS_PORT, no_block=True)
    db = DataBank()
    
    try:
        server.start()
        print(f"Modbus Server started on port {MODBUS_PORT}")
    except Exception as e:
        print(f"Failed to start Modbus Server: {e}")
        return

    # 2. Load Model
    model_path = "color_model.pkl"
    if not os.path.exists(model_path):
        print(f"Model file '{model_path}' not found. Please run train_model.py first.")
        return

    try:
        model_data = joblib.load(model_path)
        model = model_data["model"]
        print(f"Loaded model (Version: {model_data.get('feature_version', '2.0')})")
    except Exception as e:
        print(f"Error loading model: {e}")
        return

    # 3. Select Camera
    try:
        camera_index = select_camera()
    except Exception as e:
        print(e)
        return

    cap = cv2.VideoCapture(camera_index, cv2.CAP_DSHOW)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return

    print("\nPress 'q' (with camera window focused) to quit.")
    window_name = "RevPi Connected Color Sensor"
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)

    try:
        while True:
            ret, frame = cap.read()
            if not ret: break

            height, width, _ = frame.shape
            box_size = 150
            start_point = (width // 2 - box_size // 2, height // 2 - box_size // 2)
            end_point = (width // 2 + box_size // 2, height // 2 + box_size // 2)
            
            # Draw the box
            cv2.rectangle(frame, start_point, end_point, (0, 255, 0), 2)
            
            # Crop the focus area
            focus_area = frame[start_point[1]:end_point[1], start_point[0]:end_point[0]]
            
            # Smooth area to reduce noise
            focus_area_blurred = cv2.GaussianBlur(focus_area, (7, 7), 0)
            
            # Prediction
            try:
                features = extract_features(focus_area_blurred)
                prediction = model.predict([features])[0]
                
                # Confidence score
                probabilities = model.predict_proba([features])[0]
                class_index = list(model.classes_).index(prediction)
                certainty = probabilities[class_index] * 100
                
                # Update Modbus Holding Register 0
                bit_index = COLOR_MAP.get(prediction, -1)
                register_value = (1 << bit_index) if bit_index >= 0 else 0  # shift 1 into the correct bit position
                server.data_bank.set_holding_registers(1, [register_value])
                                
                # Visual Feedback
                status_text = f"RELAYING: {prediction} (Bit: {bit_index}, Val: {register_value})"
                cv2.putText(frame, status_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
                cv2.putText(frame, f"Certainty: {certainty:.1f}%", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)

                # Color Patch
                mean_bgr = cv2.mean(focus_area_blurred)[:3]
                patch = np.zeros((50, 50, 3), dtype=np.uint8)
                patch[:] = [int(c) for c in mean_bgr]
                frame[10:60, width-60:width-10] = patch
                cv2.rectangle(frame, (width-60, 10), (width-10, 60), (255, 255, 255), 1)

            except Exception as e:
                cv2.putText(frame, f"Error: {str(e)}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 1)

            cv2.imshow(window_name, frame)
            if (cv2.waitKey(1) & 0xFF == ord('q')) or (cv2.getWindowProperty(window_name, cv2.WND_PROP_VISIBLE) < 1):
                break
    finally:
        server.stop()
        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
    