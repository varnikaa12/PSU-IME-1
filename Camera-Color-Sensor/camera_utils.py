import cv2

def get_available_cameras():
    """Lists indices of available cameras."""
    index = 0
    arr = []
    while True:
        cap = cv2.VideoCapture(index, cv2.CAP_DSHOW)
        if not cap.read()[0]:
            break
        else:
            arr.append(index)
        cap.release()
        index += 1
    return arr

def select_camera():
    """Returns the selected camera index."""
    cameras = get_available_cameras()
    if len(cameras) == 0:
        raise Exception("No cameras found.")
    elif len(cameras) == 1:
        print(f"Using camera at index {cameras[0]}")
        return cameras[0]
    else:
        print("Multiple cameras found:")
        for idx in cameras:
            print(f"[{idx}] Camera {idx}")
        
        while True:
            try:
                choice = int(input(f"Select camera index ({', '.join(map(str, cameras))}): "))
                if choice in cameras:
                    return choice
                else:
                    print("Invalid choice. Try again.")
            except ValueError:
                print("Please enter a valid number.")

if __name__ == "__main__":
    try:
        idx = select_camera()
        print(f"Selected: {idx}")
    except Exception as e:
        print(e)
