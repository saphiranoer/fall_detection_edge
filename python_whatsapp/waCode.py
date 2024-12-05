import serial
import time
import pyautogui
import pywhatkit as pw
from datetime import datetime

# Open serial connection
ser = serial.Serial('COM10', 9600)  # Replace 'COM1' with the central/edge device's port

i = 1
try:
    while True:
        # Receive data from Arduino
        response = ser.readline().decode().strip()
        print(response)
        print(response == "Fall")
        if response == "Fall":
            print("Jatuh terdeteksi")
            # Get current time
            now = datetime.now()

            # Set the recipient's phone number (with country code) and the message
            phone_number = "+628........"  # Replace with the recipient's phone number
            message = "Jatuh terdeteksi pada pukul {}:{}.".format(now.hour, now.minute)

            # Send the message
            pw.sendwhatmsg(phone_number, message, now.hour, now.minute + 1)

            # time.sleep(10)
            # pyautogui.click()
            # time.sleep(10)
            # pyautogui.press('enter')

except KeyboardInterrupt:
    # Ctrl+C was pressed, stop the loop
    pass

# finally:
#     # Close the serial connection
#     ser.close()
