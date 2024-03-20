// REQUIRES the following Arduino libraries:
// - ESP32 Firebase: https://github.com/Rupakpoddar/ESP32Firebase
// - Arduino Json: https://github.com/Rupakpoddar/ESP32Firebase

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#include <WiFiManager.h>
#include <WiFi.h>

 WiFiManager wifiManager;

#include <HTTPClient.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

// Create an instance of the matrix panel
MatrixPanel_I2S_DMA* dma_display = nullptr;
#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1   // Total number of panels chained one to another

// Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,  // module width
    PANEL_RES_Y,  // module height
    PANEL_CHAIN   // Chain length
  );

  


#define DATABASE_URL "https://boardd-7e40f-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyCUkWepwXNQHOU3SlbSzbXqOhd4Ib49ekQ"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//char ssid[] = "Iphone Haifa";  // Name of wifi network
//char pass[] = "1234567890";              // Password of wifi network
const char* projectId = "boardd-7e40f";
const char* apiKey = "AIzaSyCUkWepwXNQHOU3SlbSzbXqOhd4Ib49ekQ";
const char* collectionPath = "users";  // Your Firestore collection name
#define TZ (+2 * 60 * 60)              //Timezone

bool flasher = false;
uint8_t r = 0, g = 0, b = 0;
int h, m, s, d, yr;
uint8_t month, dow;
String text;
unsigned int NewRTCh = 24;
unsigned int NewRTCm = 60;
unsigned int NewRTCs = 10;
time_t t;

const char* wd[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

unsigned long previousMillis = 0;
const long interval = 15000;  // Interval in milliseconds (1 seconds)

// Digital pin for push-button
#define BUTTON_PIN 18

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

// standard colors
uint16_t myRED = dma_display->color333(7, 0, 0);
uint16_t myGREEN = dma_display->color333(0, 7, 0);
uint16_t myBLUE = dma_display->color333(0, 0, 7);
uint16_t myWHITE = dma_display->color333(7, 7, 7);
uint16_t myYELLOW = dma_display->color333(7, 7, 0);
uint16_t myCYAN = dma_display->color333(0, 7, 7);
uint16_t myMAGENTA = dma_display->color333(7, 0, 7);
uint16_t myShadow = dma_display->color333(4, 0, 7);
uint16_t myROSE = dma_display->color333(7, 0, 4);
uint16_t myBLACK = dma_display->color333(0, 0, 0);
uint16_t myCOLORS[9] = { myRED, myGREEN, myWHITE, myMAGENTA, myBLUE, myYELLOW, myCYAN, myShadow, myROSE };

void getTim()
{
  if (flasher)
  {
    dma_display->fillRect(54, 10, 10, 6, myBLACK);
    dma_display->setCursor(19, 8);
    dma_display->setTextSize(2);
    dma_display->setTextColor(myCOLORS[g]);
    dma_display->print(":");
    b = random(0, 8);
  }
  if (NewRTCs != s / 10)
  {
    dma_display->fillRect(51, 17, 13, 7, myBLACK);
    NewRTCs = s / 10;
    dofw_text();
  }
  else
  {
    dma_display->fillRect(58, 17, 6, 7, myBLACK);
  }
  dma_display->fillRect(51, 16, 7, 12, myBLACK);
  dma_display->setCursor(51, 16);
  dma_display->setTextSize(1);
  dma_display->setTextColor(myCOLORS[r]);
  //dma_display->setFont(&kongtext4pt7b);
  dma_display->printf("%02d", s);
  //dma_display->setFont();
  if (NewRTCm != m)
  {
    dma_display->setCursor(26, 14);
    //dma_display->setFont(&FreeMonoBold9pt7b);
    dma_display->setTextColor(myMAGENTA);
    dma_display->fillRect(26, 8, 25, 15, myBLACK);
    dma_display->printf("%02d", m);
    //dma_display->setFont();
    NewRTCm = m;
  }
  if (NewRTCh != h)
  {
    dma_display->setCursor(0, 14);
    //dma_display->setFont(&FreeMonoBold9pt7b);
    dma_display->setTextColor(myMAGENTA);
    dma_display->fillRect(0, 8, 24, 15, myBLACK);
    dma_display->printf("%02d", h);
    //dma_display->setFont();
    NewRTCh = h;
  }
}

void Date_text()
{
  dma_display->setCursor(1, 23);
  dma_display->fillRect(0, 23, 64, 8, myBLACK);
  //dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextColor(myRED);
  dma_display->printf("%02d", d);
  dma_display->setTextColor(myGREEN);
  dma_display->printf("%s", months[month]);
  dma_display->setTextColor(myRED);
  dma_display->printf("%04d", yr);
  //dma_display->setFont();
}

void dofw_text()
{
  text = wd[dow];
  uint16_t text_length = text.length();
  int xpo = (64 - text_length * 7) / 2;
  dma_display->setCursor(xpo, 0);
  dma_display->fillRect(0, 0, 64, 8, myBLACK);
  dma_display->setTextSize(1);
  //dma_display->setFont(&kongtext4pt7b);
  for (int y = 0; y < 9; y++)
  {
    r = random(0, 8);
    dma_display->setTextColor(myCOLORS[r]);
    dma_display->print(text[y]);
  }
  //dma_display->setFont();
}



void setup()
{
  Serial.begin(115200);

  mxconfig.gpio.e = 32;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  // Show intro animation
  //  showIntroAnimation();

 //Push Button Configuration
  pinMode(BUTTON_PIN, INPUT_PULLUP);

   // Try to connect to WiFi
   connectToWiFi();
   

  //configTime(TZ*3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); // enable NTP
  //configTime(TZ, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  configTime(TZ, 0, "time.google.com");

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90);  //0-255
  dma_display->clearScreen();

  //  matrix.begin();                           // setup the LED matrix
  //  matrix.fillScreen(0);
}

