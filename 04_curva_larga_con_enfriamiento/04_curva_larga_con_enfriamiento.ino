// Curvas en https://colab.research.google.com/drive/15KS1SYKvHMWRtGnJpRXsxxwF8SonGAH0?usp=sharing
#include <LiquidCrystal.h>
#include <max6675.h>

const bool PRUEBA = LOW;
const int FACTOR_TIEMPO_PRUEBA = 10;

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
const int TEMPERATURA_FINAL_ESTADO_1 = 100;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_1 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_2 = 100;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_2 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_3 = 200;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_3 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_4 = 200;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_4 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_5 = 450;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_5 = 55;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_6 = 450;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_6 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_7 = 530;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_7 = 18;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_8 = 530;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_8 = 15;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_9 = 650;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_9 = 25;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_10 = 650;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_10 = 15;                           // Lo que tiene que tardar la 1er rampa en minutos
const int TEMPERATURA_FINAL_ESTADO_11 = 1000;              // De 650 a 1000 va sin apagarse
// Ver cuánto tarda en llegar a 1000, y después vamos a esperar  (1060 / 1000 - 1) * ese_tiempo
// const int TEMPERATURA_FINAL_ESTADO_12 = 800;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
// const int TIEMPO_ESTADO_12 = 10;                           // Lo que tiene que tardar la 1er rampa en minutos
// const int TEMPERATURA_FINAL_ESTADO_13 = 1020;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
const int TIEMPO_ESTADO_13 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos
// const int TEMPERATURA_FINAL_ESTADO_14 = 1020;                   // Es la temperatura a la que tiene que llegar el horno en ese estado
// const int TIEMPO_ESTADO_14 = 30;                           // Lo que tiene que tardar la 1er rampa en minutos

const int TEMPERATURAS_FINALES[] = {TEMPERATURA_FINAL_ESTADO_1, TEMPERATURA_FINAL_ESTADO_2, TEMPERATURA_FINAL_ESTADO_3, TEMPERATURA_FINAL_ESTADO_4, TEMPERATURA_FINAL_ESTADO_5, TEMPERATURA_FINAL_ESTADO_6, TEMPERATURA_FINAL_ESTADO_7, TEMPERATURA_FINAL_ESTADO_8, TEMPERATURA_FINAL_ESTADO_9, TEMPERATURA_FINAL_ESTADO_10}; //, TEMPERATURA_FINAL_ESTADO_11, TEMPERATURA_FINAL_ESTADO_12, TEMPERATURA_FINAL_ESTADO_13, TEMPERATURA_FINAL_ESTADO_14};
const int TIEMPOS[] = {TIEMPO_ESTADO_1, TIEMPO_ESTADO_2, TIEMPO_ESTADO_3, TIEMPO_ESTADO_4, TIEMPO_ESTADO_5, TIEMPO_ESTADO_6, TIEMPO_ESTADO_7, TIEMPO_ESTADO_8, TIEMPO_ESTADO_9, TIEMPO_ESTADO_10};  //, TIEMPO_ESTADO_11, TIEMPO_ESTADO_12, TIEMPO_ESTADO_13, TIEMPO_ESTADO_14};

const int ESTADO_HASTA_1000 = 11;
const int ESTADO_HASTA_1060 = 12;
const int ESTADO_MESETA_1060 = 13;
const int ENFRIAMIENTO = 14;
const int ULTIMO_ESTADO = 15;

const int MAXIMA_POTENCIA = 100;  // Tiempo fijo encendido (ms)
const int PERIODO_MS = 1000;
const int PASO_DE_ENFRIAMIENTO = 10;  // Paso de potencia en porcentaje
const int TIEMPO_ENTRE_PASOS = 30;  // Tiempo en minutos
const int TEMPERATURA_DE_CORTE = 500;  // Temperatura a la cual ya se puede apagar el horno por completo

int new_temp_final;
int new_tiempo_rampa;

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
float tiempo_hasta_1000;
float tiempo_hasta_1060;
int temperatura_objetivo_int;
int ultima_temperatura_objetivo = -1;
int ultimo_current_segundos = -1;
int estado = 0;
int temperatura_inicial = 0;
int temperatura_final = 0;
long loop_counter = 0;

bool control_automatico;
unsigned long previousMillis = 0;  // Guarda el último cambio
int potencia = 0;                // Tiempo variable apagado (ms)
int last_potencia = -1;

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

void showStartupMessage() {
  lcd.createChar(0, rightArrow);  // Crear carácter personalizado (índice 0)
  lcd.setCursor(0, 0);
  lcd.print("TIEMPO");
  lcd.write(byte(0));  // Mostrar la flecha
  lcd.print("   :  :  ");
  delay(1000);
}

