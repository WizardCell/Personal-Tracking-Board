
// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

// Digital pin for push-button
#define BUTTON_PIN 18

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status


#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
 
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

#define DAYS_OF_YEAR 366
#define MONTHS_NUM 12 //Number of months in a single year
#define INIT_X 1
#define INIT_Y 28
int months[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// Function to draw pixels for each day of the year
void drawPixelsForYear(const char* numbersOfYear) {
  int initX = INIT_X;
  int initY = INIT_Y;
  
      char number = numbersOfYear[0];
  for (int i = 0; i < MONTHS_NUM; i++) {
    for (int j = 0; j < months[i]; j++){
    
      int num = number - '0';
      if(num == 0)
      {
      dma_display->drawPixel(initX, initY, myGREEN);
      dma_display->drawPixel(initX, initY+1, myGREEN);
      dma_display->drawPixel(initX+1, initY, myGREEN);
      dma_display->drawPixel(initX+1, initY+1, myGREEN);
      }
      else{
        dma_display->drawPixel(initX, initY,myRED);
      dma_display->drawPixel(initX, initY+1, myRED);
      dma_display->drawPixel(initX+1, initY, myRED);
      dma_display->drawPixel(initX+1, initY+1, myRED);
      }
      numbersOfYear++;
       number = (*numbersOfYear);
      initX+=2;

    }
    initX = INIT_X;
    initY+=3;
}}

void drawPixelsForDate(const char* numbersOfYear, int day, int month) {
  int initX = INIT_X;
  int initY = INIT_Y;

      dma_display->fillRect(INIT_X, INIT_Y, 64, 35, myBLACK);

      for(int i=27;i<64;i+=3){
      dma_display->fillRect(0, i, 64, 1, myWHITE);
      }
      dma_display->fillRect(0, 28, 1, 36, myWHITE);
      dma_display->fillRect(63, 28, 1, 36, myWHITE);

      char number = numbersOfYear[0];
  for (int i = 0; i < MONTHS_NUM; i++) {
      if(i==month){
        break;
      }
    for (int j = 0; j < months[i]; j++){
      if(i==month-1 && j==day){
        break;
      }
      int num = number - '0';
      if(num == 0)
      {
      dma_display->drawPixel(initX, initY, myGREEN);
      dma_display->drawPixel(initX, initY+1, myGREEN);
      dma_display->drawPixel(initX+1, initY, myGREEN);
      dma_display->drawPixel(initX+1, initY+1, myGREEN);
      }
      else{
        dma_display->drawPixel(initX, initY,myRED);
      dma_display->drawPixel(initX, initY+1, myRED);
      dma_display->drawPixel(initX+1, initY, myRED);
      dma_display->drawPixel(initX+1, initY+1, myRED);
      }
      numbersOfYear++;
       number = (*numbersOfYear);
      initX+=2;

    }
    initX = INIT_X;
    initY+=3;
}}

void drawPixelsForDate2(const char* numbersOfYear, int day, int month) {
  int initX = INIT_X;
  int initY = INIT_Y;

      dma_display->fillRect(INIT_X, INIT_Y, 64, 35, myBLACK);
      /*
      //display white rows
      for(int i=27;i<64;i+=3){
      dma_display->fillRect(0, i, 64, 1, myWHITE);
      }
      dma_display->fillRect(0, 28, 1, 36, myWHITE);
      dma_display->fillRect(63, 28, 1, 36, myWHITE);

      //display white columns
      
      for(int i=0;i<64;i+=2){
      dma_display->fillRect(i, INIT_Y, 1, 36, myBLACK);
      }
      */
      char number = numbersOfYear[0];
  for (int i = 0; i < MONTHS_NUM; i++) {
      if(i==month){
        break;
      }
    for (int j = 0; j < months[i]; j++){
      if(i==month-1 && j==day){
        break;
      }
      int num = number - '0';
      if(num == 0)
      {
      dma_display->drawPixel(initX, initY, myGREEN);
      dma_display->drawPixel(initX, initY+1, myGREEN);
      //dma_display->drawPixel(initX+1, initY, myGREEN);
     // dma_display->drawPixel(initX+1, initY+1, myGREEN);
      }
      else{
        dma_display->drawPixel(initX, initY,myRED);
      dma_display->drawPixel(initX, initY+1, myRED);
      //dma_display->drawPixel(initX+1, initY, myRED);
     // dma_display->drawPixel(initX+1, initY+1, myRED);
      }
      numbersOfYear++;
       number = (*numbersOfYear);
      initX+=2;

    }
    initX = INIT_X;
    initY+=3;
}}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void drawText(int colorWheelOffset)
{
  
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(true); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(10, 14);    // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  const char *str = "Personal Tracking Board";
  for (w=0; w<9; w++) {
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
    dma_display->print(str[w]);
  }
  dma_display->setCursor(9, 22);    // next line
  for (w=9; w<18; w++) {
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
    dma_display->print(str[w]);
  }

   dma_display->setCursor(16, 30);    // next line
  for (w=18; w<23; w++) {
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
    dma_display->print(str[w]);
  }
  
  dma_display->setCursor(2, 4);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->println("Welcome To");
  
 // dma_display->setCursor(25, 32);
 // dma_display->setTextColor(dma_display->color444(15,15,15));
 // dma_display->println("MATRIX");

  dma_display->setCursor(9, 40);
  //dma_display->setTextSize(0.5);
  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->print("Wifi is");
  dma_display->setCursor(2, 48);
  dma_display->print("Connecting");
 // renderText(*dma_display, "MAR", 9, 40, dma_display->color565(255, 255, 255));
 // renderChar(*dma_display, 'B', 5, 50,dma_display->color565(255, 255, 255) );
 
  // Jump a half character
 


}

#define WIFI_ICON_X 52
#define WIFI_ICON_Y 6

void displayWiFiIcon(uint16_t color) {

  // Draw WiFi icon
    dma_display->fillRect(WIFI_ICON_X, WIFI_ICON_Y, 2, 2, color);
    dma_display->fillRect(WIFI_ICON_X+3, WIFI_ICON_Y-2, 2, 4, color);
    dma_display->fillRect(WIFI_ICON_X+6, WIFI_ICON_Y-4, 2, 6, color);
    dma_display->fillRect(WIFI_ICON_X+9, WIFI_ICON_Y-6, 2, 8, color);
   // dma_display->fillRect(WIFI_ICON_X+8, WIFI_ICON_Y-8, 1, 10, color);
}




void setup() {

  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 32;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->clearScreen();
  dma_display->fillScreen(myWHITE);

  //Push Button Configuration
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
  delay(500);

  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(500);

  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width()-1, dma_display->height()-1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width()-1, 0, 0, dma_display->height()-1, dma_display->color444(15, 0, 0));
  delay(500);

  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  delay(500);

  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
  delay(500);

  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));

