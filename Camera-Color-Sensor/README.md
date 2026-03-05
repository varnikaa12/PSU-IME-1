# Camera Color Sensor & Recognition for Revolution Pi

This system provides high-accuracy real-time color recognition using a Random Forest model. It is designed to relay color data to a **Revolution Pi (RevPi)** via **Modbus TCP**, allowing for integration with **CODESYS** and industrial ladder logic.

## Key Features

- **Advanced AI Model**: Uses a Random Forest Classifier trained on BGR and HSV features (Version 2.0).
- **Lighting Robustness**: HSV color space integration helps distinguish colors (like Red vs. Brown) in varying light.
- **Modbus TCP Integration**:
  - **`color_recognition.py`**: Runs on a PC, acts as a Modbus Server (Slave).
  - **`color_recognition_revpi.py`**: Runs on the RevPi, acts as a Modbus Client (Master).
- **Camera Selection**: Automatically identifies Windows device names (e.g., "Integrated Camera", "UC60 Video").
- **Visual Feedback**: Live feed includes a focus box, certainty percentage, and a color patch.

## Setup & Installation

### 1. Prerequisites
- Python 3.10+
- Modbus TCP enabled in CODESYS (if using RevPi)

### 2. Install Dependencies
```bash
pip install -r requirements.txt
```

### 3. Training the Model
1. Place your categorized images in the `dataset/` folder.
2. Run the training script:
   ```bash
   python train_model.py
   ```
   This generates `color_model.pkl` using the advanced feature extraction.

## Usage

### Local Testing (PC as Server)
Run this script to test the camera and host the Modbus data:
```bash
python color_recognition.py
```
- **Modbus Address**: The color ID is written to **Holding Register 0**.
- **Note**: Run as Administrator on Windows to allow port 502 access.

### Production (RevPi as Client)
Run this script on the RevPi to capture video and write to a PLC/CODESYS:
```bash
python color_recognition_revpi.py
```

## Color Mapping Reference
The following IDs are sent over Modbus to CODESYS:

| ID | Color | ID | Color |
|:---|:---|:---|:---|
| 1 | Red | 6 | Violet |
| 2 | Green | 7 | Black |
| 3 | Blue | 8 | White |
| 4 | Yellow | 9 | Grey |
| 5 | Orange | 10 | Brown |

## Project Structure
- `camera_utils.py`: Hardware-level camera discovery and naming.
- `train_model.py`: Random Forest training with HSV feature extraction.
- `color_recognition.py`: Primary PC-side script (Modbus Server).
- `color_recognition_revpi.py`: RevPi-side script (Modbus Client).
- `requirements.txt`: Project dependencies (OpenCV, Scikit-learn, pyModbusTCP).
