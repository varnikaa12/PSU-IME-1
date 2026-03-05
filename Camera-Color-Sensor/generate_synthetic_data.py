import cv2
import numpy as np
import os
import random

def create_color_image(h_range, s_range, v_range, size=(100, 100)):
    """Generates a synthetic image with slight noise and gradients."""
    # Pick a base color within the range
    h = random.randint(*h_range)
    s = random.randint(*s_range)
    v = random.randint(*v_range)
    
    # Create base flat color in HSV
    img_hsv = np.full((size[0], size[1], 3), [h, s, v], dtype=np.uint8)
    
    # Add some "jitter" to the pixels so it's not perfectly flat
    noise = np.random.randint(-5, 5, (size[0], size[1], 3))
    img_hsv = cv2.add(img_hsv, noise.astype(np.uint8))
    
    # Convert back to BGR for saving
    return cv2.cvtColor(img_hsv, cv2.COLOR_HSV2BGR)

def main():
    # Define HSV ranges for each color to "Anchor" the model
    # Note: HSV in OpenCV: H(0-180), S(0-255), V(0-255)
    COLOR_DEFINITIONS = {
        "Red": {"h": [(0, 10), (170, 180)], "s": (150, 255), "v": (100, 255)},
        "Green": {"h": [(40, 80)], "s": (100, 255), "v": (50, 255)},
        "Blue": {"h": [(100, 130)], "s": (150, 255), "v": (50, 255)},
        "Yellow": {"h": [(20, 35)], "s": (150, 255), "v": (150, 255)},
        "Orange": {"h": [(11, 19)], "s": (150, 255), "v": (150, 255)},
        "Violet": {"h": [(135, 160)], "s": (100, 255), "v": (100, 255)},
        "Black": {"h": [(0, 180)], "s": (0, 50), "v": (0, 50)},
        "White": {"h": [(0, 180)], "s": (0, 30), "v": (200, 255)},
        "Grey": {"h": [(0, 180)], "s": (0, 30), "v": (60, 190)},
        "Brown": {"h": [(0, 20)], "s": (50, 150), "v": (40, 120)}
    }

    dataset_path = "dataset"
    images_per_color = 100
    
    print(f"Generating {images_per_color} synthetic images per color...")

    for color_name, ranges in COLOR_DEFINITIONS.items():
        folder_path = os.path.join(dataset_path, color_name)
        os.makedirs(folder_path, exist_ok=True)
        
        for i in range(images_per_color):
            # Pick one of the hue ranges (some colors like Red have two)
            h_range = random.choice(ranges["h"])
            s_range = ranges["s"]
            v_range = ranges["v"]
            
            img = create_color_image(h_range, s_range, v_range)
            file_path = os.path.join(folder_path, f"synthetic_{i}.jpg")
            cv2.imwrite(file_path, img)
            
        print(f"  [+] {color_name} complete.")

    print("\nSynthetic data generation finished! You can now run 'python train_model.py'.")

if __name__ == "__main__":
    main()
