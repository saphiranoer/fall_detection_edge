# fall_detection_edge
a fall detection system utilizing edge computing, ble, and edge impulse

the dataset used is unimib shar, trained on edge impulse, and deployed in the edge device (an arduino nano 33 ble sense) that is conncted to a laptop via usb.
when a fall is detected by the central arduino, the laptop's serial monitor will detect this and deploy a whatsapp message to a chosen recipient about the fall event.

there is only 1 peripheral device.
