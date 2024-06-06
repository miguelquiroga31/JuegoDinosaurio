#include <LiquidCrystal.h>
#include "Dinosaurio.h"
#include "Cactus.h"
#include "Ave.h" // Incluir la nueva clase Ave

LiquidCrystal lcd(22, 23, 5, 18, 19, 21);
Dinosaurio dino;
Cactus cactus(16);
Ave ave(20); // Crear una instancia del ave con una posición inicial

byte dinoReposo[8] = {
      B00000,
      B00000,
      B00111,
      B00101,
      B10111,
      B10100,
      B01101,
      B01100
};
byte dinoCamina1[8] = {
      B00000,
      B00111,
      B00101,
      B10111,
      B10100,
      B01110,
      B01100,
      B00010
};
byte dinoCamina2[8] = {
      B00000,
      B00111,
      B00101,
      B10111,
      B10100,
      B01110,
      B01100,
      B10000
};
byte dinoSalta[8] = {
      B00111,
      B00101,
      B10111,
      B10100,
      B01101,
      B01100,
      B10000,
      B00000
};
byte cactus1[8] = {
      B00000,
      B00000,
      B00001,
      B10101,
      B01101,
      B00110,
      B00100,
      B00100
};
byte ave1[8] = {
    B00100,
    B01110,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
    B00000
};

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button button1 = {4, false};
const int buzzerPin = 2; // Pin del buzzer

int velocidad = 100;
int puntaje = 0;
bool juegoActivo = true;
bool estadoFinalMostrado = false; // Nueva variable para controlar la actualización de la pantalla

void IRAM_ATTR isr() {
  button1.pressed = true;
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(115200);
  Serial.println("test");

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);

  pinMode(buzzerPin, OUTPUT); // Configuramos el pin del buzzer como salida

  lcd.createChar(0, dinoReposo);
  lcd.createChar(1, dinoCamina1);
  lcd.createChar(2, dinoCamina2);
  lcd.createChar(3, dinoSalta);
  lcd.createChar(4, cactus1);
  lcd.createChar(5, ave1); // Crear el nuevo carácter para el ave

  lcd.setCursor(12, 0);
  lcd.print(puntaje);

  lcd.setCursor(0, 1);
  lcd.write(byte(0));

  delay(4000);
}

void mostrarEstadoFinal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Total:");
  lcd.print(puntaje);
  tone(buzzerPin, 500, 300); // Sonido de pérdida a 500 Hz por 1 segundo
  estadoFinalMostrado = true; // Actualizamos la variable para indicar que el estado final ha sido mostrado
}

void reiniciarJuego() {
  juegoActivo = true;
  puntaje = 0;
  cactus = Cactus(16); // Reiniciar la posición del cactus
  dino = Dinosaurio(); // Reiniciar el estado del dinosaurio
  ave = Ave(20); // Reiniciar la posición del ave
  lcd.clear();
  lcd.setCursor(12, 0);
  lcd.print(puntaje);
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  delay(2000);
  estadoFinalMostrado = false; // Reiniciamos la variable para permitir futuras actualizaciones
}

void loop() {
  if (juegoActivo) {
    // Se borra la pantalla
    lcd.clear();

    // Se pinta la pantalla
    if (button1.pressed) {
      dino.saltar();
      lcd.setCursor(0, 0);
      tone(buzzerPin, 1000, 100); // Hacemos sonar el buzzer a 1000 Hz por 200 ms
      button1.pressed = false;
    } else {
      lcd.setCursor(0, 1);
      dino.caminar();
    }

    lcd.write(byte(dino.obtenerEstado()));

    cactus.mover();
    lcd.setCursor(cactus.obtenerPosicion(), 1);
    lcd.write(byte(4));

    ave.mover();
    lcd.setCursor(ave.obtenerPosicion(), 0); // Colocamos el ave en la primera fila
    lcd.write(byte(5));

    // Comprobar la colision con el cactus
    if (dino.obtenerPosicion() == 0 && cactus.obtenerPosicion() == 0) {
      Serial.println("Colision!");
      juegoActivo = false;
    }

    // Comprobar la colision con el ave
    if (dino.obtenerPosicion() == 0 && ave.obtenerPosicion() == 0 && dino.obtenerEstado() == 3) { // El estado 3 es cuando el dinosaurio está en el aire
      Serial.println("Colision con ave!");
      juegoActivo = false;
    
    } else {
      puntaje++;
    }

    lcd.setCursor(12, 0);
    lcd.print(puntaje);
    delay(velocidad);
  } else {
    if (!estadoFinalMostrado) {
      mostrarEstadoFinal();
    }
    if (button1.pressed) {
      button1.pressed = false;
      reiniciarJuego();
    }
  }
}
