import cv2
import subprocess

def get_camera_names_win():
    """Attempt to get camera names on Windows using PowerShell."""
    try:
        # PowerShell command to list OK cameras and images
        cmd = 'powershell "Get-PnpDevice -Class Camera,Image -Status OK | Select-Object FriendlyName | Format-Table -HideTableHeaders"'
        output = subprocess.check_output(cmd, shell=True).decode('utf-8', errors='ignore')
        names = [line.strip() for line in output.split('\r\n') if line.strip()]
        # Filter out lines that are just dashes
        names = [n for n in names if not n.startswith('---')]
        return names
    except Exception:
        return []

def get_available_cameras():
    """Lists available cameras with their indices and attempted names."""
    index = 0
    available = []
    # Get names from system as a reference
    system_names = get_camera_names_win()
    
    while True:
        cap = cv2.VideoCapture(index)
        if not cap.isOpened():
            break
        
        ret, _ = cap.read()
        if ret:
            # If we found names in the system, try to associate them
            # Note: This is a best-effort mapping as indices don't always match 1:1
            name = system_names[index] if index < len(system_names) else f"Camera {index}"
            available.append({"index": index, "name": name})
        
        cap.release()
        index += 1
        # Limit search to avoid infinite loops
        if index > 10:
            break
            
    return available

def select_camera():
    """Returns the selected camera index with names displayed."""
    cameras = get_available_cameras()
    
    if len(cameras) == 0:
        raise Exception("No cameras found.")
    elif len(cameras) == 1:
        print(f"Using: {cameras[0]['name']} (Index {cameras[0]['index']})")
        return cameras[0]['index']
    else:
        print("\nMultiple cameras found:")
        for cam in cameras:
            print(f"[{cam['index']}] {cam['name']}")
        
        while True:
            try:
                choice = int(input(f"\nSelect camera index: "))
                if any(c['index'] == choice for c in cameras):
                    return choice
                else:
                    print("Invalid choice. Try again.")
            except ValueError:
                print("Please enter a valid number.")

if __name__ == "__main__":
    try:
        idx = select_camera()
        print(f"Selected index: {idx}")
    except Exception as e:
        print(e)
