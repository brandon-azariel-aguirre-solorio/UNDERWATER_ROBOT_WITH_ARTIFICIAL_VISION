#include <SoftwareSerial.h>
#define RX_RS485 9 // Pin de recepción RS485
#define TX_RS485 8  // Pin de transmisión RS485 (no se usará en este caso)
#define play 11
#define luz 10  // Pin de transmisión RS485 (no se usará en este caso)
String mensaje;
SoftwareSerial rs485(RX_RS485, TX_RS485);  // RX, TX   // RX, TX
void setup() {
  Serial.begin(115200);
  rs485.begin(115200);   // Comunicación RS485
  Serial.println("Receptor listo...");
  pinMode(play, OUTPUT);
  pinMode(luz, OUTPUT);

}

void loop() {
  if (Serial.available()) {
    mensaje = Serial.readStringUntil('\n');
  }
  if (mensaje == "MM") {
    digitalWrite(play, HIGH);
  }
  else if (mensaje == "MA") {
    digitalWrite(play, LOW);
  }
  else if (mensaje == "LON") {
    digitalWrite(luz, HIGH);
  }
  else if (mensaje == "LOFF") {
    digitalWrite(luz, LOW);
  }
  if (rs485.available()) {
    String mensajers = rs485.readStringUntil('\n');
    Serial.println(mensajers);  // Mostrar en el monitor serial
  }
}
