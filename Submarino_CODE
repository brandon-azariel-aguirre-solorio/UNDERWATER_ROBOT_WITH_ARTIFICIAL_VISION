#include <SoftwareSerial.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <SparkFun_MS5803_I2C.h>

// === Sensores ===
MPU6050 mpu;
MS5803 sensor(ADDRESS_HIGH);

// === Pines ===
#define RX_RS485 5
#define TX_RS485 4
#define COM 0x55

// Sensores DYP-L041MTW
SoftwareSerial Sonar1(9, 8);    // RX, TX
SoftwareSerial Sonar2(11, 10);  // RX, TX
unsigned char buffer1[4] = { 0 };
unsigned char buffer2[4] = { 0 };
uint16_t Distance1_mm = 0;
uint16_t Distance2_mm = 0;
float Distance1_cm = 0.0;
float Distance2_cm = 0.0;

// RS485
SoftwareSerial rs485(RX_RS485, TX_RS485);

// Ultrasonido adicional
int SItrigPin = 2, SIechoPin = 3;
long duracionSI = 0;
float distanciaSI = 0;

// === Variables generales ===
double temperature_c;
double pressure_abs;
double profundidad_m;
double presion_superficie_mbar;
int lampara = 0, Emotores = 1, Emodooperacion = 2;
float profundidad = 1;
float velocidad = 1;
float presure = 0;
int disradsuper1 = 0, disradsuper2 = 0, disradinferior = 0;
int distotalradf = 0;
int tem = 0, movimiento = 1;
String conectado = "conectado";

// === MPU variables ===
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
VectorInt16 aav, aaReal;
float ypr[3];
const float RAW_TO_MS2 = 9.81 / 16384.0;
const float ACCEL_THRESHOLD_MS2 = 0.15;
const float EVENT_THRESHOLD_MS2 = 0.5;
float diferencia_ax_umbral = 0.0;
float current_vx = 0.0;
float current_vy = 0.0;
float current_vz = 0.0;
unsigned long last_time;
float delta_t_sec = 0.0;
float velocidadZ = 0;
unsigned long tiempoAnterior = 0;
float a_x;
float a_y;
float a_z;

// === Setup ===
void setup() {
  Serial.begin(115200);
  rs485.begin(115200);
  Sonar1.begin(115200);
  Sonar2.begin(115200);
  Wire.setSDA(0);  // Corresponde al pin GP14 en el Raspberry Pi Pico
  Wire.setSCL(1);  // Corresponde al pin GP15 en el Raspberry Pi Pico
  Wire.begin();

  sensor.reset();
  sensor.begin();
  presion_superficie_mbar = sensor.getPressure(ADC_4096);

  mpu.initialize();
  if (mpu.testConnection()) {
    devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
      mpu.setDMPEnabled(true);
      dmpReady = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      Serial.print("Error DMP: ");
      Serial.println(devStatus);
    }
  }

  pinMode(SItrigPin, OUTPUT);
  pinMode(SIechoPin, INPUT);
  tiempoAnterior = micros();
  last_time = micros();
}

// === Loop ===
void loop() {
  sensor_presion();
  lectura_acelerometro();

  // Leer sensores RS485
  leerSensor(Sonar1, buffer1, Distance1_mm);
  leerSensor(Sonar2, buffer2, Distance2_mm);
  Distance1_cm = Distance1_mm / 10.0;
  Distance2_cm = Distance2_mm / 10.0;

  // Sensor ultrasónico adicional
  sonar3();
  disradsuper1 = Distance1_cm;
  disradsuper2 = Distance2_cm;
  disradinferior = distanciaSI;

  // Determinar distancia total
  if (disradsuper1 <= disradsuper2 || disradsuper2 == 0)
    distotalradf = disradsuper1;
  else
    distotalradf = disradsuper2;
  String mensaje = String(profundidad) + "," + conectado + "," + String(velocidad) + "," + String(tem) + "," + String(presure) + "," + String(disradinferior) + "," + String(distotalradf) + "," + String(movimiento);
  //profundidad
  rs485.println(mensaje);
  //Serial.println(mensaje);
  delay(50);
}

