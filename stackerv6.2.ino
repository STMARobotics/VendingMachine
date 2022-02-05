
// testshapes demo for RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x64 RGB LED matrix.

// NOTE THIS CAN ONLY BE USED ON A MEGA! NOT ENOUGH RAM ON UNO!

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Keyboard.h>
#include <Servo.h>
#include <EEPROM.h>
#include <ezButton.h>

#define OE   9
#define LAT 10
#define CLK 11
#define A   A0
#define B   A1
#define C   A2
#define D   A3
#define clear() fillScreen(0)

#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

int servosPins[] = {44,46,40,42};
int servoRunSpeeds[] = {80,80,80,80};
Servo servo_easy;
Servo servo_norm;
Servo servo_hard;
Servo servo_pain;

Servo servos[4];

ezButton limitSwitches[4] = {ezButton(45), ezButton(47), ezButton(41), ezButton(43)};

int buttonMain = 4;
int buttonEasy = 0;
int buttonMedium = 1;
int buttonHard = 2;
int buttonYes = 3;

int b1_light = 5;
int b2_light = 6;
int b3_light = 7;
int b4_light = 8;

int bMain_light = 13;

int lights[] = {5, 6, 7, 8};

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

bool to_right = true;
int block_width = 8;
int block_height = 1;
int block_speed = 990;
int blockLevel = 0;
bool isPressable = true;
bool isRunning = false;
bool nameEntered = false;
bool numEntered = false;

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
int diff = 3;
int diffSpacing = 7;

int b_speed[] = {950, 960, 975, 990};
int b_height[] = {4, 3, 2, 1};
int b_width[] = {16, 14, 10, 8};

int dc_r[] = {0, 0, 7, 7};
int dc_g[] = {0, 7, 4, 0};
int dc_b[] = {7, 0, 0, 0};
int top_bar[] = {3, 1, 1, 1};

float scoreWeights[] = {6999.9, 3307.7, 3461.5, 1735.8};

int sw = 6; //stripe width
long playerScore = 0L;
long lastPlayerScore =11111L;
int wallBounces = 0;

bool winGame = false;

byte nameCharOne = "A";
byte nameCharTwo = "A";
byte nameCharThree = "A";
byte nameCharFour = "A";
byte nameCharFive = "A";

byte teamCharOne = 1;
byte teamCharTwo = 1;
byte teamCharThree = 1;
byte teamCharFour = 1;

byte scoreCharOne = 1;
byte scoreCharTwo = 1;
byte scoreCharThree = 1;
byte scoreCharFour = 1;
byte scoreCharFive = 1;
int currentLetter = 0;
int currentNum = 0;
int cursorPosition = 0;

const char alphabet[][10] PROGMEM = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
const char endText[][10] PROGMEM = {"G", "A", "M", "E", "O", "V", "E", "R", "Y", "O", "U", "W", "I", "N"};
const char scoreText[] PROGMEM = "LVL";
const char digits[][10] PROGMEM = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
const char diffText[][10] PROGMEM = {"1EASY", "2NORM", "3HARD", "4PAIN"};
const char highScoreWords[][10] PROGMEM = {"HIGH", "SCORE"};

//highscore digets
const int playerNameDigitOne[] = {0, 14, 28, 42};
const int playerNameDigitTwo[] = {1, 15, 29, 43};
const int playerNameDigitThree[] = {2, 16, 30, 44};
const int playerNameDigitFour[] = {3, 17, 31, 45};
const int playerNameDigitFive[] = {4, 18, 32, 46};

const int playerTeamDigitOne[] = {5, 19, 33, 47};
const int playerTeamDigitTwo[] = {6, 20, 34, 48};
const int playerTeamDigitThree[] = {7, 21, 35, 49};
const int playerTeamDigitFour[] = {8, 22, 36, 50};

