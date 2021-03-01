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
int block_width = 8;
int block_height = 1;
int block_speed = 990;
int blockLevel = 0;
bool isPressable = true;
bool isRunning = false;

int lastBW = 16;
int lastX = 8;
int margin_r = 25;
int margin_l = 8;

int overhang = 0;

int r1 = 0;
int g1 = 0;
int b1 = 7;

int r2 = 7;
int g2 = 4;
int b2 = 0;

int r = 0;
int g = 0;
int b = 0;

int bw_reset = 0;
int letterDelay = 200;
int diff = 0;

int b_speed[] = {950, 960, 975, 990};
int b_height[] = {4, 2, 2, 1};
int b_width[] = {16, 14, 10, 8};

int dc_r[] = {0, 0, 7, 7};
int dc_g[] = {0, 7, 4, 0};
int dc_b[] = {7, 0, 0, 0};
int top_bar[] = {3, 1, 1, 1};

int sw = 6; //stripe width

const char endText[][10] PROGMEM = {"G", "A", "M", "E", "O", "V", "E", "R", "Y", "O", "U", "W", "I", "N"};
const char scoreText[] PROGMEM = "LVL";
const char score[][10] PROGMEM = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32"};


void setup() {
  
  bw_reset = b_width[diff];
  matrix.begin();
  matrix.setRotation(1);
  matrix.setTextWrap(true);
  matrix.setTextSize(4);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  pinMode(buttonMain, INPUT);
  splash();
  //startGame();
}

void loop() {
  if (isRunning == true)
  {
    if (to_right == true)
    {
      matrix.fillRect(b_width[diff], 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff],  matrix.Color333(0, 0, 0));
      for (uint8_t w = 0; w < 33 - b_width[diff]; w++)
      {
        matrix.fillRect(w, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), b_width[diff], b_height[diff], matrix.Color333(r,g,b));
        
        matrix.fillRect(w - 1,  64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff], matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
      }
      to_right = false;
    }
    
    else 
    {
      for (uint8_t w = 31-b_width[diff]; w > 0; w = w - 1)
      { 
        matrix.fillRect(w, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), b_width[diff], b_height[diff], matrix.Color333(r,g,b));
      
        matrix.fillRect(w+b_width[diff], 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff],  matrix.Color333(0, 0, 0));
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
    b_width[diff] = b_width[diff] - overhang;
    if (w > margin_l)
    {
      matrix.fillRect(margin_r-1, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), overhang, b_height[diff], matrix.Color333(0,0,0));
      margin_l = w;
    }

    else if (w != margin_l)
    {
      matrix.fillRect(margin_l - overhang, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), overhang, b_height[diff], matrix.Color333(0,0,0));
      margin_r = w + b_width[diff] + overhang + 1;
    }
    increaseLevel();
  }
  
  if (blockLevel > (54 / b_height[diff]) - 2)
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
  splash();
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
  splash();
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
  block_speed = 1000 - b_speed[diff];
  b_width[diff] = bw_reset;
  margin_l = (32-b_width[diff])/2;
  margin_r = margin_l + b_width[diff] + 1;
  overhang = 0;
  isRunning = true;
  matrix.fillRect(margin_l, 64 - b_height[diff], b_width[diff], b_height[diff], matrix.Color333(r1,g1,b1));
  r = r2;
  g = g2;
  b = b2;
  showScore();
  matrix.fillRect(0, 9, 32, top_bar[diff], matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
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
  matrix.setTextColor(matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
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
     
    if (b_width[diff] < 1)
        {
          gameOver();
          return;
        }
    showScore();
}

void setDifficulty(int d)
{
  diff = d-1;
}

void splash()
{

  //FILL WHITE
  matrix.fillScreen(matrix.Color333(0,0,3));
  
  //WING 7-LEFT
  matrix.fillRect(1, 19, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(2, 19, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(3, 19, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(4, 19, 8, 4, matrix.Color333(r2,g2,b2));
  
  //WING 7-RIGHT
  matrix.fillRect(30, 19, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(29, 19, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(28, 19, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(20, 19, 8, 4, matrix.Color333(r2,g2,b2));
  
  //WING 0-LEFT
  matrix.fillRect(3, 26, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(4, 26, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(5, 26, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(6, 26, 6, 4, matrix.Color333(r2,g2,b2));
  
  //WING 0-RIGHT
  matrix.fillRect(28, 26, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(27, 26, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(26, 26, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(20, 26, 6, 4, matrix.Color333(r2,g2,b2));

  //WING 2-LEFT
  matrix.fillRect(5, 33, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(6, 33, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(7, 33, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(8, 33, 4, 4, matrix.Color333(r2,g2,b2));
  
  //WING 2-RIGHT
  matrix.fillRect(26, 33, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(25, 33, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(24, 33, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(20, 33, 4, 4, matrix.Color333(r2,g2,b2));

  //WING 8-LEFT
  matrix.fillRect(8, 40, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(9, 40, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(10, 40, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(11, 40, 1, 4, matrix.Color333(r2,g2,b2));
  
  //WING 8-RIGHT
  matrix.fillRect(23, 40, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(22, 40, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(21, 40, 1, 3, matrix.Color333(r2,g2,b2));
  matrix.fillRect(20, 40, 1, 4, matrix.Color333(r2,g2,b2));

  //7
  matrix.fillRect(13, 18, 5, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(18, 19, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(17, 20, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(16, 21, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(15, 22, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(14, 23, 1, 1, matrix.Color333(r2,g2,b2));

  //0
  matrix.fillRect(14, 25, 4, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(14, 30, 4, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 26, 1, 4, matrix.Color333(r2,g2,b2));
  matrix.fillRect(18, 26, 1, 4, matrix.Color333(r2,g2,b2));
  
  //2
  matrix.fillRect(14, 32, 4, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 33, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(18, 33, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(17, 34, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(14, 35, 3, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 36, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 37, 6, 1, matrix.Color333(r2,g2,b2));

  //8
  matrix.fillRect(14, 39, 4, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 40, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(18, 40, 1, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(14, 41, 4, 1, matrix.Color333(r2,g2,b2));
  matrix.fillRect(13, 42, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(18, 42, 1, 2, matrix.Color333(r2,g2,b2));
  matrix.fillRect(14, 44, 4, 1, matrix.Color333(r2,g2,b2));

  //STRIPE

while(!isRunning)
{
  for (int k = 0; k < 12; k++)
  {
    for (int g = 0; g < 5; g ++)
    {
      for (int i = 0; i < 14; i ++)
      {
        matrix.fillRect((g*sw*2)-i-k, i, sw, 1, matrix.Color333(0,0,3));
        matrix.fillRect((g*sw*2)-i-k+sw, i, sw, 1, matrix.Color333(7,4,0));
        
        matrix.fillRect((g*sw*2)+i+k-26, i+50, sw, 1, matrix.Color333(0,0,3));
        matrix.fillRect((g*sw*2)+i+k+sw-26, i+50, sw, 1, matrix.Color333(7,4,0));
        if (digitalRead(buttonMain) == LOW)
        {
          isRunning = true;
          matrix.fillScreen(matrix.Color333(0,0,0));
          delay(2000);
          startGame();
          return;
        }
       }
    }
    delay(1);
  }
}
  

    
  
}
