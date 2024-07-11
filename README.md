# fall_detection_edge
A fall detection system utilizing edge computing, BLE, and edge impulse 

The dataset is trained on edge impulse and deployed into the edge device (an arduino nano 33 ble sense) that is connected to a laptop via usb. When a fall is detected by the central arduino, the laptop's serial monitor will detect this and deploy a whatsapp message to a chosen recipient about the fall event. There is only 1 peripheral device.

URL to the public edge impulse project: https://studio.edgeimpulse.com/studio/442806/deployment (you can immediately download the model in the deployment sub-menu)
URL to the NUMPY array format of UniMIB SHAR dataset: https://www.kaggle.com/datasets/alirezacman/unimib-shar (the original dataset is in matlab file format)

To use the central device code, you need to download the model from the edge impulse project shown above and install the library in Arduino IDE