const int playerScoreDigitOne[] = {9, 23, 37, 51};
const int playerScoreDigitTwo[] = {10, 24, 38, 52};
const int playerScoreDigitThree[] = {11, 25, 39, 53};
const int playerScoreDigitFour[] = {12, 26, 40, 54};
const int playerScoreDigitFive[] = {13, 27, 41, 55};



void setup() {
  for (int i = 0; i < 4; i ++){
    limitSwitches[i].setDebounceTime(20);
  }

  bw_reset = b_width[diff];
  matrix.begin();
  matrix.setRotation(3);
  matrix.setTextWrap(true);
  matrix.setTextSize(4);
  randomSeed(analogRead(0));
  Serial.begin(9600);
  
  pinMode(buttonMain, INPUT);
  pinMode(buttonEasy, INPUT_PULLUP);
  pinMode(buttonMedium, INPUT_PULLUP);
  pinMode(buttonHard, INPUT_PULLUP);
  pinMode(buttonYes, INPUT_PULLUP);
  
  servos[0] = servo_easy;
  servos[1] = servo_norm;
  servos[2] = servo_hard;
  servos[3] = servo_pain;

  for (int i = 0; i < 4; i ++)
  {
    pinMode(lights[i], OUTPUT);
  }

  pinMode(bMain_light, OUTPUT);

  clearEEPROM();
  splash();
}

void loop() {
  if (isRunning == true)
  {
    if (to_right == true)
    {
      matrix.fillRect(b_width[diff], 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff],  matrix.Color333(0, 0, 0));
      for (int w = 1; w < 33 - b_width[diff]; w++)
      {
        matrix.fillRect(w, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), b_width[diff], b_height[diff], matrix.Color333(r,g,b));
        
        matrix.fillRect(w - 1,  64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff], matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
      }
      wallBounces++;
      showScore(false);
      to_right = false;
    }
    
    else 
    {
      for (int w = 31-b_width[diff]; w >= 0; w = w - 1)
      { 
        matrix.fillRect(w, 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), b_width[diff], b_height[diff], matrix.Color333(r,g,b));
      
        matrix.fillRect(w+b_width[diff], 64 - (b_height[diff]*2) - (blockLevel * b_height[diff]), 1, b_height[diff],  matrix.Color333(0, 0, 0));
        buttonCheck(w);
        delay(block_speed);
      }
      wallBounces++;
      showScore(false);
      to_right = true;
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
  showScore(false);

  for(int i = 0; i < 8; i++)
  {
    showText(3 + (i*7) - (28 * (i/4)), 25 + (10 * (i/4)), 1, i, 0, 0, 7);
    delay(letterDelay);
  }
  delay(3000);
  crumble();
  //readScore();
  splash();
}

void win()
{
  winGame = true;
  for (uint8_t i = 10; i <64; i++)
  {
    matrix.fillRect(0, i,32,  1,matrix.Color333(0,0,7));
    delay(1);
  }
  isRunning = false;
  showScore(false);

  for(int i = 0; i < 6; i++)
  {
    showText(6 + (i*7) - (21 * (i/3)), 25 + (10 * (i/3)), 1, i + 8, 7, 7, 0);
    delay(letterDelay);
  }
  delay(1200);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
  dispensePrize(diff);
  crumble();
  //if (playerScore > getHighScore(diff)){
  //  nameSetMenu();
  //}
  splash();
}

void crumble()
{
  for (int i = 0; i <6000; i ++)
  {
    matrix.fillRect(random(0, 32), random(10,64), 1, 1, matrix.Color333(0,0,0));
  }
  matrix.fillScreen(0);
  //delay(500);
}