//  drawText(0);
//  delay(2000);
/*
  dma_display->setCursor(8, 18);
  dma_display->setTextColor(colorWheel(0));
  dma_display->print("Hey Roni");
*/


  

  const char* numbersOfYear = "103020004000000000000030000050000050000000040000200002000000200000300004000000241000000030000000080000004000001000000000030000400000000000020000008000030000003000000000007000000000030000000030000000000040000000000000060000800090000000050000000000000090000006000000000000050000000060005000040005000000100000000000000300000000000000700000000000700000000000020000000000";
 //drawPixelsForYear(numbersOfYear);
 //drawPixelsForDate(numbersOfYear,31,12);
 // drawPixelsForDate2(numbersOfYear,31,12);
 /*
 delay(2000);
 drawPixelsForDate(numbersOfYear,1,1);
*/

}
 int i=0;
uint8_t wheelval = 0;
void loop() {

  displayWiFiIcon(myRED);
  
  if(i==3){
    i=0;
  }
  const char* myStrings[] = {"Hey Khalid", "Hey Mhmad", "Hey Roni"};
  int day[] = {1,15,30};
  int month[] ={1,6,12};
  const char* numbersOfYear = "103020004000000000000030000050000050000000040000200002000000200000300004000000241000000030000000080000004000001000000000030000400000000000020000008000030000003000000000007000000000030000000030000000000040000000000000060000800090000000050000000000000090000006000000000000050000000060005000040005000000100000000000000300000000000000700000000000700000000000020000000000";
  // Read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);
       if (buttonState == LOW)
    {
      dma_display->clearScreen();
      dma_display->fillRect(1, 18, dma_display->width(),10, myBLACK);
      dma_display->setCursor(1, 18);
  dma_display->setTextColor(colorWheel(0));
  dma_display->print(myStrings[i]);
    Serial.println("Button is pressed");
    drawPixelsForDate2(numbersOfYear,day[i],month[i]);
    }
    
   

    i++;
    
    delay(250); 
    

  
}
