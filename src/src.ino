#include <OLED_I2C.h>
#include "BluetoothSerial.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Tone32.h>

// Sensor
MAX30105 sensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
long irValue;
float beatsPerMinute;
int beatAvg;
bool estado = true;

// Pantalla
OLED pantalla(4, 15, 16);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t corazon[];
extern uint8_t inicio[];
extern uint8_t clima_ico[];
extern uint8_t BigNumbers[];

// Bluetooth
BluetoothSerial bluetooth;

// Wifi
WiFiClient client;
const char *nombre = "red_datos";
const char *clave = "1002acosta";

// Clima
String APIKEY = "";
String ciudad = "3687238"; // Cartagena - 3687238 // Bogota - 3688689
bool id = false;
char servidor[] = "api.openweathermap.org";
String resultado;
int dato = 0;

String location;
String country;
int temperature = 0;

// Extras
class led
{
public:
    led(const int = 13);
    void encender();
    void apagar();
    void alerta(int = 1, int = 1);

private:
    int puerto; // pin
};

led::led(const int pin)
{
    puerto = pin;
    pinMode(puerto, OUTPUT);
}

void led::encender()
{
    digitalWrite(puerto, HIGH);
}

void led::apagar()
{
    digitalWrite(puerto, LOW);
}

void led::alerta(int repeticiones, int tiempo)
{
    for (int i = 0; i < repeticiones; i++)
    {
        digitalWrite(puerto, HIGH);
        delay(tiempo);
        digitalWrite(puerto, LOW);
        delay(tiempo);
    }
}

led indicador(18), interno(2);
int datos_sensor();
int clima();
#define BUZZER 5
#define BUZZER_C 0
void alerta(int = 0, int = 0, bool = false);

void setup()
{
    Serial.begin(115200);
    interno.apagar();

    // Pantall de inicio.
    pantalla.begin();
    pantalla.clrScr();
    pantalla.drawBitmap(0, 0, inicio, 128, 64);
    pantalla.update();
    WiFi.begin(nombre, clave);
    delay(3000);

    // WiFi
    if (WiFi.status() != WL_CONNECTED)
    {
        pantalla.clrScr();
        pantalla.setFont(SmallFont);
        pantalla.print("WiFi - OFF", CENTER, 32);
        pantalla.update();
        alerta(3, 500, true);
    }

    // Sensor
    Wire.begin(4, 15);
    Wire.setClock(400000);
    if (!sensor.begin(Wire, I2C_SPEED_FAST))
    {
        estado = false;
        Serial.println("Sensor Desconectado.");
        Serial.println("Comunicate con el desarrollador.");
        pantalla.clrScr();
        pantalla.setFont(SmallFont);
        pantalla.print("Sensor - OFF", CENTER, 32);
        pantalla.update();
        alerta(5, 300, true);
    }

    sensor.setup();
    sensor.setPulseAmplitudeRed(0x0A);
    sensor.setPulseAmplitudeGreen(0);
    // Pantalla
    pantalla.begin();

    // Bluetooth
    bluetooth.begin("RELOJ - CARDIO");
    // Clima
    pantalla.clrScr();
    pantalla.drawBitmap(0, 0, clima_ico, 128, 64);
    pantalla.update();
    dato = clima();
    delay(1000);
}

void loop()
{

    String sensor_datos = String(datos_sensor());

    long irValue = sensor.getIR();
    indicador.apagar();
    if (!estado || irValue < 50000)
    {
        sensor_datos = "000";
        indicador.encender();
        alerta(1, 500);
    }

    String valor = String(dato);

    if (valor == "0")
        valor = "00";

    pantalla.clrScr();
    pantalla.setFont(SmallFont);
    pantalla.print("BPM", 40, 0);
    pantalla.setFont(MediumNumbers);
    pantalla.print(sensor_datos, 0, 0);
    pantalla.drawCircle(29, 42, 2);
    pantalla.print(valor, 0, 40);
    pantalla.drawBitmap(70, 16, corazon, 32, 32);
    pantalla.update();
    bluetooth.println(sensor_datos);
}

int datos_sensor()
{
    long irValue = sensor.getIR();
    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;

            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }
    if (irValue < 50000)
        beatAvg = 0;
    return beatAvg;
}

int clima()
{
    if (client.connect(servidor, 80))
    {
        client.println("GET /data/2.5/weather?id=" + ciudad + "&units=metric&APPID=" + APIKEY);
        client.println("Host: api.openweathermap.org");
        client.println("User-Agent: ArduinoWiFi/1.1");
        client.println("Connection: close");
        client.println();
    }
    while (client.connected() && !client.available())
        delay(1);
    while (client.connected() || client.available())
    {
        char c = client.read();
        resultado = resultado + c;
    }
    client.stop();
    resultado.replace('[', ' ');
    resultado.replace(']', ' ');
    char jsonArray[resultado.length() + 1];
    resultado.toCharArray(jsonArray, sizeof(jsonArray));
    jsonArray[resultado.length() + 1] = '\0';
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, jsonArray);

    if (error)
    {
        Serial.print(F("deserializeJson()"));
        Serial.println(error.c_str());
    }

    String location = doc["name"];
    String country = doc["sys"]["country"];
    int temperature = doc["main"]["temp"];

    return temperature;
}

void alerta(int veces, int tiempo, bool estado)
{
    if (estado == false)
    {
        for (int i = 0; i < veces; i++)
        {
            tone(BUZZER, NOTE_DS8, tiempo, BUZZER_C);
            noTone(BUZZER, BUZZER_C);
            delay(100);
        }
    }
    else
    {
        for (int i = 0; i < veces; i++)
        {
            indicador.alerta(1, 100);
            tone(BUZZER, NOTE_DS8, tiempo, BUZZER_C);
            noTone(BUZZER, BUZZER_C);
        }
    }
};