// === Leer sensor DYP RS485 ===
void leerSensor(SoftwareSerial &sensor, unsigned char *buffer, uint16_t &distancia) {
  sensor.listen();
  sensor.write(COM);
  delay(100);
  if (sensor.available() > 0) {
    delay(4);
    if (sensor.read() == 0xFF) {
      buffer[0] = 0xFF;
      for (int i = 1; i < 4; i++) buffer[i] = sensor.read();
      uint8_t CS = buffer[0] + buffer[1] + buffer[2];
      if (buffer[3] == CS) distancia = (buffer[1] << 8) + buffer[2];
    }
  }
}

// === Sensor ultrasónico ===
void sonar3() {
  if (profundidad > 0) {
    digitalWrite(SItrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(SItrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(SItrigPin, LOW);
    duracionSI = pulseIn(SIechoPin, HIGH);
    distanciaSI = duracionSI * 0.1500 / 2;
  }
  {
    distanciaSI = 0;
  }
}

// === Sensor de presión ===
void sensor_presion() {
  temperature_c = sensor.getTemperature(CELSIUS, ADC_512);
  pressure_abs = sensor.getPressure(ADC_4096);
  double pressure_pa = pressure_abs * 100.0;
  double presion_superficie_pa = presion_superficie_mbar * 100.0;
  double densidad_agua = 1000.0;
  double gravedad = 9.81;
  profundidad_m = (pressure_pa - presion_superficie_pa) / (densidad_agua * gravedad);
  tem = temperature_c;
  presure = pressure_abs;
  profundidad = max(profundidad_m, 0.0);
}

// === Lectura de acelerómetro ===
void lectura_acelerometro() {
  if (!dmpReady) {
    mpu.getMotion6(NULL, NULL, NULL, NULL, NULL, NULL);
    movimiento = (mpu.getRotationX() > 0) ? 6 : 7;
    diferencia_ax_umbral = 0.0;  // Resetear si el DMP no está listo
    return;
  }
  if (mpu.getFIFOCount() < packetSize) return;
  if (mpu.getFIFOCount() >= 1024) {
    mpu.resetFIFO();
    return;
  }

  mpu.getFIFOBytes(fifoBuffer, packetSize);
  // 1. CALCULAR DELTA T
  unsigned long now = micros();
  delta_t_sec = (float)(now - last_time) / 1000000.0;
  last_time = now;

  // 2. OBTENER ACELERACIÓN LINEAL Y ORIENTACIÓN
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetAccel(&aav, fifoBuffer);
  mpu.dmpGetLinearAccel(&aaReal, &aav, &gravity);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  a_x = (float)aaReal.x * RAW_TO_MS2;
  a_y = (float)aaReal.y * RAW_TO_MS2;
  a_z = (float)aaReal.z * RAW_TO_MS2;

  if (abs(a_x) < ACCEL_THRESHOLD_MS2) a_x = 0;
  if (abs(a_z) < ACCEL_THRESHOLD_MS2) a_z = 0;

  current_vx += a_x * delta_t_sec;
  current_vz += a_z * delta_t_sec;
  velocidad = diferencia_ax_umbral;
  if (abs(a_x) > EVENT_THRESHOLD_MS2) {
    diferencia_ax_umbral = abs(a_x) - EVENT_THRESHOLD_MS2;
  } else {
    diferencia_ax_umbral = 0.0;
  }

  // 4. LÓGICA DE MOVIMIENTO (X)
  if (a_x > EVENT_THRESHOLD_MS2) {
    movimiento = 6;  //Avance
  } else if (a_x < -EVENT_THRESHOLD_MS2) {
    movimiento = 7;  //Retroceso
  }
  // Lógica de ASCENSO/DESCENSO (Eje Z) - Se prioriza sobre X si ambas ocurren
  else if (a_z > EVENT_THRESHOLD_MS2) {
    movimiento = 5;  // Elevando
  } else if (a_z < -EVENT_THRESHOLD_MS2) {
    movimiento = 4;  // Bajando
  } else {
    movimiento = 1;  // Reposo
  }                  // Reposo
}
