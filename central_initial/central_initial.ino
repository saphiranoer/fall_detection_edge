#include <fall_detection_edge_inferencing.h> 
#include <ArduinoBLE.h>

static const float features[] = {
  // raw features of fall.835
  // See https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
 9.9458, 9.5290, 9.5577, 9.8961, 10.6113, 11.8742, 12.1903, 10.4279, 9.3882, 9.2284, 9.7403, 10.5571, 10.6561, 9.6459, 8.9488, 8.7634, 8.7640, 8.7238, 8.7519, 
 8.9421, 9.5349, 9.8436, 9.8691, 9.6672, 9.6246, 9.6988, 9.8226, 9.8908, 9.9443, 9.7662, 9.7743, 9.6699, 9.7281, 9.8269, 9.6462, 9.5091, 9.4035, 9.5773, 9.6988, 
 9.6851, 9.5565, 9.5997, 9.6695, 9.9812, 10.1186, 10.1549, 10.0300, 9.7852, 9.6422, 9.6140, 9.3297, 9.2818, 9.4400, 9.5429, 9.2873, 9.3216, 9.7185, 9.6712, 
 9.4980, 9.5145, 9.6690, 9.5025, 9.4230, 9.3120, 9.4483, 9.5486, 9.7874, 10.1196, 10.3929, 10.2831, 9.9185, 9.7380, 9.3651, 9.1250, 9.0450, 8.9048, 8.6238, 
 8.6574, 9.1451, 9.7510, 9.7944, 9.3496, 8.6247, 8.1603, 8.2091, 8.3478, 8.8824, 10.5064, 14.3289, 23.0665, 27.0366, 26.6170, 25.0646, 28.4367, 26.8791, 10.3374, 
 5.1846, 14.5706, 21.5111, 20.6105, 13.6854, 7.0048, 7.8984, 12.7903, 17.3756, 21.2410, 21.0626, 17.6533, 10.3315, 9.0023, 7.5378, 7.9261, 5.8944, 5.2178, 
 6.3532, 6.5947, 8.1970, 9.3117, 8.9438, 9.4930, 10.5917, 11.9410, 12.1491, 12.0127, 12.4502, 13.9259, 16.8372, 17.3116, 16.7700, 18.8709, 18.9389, 15.7694, 
 16.4634, 14.5709, 12.4315, 10.5454, 10.3850, 10.9994, 9.9386, 8.5481, 9.2752, 9.7996, 10.4967, 11.4929, 12.2237, 12.5590, 12.7468, 13.1920, 13.6610, 13.8299
 }; 

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float)); 
  return 0;
}
float fd = 0;

void print_inference_result(ei_impulse_result_t result);

BLEService SensorService("1102");
BLEUnsignedIntCharacteristic PChar("2201", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

    if (!BLE.begin()) {
      Serial.println("* Starting BLE module failed!");
      while (1);
    }
    BLE.setLocalName("Arduino XYZT (Central)");
    BLE.setAdvertisedService(SensorService);
    SensorService.addCharacteristic(PChar);
    BLE.addService(SensorService);

}
void loop() {
  ei_printf("Edge Impulse standalone inferencing (Arduino)\n");

  if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
    ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
    EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
    delay(1000);
    return;
  }
  ei_impulse_result_t result = { 0 };

  // The features are stored into flash, and we don't want to load everything into RAM
  signal_t features_signal;    
  features_signal.total_length = sizeof(features) / sizeof(features[0]);
  features_signal.get_data = &raw_feature_get_data;

  // Invoke the impulse
  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
  if (res != EI_IMPULSE_OK) {
    ei_printf("ERR: Failed to run classifier (%d)\n", res);
    return;
  }

  // Print inference return code
  ei_printf("run_classifier returned: %d\r\n", res);
  print_inference_result(result);
}

void print_inference_result(ei_impulse_result_t result) {
  int32_t x = 0;
  BLE.scan();
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

  if (peripheral.localName() == "Arduino XYZT (peripheral)") {
    // Stop scanning
    BLE.stopScan();

    if (peripheral.connect()) {
    Serial.println("Connected");
    }
    else {
      Serial.println("Failed to connect!");
      return;
    }       
    Serial.println("Discovering attributes ...");
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    }
    else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
      return;
    }
      // Read and print device name of peripheral
      Serial.println();
      Serial.print("Device name: ");
      Serial.println(peripheral.deviceName());
      Serial.print("Appearance: 0x");
      Serial.println(peripheral.appearance(), HEX);
      Serial.println();
    }
      while (peripheral.connect()) {
      BLEService service = peripheral.service("1101");
      BLECharacteristic Xchar = service.characteristic("2101");
      if (Xchar.canRead()) {
        // Read the characteristic value
        Xchar.readValue(&x, 4);
        
        float xFloat = (float)x;  // Cast x to float
        xFloat = xFloat / 100.0;  // Perform division
        
        Serial.println(String("Magnitude: ") + x);
        ei_printf("Predictions:\r\n");
        for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
          ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
          ei_printf("%.5f\r\n", result.classification[i].value);
          if (strcmp(ei_classifier_inferencing_categories[i], "fall") == 0) {
            if(xFloat > result.classification[i].value){ 
              Serial.println(xFloat);
              Serial.println(result.classification[i].value);
                          
              ei_printf("Fall");
              Serial.println(" is Detected! Emer n gency call proceeded");
              float fd = 1;
            }                        
            else {
              ei_printf("Normal");
              Serial.println(", No Fall events detected");
              float fd = 0;
            }
          }
        }
        int pBle = fd*pow(10, 2);
        PChar.writeValue(pBle);
      }
      delay(20);
    }
  }
  // Print how long it took to perform inference
  ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
              result.timing.dsp,
              result.timing.classification,
              result.timing.anomaly);

  // Print the prediction results 
}
