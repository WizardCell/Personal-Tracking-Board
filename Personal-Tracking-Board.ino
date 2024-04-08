// REQUIRES the following Arduino libraries:
// ESP32 Firebase: https://github.com/Rupakpoddar/ESP32Firebase - 1.0.0
// ArduinoJson: https://arduinojson.org/ - 7.0.4
// WiFiManger: https://github.com/tzapu/WiFiManager - 2.0.17

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

#include <SPIFFS.h>


#include <WiFiManager.h>
#include <WiFi.h>

WiFiManager wifiManager;
bool wifiConnected = false;  // Flag to indicate whether WiFi is connected

#include <HTTPClient.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>

// Define a global JsonArray
JsonDocument usersDocument;  // Adjust the size as needed
JsonDocument jsonDocument;
JsonArray usersArray = jsonDocument.to<JsonArray>();

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


// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


const char* projectId = "trackingboard-b9106";
const char* apiKey = "AIzaSyBMhcicHhTv47Rgq2JGiJxfrl_bdUyzmbE";
const char* collectionPath = "users";  // Your Firestore collection name
#define TZ (+3 * 60 * 60)              //Timezone

#define NTP_SERVER "il.pool.ntp.org"

bool flasher = false;
uint8_t r = 0, g = 0, b = 0;
int h, m, s, d, yr;
uint8_t month, dow;
String text;
unsigned int NewRTCh = 24; // Number of hours in a single day.
unsigned int NewRTCm = 60; // Number of Mintues in a single hour.

time_t t;

const char* wd[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

unsigned long previousMillis = 0;
const long interval = 10000;  // Interval in milliseconds (1 seconds) for sending periodic HTTP and NTP requests.

// Digital pin for push-button
#define BUTTON_PIN 18

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
static bool noUsersCalled = false;

// standard colors
uint16_t myRED = dma_display->color333(7, 0, 0);
uint16_t myGREEN = dma_display->color333(0, 7, 0);
uint16_t myBLUE = dma_display->color333(0, 0, 7);
uint16_t myWHITE = dma_display->color333(3, 3, 3);
uint16_t myYELLOW = dma_display->color333(7, 7, 0);
uint16_t myCYAN = dma_display->color333(0, 7, 7);
uint16_t myMAGENTA = dma_display->color333(7, 0, 7);
uint16_t myShadow = dma_display->color333(4, 0, 7);
uint16_t myROSE = dma_display->color333(7, 0, 4);
uint16_t myBLACK = dma_display->color333(0, 0, 0);
uint16_t myCOLORS[9] = { myRED, myGREEN, myWHITE, myMAGENTA, myBLUE, myYELLOW, myCYAN, myShadow, myROSE };

void getTim() {
  if (flasher) {
    dma_display->fillRect(12, 1, 2, 2, myRED);
    dma_display->fillRect(12, 4, 2, 2, myRED);
    dma_display->fillRect(27, 1, 2, 2, myRED);
    dma_display->fillRect(27, 4, 2, 2, myRED);
    /*
    dma_display->setCursor(12, 0);
    dma_display->setTextSize(2);
    dma_display->setTextColor(myCOLORS[g]);
    dma_display->print(":");
    */
    b = random(0, 8);
  }
  
  dma_display->fillRect(29, 0, 13, 8, myBLACK);
  dma_display->setCursor(30, 0);
  dma_display->setTextSize(1);
  dma_display->setTextColor(myMAGENTA);
  //dma_display->setFont(&kongtext4pt7b);
  dma_display->printf("%02d", s);
  //dma_display->setFont();
  if (NewRTCm != m) {
    dma_display->setCursor(15, 0);
    //dma_display->setFont(&FreeMonoBold9pt7b);
    dma_display->setTextColor(myMAGENTA);
    dma_display->fillRect(14, 0, 13, 8, myBLACK);
    dma_display->printf("%02d", m);
    //dma_display->setFont();
    NewRTCm = m;
  }
  if (NewRTCh != h) {
    dma_display->setCursor(0, 0);
    //dma_display->setFont(&FreeMonoBold9pt7b);
    dma_display->setTextColor(myMAGENTA);
    dma_display->fillRect(0, 0, 12, 8, myBLACK);
    dma_display->printf("%02d", h);
    //dma_display->setFont();
    NewRTCh = h;
  }
}

void Date_text() {

  dma_display->fillRect(1, 10, 64, 10, myBLACK);
  //dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(1, 10);
  dma_display->setTextColor(myRED);
  dma_display->printf("%02d", d);
  dma_display->setTextColor(myGREEN);
  dma_display->printf("%s", months[month]);
  dma_display->setTextColor(myRED);
  dma_display->printf("%04d", yr);
  //dma_display->setFont();
}

void dofw_text() {
  text = wd[dow];
  uint16_t text_length = text.length();
  int xpo = (64 - text_length * 7) / 2;
  dma_display->setCursor(xpo, 0);
  dma_display->fillRect(0, 0, 64, 8, myBLACK);
  dma_display->setTextSize(1);
  //dma_display->setFont(&kongtext4pt7b);
  for (int y = 0; y < 9; y++) {
    r = random(0, 8);
    dma_display->setTextColor(myCOLORS[r]);
    dma_display->print(text[y]);
  }
  //dma_display->setFont();
}


void updateTime() {
  struct tm* tm;
  tm = localtime(&t);
  h = tm->tm_hour;
  m = tm->tm_min;
  s = tm->tm_sec;
  if (d != tm->tm_mday || month != tm->tm_mon || yr != tm->tm_year + 1900) {
    d = tm->tm_mday;
    dow = tm->tm_wday;
    month = tm->tm_mon;
    yr = tm->tm_year + 1900;
    Date_text();
  }
}

void saveJsonArray(const char* filename, JsonArray& arr) {
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open data.json file for writing");
    return;
  }

  serializeJson(arr, file);
  file.close();
}

