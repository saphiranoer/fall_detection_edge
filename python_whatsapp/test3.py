import serial
import time
import pyautogui
import pywhatkit as pw
from datetime import datetime

# Open serial connection
ser = serial.Serial('COM10', 9600)  # Replace 'COM1' with the appropriate port

i = 1
try:
    while True:
        # Send data to Arduino
        ser.write(b'Hello Arduino\n')

        # Receive data from Arduino
        response = ser.readline().decode().strip()
        print(response)
        print(response == "Fall")
        if response == "Fall":
            # print("Deteksi jatuh")
            i += 1
            # Delay for 5 seconds
            # time.sleep(5)

        print(i)

        if i % 5 == 0:
            print("Deteksi Jatuh")
            # Get current time
            now = datetime.now()

            # Set the recipient's phone number (with country code) and the message
            phone_number = "+6289695167393"  # Replace with the recipient's phone number
            message = "Deteksi jatuh menggunakan model 4, karena model 2 & 3 mendeteksi terbalik dari model 1, diperkirakan pada jam {}:{}.".format(now.hour, now.minute)

            # Send the message
            pw.sendwhatmsg(phone_number, message, now.hour, now.minute + 1)

            time.sleep(10)
            pyautogui.click()
            time.sleep(10)
            pyautogui.press('enter')

except KeyboardInterrupt:
    # Ctrl+C was pressed, stop the loop
    pass

# finally:
#     # Close the serial connection
#     ser.close()
