# Camera Color Sensor & Recognition

This project uses a computer's camera to recognize colors in a live feed. It uses a K-Nearest Neighbors (KNN) model trained on a color dataset to predict the color of the item in focus (the center of the frame).

## Features

- **Automatic Camera Detection**: 
  - If one camera is found, it's used automatically.
  - If multiple cameras are found, the user is prompted to select one.
  - Errors out if no cameras are detected.
- **Live Feed & Focus**: 
  - Displays a live feed with a box in the center representing the item in focus.
- **Color Recognition**: 
  - Reports the color name and the certainty percentage.
  - Shows a small color patch of the mean color detected in the focus area.
- **Custom Training**: 
  - Includes a script to train the model from a dataset.

## Setup

### 1. Prerequisites
Ensure you have Python 3.10+ installed.

### 2. Install Dependencies
Install the required libraries using pip:
```bash
pip install -r requirements.txt
```

### 3. Dataset Preparation
The model is designed to be trained on this [Kaggle Color Dataset](https://www.kaggle.com/datasets/adikurniawan/color-dataset-for-color-recognition/data).

1. Download and extract the dataset.
2. Place the folder named `training_dataset` into the project root directory.
3. The directory structure should look like this:
   ```text
   dataset/
   ├── Black/
   ├── Blue/
   ├── Green/
   ├── Red/
   └── ... (other colors)
   ```

### 4. Training the Model
Run the training script to generate the `color_model.pkl` file:
```bash
python train_model.py
```
This script extracts the mean color (BGR) from each image and trains a KNN classifier.

### 5. Running Color Recognition
Start the live recognition program:
```bash
python color_recognition.py
```
- A window will open showing your camera feed.
- Point the center box at any colored object to see the prediction and certainty.
- Press **'q' or 'Ctrl+C'** to exit the application.

## Project Structure

- `camera_utils.py`: Logic for detecting and selecting available cameras.
- `train_model.py`: Script to load the dataset, extract features, and train the model.
- `color_recognition.py`: The main application for real-time color detection.
- `requirements.txt`: List of necessary Python libraries.
- `color_model.pkl`: The saved model file (generated after training).