bool readJsonArray(const char* filename) {
   if (!SPIFFS.exists(filename)){
      usersArray.clear();
      return true;
   }
  File file = SPIFFS.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open data.json file for reading");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(usersDocument, file);
  file.close();

  if (error) {
    Serial.print("deserializeJson() data.json failed: ");
    Serial.println(error.f_str());
    return false;
  }

  if (!usersDocument.is<JsonArray>()) {
    Serial.println("File does not contain a JSON array");
    return false;
  }
  usersArray = usersDocument.as<JsonArray>();

  return true;
}

void deleteJsonArray(const char* filename) {
 
  // Check if the file exists
  if (SPIFFS.exists(filename)) {
    // Delete the file
    if (SPIFFS.remove(filename)) {
      Serial.println("data.json deleted successfully (9600 baud)");
    } else {
      Serial.println("Failed to delete data.json (9600 baud)");
    }
  } else {
    Serial.println("data.json doesn't exist (9600 baud)");
  }

}




void MakeHttpRequest() {
  // Printing number of users in firebase collection
  HTTPClient http;

  // Our Firestore collection path and query parameters
  String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + collectionPath + "?key=" + String(apiKey);

  http.begin(firestoreUrl);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();


    //apply filter for json
    JsonDocument filter;
    filter["documents"][0]["fields"] = true;
    JsonDocument filtereddoc;
    DeserializationError error = deserializeJson(filtereddoc, payload, DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(11));
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }


    // Check if "documents" array exists and get its size (number of documents)
    if (filtereddoc.containsKey("documents")) {
      JsonArray data = filtereddoc["documents"];
      JsonDocument jsonUsers;
      JsonArray newArray = jsonUsers.to<JsonArray>();

      for (JsonVariant item : data) {
        JsonObject obj = newArray.createNestedObject();
        obj["name"] = item["fields"]["name"]["stringValue"];
        obj["vec"] = item["fields"]["vec"]["stringValue"];
        obj["email"] = item["fields"]["email"]["stringValue"];
      }
      saveJsonArray("/data.json", newArray);
      Serial.println("Users's array is Updated (9600 baud)" );

    } else {
      Serial.println("No documents found in the collection. (9600 baud)");
      deleteJsonArray("/data.json");
    }
  } else {
    Serial.println("Failed to read data from Firestore (9600 baud)");
    Serial.println(httpCode);
  }

  http.end();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if (pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if (pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void showIntroAnimation(int colorWheelOffset) {

  // Display "Welcome to Personal Tracking Board" in the middle-top
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(true);  // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(10, 14);  // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  const char* str = "Personal Tracking Board";
  for (w = 0; w < 9; w++) {
    dma_display->setTextColor(colorWheel((w * 32) + colorWheelOffset));
    dma_display->print(str[w]);
  }
  dma_display->setCursor(9, 22);  // next line
  for (w = 9; w < 18; w++) {
    dma_display->setTextColor(colorWheel((w * 32) + colorWheelOffset));
    dma_display->print(str[w]);
  }

  dma_display->setCursor(16, 30);  // next line
  for (w = 18; w < 23; w++) {
    dma_display->setTextColor(colorWheel((w * 32) + colorWheelOffset));
    dma_display->print(str[w]);
  }

  dma_display->setCursor(2, 4);
  dma_display->setTextColor(dma_display->color444(15, 15, 15));
  dma_display->println("Welcome To");
  dma_display->setCursor(2, 40);
  dma_display->setTextColor(dma_display->color444(15, 15, 15));
  dma_display->print("Please Set");
  dma_display->setCursor(8, 48);
  dma_display->print("The Wifi");


  // Jump a half character



  // Display "Configure Your Wifi" underneath
}

void connectToWiFi() {

  Serial.print("Attempting to connect to Network named: ");
  // Try to connect to saved WiFi credentials
  if (!wifiManager.autoConnect("ESP32-AP")) {
    Serial.println("Failed to connect and hit timeout");
    // Reset and try again, or maybe put it to deep sleep
    ESP.restart();
  } else {
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnectToWiFi() {
  // Check if WiFi connection is still active
  Serial.println("WiFi connection lost. Reconnecting...");
  // Try to reconnect
  WiFi.reconnect();
  delay(10000);  // Wait for 10 seconds before checking again
  // If still not connected, initiate WiFiManager configuration portal
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting WiFiManager configuration...");
    WiFi.disconnect();
    if (!wifiManager.startConfigPortal("OnDemand-AP")) {
      Serial.println("Failed to connect and hit timeout");
      // Reset and try again, or maybe put it to deep sleep
      // ESP.restart();
      // delay(1000);
    } else {
      Serial.println("Connected to WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
  }
}


#define MONTHS_NUM 12  //Number of months in a single year
#define INIT_X 1      // X coordinate for printing user's task progress.
#define INIT_Y 28     // Y coordinate for printing user's task progress.
#define FIRST_MONTH_LAST_DAY_X 60 // X coordinate for printing last day in the month.
void drawPixelsForDate(const char* numbersOfYear, int day, int month) {
  int initX = INIT_X;
  int initY = INIT_Y;
  int months[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  dma_display->fillRect(INIT_X, INIT_Y, 64, 35, myBLACK);

  for (int i=0;i<month-1;i++){
    switch(months[i]) {
    case 31:
      // 31 days month
    //  dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X, INIT_Y+3*i, myMAGENTA);
     // dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X, INIT_Y + 1+3*i, myMAGENTA);
      dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X+2, INIT_Y+3*i, myBLUE);
      dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X+2, INIT_Y + 1+3*i, myBLUE);
      break; // Exit the switch statement
      
    case 30:
      // 30 days month
      // dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-2, INIT_Y+3*i, myMAGENTA);
     // dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-2, INIT_Y + 1+3*i, myMAGENTA);
      for(int j=0;j<3;j++){
       dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X+j, INIT_Y+3*i, myBLUE);
      dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X+j, INIT_Y + 1+3*i, myBLUE);
      }
     
      break; // Exit the switch statement
      
    case 29:
      //29 days month
     //  dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-4, INIT_Y+3*i, myMAGENTA);
    //  dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-4, INIT_Y + 1+3*i, myMAGENTA);
    for(int j=0;j<5;j++){
      dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-2+j, INIT_Y+3*i, myBLUE);
      dma_display->drawPixel(FIRST_MONTH_LAST_DAY_X-2+j, INIT_Y + 1+3*i, myBLUE);
    }
      break; // Exit the switch statement
      
  }

  }


  for (int i = 27; i < 64; i += 3) {
    dma_display->fillRect(0, i, 64, 1, myBLUE);
  }
  dma_display->fillRect(0, 28, 1, 36, myBLUE);
  dma_display->fillRect(63, 28, 1, 36, myBLUE);

  char number = numbersOfYear[0];
  for (int i = 0; i < MONTHS_NUM; i++) {
    if (i == month) {
      break;
    }
    for (int j = 0; j < months[i]; j++) {
      if (i == month - 1 && j == day) {
        break;
      }
      int num = number - '0';
      if (num == 0) {
        dma_display->drawPixel(initX, initY, myGREEN);
        dma_display->drawPixel(initX, initY + 1, myGREEN);
    //    dma_display->drawPixel(initX + 1, initY, myGREEN);
    //    dma_display->drawPixel(initX + 1, initY + 1, myGREEN);
      } else {
        dma_display->drawPixel(initX, initY, myRED);
        dma_display->drawPixel(initX, initY + 1, myRED);
    //    dma_display->drawPixel(initX + 1, initY, myRED);
     //   dma_display->drawPixel(initX + 1, initY + 1, myRED);
      }
      numbersOfYear++;
      number = (*numbersOfYear);
      initX += 2;
    }
    initX = INIT_X;
    initY += 3;
  }
}

void displayUserName(const char* userName) {
  dma_display->fillRect(1, 19, 64, 8, myBLACK);
  dma_display->setCursor(1, 19);
  dma_display->setTextColor(myMAGENTA);
  dma_display->print(userName);
}

static char* userEmail = NULL;

void displayNoUsers(){
  dma_display->fillRect(0, 19, 64, 8, myBLACK);
  dma_display->fillRect(0, 27, 64, 37, myBLACK);
  dma_display->setCursor(8, 30);
  dma_display->setTextColor(myMAGENTA);
  dma_display->print("No Users");
  dma_display->setCursor(8, 40);
  dma_display->print("found in");
  dma_display->setCursor(8, 50);
  dma_display->print("Database");

}

void displaySpiffsReadError(){
  dma_display->fillRect(0, 19, 64, 8, myBLACK);
  dma_display->fillRect(0, 27, 64, 37, myBLACK);
  dma_display->setCursor(8, 30);
  dma_display->setTextColor(myMAGENTA);
  dma_display->print("Failed To");
  dma_display->setCursor(8, 40);
  dma_display->print("Read Data");
  dma_display->setCursor(8, 50);
  dma_display->print("From Storage");

}



void switchToNextUser(int userIndex) {
  if(usersArray == NULL){
    noUsersCalled = true;
    displayNoUsers();
    return;
  }
  if(usersArray.size() == 0){
    noUsersCalled = true;
    displayNoUsers();
    return;
  }

  int usersNum = usersArray.size();
  if (userIndex >= usersNum) {
    userIndex = userIndex % usersNum;
  }
  JsonObject obj = usersArray[userIndex].as<JsonObject>();
  const char* userVec = obj["vec"];
  const char* userName = obj["name"];
  const char* email = obj["email"];

  free(userEmail);
  userEmail = (char*) malloc(strlen(email) * sizeof(char) + 1);
  strcpy(userEmail, email);

  struct tm* tm;
  tm = localtime(&t);
  int day = tm->tm_mday;
  int month = tm->tm_mon + 1;


  Serial.println("day");
  Serial.println(day);
  Serial.println("month");
  Serial.println(month);
  Serial.println("username");
  Serial.println(userName);
  Serial.println("email");
  Serial.println(userEmail);

  displayUserName(userName);
  drawPixelsForDate(userVec, day, month);
}

void updateCurrentUser() {
   if(usersArray == NULL){
    displayNoUsers();
    noUsersCalled = true;
    return;
  }

  if(usersArray.size() == 0){
    displayNoUsers();
    noUsersCalled = true;
    return;
  }

  if(usersArray.size()>0 && noUsersCalled ){
    noUsersCalled = false;
    switchToNextUser(0);
    return;
  }

  if(userEmail == NULL)
  {
    return;
  }
  
  bool currentUserFound = false;
  int currentIndex;

  for (int userIndex = 0; userIndex < usersArray.size(); userIndex++) {
    JsonObject obj = usersArray[userIndex].as<JsonObject>();
    const char* currentUserEmail = obj["email"];
    Serial.println("-----------------");
    Serial.println(userEmail);
    Serial.println(currentUserEmail);
    int result = strcmp(userEmail, currentUserEmail);
    Serial.println(result);
    if (strcmp(userEmail, currentUserEmail) == 0) {
      Serial.println("I'm here");
       currentUserFound = true;
       currentIndex = userIndex;
      
    }
  }
  if(currentUserFound){
    switchToNextUser(currentIndex);
  }
  else{
    switchToNextUser(currentIndex+1);
  }
}

#define WIFI_ICON_X 52 // X coordinate for printing wIFI icon.
#define WIFI_ICON_Y 6 // Y coordinate for printing wIFI icon.

void displayWiFiIcon(uint16_t color) {

  // Draw WiFi icon
  dma_display->fillRect(WIFI_ICON_X, WIFI_ICON_Y, 2, 2, color);
  dma_display->fillRect(WIFI_ICON_X + 3, WIFI_ICON_Y - 2, 2, 4, color);
  dma_display->fillRect(WIFI_ICON_X + 6, WIFI_ICON_Y - 4, 2, 6, color);
  dma_display->fillRect(WIFI_ICON_X + 9, WIFI_ICON_Y - 6, 2, 8, color);

  // dma_display->fillRect(WIFI_ICON_X+8, WIFI_ICON_Y-8, 1, 10, color);
}


TaskHandle_t UpdateBoardTaskHandle;

void setup() {
  Serial.begin(115200);
  // Start serial communication at 9600 baud rate
  //Serial.begin(9600);
  // Add a delay to ensure serial communication is ready
  delay(100);
  mxconfig.gpio.e = 32;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90);  //0-255
  dma_display->clearScreen();

  //SPIFFS mounting
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  delay(500);



  // Initialize WiFiManager
  Serial.print("Attempting to connect to Network named: ");


  // Read the JSON array from the file
  if (!readJsonArray("/data.json")) {
    Serial.println("Failed to read JSON array from file");
    displaySpiffsReadError();
    return;
  }


  // Get the size of the JSON array
  int jsonArraySize = usersArray.size();
  Serial.print("Size of jsonArray: ");
  Serial.println(jsonArraySize);

  //Push Button Configuration
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  delay(500);

  // Show intro animation
  showIntroAnimation(0);
  // Try to connect to WiFi
  connectToWiFi();


  delay(3000);
  dma_display->clearScreen();
  //configTime(TZ*3600, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); // enable NTP
  //configTime(TZ, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
 // configTime(TZ, 0, "time.google.com");
  configTime(TZ, 0, NTP_SERVER);


  //  matrix.begin();                           // setup the LED matrix
  //  matrix.fillScreen(0);
  wifiManager.setConfigPortalBlocking(false);

  xTaskCreatePinnedToCore(
    UpdateBoard,            /* Function to implement the task */
    "UpdateBoardTask",      /* Name of the task */
    10000,                  /* Stack size in words */
    NULL,                   /* Task input parameter */
    0,                      /* Priority of the task */
    &UpdateBoardTaskHandle, /* Task handle. */
    0);                     /* Core where the task should run */


   
}