void startGame(int d)
{
  winGame = false;
  setDifficulty(d);
  playerScore = 0L;
  lastPlayerScore = 0L;
  wallBounces = 0;
  
  for (int i = 0; i < 4; i ++)
  {
    digitalWrite(lights[i], LOW);
  }
  
  for (int g = 0; g < 64; g++)
    {
      matrix.fillRect(0,63-g,32,1, matrix.Color333(0,0,0));
      if (g % 10 == 0)
      {
        digitalWrite(lights[d], LOW);
      }

      else if (g % 5 == 0)
      {
        digitalWrite(lights[d], HIGH);
      }
      delay(10);
     }

   
  digitalWrite(lights[d], LOW);
  delay(1500);
  blockLevel = 0;
  block_speed = 1000 - b_speed[diff];
  bw_reset = b_width[diff];
  margin_l = (32-b_width[diff])/2;
  margin_r = margin_l + b_width[diff] + 1;
  overhang = 0;
  isRunning = true;
  showScore(true);
  matrix.fillRect(margin_l, 64 - b_height[diff], b_width[diff], b_height[diff], matrix.Color333(r1,g1,b1));
  r = r2;
  g = g2;
  b = b2;
  matrix.fillRect(0, 9, 32, top_bar[diff], matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
}

void showText(int x, int y, int s, int text, int r, int g, int b)
{
  matrix.setTextSize(s);
  matrix.setTextColor(matrix.Color333(r,g,b));
  matrix.setCursor(x, y);
  matrix.print(F2(endText[text]));
}

long extractDigit(long V, int P)
{
  return long(V/(pow(10,P-1))) - long(V/(pow(10,P)))*10; 
}

void showScore(bool force)
{
  lastPlayerScore = long(playerScore);
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));

  if (!isRunning && winGame)
  {
    //playerScore = long(playerScore + (long(b_width[diff]) * 1000));
    delay(1);
    playerScore = playerScore + (1000L * long(b_width[diff]) * (diff + 1));
  }

  else
  { // maxscore = 99999 
    //
    //playerScore = long((long(blockLevel) * long(scoreWeights[diff])) - ((long(wallBounces)-long(blockLevel)-1)*(5135-(1354*long(diff)))));  *This led to score overflow, values were too high
    //playerScore = long((long(blockLevel) * long(scoreWeights[diff])) - ((long(wallBounces)-long(blockLevel)-1)*(2000-(525*long(diff)))));
    playerScore = long(blockLevel) * 1000L - (wallBounces * 100);
  }

  if (playerScore < 0)
  {
    playerScore = 0L;
  }

  for (int i = 0; i < 5; i++)
  {
     matrix.setCursor(1 + (i  * 6), 1);
     if(digits[extractDigit(lastPlayerScore,5-i)] != digits[extractDigit(playerScore,5-i)] || force)
     {
      matrix.fillRect(1 + (i  * 6), 1, 6, 8,matrix.Color333(0,0,0));
      matrix.print(F2(digits[extractDigit(playerScore,5-i)]));
     }
     
  }
}

void increaseLevel()
{
  if (b_width[diff] < 1)
    {
      gameOver();
      return;
    } // Dylan was here
    //Justin was here
   if (blockLevel == 0)
   {
    wallBounces = 0;
   }
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
     
    showScore(false);
}

void setDifficulty(int d)
{
  diff = d;
}

void splash()
{

  //FILL BLUE
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

  b_width[diff] = bw_reset;

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
            //matrix.fillScreen(matrix.Color333(0,0,0));
            //delay(500);
            //startGame();
            diffSelect();
            return;
          }
        }
      }
      delay(1);
    }
  }  
  
}

void diffSelect()
{
  
  matrix.fillRect(1,16,32,36,matrix.Color333(0,0,3));
  
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 16);
  matrix.print(F2(diffText[0]));

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.setCursor(1, 24);
  matrix.print(F2(diffText[1]));

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,4,0));
  matrix.setCursor(1, 33);
  matrix.print(F2(diffText[2]));

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(7,0,0));
  matrix.setCursor(1, 41);
  matrix.print(F2(diffText[3]));

  for (int i = 0; i < 4; i ++)
  {
    digitalWrite(lights[i], HIGH);
  }

  while (true)
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

          if(g % 2 == 0) {

            digitalWrite(bMain_light, LOW);
          } else {

            digitalWrite(bMain_light, HIGH);
          }
                    
          
          if (digitalRead(buttonEasy) == LOW)
          {
            isRunning = true;
            startGame(0);
            return;
          }
          if (digitalRead(buttonMedium) == LOW)
          {
            isRunning = true;
            startGame(1);
            return;
          }
          if (digitalRead(buttonHard) == LOW)
          {
            isRunning = true;
            startGame(2);
            return;
          }
          if (digitalRead(buttonYes) == LOW)
          {
            isRunning = true;
            startGame(3);
            return;
          }
        }
      }
      delay(1);
    }
  }   
}

