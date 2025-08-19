#include <WiFi.h>
#include <Wire.h>
#include <esp32_acc_inferencing.h>
#include <MPU6050.h>

MPU6050 mpu;

// Wi-Fi credentials
const char *ssid = "VNRVJIET_E";
const char *password = "vnrvjiet@123";

// Server IP and port (replace with your Python receiver IP and port)
const char *serverIP = "10.100.2.215";  // Replace with your laptop's IP address
const int serverPort = 12345;            // Port to send data to

WiFiClient client;

// Buffer to hold the 39 MPU6050 readings (13 samples x 3 axes)
float features[39];

// Function to read accelerometer data from MPU6050 and fill the features array
void read_mpu_data(int index) {
  int16_t ax_raw, ay_raw, az_raw;
  mpu.getAcceleration(&ax_raw, &ay_raw, &az_raw);

  // Debug: Print raw MPU6050 values
  Serial.print("MPU6050 Raw: ");
  Serial.print(ax_raw);
  Serial.print(", ");
  Serial.print(ay_raw);
  Serial.print(", ");
  Serial.println(az_raw);

  // Convert raw values to 'g' units
  features[index * 3] = ax_raw / 16384.0;
  features[index * 3 + 1] = ay_raw / 16384.0;
  features[index * 3 + 2] = az_raw / 16384.0;
}

void collect_data_for_duration() {
  unsigned long start_time = millis();
  int sample_index = 0;

  Serial.println("Collecting data...");
  while (millis() - start_time < 3000 && sample_index < 13) {
    read_mpu_data(sample_index);
    sample_index++;
    delay(20);
  }

  // Debug: Print collected feature values
  Serial.println("Collected Features:");
  for (int i = 0; i < 39; i++) {
    Serial.print(features[i], 4);
    Serial.print(" ");
  }
  Serial.println();
}

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  if (offset + length > 39) return -1;
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");

  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1)
      ;
  } else {
    Serial.println("MPU6050 connected successfully!");
  }
}

void loop() {
  collect_data_for_duration();

  // Prepare signal for Edge Impulse
  signal_t features_signal;
  features_signal.total_length = sizeof(features) / sizeof(features[0]);
  features_signal.get_data = &raw_feature_get_data;

  ei_impulse_result_t result = { 0 };

  // Run Edge Impulse classification
  Serial.println("Running classifier...");
  EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
  Serial.println("Classifier run completed.");

  if (res != EI_IMPULSE_OK) {
    Serial.println("Error in classification!");
  } else {
    Serial.println("Classification successful.");

    float max_confidence = 0;
    String detected_gesture = "";

    Serial.println("Classification Results:");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
      Serial.print(ei_classifier_inferencing_categories[i]);
      Serial.print(": ");
      Serial.println(result.classification[i].value, 4);

      if (result.classification[i].value > max_confidence) {
        max_confidence = result.classification[i].value;
        detected_gesture = ei_classifier_inferencing_categories[i];
      }
    }

    // Check confidence level and send gesture to server if above threshold
    if (max_confidence > 0.5) {
      Serial.print("Detected Gesture: ");
      Serial.print(detected_gesture);
      Serial.print(" with confidence ");
      Serial.println(max_confidence);

      String message = "";
      if (detected_gesture == "left_right") message = "L";
      else if (detected_gesture == "up_down") message = "U";
      else if (detected_gesture == "tap") message = "T";

      Serial.print("Sending gesture: ");
      Serial.println(message);

      if (client.connect(serverIP, serverPort)) {
        Serial.println("Connected to server.");
        client.print(message);  // Send gesture code
        client.stop();
        Serial.println("Message sent.");
      } else {
        Serial.println("Failed to connect to server.");
      }
    } else {
      Serial.println("No gesture detected with high confidence.");
    }
  }

  delay(3000);
}