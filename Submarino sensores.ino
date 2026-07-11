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
//const float ACCEL_THRESHOLD_MS2 = 0.15;
//const float EVENT_THRESHOLD_MS2 = 0.5;
float diferencia_ax_umbral = 0.0;
float current_vx = 0.0;
float current_vy = 0.0;
float current_vz = 0.0;
unsigned long last_time;
float delta_t_sec = 0.0;
float velocidadZ = 0;
unsigned long tiempoAnterior = 0;
// --- Parámetros más conservadores para ambiente submarino ---
const float ACCEL_THRESHOLD_MS2 = 1.0;  // ignora ruido pequeño
const float EVENT_THRESHOLD_MS2 = 1.50;  // requiere más aceleración para declarar evento
const float MAX_ACCEL_JUMP = 0.5;        // limita saltos bruscos

// Filtro pasa-bajas: alpha pequeño = más suavizado (responde más lento, ideal bajo agua)
const float ALPHA_LP = 0.03;  // (antes 0.10)

// Kalman: menos confianza en proceso, más en medición de profundidad
const float Q_pos = 1e-8;         // proceso posición (más pequeño = más suave)5
const float Q_vel = 1e-8;         // proceso velocidad4
const float R_vel_meas = 1e-3;    // medición velocidad (cuando aplicamos vel=0)
const float R_depth_meas = 1e-2;  // ruido de la medición de profundidad (MS5803), ajustar según pruebas

// Variables de estado (si no las tienes ya)
float pos_x = 0.0;
float pos_z = 0.0;

// Covarianzas Kalman X
float P_x_x = 0.01, P_x_v = 0.0, P_v_x = 0.0, P_v_v = 0.01;
// Covarianzas Kalman Z
float Pz_x = 0.01, Pz_v = 0.0, Pv_z = 0.0, Pv_vz = 0.01;

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
  if (!mpu.testConnection()) {

  } else {
    devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
      mpu.setDMPEnabled(true);
      dmpReady = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      Serial.print(F("Fallo la inicializacion DMP. Error: "));
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
  String mensaje = String(profundidad) + "," + conectado + "," + String(velocidad) + ","
                   + String(tem) + "," + String(presure) + "," + String(disradinferior) + "," + String(distotalradf)
                   + "," + String(movimiento);
  //profundidad
  rs485.println(mensaje);
  Serial.println(mensaje);
  delay(50);
}

