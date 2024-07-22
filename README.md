# fall_detection_edge
A fall detection system utilizing edge computing, BLE, and edge impulse 

The dataset is trained on edge impulse and the model is deployed into the edge device (an arduino nano 33 ble sense) that is connected to a laptop via USB. When a fall is detected by the edge arduino, the laptop's serial monitor will detect this and deploy a whatsapp message to a chosen recipient about the fall event. There is only 1 wearable device in this project.

The devices communicate with BLE, thus:
- The edge device is the central device
- The wearable is the peripheral device

URL to the NUMPY array format of UniMIB SHAR dataset: https://www.kaggle.com/datasets/alirezacman/unimib-shar (the original dataset is in matlab file format) 

The "Prepocess" folder contains the code to transform the .npy file nto CSVs should you intend to train your own model with CSVs. The naming of each CSV files are already set inside the python code (<Class>.<Sub-array_index>_<Segment_index>.csv) following edge impulse's CSV naming convention (<Label_name>.<identification_number>.csv). You can modify the naming to fit your needs.

To use the central device's code, add the edge impulse library (ei-fall-detection-edge-arduino-1.0.18.ei) in Arduino IDE.

The program waCode.py is used to interact with WhatsApp by opening it in either command prompt or idle (python IDE) after the central/edge code is uploaded into the arduino. CLOSE THE CENTRAL/EDGE'S SERIAL MONITOR IN THE ARDUINO IDE, and then run waCode.py.

