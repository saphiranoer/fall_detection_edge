import subprocess
import pywhatkit as pw
from datetime import datetime

# Get current time
now = datetime.now()

# Set the recipient's phone number (with country code) and the message
phone_number = "+6289695167393"  # Replace with the recipient's phone number
message = "Halo, ini pesan otomatis untuk mengetes pesan tanpa login qr yang dikirim pada jam {}:{}.".format(now.hour, now.minute)

# Send the message
pw.sendwhatmsg(phone_number, message, now.hour, now.minute + 1)
