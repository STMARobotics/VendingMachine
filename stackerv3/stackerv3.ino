// testshapes demo for RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x64 RGB LED matrix.

// NOTE THIS CAN ONLY BE USED ON A MEGA! NOT ENOUGH RAM ON UNO!

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Keyboard.h>

#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define clear() fillScreen(0)

#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

int buttonPin = 4;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

bool to_right = true;
int block_width = 16;
int block_height = 2;
int block_speed = 970;
int blockLevel = 0;
bool isPressable = true;
bool isRunning = true;

int lastBW = 16;
int lastX = 8;
int margin_r = 25;
int margin_l = 8;

int overhang = 0;

int r = 0;
int g = 0;
int b = 7;

int bw_reset = 0;
int letterSpace = 2;
int letterDelay = 200;

//const char endText[][10] PROGMEM = {"GAME", "OVER", "YOU", "WIN"};
const char endText[][10] PROGMEM = {"G", "A", "M", "E", "O", "V", "E", "R", "Y", "O", "U", "W", "I", "N"};
const char scoreText[] PROGMEM = "LVL";
const char score[][10] PROGMEM = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"};


void setup() {
  
  bw_reset = block_width;
  matrix.begin();
  
  matrix.setRotation(1);
  
  block_speed = 1000 - block_speed;
  margin_l = (32-block_width)/2;
  margin_r = margin_l + block_width + 1;
  matrix.fillRect(margin_l, 64- block_height, block_width, block_height, matrix.Color333(7,7,0));
  matrix.fillRect(0, 9, 32, 1, matrix.Color333(7, 7, 7));
  
  matrix.setTextWrap(true);
  matrix.setTextSize(4);
  
  showScore();

  randomSeed(analogRead(0));

  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
}

void loop() {
  if (isRunning == true)
  {
    if (to_right == true)
    {
      for (uint8_t w = 0; w < 32 - block_width; w++)
      {
        if (blockLevel % 2 == 0)
        {
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(0,0,7));
        }
        
        else
        {
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(7,7,0));
        }
        
        if (w !=31)
        {
          matrix.fillRect(w - 1,  64 - (block_height*2) - (blockLevel * block_height), 1, block_height, matrix.Color333(0, 0, 0));
        }
        buttonCheck(w);
        delay(block_speed);
      }
      to_right = false;
    }
    
    else 
    {
      for (uint8_t w = 32-block_width; w > 0; w = w - 1)
      {
        if (blockLevel % 2 == 0)
        {
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(0,0,7));
        }
        
        else
        {
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(7,7,0));
        }
        
        matrix.fillRect(w+block_width, 64 - (block_height*2) - (blockLevel * block_height), 1, block_height,  matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
        
        to_right = true;
      }
    }
  }
}

void buttonCheck(uint8_t w) {
  if (digitalRead(buttonPin) == LOW && isPressable == true && isRunning == true)
  {
    overhang = abs(w-margin_l);
    block_width = block_width - overhang;
    if (w > margin_l)
    {
      matrix.fillRect(margin_r-1, 64 - (block_height*2) - (blockLevel * block_height), overhang, block_height, matrix.Color333(0,0,0));
      margin_l = w;
    }

    else if (w != margin_l)
    {
      matrix.fillRect(margin_l - overhang, 64 - (block_height*2) - (blockLevel * block_height), overhang, block_height, matrix.Color333(0,0,0));
      margin_r = w + block_width + overhang + 1;
    }

    blockLevel = blockLevel + 1;
    if (block_width < 1)
        {
          gameOver();
          return;
        }
    showScore();
  }
  
  if (blockLevel > (54 / block_height) - 2)
  {
    win();
  }
  
  else if (digitalRead(buttonPin) == HIGH && isRunning == true)
  {
    isPressable = true;
  }
  
  else
  {
    isPressable = false;
  }
}

void gameOver()
{
  for (uint8_t i = 63; i >=10; i= i -1)
  {
    matrix.fillRect(0, i,32,  1,matrix.Color333(7,7,0));
    delay(1);
  }
  isRunning = false;

  for(int i = 0; i < 8; i++)
  {
    showText(3 + (i*7) - (28 * (i/4)), 25 + (10 * (i/4)), 1, i, 0, 0, 7);
    delay(letterDelay);
  }
  //scrollText(0);
  delay(3000);
  crumble();
  restart();
}

void win()
{
  for (uint8_t i = 10; i <64; i++)
  {
    matrix.fillRect(0, i,32,  1,matrix.Color333(0,0,7));
    delay(1);
  }
  isRunning = false;

  for(int i = 0; i < 6; i++)
  {
    showText(6 + (i*7) - (21 * (i/3)), 25 + (10 * (i/3)), 1, i + 8, 7, 7, 0);
    delay(letterDelay);
  }
  delay(3000);
  crumble();
  restart();
}

void crumble()
{
  for (int i = 0; i <6000; i ++)
  {
    matrix.fillRect(random(0, 32), random(10,64), 1, 1, matrix.Color333(0,0,0));
  }
  matrix.fillScreen(0);
  delay(3000);
}

void restart()
{
  blockLevel = 0;
  block_width = bw_reset;
  margin_l = (32-block_width)/2;
  margin_r = margin_l + block_width + 1;
  overhang = 0;
  isRunning = true;
  matrix.fillRect(margin_l, 64 - block_height, block_width, block_height, matrix.Color333(7,7,0));
  showScore();
  matrix.fillRect(0, 9, 32, 1, matrix.Color333(7, 7, 7));
}

void showText(int x, int y, int s, int text, int r, int g, int b)
{
  matrix.setTextSize(s);
  matrix.setTextColor(matrix.Color333(r,g,b));
  matrix.setCursor(x, y);
  matrix.print(F2(endText[text]));
}

void showScore()
{
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 1);
  matrix.print(F2(scoreText));
  matrix.fillRect(20,1,15,7, matrix.Color333(0,0,0));
  matrix.setCursor(20, 1);
  matrix.print(F2(score[blockLevel]));
}

void scrollText(int text)
{
  
  matrix.setTextWrap(false);
  matrix.setTextSize(2);
  for (int i =31; i >-200; i = i - 1)
  {
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.setCursor(i, 12);
    matrix.print(F2(endText[text]));
    delay(50);
    matrix.setTextColor(matrix.Color333(7,0,0));
    matrix.setCursor(i+2, 12);
    matrix.print(F2(endText[text]));
  }
 
  
  
  matrix.setTextWrap(true);
}
