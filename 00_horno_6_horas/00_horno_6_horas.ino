#include <LiquidCrystal.h>
#include <max6675.h>

#define led_pin 13
#define horno_pin 6

// pines del LCD
#define lcd_rs 12
#define lcd_enable 11
#define lcd_d4 10
#define lcd_d5 9
#define lcd_d6 8
#define lcd_d7 7

// pines termocupla
#define termo_S0 4
#define termo_CS 3
#define termo_SCK 2

// Instancia LCD
LiquidCrystal lcd(lcd_rs, lcd_enable, lcd_d4, lcd_d5, lcd_d6,lcd_d7);

// Instancia termocupla
MAX6675 ktc(termo_SCK, termo_CS, termo_S0);

// Constantes y variables
const int TIEMPO_LECTURA_TEMP = 700;
const float MARGEN_TEMPERATURA_ENCENDIDO = 2;             // margen de diferencia de grados entre la temperatura objetivo y la que mide la termocupla (2 grados)
const float MARGEN_TEMPERATURA_APAGADO = -1;             // margen de diferencia de grados entre la temperatura objetivo y la que mide la termocupla (2 grados)

// rampas de los Estados
const int TIEMPO_ESTADO_1 = 60;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_1 = 100;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_2 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_2 = 100;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_3 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_3 = 200;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_4 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_4 = 200;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_5 = 50;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_5 = 450;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_6 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_6 = 450;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_7 = 25;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_7 = 530;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_8 = 15;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_8 = 530;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_9 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_9 = 650;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_10 = 15;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_10 = 650;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_11 = 40;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_11 = 850;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_12 = 15;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_12 = 850;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_13 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_13 = 1060;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_14 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_14 = 1060;                   // Es la temperatura a la que tiene que llegar el horno en ese estado

float velocidad_temperatura = 0;                        // grados por minuto
bool ledState = LOW;                                    // Estado inicial del LED
int cronometro_segundos;
int cronometro_minutos;
int cronometro_horas;
int ultimo_cronometro_segundos = -1;
int ultimo_cronometro_minutos = -1;
int ultimo_cronometro_horas = -1;

int valor_termocupla;                            // Declaramos la lectura de la temperatura
int ultima_temp = -1;
long ultimo_tiempo_temp = -1;
float current_segundos;
float current_minutos;
float temperatura_objetivo;
float tiempo_inicial = 0;
float tiempo_rampa;
int temperatura_objetivo_int;
int ultima_temperatura_objetivo = -1;
int estado = 0;
int temperatura_inicial = 0;
int temperatura_final = 0;
// Definir el carácter de flecha derecha (8x5 píxeles)
byte rightArrow[8] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};

