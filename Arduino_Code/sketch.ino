// Smart Air Quality Monitor with Blynk Integration
#define BLYNK_TEMPLATE_ID "Your-Blynk-Template-ID"
#define BLYNK_TEMPLATE_NAME "Smart Air Quality Monitor"
#define BLYNK_AUTH_TOKEN "Your-Blynk-Auth-Token"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials (Wokwi simulation)
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Pin definitions
#define DHTPIN 4
#define MQ135_PIN 34
#define LED_GREEN 25
#define LED_YELLOW 26
#define LED_RED 27
#define BUZZER_PIN 32
#define FAN_RELAY_PIN 33
#define DHTTYPE DHT22

// Objects
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

// Sensor variables
float temperature = 0;
float humidity = 0;
int gasValue = 0;
int pm25_estimated = 0;
int pm10_estimated = 0;
int co_estimated = 0;
int aqi = 0;
String healthCategory = "Initializing";
bool alertActive = false;

// AQI Thresholds
const int AQI_GOOD = 50;
const int AQI_SATISFACTORY = 100;
const int AQI_MODERATE = 200;
const int AQI_POOR = 300;

// ============================================
// SENSOR FUNCTIONS
// ============================================

int calculateAQI(int pm25) {
  if (pm25 <= 30) return map(pm25, 0, 30, 0, 50);
  else if (pm25 <= 60) return map(pm25, 31, 60, 51, 100);
  else if (pm25 <= 90) return map(pm25, 61, 90, 101, 200);
  else if (pm25 <= 120) return map(pm25, 91, 120, 201, 300);
  else if (pm25 <= 250) return map(pm25, 121, 250, 301, 400);
  else return map(pm25, 251, 500, 401, 500);
}

String getHealthCategory(int aqi, int pm25) {
  if (pm25 <= 30 && aqi <= AQI_GOOD) return "Good-Safe";
  else if (pm25 <= 60 && aqi <= AQI_SATISFACTORY) return "Satisfactory";
  else if (pm25 <= 90 && aqi <= AQI_MODERATE) return "Moderate";
  else if (pm25 <= 120 && aqi <= AQI_POOR) return "Poor-Caution";
  else if (pm25 <= 250) return "VeryPoor";
  else return "Severe-Danger";
}

void updateLEDs(int aqi) {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  
  if (aqi <= AQI_SATISFACTORY) {
    digitalWrite(LED_GREEN, HIGH);
  } else if (aqi <= AQI_MODERATE) {
    digitalWrite(LED_YELLOW, HIGH);
  } else {
    digitalWrite(LED_RED, HIGH);
  }
}

void checkAndAlert(int aqi, String category) {
  static bool alertSent = false;
  
  if (aqi > AQI_POOR) {
    if (!alertActive) {
      tone(BUZZER_PIN, 1000, 500);
      alertActive = true;
      
      Serial.println("\n🚨 ================================");
      Serial.println("🚨 AIR QUALITY ALERT!");
      Serial.println("🚨 Category: " + category);
      Serial.println("🚨 AQI: " + String(aqi));
      Serial.println("🚨 ================================\n");
      
      // Send Blynk notification
      if (!alertSent) {
        Blynk.logEvent("air_alert", "AQI: " + String(aqi) + " - " + category);
        alertSent = true;
      }
    }
  } else {
    alertActive = false;
    alertSent = false;
  }
}

void controlFan(int aqi) {
  if (aqi > AQI_MODERATE) {
    digitalWrite(FAN_RELAY_PIN, HIGH);
  } else {
    digitalWrite(FAN_RELAY_PIN, LOW);
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AQI:");
  lcd.print(aqi);
  lcd.print(" PM:");
  lcd.print(pm25_estimated);
  
  lcd.setCursor(0, 1);
  String displayCategory = healthCategory;
  if (displayCategory.length() > 16) {
    displayCategory = displayCategory.substring(0, 16);
  }
  lcd.print(displayCategory);
}

void printSerialData() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   SMART AIR QUALITY MONITORING SYSTEM  ║");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ Temperature    : "); 
  Serial.print(temperature, 1); 
  Serial.println(" °C");
  Serial.print("║ Humidity       : "); 
  Serial.print(humidity, 1); 
  Serial.println(" %");
  Serial.print("║ Gas Sensor     : "); 
  Serial.println(gasValue);
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ PM2.5 (Est)    : "); 
  Serial.print(pm25_estimated); 
  Serial.println(" µg/m³");
  Serial.print("║ PM10 (Est)     : "); 
  Serial.print(pm10_estimated); 
  Serial.println(" µg/m³");
  Serial.print("║ CO (Est)       : "); 
  Serial.print(co_estimated); 
  Serial.println(" ppm");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ AQI Score      : "); 
  Serial.println(aqi);
  Serial.print("║ Health Status  : "); 
  Serial.println(healthCategory);
  Serial.print("║ Alert Status   : "); 
  Serial.println(alertActive ? "🚨 ACTIVE" : "✓ Normal");
  Serial.println("╠════════════════════════════════════════╣");
  Serial.print("║ Fan Status     : ");
  Serial.println(digitalRead(FAN_RELAY_PIN) ? "ON 🌀" : "OFF");
  Serial.print("║ Blynk Status   : ");
  Serial.println(Blynk.connected() ? "Connected ✓" : "Disconnected ✗");
  Serial.println("╚════════════════════════════════════════╝\n");
}

