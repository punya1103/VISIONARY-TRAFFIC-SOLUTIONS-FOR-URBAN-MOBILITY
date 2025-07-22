#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

WidgetBridge bridge1(V15);
int red = D0;
int green = D4;
int traffic_sensor = D5;
int traffic_value = 1;
int traffic_sensor1 = D6;
int traffic_value1 = 1;
int normal;
int normal_flag;
int traffic;
bool overrideGreen = false;  // Flag to track if the green light is manually overridden

char auth[] = "eIKnWB0uldhFu8DhWztWTD-JDgHhBp8p";
char ssid[] = "iot";
char pass[] = "12345678";

BLYNK_WRITE(V10) {
  int a = param.asInt();

  // When the button is pressed, override the traffic control and turn green on
  if (a == 1) {
    overrideGreen = true;
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    Blynk.virtualWrite(V0, "Green Light ON - Manual Override");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AMBULANCE APPROACH");
    lcd.setCursor(0,1);
    lcd.print("TRAFFIC GREEN");
  }
  // When the button is released, resume traffic control logic
  else {
    overrideGreen = false;
    Blynk.virtualWrite(V0, "Manual Override Off");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DENSITY BASED");
  lcd.setCursor(0, 1);
  lcd.print("TRAFFIC CONTROL");
  delay(2000);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(traffic_sensor, INPUT);
  pinMode(traffic_sensor1, INPUT);

  Blynk.begin(auth, ssid, pass, "18.201.3.122", 8080);
  timer.setInterval(1000L, main_fun);
}

void main_fun() {
  // If the green light is manually overridden (button pressed), ignore traffic sensor readings
  if (overrideGreen) {
    return;  // Skip the normal traffic control logic if green is overridden
  }

  if (traffic_value == 0) {
    normal_flag = 0;
    traffic = 0;
    traffic_value = 1;
    while (traffic <= 15) {
      traffic_value1 = digitalRead(traffic_sensor1);
      traffic += 1;
      Serial.println("Ir 1 Count:" + String(traffic));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time:" + String(traffic));
      lcd.setCursor(0, 1);
      lcd.print("Medium Traffic");
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      Blynk.virtualWrite(V2, "MEDIUM TRAFFIC!!!");
      delay(1000);
    }
  }
  else if (traffic_value1 == 0) {
    normal_flag = 0;
    traffic = 0;
    traffic_value1 = 1;
    while (traffic <= 20) {
      traffic += 1;
      Serial.println("Ir 2 Count:" + String(traffic));
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time:" + String(traffic));
      lcd.setCursor(0, 1);
      lcd.print("DENSE Traffic");
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      Blynk.virtualWrite(V2, "DENSE TRAFFIC!!!");
      delay(1000);
    }
  }
  else {
    normal += 1;
    Serial.println("Count:" + String(normal));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time:" + String(normal));
    lcd.setCursor(0, 1);
    lcd.print("Normal Traffic");
    Blynk.virtualWrite(V2, "NORMAL Traffic");

    if (normal >= 10) {
      traffic_value = digitalRead(traffic_sensor);
      normal_flag = !normal_flag;
      normal = 0;
    }

    if (normal_flag == 0) {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    }
    else {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
    }
  }
}


void loop() {
  Blynk.run();
  timer.run();
}