void serial_salida(int valor1, int valor2, int valor3, int valor4, int valor5, int valor6){
  Serial.print(valor1);
  Serial.print(",");
  Serial.print(valor2);
  Serial.print(",");
  Serial.print(valor3);
  Serial.print(",");
  Serial.print(valor4);
  Serial.print(",");
  Serial.print(valor5);
  Serial.print(",");
  Serial.println(valor6);
}

// puedo hacer esto porque todas las variables son globales
void change_estado(bool cambiar_temp_inicial=HIGH){
    new_temp_final = TEMPERATURAS_FINALES[estado];
    new_tiempo_rampa = TIEMPOS[estado];
    estado++;
    if (new_temp_final != 0){
      tiempo_inicial = current_minutos;
      if (cambiar_temp_inicial){
        temperatura_inicial = temperatura_final;
      }
      tiempo_rampa = new_tiempo_rampa;
      temperatura_final = new_temp_final;
      velocidad_temperatura = float(temperatura_final - temperatura_inicial) / tiempo_rampa;
      lcd.setCursor(9, 1);
      lcd.print(estado);
    }
}

void setup() {
  Serial.begin(9600); // Inicializo el puerto serie
  pinMode(horno_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  lcd.begin(16, 2);
  showStartupMessage();
  digitalWrite(led_pin, ledState);
  serial_salida(0, 0, 0, 0, 0, 0);  // Primer mensaje serie con todos ceros
}

void loop() {
  loop_counter++;
  unsigned long currentMillis = millis();
  current_segundos = currentMillis / 1000;
  if (PRUEBA){
    current_minutos = current_segundos / 60 * FACTOR_TIEMPO_PRUEBA;
  }
  else{
    current_minutos = current_segundos / 60;
  }
  cronometro_segundos = int(current_segundos) % 60;
  cronometro_minutos = int(current_minutos) % 60;
  cronometro_horas = int(current_minutos / 60);
  temperatura_objetivo = temperatura_inicial + velocidad_temperatura * (current_minutos - tiempo_inicial);
  temperatura_objetivo_int = int(temperatura_objetivo);

  if (currentMillis - ultimo_tiempo_temp >= TIEMPO_LECTURA_TEMP){
    if (PRUEBA){
      valor_termocupla = 0;
    }
    else{
      valor_termocupla = ktc.readCelsius();
    }
    ultimo_tiempo_temp = currentMillis;
  }

  if (control_automatico){
    if (temperatura_objetivo_int != ultima_temperatura_objetivo) {
      if (estado < 15){
        lcd.setCursor(1, 1);
        lcd.print(temperatura_objetivo_int);
        lcd.print("\337");
        lcd.print(" ");
        ultima_temperatura_objetivo = temperatura_objetivo_int;
      }
    }
  }
  else {
    // Actualiza el LCD si el valor cambió
    if (potencia != last_potencia) {
      lcd.setCursor(0, 1);
      lcd.print(potencia * 100 / MAXIMA_POTENCIA);
      lcd.print("%  ");
      last_potencia = potencia;
    }
  }
  // Actualiza el LCD si cambió la temp
  if (valor_termocupla != ultima_temp) {
    lcd.setCursor(12, 1);
    lcd.print(valor_termocupla);
    lcd.print("\337");
    lcd.print(" ");
    ultima_temp = valor_termocupla;
  }

  // Si pasan 10 segundos envío nueva información por el Serial
  // No uso corometro_segundos porque ese se frena cuando llega a FIN y se queda entrando siempre a ese if
  else if (int(current_segundos) != ultimo_current_segundos) {
    if (int(current_segundos) % 10 == 0){
      serial_salida(current_segundos, temperatura_objetivo, valor_termocupla, ledState, estado, potencia);
      loop_counter = 0;
    }
    ultimo_current_segundos = int(current_segundos);
  }

  // parte para mostrar los segundos en el cronómetro
  if (estado < ULTIMO_ESTADO){
    if (cronometro_segundos != ultimo_cronometro_segundos) {
      lcd.setCursor(14, 0);
      if (cronometro_segundos < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_segundos);

      ultimo_cronometro_segundos = cronometro_segundos;
    }
    else if (cronometro_minutos != ultimo_cronometro_minutos) {
      lcd.setCursor(11, 0);
      if (cronometro_minutos < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_minutos);

      ultimo_cronometro_minutos = cronometro_minutos;
    }
    else if (cronometro_horas != ultimo_cronometro_horas) {
      lcd.setCursor(8, 0);
      if (cronometro_horas < 10){
        lcd.print("0");
      }
      lcd.print(cronometro_horas);

      ultimo_cronometro_horas = cronometro_horas;
    }
    else if (temperatura_objetivo_int != ultima_temperatura_objetivo) {
      lcd.setCursor(1, 1);
      lcd.print(temperatura_objetivo_int);
      lcd.print("\337");
      lcd.print(" ");
      ultima_temperatura_objetivo = temperatura_objetivo_int;
    }

    if (control_automatico == HIGH){
      // Control automatico por temperatura
      if (ledState == HIGH) {
        // Si el horno está prendido esperamos que valor_termocupla (termocupla) menos temp_objetivo sea mayor a 3 grados
        if (valor_termocupla - temperatura_objetivo > MARGEN_TEMPERATURA_APAGADO) {
          ledState = LOW;
          digitalWrite(led_pin, ledState);
          digitalWrite(horno_pin, ledState);
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
      // Control manual por potencia
      if (ledState == HIGH) {
        // Si el LED está encendido, espera TIEMPO_ENCENDIDO
        if (potencia < MAXIMA_POTENCIA){
          if (currentMillis - previousMillis >= float(potencia) / 100 * PERIODO_MS) {
            previousMillis = currentMillis;
            ledState = LOW;
            digitalWrite(led_pin, ledState);
            digitalWrite(horno_pin, ledState);
          }
        }
        else {
          previousMillis = currentMillis;
        }
      } else {
        // Si el LED está apagado, espera potencia
        if (potencia > 0) {
          if (currentMillis - previousMillis >= float(MAXIMA_POTENCIA - potencia) / 100 * PERIODO_MS) {
            previousMillis = currentMillis;
            ledState = HIGH;
            digitalWrite(led_pin, ledState);
            digitalWrite(horno_pin, ledState);
          }
        }
        else {
          previousMillis = currentMillis;
        }
      }
    }
  }
  else {
    ledState = LOW;
    digitalWrite(led_pin, ledState);
    digitalWrite(horno_pin, ledState);
  }

  if (estado == 0) {
      control_automatico = HIGH;
      temperatura_inicial = valor_termocupla;
      change_estado(LOW);
  }
  else if (estado < ESTADO_HASTA_1000) {
    if (current_minutos - tiempo_inicial >= tiempo_rampa){
      change_estado();
    }
  }
  else if (estado == ESTADO_HASTA_1000) {
    control_automatico = LOW;
    ledState = HIGH;
    digitalWrite(led_pin, ledState);
    digitalWrite(horno_pin, ledState);
    potencia = MAXIMA_POTENCIA;
    int valor_comparar;
    if (PRUEBA){
      valor_comparar = current_segundos * FACTOR_TIEMPO_PRUEBA / 20;
    }
    else{
      valor_comparar = valor_termocupla;
    }
    if (valor_comparar >= TEMPERATURA_FINAL_ESTADO_11){
      // Ver cuánto tarda en llegar a 1000, y después vamos a esperar  (1060 / 1000 - 1) * ese_tiempo
      tiempo_hasta_1000 = current_minutos - tiempo_inicial;
      tiempo_hasta_1060 = tiempo_hasta_1000 * (1060 - 1000) / (1000 - temperatura_final);
      tiempo_inicial = current_minutos;
      estado++;
      Serial.print("Tiempo hasta 1000: ");
      Serial.println(tiempo_hasta_1000);
      Serial.print("Tiempo hasta 1060: ");
      Serial.println(tiempo_hasta_1060);
    }
  }
  else if (estado == ESTADO_HASTA_1060) {
    if (current_minutos - tiempo_inicial >= tiempo_hasta_1060){
      estado++;
      tiempo_inicial = current_minutos;
    }
  }
  else if (estado == ESTADO_MESETA_1060) {
    if (current_minutos - tiempo_inicial >= TIEMPO_ESTADO_13){
      estado++;
      potencia = 85;
      tiempo_inicial = current_minutos;
    }
  }
  else if (estado == ENFRIAMIENTO) {
    if (current_minutos - tiempo_inicial >= TIEMPO_ENTRE_PASOS){
      potencia -= PASO_DE_ENFRIAMIENTO;
      tiempo_inicial = current_minutos;
    }
    int valor_comparar;
    if (PRUEBA){
      valor_comparar = temperatura_objetivo_int;
    }
    else{
      valor_comparar = valor_termocupla;
    }
    if ((valor_comparar <= TEMPERATURA_DE_CORTE) || (potencia <= 10)){
      potencia = 0;
      // Pasa al estado 15 para terminar
      // Es lo mismo que hacer estado = ULTIMO_ESTADO;
      estado++;
      lcd.setCursor(7, 1);
      lcd.print("FIN");
    }
  }
}
