#define BLYNK_TEMPLATE_ID"TMPL3JxepcboA"
#define BLYNK_TEMPLATE_NAME"plant"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Blynk Credentials
char auth[] = "Z1bigbdsPO9KSm_mIk8Zrqgy87lD5-oB";  // Enter Blynk token
char ssid[] = "Aj";         // Enter WiFi name
char pass[] = "briyaniii";     // Enter WiFi password


// Pin Definitions
#define MOISTURE_SENSOR A0
#define DHTPIN D7
#define DHTTYPE DHT11
#define TRIG_PIN D3
#define ECHO_PIN D4
#define MQ_SENSOR D5
#define LDR_SENSOR D6
#define RELAY_PIN D8

// Initialize Objects
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
    
    pinMode(MOISTURE_SENSOR, INPUT);
    pinMode(MQ_SENSOR, INPUT);
    pinMode(LDR_SENSOR, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Pump OFF by default
    
    dht.begin();
    //lcd.begin(16, 2);
    lcd.backlight();
}

// Function to read Ultrasonic Sensor
int getWaterLevel() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;  // Convert to cm
}

void loop() {
    Blynk.run();

    // Read Sensors
    int moistureValue = analogRead(MOISTURE_SENSOR);
    int moisturePercentage = map(moistureValue, 0, 1023, 100, 0);
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int waterLevel = getWaterLevel();
    bool mqStatus = digitalRead(MQ_SENSOR);
    bool ldrStatus = digitalRead(LDR_SENSOR);

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature);
    lcd.print("C ");
    lcd.setCursor(0, 1);
    lcd.print("Moist:");
    lcd.print(moisturePercentage);
    lcd.print("% ");
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidity:");
    lcd.print(humidity);
    lcd.print("% ");
    lcd.setCursor(0, 1);
    lcd.print("Water:");
    lcd.print(waterLevel);
    lcd.print("cm");
    delay(2000);

    // Send Data to Blynk
    Blynk.virtualWrite(V1, String(temperature) + "C, " + String(humidity) + "%");
    Blynk.virtualWrite(V2, moisturePercentage);
    Blynk.virtualWrite(V6, temperature);
    Blynk.virtualWrite(V5, humidity);
    Blynk.virtualWrite(V3, waterLevel);
    Blynk.virtualWrite(V4, mqStatus);
    Blynk.virtualWrite(V7, ldrStatus);

    // Auto Water Pump Control
    if (moisturePercentage < 30) {
        digitalWrite(RELAY_PIN, LOW);  // Pump ON
        Blynk.virtualWrite(V8, 1);
    } else {
        digitalWrite(RELAY_PIN, HIGH);  // Pump OFF
        Blynk.virtualWrite(V8, 0);
    }

    delay(5000); // Wait before next update
}

// Manual Water Pump Control via Blynk Button
BLYNK_WRITE(V8) {
    int value = param.asInt();
    digitalWrite(RELAY_PIN, value ? LOW : HIGH);
}