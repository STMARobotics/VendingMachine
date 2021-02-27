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

int buttonMain = 4;
int buttonRue = 0;
int buttonMeduim = 1;
int buttonHard = 2;
int buttonYes = 3;

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

bool to_right = true;
int block_width = 30;
int block_height = 4;
int block_speed = 950;
int blockLevel = 0;
bool isPressable = true;
bool isRunning = true;

int lastBW = 16;
int lastX = 8;
int margin_r = 25;
int margin_l = 8;

int overhang = 0;

int r1 = 0;
int g1 = 0;
int b1 = 7;

int r2 = 7;
int g2 = 7;
int b2 = 0;

int r = 0;
int g = 0;
int b = 0;

int bw_reset = 0;
int letterDelay = 200;

const char endText[][10] PROGMEM = {"G", "A", "M", "E", "O", "V", "E", "R", "Y", "O", "U", "W", "I", "N"};
const char scoreText[] PROGMEM = "LVL";
const char score[][10] PROGMEM = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"};


void setup() {
  
  bw_reset = block_width;
  matrix.begin();
  matrix.setRotation(1);
  matrix.setTextWrap(true);
  matrix.setTextSize(4);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  pinMode(buttonMain, INPUT);
  block_speed = 1000 - block_speed;

  startGame();
}

void loop() {
  if (isRunning == true)
  {
    if (to_right == true)
    {
      matrix.fillRect(block_width, 64 - (block_height*2) - (blockLevel * block_height), 1, block_height,  matrix.Color333(0, 0, 0));
      for (uint8_t w = 0; w < 33 - block_width; w++)
      {
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(r,g,b));
        
        matrix.fillRect(w - 1,  64 - (block_height*2) - (blockLevel * block_height), 1, block_height, matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
      }
      to_right = false;
    }
    
    else 
    {
      for (uint8_t w = 31-block_width; w > 0; w = w - 1)
      { 
        matrix.fillRect(w, 64 - (block_height*2) - (blockLevel * block_height), block_width, block_height, matrix.Color333(r,g,b));
      
        matrix.fillRect(w+block_width, 64 - (block_height*2) - (blockLevel * block_height), 1, block_height,  matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
        
        to_right = true;
      }
    }
  }
}

void buttonCheck(uint8_t w) {
  if (digitalRead(buttonMain) == LOW && isPressable == true && isRunning == true)
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
    increaseLevel();
  }
  
  if (blockLevel > (54 / block_height) - 2)
  {
    win();
  }
  
  else if (digitalRead(buttonMain) == HIGH && isRunning == true)
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
  delay(3000);
  crumble();
  startGame();
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
  startGame();
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

void startGame()
{
  //setDifficulty(0);
  blockLevel = 0;
  block_width = bw_reset;
  margin_l = (32-block_width)/2;
  margin_r = margin_l + block_width + 1;
  overhang = 0;
  isRunning = true;
  matrix.fillRect(margin_l, 64 - block_height, block_width, block_height, matrix.Color333(r1,g1,b1));
  r = r2;
  g = g2;
  b = b2;
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

void increaseLevel()
{
    blockLevel = blockLevel + 1;
   if(blockLevel % 2 == 1)
    {
      r = r1;
      g = g1;
      b = b1;
    }

    else
    {
      r = r2;
      g = g2;
      b = b2;
    }
     
    if (block_width < 1)
        {
          gameOver();
          return;
        }
    showScore();
}

void setDifficulty(int diff)
{
  if (diff == 0)
  {
    block_speed = block_speed- 960;
    block_width = 16;
    block_height = 4;
  }
  else if (diff == 1)
  {
    block_speed = block_speed- 960;
    block_width = 14;
    block_height = 2;
  }
  else if (diff == 2)
  {
    block_speed = block_speed- 975;
    block_width = 10;
    block_height = 2;
  }
  else if (diff == 3)
  {
    block_speed = block_speed- 990;
    block_width = 8;
    block_height = 1;
  }
}