void updateTime()
{
  struct tm* tm;
  tm = localtime(&t);
  h = tm->tm_hour;
  m = tm->tm_min;
  s = tm->tm_sec;
  if (d != tm->tm_mday || month != tm->tm_mon || yr != tm->tm_year + 1900)
  {
    d = tm->tm_mday;
    dow = tm->tm_wday;
    month = tm->tm_mon;
    yr = tm->tm_year + 1900;
    Date_text();
  }
}

void MakeHttpRequest()
{
  // Printing number of users in firebase collection
  HTTPClient http;

  // Our Firestore collection path and query parameters
  String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + collectionPath + "?key=" + String(apiKey);

  http.begin(firestoreUrl);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
    String payload = http.getString();

    // Parse JSON response using Arduino JSON library
    DynamicJsonDocument doc(2048);  // Adjust the size based on your expected response size
    deserializeJson(doc, payload);

    // Access the data from the parsed JSON
    JsonObject root = doc.as<JsonObject>();

    // Check if "documents" array exists and get its size (number of documents)
    if (root.containsKey("documents"))
    {
      int documentCount = root["documents"].size();
      Serial.print("Number of documents in the collection: ");
      Serial.println(documentCount);
      dma_display->setCursor(2, 40);
      dma_display->setTextColor(dma_display->color444(15, 15, 15));
      dma_display->print("Users: ");
      dma_display->setCursor(40, 40);
      dma_display->setTextColor(dma_display->color444(15, 15, 15));
      dma_display->print("   ");
      dma_display->fillRect(40, 40, 22, 7, myBLACK);
      dma_display->setCursor(40, 40);
      dma_display->setTextColor(dma_display->color444(15, 15, 15));
      dma_display->print(documentCount);

    }
    else
    {
      Serial.println("No documents found in the collection.");
    }
  }
  else
  {
    Serial.println("Failed to read data from Firestore");
    Serial.println(httpCode);
  }

  http.end();
}

void showIntroAnimation() {

   // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90);  //0-255
  dma_display->clearScreen();

  // Display "Welcome to Personal Tracking Board" in the middle-top


    // Display "Configure Your Wifi" underneath
   
   
}

void connectToWiFi() {

    Serial.print("Attempting to connect to Network named: ");
    // Try to connect to saved WiFi credentials
    if (!wifiManager.autoConnect("ESP32-AP")) {
        Serial.println("Failed to connect and hit timeout");
        // Reset and try again, or maybe put it to deep sleep
        ESP.restart();
    }

    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void reconnectToWiFi(){
   // Check if WiFi connection is still active
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        // Try to reconnect
        WiFi.reconnect();
        delay(5000); // Wait for 5 seconds before checking again
        // If still not connected, initiate WiFiManager configuration portal
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Attempting WiFiManager configuration...");
            if (!wifiManager.startConfigPortal("ESP32-AP")) {
                Serial.println("Failed to connect and hit timeout");
                // Reset and try again, or maybe put it to deep sleep
                ESP.restart();
                delay(1000);
            }
            Serial.println("Connected to WiFi!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
        }

}}


void loop()
{
  // Check if WiFi connection is still active
  reconnectToWiFi();
        
  // Read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);

  //Check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (buttonState == LOW)
  {
    Serial.println("Button is pressed");
    //dma_display->drawPixel(30, 54, MatrixPanel_I2S_DMA::color565(255, 0, 255));
    dma_display->setCursor(15, 40);
    dma_display->setTextColor(dma_display->color444(0, 255, 255));
    dma_display->print("Khalid");
  
    
  }
  else
  {
    //Serial.println("Button is released");
    dma_display->fillRect(2, 40, 60, 60, myBLACK);
  }
 // delay(100);

  //if (WiFi.status() != WL_CONNECTED)
  //{
   // ESP.restart();
 // }

  static time_t last_t;
  t = time(NULL);
  if (last_t != t)
  {
    updateTime();
    getTim();
    flasher = !flasher;
    last_t = t;
  }

  unsigned long currentMillis = millis();

  // Check if it's time to make a http request
  if (currentMillis - previousMillis >= interval)
  {
    // Save the last time the the http request was sent
    previousMillis = currentMillis;

    // Send an http request to our firestore database that fetches our neccessary data
    MakeHttpRequest();
  }
}