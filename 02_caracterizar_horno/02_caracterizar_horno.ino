#include <LiquidCrystal.h>
#include <max6675.h>
#include <movingAvg.h>          // https://github.com/JChristensen/movingAvg

const bool PRUEBA = LOW;

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

const int CALENTANDO = 0;
const int ENFRIANDO = 1;

// Instancia LCD
LiquidCrystal lcd(lcd_rs, lcd_enable, lcd_d4, lcd_d5, lcd_d6,lcd_d7);

// Instancia termocupla
MAX6675 ktc(termo_SCK, termo_CS, termo_S0);

// Moving Average para la termocupla
// Modo de uso en https://github.com/JChristensen/movingAvg
movingAvg valor_temp_average(10);  // siempre se va a estar haciendo un promedio de los últimos 10 valores

// Constantes y variables
const int MAXIMA_POTENCIA = 100;  // Tiempo fijo encendido (ms)
const int PERIODO_MS = 1000;
const int TIEMPO_LECTURA_TEMP_MS = 700;
const int TEMPERATURA_DE_CORTE = 1000;
const int PASO_ENTRE_POTENCIAS = 5;  // Valor en porcentaje
const int TIEMPO_ENTRE_POTENCIAS_MIN = 10;

unsigned long previousMillis = 0;  // Guarda el último cambio
int potencia = 0;                // Tiempo variable apagado (ms)
int valor_temp;                   // Declaramos la lectura de la temperatura
int ultima_temp = -1;
int last_potencia = -1;
int estado = CALENTANDO;
int ultimo_current_segundos = -1;
bool ledState = LOW;               // Estado inicial del LED
long ultimo_tiempo_temp = -1;

float current_segundos;
float current_minutos;
float tiempo_inicial;

int cronometro_segundos;
int cronometro_minutos;
int cronometro_horas;
int ultimo_cronometro_segundos = -1;
int ultimo_cronometro_minutos = -1;
int ultimo_cronometro_horas = -1;

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
  // current_segundos, estado, potencia, valor_temp
  if (PRUEBA) {
    Serial.print("Prueba");
    Serial.print(",");
  }
  Serial.print("Tiempo");
  Serial.print(",");
  Serial.print("Estado");
  Serial.print(",");
  Serial.print("Potencia");
  Serial.print(",");
  Serial.println("Termocupla");
  delay(1000);
}

void serial_salida(int valor1, int valor2, int valor3, int valor4){
  if (PRUEBA) {
    Serial.print("Prueba");
    Serial.print(",");
  }
  Serial.print(valor1);
  Serial.print(",");
  Serial.print(valor2);
  Serial.print(",");
  Serial.print(valor3);
  Serial.print(",");
  Serial.println(valor4);
}

void setup() {
  Serial.begin(9600);
  valor_temp_average.begin();  // Inicializo el moving average de las lecturas de la termocupla
  pinMode(horno_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  lcd.begin(16, 2);
  showStartupMessage();
  digitalWrite(led_pin, ledState);
}

void loop() {
  unsigned long currentMillis = millis();
  current_segundos = float(currentMillis) / 1000;
  current_minutos = current_segundos / 60;
  cronometro_segundos = int(current_segundos) % 60;
  cronometro_minutos = int(current_minutos) % 60;
  cronometro_horas = int(current_minutos / 60);

  if (currentMillis - ultimo_tiempo_temp >= TIEMPO_LECTURA_TEMP_MS){
    if (PRUEBA) {
      valor_temp_average.reading(current_segundos * 7);
    }
    else {
     valor_temp_average.reading(ktc.readCelsius());
    }
    valor_temp = valor_temp_average.getAvg();
    ultimo_tiempo_temp = currentMillis;
  }

  // mando por el serie cada 10 segundos
  if (int(current_segundos) != ultimo_current_segundos) {
    if (int(current_segundos) % 10 == 0){
      serial_salida(current_segundos, estado, potencia, valor_temp);
    }
    ultimo_current_segundos = int(current_segundos);
  }

  // Actualiza el LCD si cambió la temp
  if (valor_temp != ultima_temp) {
    lcd.setCursor(12, 1);
    lcd.print(valor_temp);
    lcd.print("\337");
    lcd.print(" ");
    ultima_temp = valor_temp;
  }

  // Actualiza el LCD si el valor cambió
  if (potencia != last_potencia) {
    //lcd.setCursor(1, 1);
    //lcd.print("    ");    
    lcd.setCursor(1, 1);
    lcd.print(potencia * 100 / MAXIMA_POTENCIA);
    lcd.print(" % ");
    last_potencia = potencia;
  }

  // Actualizo cronómetro
  if (cronometro_segundos != ultimo_cronometro_segundos) {
    lcd.setCursor(14, 0);
    if (cronometro_segundos < 10){
      lcd.print("0");
    }
    lcd.print(cronometro_segundos);
           
    ultimo_cronometro_segundos = cronometro_segundos;
  }
  if (cronometro_minutos != ultimo_cronometro_minutos) {
    lcd.setCursor(11, 0);
    if (cronometro_minutos < 10){
      lcd.print("0");
    }
    lcd.print(cronometro_minutos);
     
    ultimo_cronometro_minutos = cronometro_minutos;
  }
  if (cronometro_horas != ultimo_cronometro_horas) {
    lcd.setCursor(8, 0);
    if (cronometro_horas < 10){
      lcd.print("0");
    }
    lcd.print(cronometro_horas);

    ultimo_cronometro_horas = cronometro_horas;
  }

  // Control del LED
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

  switch (estado){
    case CALENTANDO:
      potencia = MAXIMA_POTENCIA;
      if (valor_temp >= TEMPERATURA_DE_CORTE){
        potencia = potencia - PASO_ENTRE_POTENCIAS;
        estado = ENFRIANDO;
        tiempo_inicial = current_minutos;
      }
      break;
    case ENFRIANDO:
      if (current_minutos - tiempo_inicial > TIEMPO_ENTRE_POTENCIAS_MIN){
        potencia = potencia - PASO_ENTRE_POTENCIAS;
        estado = ENFRIANDO;
        tiempo_inicial = current_minutos;
      }
      break;
    default:
      estado = ENFRIANDO;
      break;
  }
}