void stopServo(int p){
  bool finish = false;
  while(true){
    limitSwitches[p].loop();
    if (limitSwitches[p].isPressed()){
      while(true){
        limitSwitches[p].loop();
        if (limitSwitches[p].isReleased()){
          servos[p].detach();
          finish = true;
          break;
          
        }
      }
    }
  if (finish){
    break;
  }
  }
}

void dispensePrize(int p)
{
  servos[p].attach(servosPins[p]);
  servos[p].write(servoRunSpeeds[p]);
  stopServo(p);
}

void readScore() 
{
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
  matrix.setCursor(4, 1);
  matrix.print(F2(highScoreWords[0]));
  matrix.setCursor(1, 9);
  matrix.print(F2(highScoreWords[1]));
  matrix.fillRect(0, 17, 32, 1, matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
  
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(7, 23);

  //team
  matrix.setCursor(1, 23);
  matrix.print("#");
  matrix.setCursor(7, 23);
  matrix.print(EEPROM.read(F2(playerTeamDigitOne[diff])));
  matrix.setCursor(13, 23);
  matrix.print(EEPROM.read(F2(playerTeamDigitTwo[diff])));
  matrix.setCursor(19, 23);
  matrix.print(EEPROM.read(F2(playerTeamDigitThree[diff])));
  matrix.setCursor(25, 23);
  matrix.print(EEPROM.read(F2(playerTeamDigitFour[diff])));

  //name
  matrix.setCursor(1, 31);
  matrix.print(char(EEPROM.read(F2(playerNameDigitOne[diff]))));
  matrix.setCursor(7, 31);
  matrix.print(EEPROM.read(F2(playerNameDigitTwo[diff])));
  matrix.setCursor(13, 31);
  matrix.print(EEPROM.read(F2(playerNameDigitThree[diff])));
  matrix.setCursor(19, 31);
  matrix.print(EEPROM.read(F2(playerNameDigitFour[diff])));
  matrix.setCursor(25, 31);
  matrix.print(EEPROM.read(F2(playerNameDigitFive[diff])));

  //score
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.setCursor(1, 52);
  matrix.print(EEPROM.read(F2(playerScoreDigitOne[diff])));
  matrix.setCursor(7, 52);
  matrix.print(EEPROM.read(F2(playerScoreDigitTwo[diff])));
  matrix.setCursor(13, 52);
  matrix.print(EEPROM.read(F2(playerScoreDigitThree[diff])));
  matrix.setCursor(19, 52);
  matrix.print(EEPROM.read(F2(playerScoreDigitFour[diff])));
  matrix.setCursor(25, 52);
  matrix.print(EEPROM.read(F2(playerScoreDigitFive[diff])));
  
  matrix.setTextColor(matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
  matrix.setCursor(1, 44);
  matrix.print(F2(highScoreWords[1]));
  matrix.setTextColor(matrix.Color333(7,7,7));
  delay(5000);
  splash();
  //matrix.fillRect(0, 46, 32, 1, matrix.Color333(dc_r[diff], dc_g[diff], dc_b[diff]));
}

void setName() 
{
  scoreCharOne = byte(extractDigit(playerScore, 1));
  scoreCharTwo = byte(extractDigit(playerScore, 2));
  scoreCharThree = byte(extractDigit(playerScore, 3));
  scoreCharFour = byte(extractDigit(playerScore, 4));
  scoreCharFive = byte(extractDigit(playerScore, 5));

  if (diff == 0) 
    {
      //name
      EEPROM.update(0, byte(nameCharOne));
//      EEPROM.put(1, byte(nameCharTwo));
//      EEPROM.put(2, byte(nameCharThree));
//      EEPROM.put(3, byte(nameCharFour));
//      EEPROM.put(4, byte(nameCharFive));
//      //team
//      EEPROM.put(5, byte(teamCharOne));
//      EEPROM.put(6, byte(teamCharTwo));
//      EEPROM.put(7, byte(teamCharThree));
//      EEPROM.put(8, byte(teamCharFour));
//      //score
//      EEPROM.put(9, byte(scoreCharOne));
//      EEPROM.put(10, byte(scoreCharTwo));
//      EEPROM.put(11, byte(scoreCharThree));
//      EEPROM.put(12, byte(scoreCharFour));
//      EEPROM.put(13, byte(scoreCharFive));
    }
   else if (diff == 1) 
    {
      EEPROM.update(14, nameCharOne);
      EEPROM.update(15, nameCharTwo);
      EEPROM.update(16, nameCharThree);
      EEPROM.update(17, nameCharFour);
      EEPROM.update(18, nameCharFive);
      //team
      EEPROM.update(19, teamCharOne);
      EEPROM.update(20, teamCharTwo);
      EEPROM.update(21, teamCharThree);
      EEPROM.update(22, teamCharFour);
      //score
      EEPROM.update(23, scoreCharOne);
      EEPROM.update(24, scoreCharTwo);
      EEPROM.update(25, scoreCharThree);
      EEPROM.update(26, scoreCharFour);
      EEPROM.update(27, scoreCharFive);
    }
   else if (diff == 2)
    {
      EEPROM.update(28, nameCharOne);
      EEPROM.update(29, nameCharTwo);
      EEPROM.update(30, nameCharThree);
      EEPROM.update(31, nameCharFour);
      EEPROM.update(32, nameCharFive);
      //team
      EEPROM.update(33, teamCharOne);
      EEPROM.update(34, teamCharTwo);
      EEPROM.update(35, teamCharThree);
      EEPROM.update(36, teamCharFour);
      //score
      EEPROM.update(37, scoreCharOne);
      EEPROM.update(38, scoreCharTwo);
      EEPROM.update(39, scoreCharThree);
      EEPROM.update(40, scoreCharFour);
      EEPROM.update(41, scoreCharFive);
    }
   else if (diff == 3)
    {
      EEPROM.update(42, nameCharOne);
      EEPROM.update(43, nameCharTwo);
      EEPROM.update(44, nameCharThree);
      EEPROM.update(45, nameCharFour);
      EEPROM.update(46, nameCharFive);
      //team
      EEPROM.update(47, teamCharOne);
      EEPROM.update(48, teamCharTwo);
      EEPROM.update(49, teamCharThree);
      EEPROM.update(50, teamCharFour);
      //score
      EEPROM.update(51, scoreCharOne);
      EEPROM.update(52, scoreCharTwo);
      EEPROM.update(53, scoreCharThree);
      EEPROM.update(54, scoreCharFour);
      EEPROM.update(55, scoreCharFive);
    }
}

void nameSetMenu(){
  
      matrix.setTextSize(1);
      matrix.setTextColor(matrix.Color333(1, 1, 1));
      matrix.setCursor(1,3);
      matrix.print("NAME");
      matrix.setCursor(1, 11);
      matrix.print("A");
      matrix.setCursor(7, 11);
      matrix.print("A");
      matrix.setCursor(13, 11);
      matrix.print("A");
      matrix.setCursor(19, 11);
      matrix.print("A");
      matrix.setCursor(25, 11);
      matrix.print("A");
      matrix.setCursor(1, 27);
      matrix.print("TEAM");
      matrix.setCursor(1, 35);
      matrix.print("0");
      matrix.setCursor(7, 35);
      matrix.print("0");
      matrix.setCursor(13, 35);
      matrix.print("0");
      matrix.setCursor(19, 35);
      matrix.print("0");
      matrix.setCursor(1, 13);
      matrix.print("_");

      while(nameEntered == false) {
        
        if(digitalRead(buttonMain) == LOW) {

            matrix.fillRect((cursorPosition * 6) + 1, 19, 5, 1, matrix.Color333(0, 0, 0));
            if(cursorPosition >= 4){
              cursorPosition = 0;
            } else {
              cursorPosition = cursorPosition + 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 13);
            matrix.print("_");
            delay(200);
        }
        
        if(digitalRead(buttonEasy) == LOW) {
          
            matrix.fillRect((cursorPosition * 6) + 1, 11, 5, 7, matrix.Color333(0, 0, 0));
            if(currentLetter >= 25) {
              currentLetter = 0;
            } else {
              currentLetter = currentLetter + 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 11);
            matrix.print(F2(alphabet[currentLetter]));

            if (cursorPosition == 0 )
            {
              nameCharOne = currentLetter;
            } 
            else if (cursorPosition == 1)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 2)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 3)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 4)
            {
              nameCharOne = currentLetter;
            }
            
            delay(200);
        }

        if(digitalRead(buttonMedium) == LOW) {

            matrix.fillRect((cursorPosition * 6) + 1, 11, 5, 7, matrix.Color333(0, 0, 0));
            if(currentLetter <= 0) {
              currentLetter = 25;
            } else {
              currentLetter = currentLetter - 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 11);
            matrix.print(F2(alphabet[currentLetter]));

            if (cursorPosition == 0 )
            {
              nameCharOne = currentLetter;
            } 
            else if (cursorPosition == 1)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 2)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 3)
            {
              nameCharOne = currentLetter;
            }
            else if (cursorPosition == 4)
            {
              nameCharOne = currentLetter;
            }
            
            delay(200);
        }

        if(digitalRead(buttonHard) == LOW) {
          
            nameEntered = true;
            matrix.fillRect((cursorPosition * 6) + 1, 19, 5, 1, matrix.Color333(0, 0, 0));
            cursorPosition = 0;
            delay(500);
        }
      }

    matrix.setCursor(1, 37);
    matrix.print("_");

      while(numEntered == false) {

         if(digitalRead(buttonMain) == LOW) {
            
            matrix.fillRect((cursorPosition * 6) + 1, 43, 5, 1, matrix.Color333(0, 0, 0));
            if(cursorPosition >= 3){
                cursorPosition = 0;
            } else {
                cursorPosition = cursorPosition + 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 37);
            matrix.print("_");
            delay(200);
         }
        
         if(digitalRead(buttonEasy) == LOW) {
          
            matrix.fillRect((cursorPosition * 6) + 1, 35, 5, 7, matrix.Color333(0, 0, 0));
            if(currentNum >= 9) {
              currentNum = 0;
            } else {
              currentNum = currentNum + 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 35);
            matrix.print(F2(digits[currentNum]));
            delay(200);
         }

        if(digitalRead(buttonMedium) == LOW) {

            matrix.fillRect((cursorPosition * 6) + 1, 35, 5, 7, matrix.Color333(0, 0, 0));
            if(currentNum <= 0) {
              currentNum = 9;
            } else {
              currentNum = currentNum - 1;
            }
            matrix.setCursor((cursorPosition * 6) + 1, 35);
            matrix.print(F2(digits[currentNum]));
            delay(200);
        }

        if(digitalRead(buttonHard) == LOW) {
          
            numEntered = true;
            delay(500);
        }
      }
      setName();
      readScore();
   }
void clearEEPROM(){
  for (int i = 0; i < 55; i++){
    EEPROM.update(i,0);
  }

}

int getHighScore(int d)
{
  return (playerScoreDigitOne[d]*10000) + (playerScoreDigitTwo[d]*1000) + (playerScoreDigitThree[d]*100) + (playerScoreDigitFour[d]*10) + playerScoreDigitFive;
}
