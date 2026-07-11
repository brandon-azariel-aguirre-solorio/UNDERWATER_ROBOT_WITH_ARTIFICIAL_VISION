#include <PS2X_lib.h>
#include <printf.h>
#include <Servo.h>  //librería necesaria para las ESC de control

Servo myESC_M1, myESC_M2, myESC_M3, myESC_M4;  // Crea un objeto Servo
int C_V_M_1 = 5, C_V_M_2 = 6;                  //subir y bajar  Control_Velocidad_Motor
int C_V_M_3 = 3, C_V_M_4 = 9;                  //laterales Control_Velocidad_Motor
long tiempo;
int distancia;
int espera = 20;
int V_M_1 = 1000, V_M_2 = 1000, V_M_3 = 1000, V_M_4 = 1000;  //1000 apagado 2000 Maxima velocidad // Velocidad_Motor_#
unsigned long last_Time = 0;
PS2X ps2x;
int error = 0;
byte type = 0;
byte vibrate = 0;
// RELEVADORES
int M_s_1_R1 = 19, M_s_1_R2 = 4, M_s_2_R1 = 7, M_s_2_R2 = 8;     //Motor_Superior_#_Rele#
int M_l_1_R1 = 14, M_l_1_R2 = 15, M_l_2_R1 = 16, M_l_2_R2 = 17;  //Motor_Lateral_#_Rele#

// Submarino
int via = 2000;
int bateria, luces = 18, luz = 10;
String conectado = "conectado";
#define DEBUG(a) Serial.println(a);
String data = "", codigo = "";
int etadoras = 20, estar = 0, pincon;
int etadoluz = 21, luzr = 0, pinconluz;
void setup() {
  Inicializacion_de_pines();
  error = ps2x.config_gamepad(13, 11, 2, 12, false, true);
  //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  Serial.begin(9600);
  if (error == 0) {
    Serial.println("Found Controller, configured successful");
  }
  type = ps2x.readType();
  Serial.begin(115200);
  myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
  myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
  type = 1;
  pinMode(etadoras, INPUT);
}

