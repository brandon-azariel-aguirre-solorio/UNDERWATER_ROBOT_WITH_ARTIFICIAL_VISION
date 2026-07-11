#include <PS2X_lib.h>
#include <printf.h>
#include <Servo.h>  //librería necesaria para las ESC de control

Servo myESC;  // Crea un objeto Servo
int Control_Velocidad_Motor_1 = 5;
long tiempo;
int distancia;
int espera = 20;
int Velocidad_Motor_1 = 1000;
unsigned long last_Time = 0;
PS2X ps2x;
int error = 0;
byte type = 0;
byte vibrate = 0;
//8 RELEVADORES
int M_superior_1_R1 = 6, M_superior_1_R2 = 7, M_superior_2_R1, M_superior_2_R2;
int M_lateral_1_R1, M_lateral_1_R2, M_lateral_2_R1, M_lateral_2_R2;
void setup() {
  pinMode(Control_Velocidad_Motor_1, OUTPUT);
  pinMode(M_superior_1_R2, OUTPUT);
  pinMode(M_superior_1_R1, OUTPUT);
  error = ps2x.config_gamepad(13, 11, 10, 12, false, true);
  myESC.attach(Control_Velocidad_Motor_1);  // pin de control de ESC, pin 9
  //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  prenderM1();
  Serial.begin(9600);
  if (error == 0) {
    Serial.println("Found Controller, configured successful");
  }

  type = ps2x.readType();
}

void loop() {
  if (type == 1) {
    ps2x.read_gamepad(false, vibrate);  //Lectura del estado
    //Lectura de las palancas analógicas

    if (ps2x.Analog(PSS_LY) >= 120 && ps2x.Analog(PSS_LY) <= 140 && ps2x.Analog(PSS_LX) >= 120 && ps2x.Analog(PSS_LX) <= 140 && ps2x.Analog(PSS_RY) >= 120 && ps2x.Analog(PSS_RY) <= 140 && ps2x.Analog(PSS_RX) >= 120 && ps2x.Analog(PSS_RX) <= 140) {
      //Neutro
      Velocidad_Motor_1 = 1000;
      myESC.writeMicroseconds(Velocidad_Motor_1);
      //Lectura de los botones de dirección
      if (ps2x.Button(PSB_PAD_UP)) {  //Arriba
        prenderM1();
           Serial.println("arriba");
        Velocidad_Motor_1 = 2000;
        myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
        delay(espera);

      } else if (ps2x.Button(PSB_PAD_RIGHT)) {
        Velocidad_Motor_1 = 1900;
        myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
        delay(espera);
      } else if (ps2x.Button(PSB_PAD_LEFT)) {
        Velocidad_Motor_1 = 1800;
        myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
        delay(espera);
      } else if (ps2x.Button(PSB_PAD_DOWN)) {
        //abajo
        apagarM1();
        Serial.println("abajo");
        Velocidad_Motor_1 = 1700;
        myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
        delay(espera);
      }
      //Lectura del estado de botones
      if (ps2x.NewButtonState()) {
        if (ps2x.Button(PSB_RED)) {
          Velocidad_Motor_1 = 2000;
          myESC.writeMicroseconds(Velocidad_Motor_1);  //XD 10 SEGUNDOS
          delay(30000);
        } else if (ps2x.Button(PSB_GREEN)) {
          Velocidad_Motor_1 = 2000;
          myESC.writeMicroseconds(Velocidad_Motor_1);  //FLORAR 10 SEGUNDOS
          delay(10000);

        } else if (ps2x.Button(PSB_PINK)) {
          Velocidad_Motor_1 = 2000;
          myESC.writeMicroseconds(Velocidad_Motor_1);  //XD 10 SEGUNDOS
          delay(20000);

        } else if (ps2x.Button(PSB_BLUE)) {
          Velocidad_Motor_1 = 2000;
          myESC.writeMicroseconds(Velocidad_Motor_1);  //SUMERGIRSE 10 SEGUNDOS
          delay(10000);
        } else if (ps2x.Button(PSB_L3)) {
          //ERAR fondoACEL

        } else if (ps2x.Button(PSB_R3)) {
          //retroceder
        } else if (ps2x.Button(PSB_L2)) {
          //LUZ PRENDER
        }

        else if (ps2x.Button(PSB_R2)) {
          //LUZ APAGAR
        }
      }
    }
    //palanca izquierda
    else if (ps2x.Analog(PSS_LY) >= 140) {
      //izquierda ATRAS
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_LY), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_LY) <= 120) {
      //DELANTE izquierda
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_LY), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_LX) >= 140) {
      //derecha left
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_LX), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_LX) <= 120) {
      //left izquierda
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_LX), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    }
    //palanca derecha
    else if (ps2x.Analog(PSS_RY) >= 190) {
      //derecha ATRAS
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_RY), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RY) <= 170) {
      //DELANTE derecha
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_RY), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RX) >= 195) {
      //derecha rigt
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_RX), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    } else if (ps2x.Analog(PSS_RX) <= 170) {
      //rigt izquierda
      Serial.println("izquierda");
      Velocidad_Motor_1 = map(ps2x.Analog(PSS_RX), 0, 255, 1000, 2000);
      myESC.writeMicroseconds(Velocidad_Motor_1);  //señal PWM enviada a la ESC
      delay(espera);
    }
  } else {
    return;
  }

  delay(100);
}
void prenderM1() {
  digitalWrite(M_superior_1_R1, HIGH);
  digitalWrite(M_superior_1_R2, HIGH);
}
void apagarM1() {
  digitalWrite(M_superior_1_R1, LOW);
  digitalWrite(M_superior_1_R2, LOW);
}