void setup() {
  pinMode(horno_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  lcd.begin(16, 2);
  showStartupMessage();
  digitalWrite(led_pin, ledState);
}

void loop() {
  unsigned long currentMillis = millis();
  current_segundos = currentMillis / 1000;
  current_minutos = current_segundos / 60;
  cronometro_segundos = int(current_segundos) % 60;
  cronometro_minutos = int(current_minutos) % 60;
  cronometro_horas = int(current_minutos / 60);
  temperatura_objetivo = temperatura_inicial + velocidad_temperatura * (current_minutos - tiempo_inicial);
  temperatura_objetivo_int = int(temperatura_objetivo);
 
   
  if (currentMillis - ultimo_tiempo_temp >= TIEMPO_LECTURA_TEMP){
    valor_termocupla = ktc.readCelsius();
    ultimo_tiempo_temp = currentMillis;
  }
  // Actualiza el LCD si cambió la temp
 
  if (valor_termocupla != ultima_temp) {
    //lcd.setCursor(12, 1);
    //lcd.print("    ");    
    lcd.setCursor(12, 1);
    lcd.print(valor_termocupla);
    // lcd.print(" ° ");
    //lcd.print(" ");
    lcd.print("\337");
    lcd.print(" ");
    ultima_temp = valor_termocupla;
  }

  if (temperatura_objetivo_int != ultima_temperatura_objetivo) {
    //lcd.setCursor(12, 1);
    //lcd.print("    ");  
    if (estado < 15){
      lcd.setCursor(1, 1);
      lcd.print(temperatura_objetivo_int);
      // lcd.print(" ° ");
      //lcd.print(" ");
      lcd.print("\337");
      lcd.print(" ");
      ultima_temperatura_objetivo = temperatura_objetivo_int;
    }
  }
// parte para mostrar los segundos en el cronómetro
  if (cronometro_segundos != ultimo_cronometro_segundos) {
    if (estado < 15){
      lcd.setCursor(14, 0);
      if (cronometro_segundos < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_segundos);
           
      ultimo_cronometro_segundos = cronometro_segundos;
    }
  }

  if (cronometro_minutos != ultimo_cronometro_minutos) {
    if (estado < 15){
      lcd.setCursor(11, 0);
      if (cronometro_minutos < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_minutos);
     
      ultimo_cronometro_minutos = cronometro_minutos;
    }
  }

  if (cronometro_horas != ultimo_cronometro_horas) {
    if (estado < 15){
      lcd.setCursor(8, 0);
      if (cronometro_horas < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_horas);
     
      ultimo_cronometro_horas = cronometro_horas;
    }
  }

  if (estado < 15){
    // Control del LED
    if (ledState == HIGH) {
      // Si el horno está prendido esperamos que valor_termocupla (termocupla) menos temp_objetivo sea mayor a 3 grados
      if (valor_termocupla - temperatura_objetivo > MARGEN_TEMPERATURA_APAGADO) {
        ledState = LOW;
        digitalWrite(led_pin, ledState);
        digitalWrite(horno_pin, ledState);

      }
      // Esto es un stop forzado por si la termocupla deja de andar no quede el horno calentando y se pare
      if (temperatura_objetivo - valor_termocupla > 50){
        estado = 15;
        lcd.setCursor(6, 1);
        lcd.print("ERROR");
      }

    } else {
      // Si el horno está apagado lo encendemos cuando la temp_objetivo sea 3 grados más grande que valor_termocupla)
        if (temperatura_objetivo - valor_termocupla > MARGEN_TEMPERATURA_ENCENDIDO) {
          ledState = HIGH;
          digitalWrite(led_pin, ledState);
          digitalWrite(horno_pin, ledState);
        }
     
    }



  }
  else {
    ledState = LOW;
    digitalWrite(led_pin, ledState);
    digitalWrite(horno_pin, ledState);
  }
 

  if (estado == 0){
    temperatura_inicial = valor_termocupla;
    tiempo_rampa = TIEMPO_ESTADO_1;
    temperatura_final = TEMPERATURA_FINAL_ESTADO_1;
    velocidad_temperatura = (temperatura_final - temperatura_inicial) / tiempo_rampa;      // =
    estado = 1;
    tiempo_inicial = current_minutos;
    lcd.setCursor(9, 1);
    lcd.print(estado);
  }
  // estado que calienta hasta 100 grados porque el estado 0 me lo dijo
  if (estado == 1){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_2;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_2;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta
      estado = 2;    
      tiempo_inicial = current_minutos;
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 2){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_3;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_3;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 3;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 3){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_4;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_4;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 4;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
// estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 4){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_5;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_5;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 5;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 5){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_6;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_6;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 6;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
// estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 6){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_7;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_7;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 7;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 7){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_8;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_8;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 8;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
// estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 8){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_9;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_9;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 9;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 9){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_10;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_10;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 10;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
// estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 10){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_11;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_11;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 11;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 11){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_12;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_12;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 12;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
// estado es una meseta de 10 minutos porque me lo dijo el estado 1
  if (estado == 12){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_13;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_13;
      velocidad_temperatura = (temperatura_final - valor_termocupla) / tiempo_rampa;      // = es cero porque queremos que sea una meseta
      estado = 13;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }
  // calienta hasta 450 grados en 20 minutos que me lo dijo el estado anterior
  if (estado == 13){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      temperatura_inicial = temperatura_final;
      tiempo_rampa = TIEMPO_ESTADO_14;
      temperatura_final = TEMPERATURA_FINAL_ESTADO_14;
      velocidad_temperatura = 0;      // = es cero porque queremos que sea una meseta (el estado siguiente)
      estado = 14;
      tiempo_inicial = current_minutos;  
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
  }

  // estado es una meseta de 10 minutos porque me lo dijo el estado anterior
  if (estado == 14){
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      estado = 15;
      lcd.setCursor(8, 1);
      lcd.print("FIN");
    }
  }

}

void showStartupMessage() {
  lcd.createChar(0, rightArrow);  // Crear carácter personalizado (índice 0)
  lcd.setCursor(0, 0);
  lcd.print("TIEMPO");
  lcd.write(byte(0));  // Mostrar la flecha
  lcd.print("   :  :  ");
  delay(1000);
 
}