void loop() {
  //estar = digitalRead(etadoras);
  estar = analogRead(etadoras);
  pincon = map(estar, 0, 1024, 0, 100);
  luzr = analogRead(etadoluz);
  pinconluz = map(luzr, 0, 1024, 0, 100);
  if (pincon >= 20) {
    V_M_1 = 1000, V_M_2 = 1000, V_M_3 = 1000, V_M_4 = 1000;
    myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
    myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
    type = 0;
  } else {
    type = 1;
  }
  if (Serial.available()) {
    data = Serial.readStringUntil('\n');
    DEBUG(data);
    codigo = data.substring(0, 6);  //extraer dato
  }
  if (codigo == "MOFF") {
    via = 2000;
  }
  if (codigo == "MON") {
    via = 1000;
  }
  if (type == 1) {
    ps2x.read_gamepad(false, vibrate);  //Lectura del estado
    //Lectura de las palancas analógicas
    if (ps2x.Analog(PSS_LY) >= 120 && ps2x.Analog(PSS_LY) <= 140 && ps2x.Analog(PSS_LX) >= 120 && ps2x.Analog(PSS_LX) <= 140 && ps2x.Analog(PSS_RY) >= 120 && ps2x.Analog(PSS_RY) <= 140 && ps2x.Analog(PSS_RX) >= 120 && ps2x.Analog(PSS_RX) <= 140) {
      //Neutro
      V_M_1 = 1000;
      V_M_2 = 1000;
      V_M_3 = 1000;
      V_M_4 = 1000;
      myESC_M1.writeMicroseconds(V_M_1);
      myESC_M2.writeMicroseconds(V_M_2);
      myESC_M3.writeMicroseconds(V_M_3);
      myESC_M4.writeMicroseconds(V_M_4);
      //Lectura de los botones de dirección
      if (ps2x.Button(PSB_PAD_UP)) {  //Arriba
        Flotar();
        V_M_1 = via;
        V_M_2 = via;
        myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
        myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
        delay(espera);

      } else if (ps2x.Button(PSB_PAD_RIGHT)) {
        V_M_3 = via;
        myESC_M3.writeMicroseconds(V_M_3);  //señal PWM enviada a la ESC
        delay(espera);
      } else if (ps2x.Button(PSB_PAD_LEFT)) {
        V_M_4 = via;
        myESC_M4.writeMicroseconds(V_M_4);  //señal PWM enviada a la ESC
        delay(espera);
      } else if (ps2x.Button(PSB_PAD_DOWN)) {
        //abajo
        
        Sumergirse();
        V_M_1 = via;
        V_M_2 = via;
        myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
        myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
        delay(espera);
      }
      //Lectura del estado de botones
      if (ps2x.NewButtonState()) {
        if (ps2x.Button(PSB_RED)) {
        } else if (ps2x.Button(PSB_GREEN)) {
          digitalWrite(luces, LOW);
          analogWrite(luz, 255);
        } else if (ps2x.Button(PSB_PINK)) {
        } else if (ps2x.Button(PSB_BLUE)) {
          digitalWrite(luces, HIGH);
          analogWrite(luz, 255);
        } else if (ps2x.Button(PSB_L3)) {
          //ERAR fondoACEL
        } else if (ps2x.Button(PSB_R3)) {
          //retroceder
        } else if (ps2x.Button(PSB_L2)) {
          //LUZ PRENDER

        } else if (ps2x.Button(PSB_R2)) {
          //LUZ APAGAR
        }
      }
    }
    //palanca izquierda
    else if (ps2x.Analog(PSS_LY) >= 140) {
      //izquierda ATRAS
      Sumergirse();
      V_M_1 = map(ps2x.Analog(PSS_LY), 0, 255, 1000, 2000);
      V_M_2 = map(ps2x.Analog(PSS_LY), 0, 255, 1000, 2000);
      myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
      myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_LY) <= 120) {
      //DELANTE izquierda
      Flotar();
      V_M_1 = map(ps2x.Analog(PSS_LY), 0, 255, 2000, 1000);
      V_M_2 = map(ps2x.Analog(PSS_LY), 0, 255, 2000, 1000);
      myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
      myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_LX) >= 140) {
      //derecha left
      SubirM1();
      V_M_1 = map(ps2x.Analog(PSS_LY), 0, 255, 1000, 2000);
      myESC_M1.writeMicroseconds(V_M_1);  //señal PWM enviada a la ESC
    } else if (ps2x.Analog(PSS_LX) <= 120) {
      SubirM2();
      V_M_2 = map(ps2x.Analog(PSS_LY), 0, 255, 2000, 1000);
      myESC_M2.writeMicroseconds(V_M_2);  //señal PWM enviada a la ESC
      delay(espera);
    }
    //palanca derecha
    else if (ps2x.Analog(PSS_RY) >= 140) {
      //derecha ATRAS
      Avanzar();
      V_M_3 = map(ps2x.Analog(PSS_RY), 0, 255, 1000, 2000);
      V_M_4 = map(ps2x.Analog(PSS_RY), 0, 255, 1000, 2000);
      myESC_M3.writeMicroseconds(V_M_3);  //señal PWM enviada a la ESC
      myESC_M4.writeMicroseconds(V_M_4);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RY) <= 120) {
      //DELANTE derecha
      Retroceder();
      V_M_3 = map(ps2x.Analog(PSS_RY), 0, 255, 2000, 1000);
      V_M_4 = map(ps2x.Analog(PSS_RY), 0, 255, 2000, 1000);
      myESC_M3.writeMicroseconds(V_M_3);  //señal PWM enviada a la ESC
      myESC_M4.writeMicroseconds(V_M_4);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RX) >= 140) {
      //derecha rigt
      derecha();
      V_M_3 = map(ps2x.Analog(PSS_RX), 0, 255, 1000, 2000);
      V_M_4 = map(ps2x.Analog(PSS_RX), 0, 255, 1000, 2000);
      myESC_M3.writeMicroseconds(V_M_3);  //señal PWM enviada a la ESC
      myESC_M4.writeMicroseconds(V_M_4);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RX) <= 120) {
      //rigt izquierda
      izquierda();
      V_M_3 = map(ps2x.Analog(PSS_RX), 0, 255, 2000, 1000);
      V_M_4 = map(ps2x.Analog(PSS_RX), 0, 255, 2000, 1000);
      myESC_M3.writeMicroseconds(V_M_3);  //señal PWM enviada a la ESC
      myESC_M4.writeMicroseconds(V_M_4);  //señal PWM enviada a la ESC
      delay(espera);
    }
  } else {
    return;
  }

  delay(100);
}
void Inicializacion_de_pines() {
  pinMode(luces, OUTPUT);
  pinMode(luz, OUTPUT);
  pinMode(C_V_M_1, OUTPUT);
  pinMode(C_V_M_2, OUTPUT);
  pinMode(C_V_M_3, OUTPUT);
  pinMode(C_V_M_4, OUTPUT);  //Control_Velocidad_Motor
  pinMode(M_s_1_R1, OUTPUT);
  pinMode(M_s_1_R2, OUTPUT);  //Motor_Superior_#_Rele#
  pinMode(M_s_2_R1, OUTPUT);
  pinMode(M_s_2_R2, OUTPUT);
  pinMode(M_l_1_R1, OUTPUT);  //Motor_Lateral_#_Rele#
  pinMode(M_l_1_R2, OUTPUT);
  pinMode(M_l_2_R1, OUTPUT);
  pinMode(M_l_2_R2, OUTPUT);
  myESC_M1.attach(C_V_M_1);  // pin de control de ESC
  myESC_M2.attach(C_V_M_2);
  myESC_M3.attach(C_V_M_3);
  myESC_M4.attach(C_V_M_4);
}
void Sumergirse() {
  digitalWrite(M_s_1_R1, LOW);
  digitalWrite(M_s_1_R2, LOW);
  digitalWrite(M_s_2_R1, HIGH);
  digitalWrite(M_s_2_R2, HIGH);
}
void Flotar() {
  digitalWrite(M_s_1_R1, HIGH);
  digitalWrite(M_s_1_R2, HIGH);
  digitalWrite(M_s_2_R1, LOW);
  digitalWrite(M_s_2_R2, LOW);
}
void SubirM1() {
  digitalWrite(M_s_1_R1, HIGH);
  digitalWrite(M_s_1_R2, HIGH);
}
void BajarM1() {
  digitalWrite(M_s_1_R1, LOW);
  digitalWrite(M_s_1_R2, LOW);
}
void SubirM2() {
  digitalWrite(M_s_2_R1, HIGH);
  digitalWrite(M_s_2_R2, HIGH);
}
void BajarM2() {
  digitalWrite(M_s_2_R1, LOW);
  digitalWrite(M_s_2_R2, LOW);
}
void izquierda() {
  digitalWrite(M_l_1_R1, LOW);
  digitalWrite(M_l_1_R2, LOW);
  digitalWrite(M_l_2_R1, HIGH);
  digitalWrite(M_l_2_R2, HIGH);
}
void derecha() {
  digitalWrite(M_l_2_R1, LOW);
  digitalWrite(M_l_2_R2, LOW);
  digitalWrite(M_l_1_R1, HIGH);
  digitalWrite(M_l_1_R2, HIGH);
}
void Avanzar() {
  digitalWrite(M_l_1_R1, LOW);
  digitalWrite(M_l_1_R2, LOW);
  digitalWrite(M_l_2_R1, LOW);
  digitalWrite(M_l_2_R2, LOW);
}
void Retroceder() {
  digitalWrite(M_l_1_R1, HIGH);
  digitalWrite(M_l_1_R2, HIGH);
  digitalWrite(M_l_2_R1, HIGH);
  digitalWrite(M_l_2_R2, HIGH);
}