// === Leer sensor DYP ultrasonico ===
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
void lectura_acelerometro() {
  // === SI DMP NO ESTÁ LISTO ===
  if (!dmpReady) {
    mpu.getMotion6(NULL, NULL, NULL, NULL, NULL, NULL);
    movimiento = (mpu.getRotationX() > 0) ? 6 : 7;
    diferencia_ax_umbral = 0.0;
    return;
  }
  // === FIFO VALIDACIÓN ===
  if (mpu.getFIFOCount() < packetSize) return;
  if (mpu.getFIFOCount() >= 1024) {
    mpu.resetFIFO();
    return;
  }

  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // === DELTA T ===
  unsigned long now = micros();
  delta_t_sec = (float)(now - last_time) / 1000000.0;
  last_time = now;

  // === ORIENTACIÓN + ACEL LINEAL ===
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetAccel(&aav, fifoBuffer);
  mpu.dmpGetLinearAccel(&aaReal, &aav, &gravity);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  float raw_ax = (float)aaReal.x * RAW_TO_MS2;
  float raw_az = (float)aaReal.z * RAW_TO_MS2;

  // === PASA-BAJAS (submarino) ===
  static float a_x_f = 0, a_z_f = 0;
  a_x_f = a_x_f + ALPHA_LP * (raw_ax - a_x_f);
  a_z_f = a_z_f + ALPHA_LP * (raw_az - a_z_f);

  float a_x = a_x_f;
  float a_z = a_z_f;

  // === ANTI-PICOS ===
  static float last_ax = 0, last_az = 0;
  if (abs(a_x - last_ax) > MAX_ACCEL_JUMP) a_x = last_ax + ((a_x > last_ax) ? MAX_ACCEL_JUMP : -MAX_ACCEL_JUMP);
  if (abs(a_z - last_az) > MAX_ACCEL_JUMP) a_z = last_az + ((a_z > last_az) ? MAX_ACCEL_JUMP : -MAX_ACCEL_JUMP);
  last_ax = a_x;
  last_az = a_z;

  // UMBRAL DE RUIDO
  if (abs(a_x) < ACCEL_THRESHOLD_MS2) a_x = 0;
  if (abs(a_z) < ACCEL_THRESHOLD_MS2) a_z = 0;
  float pos_pred_x = pos_x + current_vx * delta_t_sec + 0.5f * a_x * delta_t_sec * delta_t_sec;
  float vel_pred_x = current_vx + a_x * delta_t_sec;

  // Covarianza
  float dt = delta_t_sec;
  float dt2 = dt * dt;

  float P00 = P_x_x + dt * (P_x_v + P_v_x) + dt2 * P_v_v + Q_pos;
  float P01 = P_x_v + dt * P_v_v;
  float P10 = P_v_x + dt * P_v_v;
  float P11 = P_v_v + Q_vel;

  P_x_x = P00;
  P_x_v = P01;
  P_v_x = P10;
  P_v_v = P11;

  // *** Corrección "velocidad = 0" cuando no hay aceleración ***
  if (a_x == 0) {
    float S = P_x_v + R_vel_meas;
    float K = P_x_v / S;
    float y = (0 - vel_pred_x);

    vel_pred_x += K * y;
    pos_pred_x += 0.0;  // no corrige posición aquí

    float P00_new = P_x_x - K * P_x_v;
    float P01_new = P_x_v - K * P_x_v;

    P_x_x = P00_new;
    P_x_v = P01_new;
    P_v_x = P01_new;
  }

  pos_x = pos_pred_x;
  current_vx = vel_pred_x;
  float pos_pred_z = pos_z + current_vz * dt + 0.5f * a_z * dt * dt;
  float vel_pred_z = current_vz + a_z * dt;

  float Pz00 = Pz_x + dt * (Pz_v + Pv_z) + dt2 * Pv_vz + Q_pos;
  float Pz01 = Pz_v + dt * Pv_vz;
  float Pz10 = Pv_z + dt * Pv_vz;
  float Pz11 = Pv_vz + Q_vel;

  Pz_x = Pz00;
  Pz_v = Pz01;
  Pv_z = Pz10;
  Pv_vz = Pz11;
  bool depth_valid = (profundidad_m >= 0.0 && profundidad_m < 1000.0);
  if (depth_valid) {

    float z_meas = profundidad_m;  // metros reales

    float S = Pz_x + R_depth_meas;
    float K_pos = Pz_x / S;
    float K_vel = Pv_z / S;

    float y = z_meas - pos_pred_z;

    pos_pred_z += K_pos * y;
    vel_pred_z += K_vel * y;

    float newP00 = Pz_x - K_pos * Pz_x;
    float newP01 = Pz_v - K_pos * Pz_v;
    float newP10 = Pv_z - K_vel * Pz_x;
    float newP11 = Pv_vz - K_vel * Pz_v;

    Pz_x = newP00;
    Pz_v = newP01;
    Pv_z = newP10;
    Pv_vz = newP11;
  }

  pos_z = pos_pred_z;
  current_vz = vel_pred_z;
  if (abs(a_x) > EVENT_THRESHOLD_MS2) {
    diferencia_ax_umbral = abs(a_x) - EVENT_THRESHOLD_MS2;
  } else {
    diferencia_ax_umbral = 0.0;
  }

  velocidad = diferencia_ax_umbral;

  if (a_x > EVENT_THRESHOLD_MS2) {
    movimiento = 6;  // Avance
  } else if (a_x < -EVENT_THRESHOLD_MS2) {
    movimiento = 7;  // Retroceso
  } else if (a_z > EVENT_THRESHOLD_MS2) {
    movimiento = 5;  // Subiendo
  } else if (a_z < -EVENT_THRESHOLD_MS2) {
    movimiento = 4;  // Bajando
  } else {
    movimiento = 1;  // Reposo
  }
}
