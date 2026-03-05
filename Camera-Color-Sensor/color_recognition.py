import cv2
import numpy as np
import joblib
import os
from camera_utils import select_camera

def extract_features(img):
    """Must match the training script's feature extraction exactly."""
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mean_bgr = cv2.mean(img)[:3]
    mean_hsv = cv2.mean(hsv_img)[:3]
    std_bgr = np.std(img, axis=(0, 1))
    std_hsv = np.std(hsv_img, axis=(0, 1))
    return np.concatenate([mean_bgr, mean_hsv, std_bgr, std_hsv])

def main():
    model_path = "color_model.pkl"
    if not os.path.exists(model_path):
        print(f"Model file '{model_path}' not found. Please run train_model.py first.")
        return

    try:
        model_data = joblib.load(model_path)
        model = model_data["model"]
        print(f"Loaded advanced model (Version: {model_data.get('feature_version', '1.0')})")
    except Exception as e:
        print(f"Error loading model: {e}")
        return

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
    window_name = "Advanced Color Recognition - Live Feed"
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)

    try:
        while True:
            ret, frame = cap.read()
            if not ret: break

            height, width, _ = frame.shape
            
            # Larger box for better feature extraction
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
                
                # Confidence score (probability)
                probabilities = model.predict_proba([features])[0]
                class_index = list(model.classes_).index(prediction)
                certainty = probabilities[class_index] * 100
                
                # Display Results
                text = f"Color: {prediction} ({certainty:.1f}%)"
                cv2.putText(frame, text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

                # Color Patch (BGR for display)
                mean_bgr = cv2.mean(focus_area_blurred)[:3]
                patch_size = 50
                patch = np.zeros((patch_size, patch_size, 3), dtype=np.uint8)
                patch[:] = [int(c) for c in mean_bgr]
                frame[10:10+patch_size, width-10-patch_size:width-10] = patch
                cv2.rectangle(frame, (width-10-patch_size, 10), (width-10, 10+patch_size), (255, 255, 255), 1)

            except Exception as e:
                cv2.putText(frame, f"Error: {str(e)}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 1)

            cv2.imshow(window_name, frame)
            if (cv2.waitKey(1) & 0xFF == ord('q')) or (cv2.getWindowProperty(window_name, cv2.WND_PROP_VISIBLE) < 1):
                break
    finally:
        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
