#include <Wire.h>
#include <LiquidCrystal.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <SoftwareSerial.h> // Include the SoftwareSerial library

// Configura el LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Configura el RTC con los pines analógicos A1, A2 y A3
ThreeWire myWire(A1, A2, A3); // DAT, CLK, RST para DS1302
RtcDS1302<ThreeWire> Rtc(myWire);

// Define el pin del pulsador y variables para el control del estado
const int buttonPin = 7;
bool lcdOn = true;
bool lastButtonState = HIGH; // Estado anterior del pulsador
bool buttonPressed = false; // Estado de si el botón fue presionado
unsigned long lastDebounceTime = 0; // Tiempo del último cambio de estado
unsigned long debounceDelay = 50; // Retardo para el "debounce"

// Define el pin del buzzer
const int buzzerPin = 8;

// Define los pines para el módulo Bluetooth
SoftwareSerial bluetooth(9, 10); // RX, TX

// Variables para la configuración de alarmas (hasta 5 tratamientos)
const int maxTreatments = 5; // Máximo número de tratamientos
int alarmHour[maxTreatments] = {0, 0, 0, 0, 0};
int alarmMinute[maxTreatments] = {0, 0, 0, 0, 0};
int alarmFrequency[maxTreatments] = {0, 0, 0, 0, 0};
bool alarmActive[maxTreatments] = {false, false, false, false, false};

void setup() {
  // Configura la comunicación con el puerto serial
  Serial.begin(9600);

  // Inicializa la comunicación Bluetooth
  bluetooth.begin(9600);

  // Inicializa el LCD
  lcd.begin(16, 2);

  // Configura el pin del pulsador
  pinMode(buttonPin, INPUT_PULLUP); // Configura el pin como entrada con resistencia pull-up

  // Configura el pin del buzzer
  pinMode(buzzerPin, OUTPUT);

  // Inicializa el RTC
  Rtc.Begin();

  // Configura la fecha y hora inicial si es necesario
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);

  // Muestra la fecha actual en el LCD
  displayDate();
}

void loop() {
  // Lee el estado del pulsador
  int reading = digitalRead(buttonPin);

  // Detecta el cambio de estado del pulsador
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Reinicia el temporizador de debounce
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Si el estado del pulsador cambió
    if (reading != buttonPressed) {
      buttonPressed = reading; // Actualiza el estado del botón

      if (buttonPressed == LOW) { // Pulsador presionado (LOW porque se usa INPUT_PULLUP)
        lcdOn = !lcdOn; // Cambia el estado de la pantalla LCD
        if (lcdOn) {
          lcd.display(); // Enciende la pantalla LCD
          Serial.println("LCD Encendido");
          bluetooth.println("LCD Encendido"); // Envía al Bluetooth
        } else {
          lcd.noDisplay(); // Apaga la pantalla LCD
          Serial.println("LCD Apagado");
          bluetooth.println("LCD Apagado"); // Envía al Bluetooth
        }

        // Cambia el estado del buzzer si alguna alarma está activa
        for (int i = 0; i < maxTreatments; i++) {
          if (alarmActive[i]) {
            noTone(buzzerPin); // Detiene el buzzer si estaba activo
            alarmActive[i] = false;
            Serial.print("ALARMA DETENIDA para el tratamiento ");
            Serial.println(i + 1);
            bluetooth.print("ALARMA DETENIDA para el tratamiento ");
            bluetooth.println(i + 1); // Envía al Bluetooth
            displayDate(); // Muestra la fecha y hora después de detener la alarma
            setNextAlarm(i); // Configura la próxima alarma según la frecuencia
          }
        }
      }
    }
  }

  lastButtonState = reading; // Guarda el estado actual del pulsador

  // Actualiza la hora en el LCD, el monitor serial y Bluetooth si la pantalla está encendida
  if (lcdOn && !anyAlarmActive()) {
    updateLCD();
    updateSerial();
    updateBluetooth(); // Actualiza el Bluetooth con la fecha y hora actuales
  }

  // Verificar cada alarma
  RtcDateTime now = Rtc.GetDateTime();

  for (int i = 0; i < maxTreatments; i++) {
    if (now.Hour() == alarmHour[i] && now.Minute() == alarmMinute[i] && now.Second() == 0) {
      if (!alarmActive[i]) {
        Serial.print("ALARMA ACTIVADA para el tratamiento ");
        Serial.println(i + 1);
        bluetooth.print("ALARMA ACTIVADA para el tratamiento ");
        bluetooth.println(i + 1);
        alarmActive[i] = true;
        activateAlarm(i + 1); // Mostrar mensaje de alarma en el LCD
      }

      while (alarmActive[i]) {
        tone(buzzerPin, 1000);
        delay(500);

        if (digitalRead(buttonPin) == LOW) {
          noTone(buzzerPin);
          alarmActive[i] = false;
          Serial.print("ALARMA DETENIDA para el tratamiento ");
          Serial.println(i + 1);
          bluetooth.print("ALARMA DETENIDA para el tratamiento ");
          bluetooth.println(i + 1);
          displayDate();
          setNextAlarm(i); // Configura la próxima alarma según la frecuencia
        }
      }
    }
  }

  // Escucha los comandos del Bluetooth
  if (bluetooth.available()) {
    String receivedData = bluetooth.readStringUntil('\n');
    processReceivedData(receivedData);
  }

  delay(50); // Espera 50 milisegundos para mayor precisión
}

