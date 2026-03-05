import cv2
import os
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, classification_report
import joblib

def extract_features(img):
    """
    Extracts advanced color features using CIELAB color space.
    CIELAB is perceptually uniform and better at separating Red/Violet.
    - L: Lightness
    - a: Green-Red axis
    - b: Blue-Yellow axis
    """
    # Convert to CIELAB
    lab_img = cv2.cvtColor(img, cv2.COLOR_BGR2Lab)
    
    # Calculate means and standard deviations for BGR and LAB
    mean_bgr = cv2.mean(img)[:3]
    mean_lab = cv2.mean(lab_img)[:3]
    
    std_bgr = np.std(img, axis=(0, 1))
    std_lab = np.std(lab_img, axis=(0, 1))
    
    # Combined feature vector (12 features total)
    return np.concatenate([mean_bgr, mean_lab, std_bgr, std_lab])

def load_dataset(dataset_path="dataset"):
    X = []
    y = []
    
    if not os.path.exists(dataset_path):
        return None, None, None

    labels = sorted([d for d in os.listdir(dataset_path) if os.path.isdir(os.path.join(dataset_path, d))])
    
    for label in labels:
        label_path = os.path.join(dataset_path, label)
        print(f"Processing {label}...")
        
        for img_name in os.listdir(label_path):
            img_path = os.path.join(label_path, img_name)
            img = cv2.imread(img_path)
            if img is None:
                continue
            
            # Extract CIELAB features
            features = extract_features(img)
            X.append(features)
            y.append(label)
            
    return np.array(X), np.array(y), labels

def train():
    dataset_path = "dataset"
    X, y, labels = load_dataset(dataset_path)
    
    if X is None or len(X) == 0:
        print("Dataset not found or empty.")
        return

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    print(f"\nTraining Random Forest (CIELAB version) on {len(X_train)} samples...")
    model = RandomForestClassifier(n_estimators=100, random_state=42)
    model.fit(X_train, y_train)

    y_pred = model.predict(X_test)
    acc = accuracy_score(y_test, y_pred)
    print(f"Accuracy: {acc * 100:.2f}%")
    print("\nDetailed Report:")
    print(classification_report(y_test, y_pred))

    model_data = {
        "model": model,
        "labels": labels,
        "feature_version": "3.0 (CIELAB)" 
    }
    joblib.dump(model_data, "color_model.pkl")
    print("\nCIELAB-based model saved as color_model.pkl")

if __name__ == "__main__":
    train()