static int user_index = 0;
bool firstTimeLoop = true;
bool portalStarted = true;
void loop() {

   // update time using NTP protocol
  static time_t last_t;
  t = time(NULL);
  if (last_t != t) {
    updateTime();
    getTim();
    flasher = !flasher;
    last_t = t;
  }

  if(firstTimeLoop){
    switchToNextUser(user_index);
    user_index++;
    firstTimeLoop = false;
  }
  wifiManager.startConfigPortal("OnDemand-AP");
 
  wifiManager.process();

  // display Wifi status
  if (WiFi.status() == WL_CONNECTED) {
    displayWiFiIcon(myGREEN);
  } else if (WiFi.status() != WL_CONNECTED && flasher ) {
    displayWiFiIcon(myRED);
  }
    else if (WiFi.status() != WL_CONNECTED && (!flasher)){
      displayWiFiIcon(myBLACK);
    }


  // Read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);

  //Check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (buttonState == LOW) {
    Serial.println("Button is pressed");
    switchToNextUser(user_index);
  }
  delay(150);
  user_index++;
}

void UpdateBoard(void* parameter) {
  while (1) {
    unsigned long currentMillis = millis();
    // Check if it's time to make a http request
    if (currentMillis - previousMillis >= interval) {

      // Save the last time the the http request was sent
      previousMillis = currentMillis;

      // Send an http request to our firestore database that fetches our neccessary data
      if (WiFi.status() == WL_CONNECTED) {
        configTime(TZ, 0, "il.pool.ntp.org");
        MakeHttpRequest();
      }

      // Read the updated JSON array from the file
      if (!readJsonArray("/data.json")) {
        Serial.println("Failed to read the updated JSON array from file");
        return;
      }

      updateCurrentUser();
    }
  }
}
