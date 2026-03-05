import cv2
import numpy as np
import joblib
import os
from camera_utils import select_camera

def main():
    # Load model
    model_path = "color_model.pkl"
    if not os.path.exists(model_path):
        print(f"Model file '{model_path}' not found. Please run train_model.py first.")
        return

    try:
        model_data = joblib.load(model_path)
        model = model_data["model"]
    except Exception as e:
        print(f"Error loading model: {e}")
        return

    # Select camera
    try:
        camera_index = select_camera()
    except Exception as e:
        print(e)
        return

    cap = cv2.VideoCapture(camera_index, cv2.CAP_DSHOW)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return

    print("Press 'q' (with the camera window focused) to quit.")

    # Create window explicitly
    window_name = "Color Recognition - Live Feed"
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                print("Failed to grab frame.")
                break

            height, width, _ = frame.shape
            
            # Define the box around the item in focus (center of the screen)
            box_size = 50
            start_point = (width // 2 - box_size // 2, height // 2 - box_size // 2)
            end_point = (width // 2 + box_size // 2, height // 2 + box_size // 2)
            
            # Draw the box
            cv2.rectangle(frame, start_point, end_point, (0, 255, 0), 2)
            
            # Crop the focus area
            focus_area = frame[start_point[1]:end_point[1], start_point[0]:end_point[0]]
            
            # Extract features (Mean BGR)
            mean_color = cv2.mean(focus_area)[:3]
            
            # Prediction
            try:
                prediction = model.predict([mean_color])[0]
                probabilities = model.predict_proba([mean_color])[0]
                max_prob = np.max(probabilities)
                certainty = max_prob * 100
                
                # Display results
                text = f"Color: {prediction} ({certainty:.1f}%)"
                cv2.putText(frame, text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

                # Draw a color patch for the detected color
                patch_size = 50
                patch = np.zeros((patch_size, patch_size, 3), dtype=np.uint8)
                patch[:] = [int(c) for c in mean_color]
                
                # Place patch on top right
                frame[10:10+patch_size, width-10-patch_size:width-10] = patch
                cv2.rectangle(frame, (width-10-patch_size, 10), (width-10, 10+patch_size), (255, 255, 255), 1)

            except Exception as e:
                cv2.putText(frame, "Recognition Error", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

            # Show live feed
            cv2.imshow(window_name, frame)

            # Wait for 1ms and check for 'q'
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                print("'q' pressed. Exiting...")
                break
            
            # Handle window close button (X)
            if cv2.getWindowProperty(window_name, cv2.WND_PROP_VISIBLE) < 1:
                break
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
    finally:
        print("Releasing resources...")
        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