// ============================================
// BLYNK FUNCTION - Reads sensors and sends to Blynk
// ============================================
void sendSensorDataToBlynk() {
  // Read DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ DHT sensor read error!");
    temperature = 25.0;
    humidity = 50.0;
  }
  
  // Read MQ-135
  gasValue = analogRead(MQ135_PIN);
  
  // Estimate pollutants
  pm25_estimated = map(gasValue, 0, 4095, 10, 300);
  pm10_estimated = pm25_estimated * 1.5;
  co_estimated = map(gasValue, 0, 4095, 50, 500);
  
  pm25_estimated = constrain(pm25_estimated, 0, 500);
  pm10_estimated = constrain(pm10_estimated, 0, 600);
  co_estimated = constrain(co_estimated, 0, 1000);
  
  // Calculate AQI
  aqi = calculateAQI(pm25_estimated);
  healthCategory = getHealthCategory(aqi, pm25_estimated);
  
  // Update outputs
  updateLEDs(aqi);
  checkAndAlert(aqi, healthCategory);
  controlFan(aqi);
  updateLCD();
  printSerialData();
  
  // Send to Blynk
  Blynk.virtualWrite(V0, pm25_estimated);
  Blynk.virtualWrite(V1, pm10_estimated);
  Blynk.virtualWrite(V2, co_estimated);
  Blynk.virtualWrite(V3, temperature);
  Blynk.virtualWrite(V4, humidity);
  Blynk.virtualWrite(V5, aqi);
  Blynk.virtualWrite(V6, healthCategory);
  Blynk.virtualWrite(V7, alertActive ? 1 : 0);
  
  Serial.println("📤 Data sent to Blynk successfully!");
}

// ============================================
// SETUP
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n╔════════════════════════════════════════╗");
  Serial.println("║  🌬️  SMART AIR QUALITY MONITOR v3.0  ║");
  Serial.println("║     With Blynk Cloud Integration       ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // Initialize GPIO pins
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);
  
  Serial.println("✓ GPIO pins initialized");
  
  // Initialize sensors
  dht.begin();
  Serial.println("✓ DHT22 sensor initialized");
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Air Quality");
  lcd.setCursor(0, 1);
  lcd.print("Monitor v3.0");
  Serial.println("✓ LCD display initialized");
  
  delay(2000);
  
  // Connect to Blynk
  lcd.clear();
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);
  lcd.print("Blynk Cloud");
  
  Serial.println("\n[Blynk] Connecting to Blynk Cloud...");
  Blynk.begin(auth, ssid, pass);
  
  if (Blynk.connected()) {
    Serial.println("✓ Connected to Blynk Cloud!");
    lcd.clear();
    lcd.print("Blynk Connected!");
    delay(2000);
  } else {
    Serial.println("✗ Blynk connection failed!");
    lcd.clear();
    lcd.print("Blynk Failed!");
    delay(2000);
  }
  
  lcd.clear();
  lcd.print("System Ready!");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring...");
  
  Serial.println("\n[SYSTEM] ✓ Initialization complete!");
  Serial.println("[SYSTEM] 📊 Starting monitoring...\n");
  
  // Setup timer to read sensors and send to Blynk every 3 seconds
  timer.setInterval(3000L, sendSensorDataToBlynk);
  
  delay(1500);
  
  // Initial reading
  sendSensorDataToBlynk();
}

// ============================================
// MAIN LOOP
// ============================================
void loop() {
  Blynk.run();
  timer.run();
}