void displayDate() {
  RtcDateTime now = Rtc.GetDateTime();

  if (!now.IsValid()) {
    lcd.setCursor(0, 0);
    lcd.print("ERROR RTC");
    Serial.println("ERROR RTC");
    bluetooth.println("ERROR RTC"); // Envía al Bluetooth
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Fecha=");
  lcd.print(now.Day() < 10 ? "0" : "");
  lcd.print(now.Day());
  lcd.print('/');
  lcd.print(now.Month() < 10 ? "0" : "");
  lcd.print(now.Month());
  lcd.print('/');
  lcd.print(now.Year());
}

void updateLCD() {
  RtcDateTime now = Rtc.GetDateTime();

  if (!now.IsValid()) {
    lcd.setCursor(0, 1);
    lcd.print("ERROR RTC");
    Serial.println("ERROR RTC");
    bluetooth.println("ERROR RTC"); // Envía al Bluetooth
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("                "); // 16 espacios para limpiar la línea

  lcd.setCursor(0, 1);
  lcd.print("Hora=");
  lcd.print(now.Hour() < 10 ? '0' : ' ');
  lcd.print(now.Hour());
  lcd.print(':');
  lcd.print(now.Minute() < 10 ? '0' : ' ');
  lcd.print(now.Minute());
  lcd.print(':');
  lcd.print(now.Second() < 10 ? '0' : ' ');
  lcd.print(now.Second());
}

void updateSerial() {
  RtcDateTime now = Rtc.GetDateTime();

  Serial.print("Fecha y Hora: ");
  Serial.print(now.Day());
  Serial.print('/');
  Serial.print(now.Month());
  Serial.print('/');
  Serial.print(now.Year());
  Serial.print(" ");
  Serial.print(now.Hour() < 10 ? '0' : ' ');
  Serial.print(now.Hour());
  Serial.print(':');
  Serial.print(now.Minute() < 10 ? '0' : ' ');
  Serial.print(now.Minute());
  Serial.print(':');
  Serial.print(now.Second() < 10 ? '0' : ' ');
  Serial.println(now.Second());
}

void updateBluetooth() {
  RtcDateTime now = Rtc.GetDateTime();

  bluetooth.print("Fecha y Hora: ");
  bluetooth.print(now.Day());
  bluetooth.print('/');
  bluetooth.print(now.Month());
  bluetooth.print('/');
  bluetooth.print(now.Year());
  bluetooth.print(" ");
  bluetooth.print(now.Hour() < 10 ? '0' : ' ');
  bluetooth.print(now.Hour()); 
  bluetooth.print(':');
  bluetooth.print(now.Minute() < 10 ? '0' : ' ');
  bluetooth.print(now.Minute());
  bluetooth.print(':');
  bluetooth.print(now.Second() < 10 ? '0' : ' ');
  bluetooth.println(now.Second());
}

void processReceivedData(String data) {
  Serial.println(data);

  // Separar los datos basados en las comas
  int separatorIndex = data.indexOf(',');
  if (separatorIndex == -1) return;

  String firstDoseTime = data.substring(0, separatorIndex);
  data = data.substring(separatorIndex + 1);

  separatorIndex = data.indexOf(',');
  if (separatorIndex == -1) return;

  String frequency = data.substring(0, separatorIndex);
  data = data.substring(separatorIndex + 1);

  separatorIndex = data.indexOf(',');
  if (separatorIndex == -1) return;

  String medicineName = data.substring(0, separatorIndex);
  data = data.substring(separatorIndex + 1);

  separatorIndex = data.indexOf(',');
  if (separatorIndex == -1) return;

  int treatmentNumber = data.substring(0, separatorIndex).toInt();
  if (treatmentNumber < 1 || treatmentNumber > maxTreatments) return; // Validar número de tratamiento

  data = data.substring(separatorIndex + 1);

  int dose = data.toInt();

  // Procesar la hora de la primera toma
  separatorIndex = firstDoseTime.indexOf(':');
  if (separatorIndex == -1) return;

  alarmHour[treatmentNumber - 1] = firstDoseTime.substring(0, separatorIndex).toInt();
  alarmMinute[treatmentNumber - 1] = firstDoseTime.substring(separatorIndex + 1).toInt();

  // Procesar la frecuencia en horas y minutos
  int freqHour = frequency.substring(0, 2).toInt();
  int freqMinute = frequency.substring(3, 5).toInt();
  alarmFrequency[treatmentNumber - 1] = freqHour * 60 + freqMinute;

  // Mostrar la alarma configurada
  Serial.print("Tratamiento ");
  Serial.print(treatmentNumber);
  Serial.print(": Alarma configurada a las ");
  Serial.print(alarmHour[treatmentNumber - 1]);
  Serial.print(":");
  Serial.print(alarmMinute[treatmentNumber - 1] < 10 ? "0" : "");
  Serial.print(alarmMinute[treatmentNumber - 1]);
  Serial.print(" con una frecuencia de ");
  Serial.print(freqHour);
  Serial.print(" horas y ");
  Serial.println(freqMinute);

  // Enviar la información de la alarma configurada al Bluetooth
  bluetooth.print("Tratamiento ");
  bluetooth.print(treatmentNumber);
  bluetooth.print(": Alarma configurada a las ");
  bluetooth.print(alarmHour[treatmentNumber - 1]);
  bluetooth.print(":");
  bluetooth.print(alarmMinute[treatmentNumber - 1] < 10 ? "0" : "");
  bluetooth.print(alarmMinute[treatmentNumber - 1]);
  bluetooth.print(" con una frecuencia de ");
  bluetooth.print(freqHour);
  bluetooth.print(" horas y ");
  bluetooth.println(freqMinute);
}

void activateAlarm(int treatmentNumber) {
  lcd.clear(); // Limpia la pantalla
  lcd.setCursor(0, 0);
  lcd.print("ALARMA ");
  lcd.print(treatmentNumber);
  lcd.print(" ACTIVADA!");
  lcd.setCursor(0, 1);
  lcd.print("Pulse el boton");
}

void setNextAlarm(int treatmentIndex) {
  alarmMinute[treatmentIndex] += alarmFrequency[treatmentIndex];
  if (alarmMinute[treatmentIndex] >= 60) {
    alarmMinute[treatmentIndex] -= 60;
    alarmHour[treatmentIndex]++;
    if (alarmHour[treatmentIndex] >= 24) {
      alarmHour[treatmentIndex] = 0;
    }
  }
}

bool anyAlarmActive() {
  for (int i = 0; i < maxTreatments; i++) {
    if (alarmActive[i]) {
      return true;
    }
  }
  return false;
}
