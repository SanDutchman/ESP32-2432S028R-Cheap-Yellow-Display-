#include "WiFi.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <Arduino.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

int past_hour = 0;
int past_minute = 0;
int past_second = 0;

#define SSID "your ssid"
#define PASS "your password"

const long utcOffsetWinter = 3600; // Offset from UTC in seconds (3600 seconds = 1h) -- UTC+1 (Central European Winter Time)
const long utcOffsetSummer = 7200; // Offset from UTC in seconds (7200 seconds = 2h) -- UTC+2 (Central European Summer Time)
unsigned long lastupdate = 0UL;

int digit_color = TFT_CYAN;
const int background_color = TFT_BLACK;
const int text_color = TFT_WHITE;
const int color_array[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};

// Define NTP Client to get time
WiFiUDP udpSocket;
NTPClient ntpClient(udpSocket, "pool.ntp.org", utcOffsetWinter);

#define CYD_LED_BLUE 17
#define CYD_LED_RED 4
#define CYD_LED_GREEN 16
void setup() {
  Serial.begin(9600);
  pinMode(CYD_LED_BLUE, OUTPUT);
  pinMode(CYD_LED_RED, OUTPUT);
  pinMode(CYD_LED_GREEN, OUTPUT);
  digitalWrite(CYD_LED_BLUE, HIGH);
  digitalWrite(CYD_LED_RED, HIGH );
  digitalWrite(CYD_LED_GREEN, HIGH);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(background_color);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
  }
  ntpClient.begin();

  // LAYOUT
  // Scale binary
  tft.drawLine(5, 28, 120, 28, text_color);
  tft.drawLine(151, 28, 260, 28, text_color);
  tft.drawLine(5, 28, 5, 33, text_color);
  tft.drawLine(260, 28, 260, 33, text_color);
  tft.setCursor(128, 25);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("BIN");
  tft.setCursor(19, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("128");
  tft.setCursor(51, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("64");
  tft.setCursor(81, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("32");
  tft.setCursor(111, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("16");
  tft.setCursor(145, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("8");
  tft.setCursor(175, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("4");
  tft.setCursor(205, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("2");
  tft.setCursor(235, 35);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("1");
  // Hours screen
  tft.drawRoundRect(5, 50, 255, 55, 5, text_color);
  tft.drawRoundRect(265, 55, 50, 45, 5, text_color);
  tft.fillRect(200, 47, 45, 10, background_color);
  tft.setCursor(205, 47);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print(" HOUR");
  tft.fillRect(278, 47, 25, 10, background_color);
  tft.setCursor(283, 52);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("DEC");
  // Minutes screen
  tft.drawRoundRect(5, 115, 255, 55, 5, text_color);
  tft.drawRoundRect(265, 120, 50, 45, 5, text_color);
  tft.fillRect(200, 112, 45, 10, background_color);
  tft.setCursor(205, 112);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("MINUTE");
  tft.fillRect(278, 112, 25, 10, background_color);
  tft.setCursor(283, 117);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("DEC");
  // Seconds screen
  tft.drawRoundRect(5, 180, 255, 55, 5, text_color);
  tft.drawRoundRect(265, 185, 50, 45, 5, text_color);
  tft.fillRect(200, 177, 45, 10, background_color);
  tft.setCursor(205, 177);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("SECOND");
  tft.fillRect(278, 177, 25, 10, background_color);
  tft.setCursor(283, 182);
  tft.setTextColor(text_color);
  tft.setTextSize(1);
  tft.print("DEC");
}

void loop() {
  // Get time
  ntpClient.update();
  // Setup time variables
  int current_second = ntpClient.getSeconds();
  int current_minute = ntpClient.getMinutes();
  int current_hour = ntpClient.getHours();

  if (current_second > past_second) {
    print_seconds();
  }
  if (current_second < past_second) {
    for (int i = 0; i < 320; i += 5) {
      tft.fillRect(i, 0, 1, 20, background_color);
    }
    print_seconds();
  }
  if (current_minute != past_minute) {
// generate a random color between color_array
    random_color_generator();
    led_color_change();
    print_seconds();
    print_minutes();
    print_hours();
  }
  if (current_hour != past_hour) {
    print_hours();
  }
}

    void random_color_generator(){
          int random_number = random(6);
          if(color_array[random_number] == digit_color){
            random_color_generator();
          }
          else{
                digit_color = color_array[random_number];
          }
      }

void led_color_change() {
  if (digit_color == TFT_RED) {
    digitalWrite(CYD_LED_RED, LOW );
    digitalWrite(CYD_LED_GREEN, HIGH);
    digitalWrite(CYD_LED_BLUE, HIGH);
  }
  if (digit_color == TFT_GREEN) {
    digitalWrite(CYD_LED_RED, HIGH );
    digitalWrite(CYD_LED_GREEN, LOW);
    digitalWrite(CYD_LED_BLUE, HIGH);
  }
  if (digit_color == TFT_BLUE) {
    digitalWrite(CYD_LED_RED, HIGH );
    digitalWrite(CYD_LED_GREEN, HIGH);
    digitalWrite(CYD_LED_BLUE, LOW);
  }
  if (digit_color == TFT_CYAN) {
    digitalWrite(CYD_LED_RED, HIGH );
    digitalWrite(CYD_LED_GREEN, LOW);
    digitalWrite(CYD_LED_BLUE, LOW);
  }
  if (digit_color == TFT_YELLOW) {
    digitalWrite(CYD_LED_RED, LOW );
    digitalWrite(CYD_LED_GREEN, LOW);
    digitalWrite(CYD_LED_BLUE, HIGH);
  }
  if (digit_color == TFT_MAGENTA) {
    digitalWrite(CYD_LED_RED, LOW );
    digitalWrite(CYD_LED_GREEN, HIGH);
    digitalWrite(CYD_LED_BLUE, LOW);
  }
}

void print_seconds() {
  // Second indicator on top of screen
  int inticator_bars = map(ntpClient.getSeconds(), 0, 64, 0, 320);
  for ( int i = 0; i < inticator_bars; i += 5) {
    tft.fillRect(i, 0, 1, 20, digit_color);
  }
  // Print binary
  tft.fillRect(15, 190, 240, 40, background_color);
  tft.setCursor(15, 190);
  tft.setTextColor(digit_color);
  tft.setTextSize(5);
  for (int i = 7; i >= 0; i--) {
    bool m = bitRead(ntpClient.getSeconds(), i);
    tft.print(m, BIN);
  }
  // Print decimal
  tft.fillRect(274, 197, 35, 21, background_color);
  tft.setCursor(274, 197);
  tft.setTextColor(text_color);
  tft.setTextSize(3);
  tft.print(ntpClient.getSeconds());
  // past_second get new time (current_second)
  past_second = ntpClient.getSeconds();
}

void print_minutes() {
  // Print binary
  tft.fillRect(15, 125, 240, 40, background_color);
  tft.setCursor(15, 125);
  tft.setTextColor(digit_color);
  tft.setTextSize(5);
  for (int i = 7; i >= 0; i--) {
    bool m = bitRead(ntpClient.getMinutes(), i);
    tft.print(m, BIN);
  }
  // Print decimal
  tft.fillRect(274, 132, 35, 21, background_color);
  tft.setCursor(274, 132);
  tft.setTextColor(text_color);
  tft.setTextSize(3);
  tft.print(ntpClient.getMinutes());
  // past_minute get new time (current_minute)
  past_minute = ntpClient.getMinutes();
}

void print_hours() {
  // Print binary
  tft.fillRect(15, 60, 240, 40, background_color);
  tft.setCursor(15, 60);
  tft.setTextColor(digit_color);
  tft.setTextSize(5);
  for (int i = 7; i >= 0; i--) {
    bool m = bitRead(ntpClient.getHours(), i);
    tft.print(m, BIN);
  }
  // Print decimal
  tft.fillRect(274, 67, 35, 21, background_color);
  tft.setCursor(274, 67);
  tft.setTextColor(text_color);
  tft.setTextSize(3);
  tft.print(ntpClient.getHours() );
  // past_hour get new time (current_hour)
  past_hour = ntpClient.getHours() ;
}
