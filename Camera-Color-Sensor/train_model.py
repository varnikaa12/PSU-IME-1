import cv2
import os
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import accuracy_score
import joblib

def load_dataset(dataset_path="dataset"):
    """Loads images from dataset_path/color_name/*.jpg"""
    X = []
    y = []
    labels = sorted(os.listdir(dataset_path))
    
    for label in labels:
        label_path = os.path.join(dataset_path, label)
        if not os.path.isdir(label_path):
            continue
            
        print(f"Loading {label} images...")
        for img_name in os.listdir(label_path):
            img_path = os.path.join(label_path, img_name)
            img = cv2.imread(img_path)
            if img is None:
                continue
            
            # Extract features: Mean BGR values
            # We can also add more like standard deviation or HSV
            # For simplicity, we'll start with Mean BGR
            mean_color = cv2.mean(img)[:3]
            X.append(mean_color)
            y.append(label)
            
    return np.array(X), np.array(y), labels

def train():
    dataset_path = "training_dataset"
    if not os.path.exists(dataset_path):
        print(f"Dataset folder '{dataset_path}' not found. Please provide the dataset from Kaggle.")
        return

    X, y, labels = load_dataset(dataset_path)
    if len(X) == 0:
        print("No data found in dataset.")
        return

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    print(f"Training on {len(X_train)} samples...")
    model = KNeighborsClassifier(n_neighbors=5)
    model.fit(X_train, y_train)

    y_pred = model.predict(X_test)
    acc = accuracy_score(y_test, y_pred)
    print(f"Training Accuracy: {acc * 100:.2f}%")

    # Save model and labels
    model_data = {
        "model": model,
        "labels": labels
    }
    joblib.dump(model_data, "color_model.pkl")
    print("Model saved as color_model.pkl")

if __name__ == "__main__":
    train()
