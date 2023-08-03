// Team VOVO Circuit
// Smart Irrigation System
// Version 1.0.2 
// Arduino IDE 2.1.1


// Blynk FIRMWARE CONFIGURATION
#define BLYNK_TEMPLATE_ID "TMPL6tQCwAGbZ"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System"
#define BLYNK_AUTH_TOKEN "GVJc7ErICorh5NpgjKfot_PVyO_bUU00"
#define BLYNK_PRINT Serial
#define DHTTYPE DHT22 
// Component pins
#define soil A0     //A0 Soil Moisture Sensor
#define PIR D5      //D5 PIR Motion Sensor
#define RELAY_PIN_1       D3   //D3 Relay
#define PUSH_BUTTON_1     D7   //D7 Button
#define VPIN_BUTTON_1    V12 
// Library files
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <DHT.h>


/* ESP8266 Board Connections

Relay. D3
Btn.   D7
Soil.  A0
PIR.   D5
SDA.   D2
SCL.   D1
Temp.  D4

*/


//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Wifi Information

char auth[] = "GVJc7ErICorh5NpgjKfot_PVyO_bUU00";  //Blynk Auth token
char ssid[] = "Siddiq Sazzad";  //WIFI SSID
char pass[] = "lol@#$789";  //WIFI Password

// DHT Information

uint8_t DHTPin = D4;
DHT dht(DHTPin, DHTTYPE);
BlynkTimer timer;


int PIR_ToggleValue = 0;

void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;


//Create three variables for pressure
double T, P;
char status;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  pinMode(PIR, INPUT);

 pinMode(RELAY_PIN_1, OUTPUT);
 digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(DHTPin, INPUT);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("   Connecting  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  
  lcd.clear();// Clear the preivous one :)
   lcd.setCursor(0, 0);
   lcd.print("Smart Irrigation");
   lcd.setCursor(0, 1);
   lcd.print("     System     ");
  lcd.clear(); // Clear the preivous one :)
   lcd.setCursor(11, 1);
   lcd.print("W:OFF");
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT22sensor);
  timer.setInterval(500L, checkPhysicalButton);
}


//Get the DHT22 sensor values
void DHT22sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}


//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");

}

//Get the PIR sensor values
void PIRsensor() {
  bool value = digitalRead(PIR);
  if (value) {
    Blynk.logEvent("motion","WARNNG! Motion Detected!"); //Event Name
    WidgetLED LED(V5);
    LED.on();
  } else {
    WidgetLED LED(V5);
    LED.off();
  }  
  }

BLYNK_WRITE(V6)
{
 PIR_ToggleValue = param.asInt();  
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

void checkPhysicalButton() {
  static int lastButtonState = LOW;  // Keeps track of the previous button state
  int buttonState = digitalRead(PUSH_BUTTON_1);

  // Check if the button state has changed
  if (buttonState != lastButtonState) {
    delay(50);  // Debounce delay to avoid rapid toggling

    // Check the button state again after the debounce delay
    buttonState = digitalRead(PUSH_BUTTON_1);

    // If the button is pressed (LOW state), update the relay state and button widget
    if (buttonState == LOW) {
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
  }

  // Update lastButtonState for the next iteration
  lastButtonState = buttonState;
}



void loop() {

  // Motor Automatic On/Off
    if (PIR_ToggleValue == 1)
    {
    lcd.setCursor(5, 1);
    lcd.print("M:ON ");
      PIRsensor();
      }
     else
     {
    lcd.setCursor(5, 1);
    lcd.print("M:OFF");
    WidgetLED LED(V5);
    LED.off();
     }

if (relay1State == HIGH)
{
  lcd.setCursor(11, 1);
  lcd.print("W:ON ");
  }
  else if (relay1State == LOW)
  {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
    }
    
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer

  }
