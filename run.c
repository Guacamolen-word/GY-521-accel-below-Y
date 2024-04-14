#include "Wire.h" // This library allows you to communicate with I2C devices.

const int MPU1 = 0x68; // I2C address of the first MPU-6050
const int MPU2 = 0x69; // I2C address of the second MPU-6050

int16_t accelerometer_x1, accelerometer_y1, accelerometer_z1; // variables for first accelerometer raw data
int16_t accelerometer_x2, accelerometer_y2, accelerometer_z2; // variables for second accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
const int outputPin1 = 7; // Pin for first impulse
const int outputPin2 = 8; // Pin for second impulse

char tmp_str[7]; // temporary variable used in convert function

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU1); // Begins a transmission to the first MPU-6050
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU2); // Begins a transmission to the second MPU-6050
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
}

void loop() {
  readAccelerometerData(MPU1, accelerometer_x1, accelerometer_y1, accelerometer_z1); // Read data from the first accelerometer
  readAccelerometerData(MPU2, accelerometer_x2, accelerometer_y2, accelerometer_z2); // Read data from the second accelerometer

  // print out data for first accelerometer
  Serial.print("aX1 = "); Serial.print(convert_int16_to_str(accelerometer_x1));
  Serial.print(" | aY1 = "); Serial.print(convert_int16_to_str(accelerometer_y1));
  Serial.print(" | aZ1 = "); Serial.print(convert_int16_to_str(accelerometer_z1));

  // print out data for second accelerometer
  Serial.print(" | aX2 = "); Serial.print(convert_int16_to_str(accelerometer_x2));
  Serial.print(" | aY2 = "); Serial.print(convert_int16_to_str(accelerometer_y2));
  Serial.print(" | aZ2 = "); Serial.print(convert_int16_to_str(accelerometer_z2));
  Serial.println();

  // Generate random impulses
  if (random(0, 100) < 20) { // 20% chance of generating impulse for first pin
    digitalWrite(outputPin1, LOW);
    Serial.println("Impulse 1 Generated");
  }

  if (random(0, 100) < 20) { // 20% chance of generating impulse for second pin
    digitalWrite(outputPin2, LOW);
    Serial.println("Impulse 2 Generated");
  }

  // Deactivate pins based on accelerometer readings
  if (accelerometer_y1 > 5000 || accelerometer_y1 < -5000) { // Deactivate first pin if condition met for first accelerometer
    digitalWrite(outputPin1, HIGH);
    Serial.println("Pin 1 Deactivated");
  }

  if (accelerometer_y2 > 5000 || accelerometer_y2 < -5000) { // Deactivate second pin if condition met for second accelerometer
    digitalWrite(outputPin2, HIGH);
    Serial.println("Pin 2 Deactivated");
  }

  // delay
  delay(1000);
}

void readAccelerometerData(int address, int16_t& accel_x, int16_t& accel_y, int16_t& accel_z) {
  Wire.beginTransmission(address);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(address, 7 * 2, true); // request a total of 7*2=14 registers

  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();
  temperature = Wire.read() << 8 | Wire.read();
  accel_x = Wire.read() << 8 | Wire.read();
  accel_y = Wire.read() << 8 | Wire.read();
  accel_z = Wire.read() << 8 | Wire.read();
}
