#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <TinyGPS++.h>
#include "MAX30100_PulseOximeter.h"
/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try
the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16
#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
#define REPORTING_PERIOD_MS 1000
#define RXD2 16
#define TXD2 17
HardwareSerial neogps(1);
60
TinyGPSPlus gps;
PulseOximeter pox;
uint32_t tsLastReport = 0;
static const unsigned char PROGMEM bitmap[] =
{ B00000000, B11000000,
B00000001, B11000000,
B00000001, B11000000,
B00000011, B11100000,
B11110011, B11100000,
B11111110, B11111000,
B01111110, B11111111,
B00110011, B10011111,
B00011111, B11111100,
B00001101, B01110000,
B00011011, B10100000,
B00111111, B11100000,
B00111111, B11110000,
B01111100, B11110000,
B01110000, B01110000,
B00000000, B00110000
};
void onBeatDetected()
{
Serial.println("Beat!");
display.drawBitmap( 60, 20, bitmap, 28, 28, 1);
display.display();
}
void setup() {
Serial.begin(9600);
// Show image buffer on the display hardware.
// Since the buffer is intialized with an Adafruit splashscreen
61
// internally, this will display the splashscreen.
delay(250); // wait for the OLED to power up
display.begin(i2c_Address, true); // Address 0x3C default
//display.setContrast (0); // dim display
display.display();
delay(2000);
// Clear the buffer.
display.clearDisplay();
// draw a single pixel
display.drawPixel(10, 10, SH110X_WHITE);
neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
// Show the display buffer on the hardware.
// NOTE: You _must_ call display after making any drawing commands
// to make them visible on the display hardware!
display.display();
delay(2000);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.println("Initializing pulse oximeter..");
display.display();
Serial.print("Initializing pulse oximeter..");
if (!pox.begin()) {
Serial.println("FAILED");
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
62
display.println("FAILED");
display.display();
for(;;);
} else {
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.println("SUCCESS");
display.display();
Serial.println("SUCCESS");
}
pox.setOnBeatDetectedCallback(onBeatDetected);
{
pinMode(Buzz, OUTPUT);
pinMode(LED, OUTPUT);
pinMode(vs, INPUT);
}
{
gsmSerial.begin(115200); // Setting the baud rate of GSM Module
Serial.begin(115200); // Setting the baud rate of Serial Monitor (Arduino)
Serial.println("Preparing to send SMS");
pinMode(3,OUTPUT);
}
}
void loop() {
pox.update();
if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
Serial.print("Heart BPM:");
Serial.print(pox.getHeartRate());
Serial.print("-----");
Serial.print("Oxygen Percent:");
63
Serial.print(pox.getSpO2());
Serial.println("\n");
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0,16);
display.println(pox.getHeartRate());
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 0);
display.println("Heart BPM");
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0, 30);
display.println("Spo2");
display.setTextSize(1);
display.setTextColor(1);
display.setCursor(0,45);
display.println(pox.getSpO2());
display.display();
tsLastReport = millis();
}
{
boolean newData = false;
for (unsigned long start = millis(); millis() - start < 1000;)
{
while (neogps.available())
{
if (gps.encode(neogps.read()))
{
newData = true;
64
}
}
}
//If newData is true
if(newData == true)
{
newData = false;
Serial.println(gps.satellites.value());
print_speed();
}
else
{
display.clearDisplay();
display.setTextColor(SH110X_WHITE);
display.setCursor(0, 0);
display.setTextSize(3);
display.print("No Data");
display.display();
}
}
{
int vib = digitalRead(vs);
if(vib == HIGH)
{
for(i=0;i<10;i++)
{
digitalWrite(Buzz, HIGH);
digitalWrite(LED, HIGH);
delay(500);
digitalWrite(Buzz, LOW);
digitalWrite(LED, LOW);
delay(100);
}
65
}
else{
digitalWrite(Buzz, LOW);
digitalWrite(LED, LOW);
}
}
{
int HeartBeat=analogRead(A0);
Serial.print("HeartBeat is: ");
Serial.println(HeartBeat);
int Lat,lon=digitalRead(8);
Serial.print("Lat,lon is: ");
Serial.println(Lat,lon);
delay(1000);
if(HeartBeat>60)
{
digitalWrite(3,HIGH);
SendMessage1();
}
else
{
digitalWrite(3,LOW);
}
if(Lat,lon==0)
{
digitalWrite(3,HIGH);
SendMessage2();
}
else
{
digitalWrite(3,LOW);
}
}
}
void print_speed()
66
{
display.clearDisplay();
display.setTextColor(SH110X_WHITE);
if (gps.location.isValid() == 1)
{
//String gps_speed = String(gps.speed.kmph());
display.setTextSize(1);
display.setCursor(25, 5);
display.print("Lat: ");
display.setCursor(50, 5);
display.print(gps.location.lat(),6);
display.setCursor(25, 20);
display.print("Lng: ");
display.setCursor(50, 20);
display.print(gps.location.lng(),6);
display.setCursor(25, 35);
display.print("Speed: ");
display.setCursor(65, 35);
display.print(gps.speed.kmph());
display.setTextSize(1);
display.setCursor(0, 50);
display.print("SAT:");
display.setCursor(25, 50);
display.print(gps.satellites.value());
display.setTextSize(1);
display.setCursor(70, 50);
display.print("ALT:");
display.setCursor(95, 50);
display.print(gps.altitude.meters(), 0);
67
display.display();
}
else
{
display.clearDisplay();
display.setTextColor(SH110X_WHITE);
display.setCursor(0, 0);
display.setTextSize(3);
display.print("No Data");
display.display();
}
}
void SendMessage1()
{
Serial.println("Setting the GSM in text mode");
gsmSerial.println("AT+CMGF=1\r");
delay(2000);
Serial.println("Sending SMS to the desired phone number!");
gsmSerial.println("AT+CMGS=\"+918309316891\"\r");
// Replace x with mobile number
delay(2000);
gsmSerial.println("GAS LEAKAGE OCCURRED"); // SMS Text
delay(200);
gsmSerial.println((char)26); // ASCII code of CTRL+Z
delay(2000);
}
void SendMessage2()
{
Serial.println("Setting the GSM in text mode");
gsmSerial.println("AT+CMGF=1\r");
delay(2000);
Serial.println("Sending SMS to the desired phone number!");
gsmSerial.println("AT+CMGS=\"+918309316891\"\r");
68
// Replace x with mobile number
delay(2000);
gsmSerial.println("I AM IN TROUBLE"); // SMS Text
delay(200);
gsmSerial.println((char)26); // ASCII code of CTRL+Z
delay(2000);
}
 
