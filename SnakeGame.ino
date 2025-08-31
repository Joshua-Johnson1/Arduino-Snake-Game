//Pin Assignments for shift register
const int dataPin = 11;  
const int latchPin = 10;
const int clockPin = 9;

//Pin assignments for D-Pad input buttons
const int leftPin = 2;  
const int rightPin = 3;
const int upPin = 4;
const int downPin = 5;
const int selectPin = 6;

/*
Rows 0-7: Top to bottom on the matrix
Columns 0-7: Left to right on matrix

"column" is for matrix 1
"columnTwo" is for matrix 2
*/
const int row[8] = {B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001};
int column, columnTwo = B00000000;

/* 
For loop that affects frame time

Uses millis(), which tracks the arduino's internal lock in miliseconds
travelTime is serves as a way to keep track of the amount of time the snake should be moving (.75 seconds)
*/
unsigned long travelTime = 0;

//User input variables
bool inputUp, inputDown, inputLeft, inputRight; //For communicating a change in user input to other functions

//Snake Game Variables
int snakeRow[64], snakeColumn[64]; //The position of the snake's head [0] and body [1-63] based on its row and column number
int snakeSize; //The size of the snake, increases in length for each apple that is eaten
int openSpace; //The number of open space left in the matrix (# of LEDs off)
int appleNum; //A selected random number between 0 to (# of LEDs off)
int appleRow, appleColumn; //The current position of the apple based on its row and column number
int appleCount; //Serves as a counter variable in a loop for apple spawning
bool newApple; //A variable for when a new apple should be spawning in
bool snakeUp, snakeDown, snakeLeft, snakeRight; //Movement directions for the snake
bool horizontalMove, verticalMove; //For locking horizontal/vertical movement for snake to either turn up & down or left & right
int score; //A score counter for how many apples the snake has eaten thus far

//Double array that basically mimics the 8x8 matrix display, all 1s are dots ON, 0s are OFF
int matrix[8][8] =
{
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
};

//A second array for a second matrix used for displaying scores
int matrixTwo[8][8]
{
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
};

void matrixOff() 
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, B11111111);  //Turn off all columns (matrix 2)
  shiftOut(dataPin, clockPin, MSBFIRST, B11111111);  //Turn off all columns (matrix 1)
  shiftOut(dataPin, clockPin, MSBFIRST, B00000000);  //Turn off all rows
  digitalWrite(latchPin, HIGH);
}


//A function for updating the grid-like matrix array called matrix[8][8] and matrixTwo[8][8]
//The "matrix" array serves as the play area for the snake to update its movement
//The "matrixTwo" array serves solely as a counter for the user to see their score
//This function doesn't directly update anything yet to the real LED matrix
void updateGrid()
{
  
  //Clear the entire matrix grid to prepare for a new update (everything is zero)
  //This prevents previous segments of the snake's tail from sticking around even after it moved
  //Also helps with visually spawning in the apple and refreshing its grid position
  for (int rowNumber = 0; rowNumber<8; rowNumber++)
  {
    for (int columnNumber = 0; columnNumber<8; columnNumber++)
    {
      matrix[rowNumber][columnNumber] = 0;
      matrixTwo[rowNumber][columnNumber] = 0; 
    }
  }

  //Update the new position of the snake on the first matrix to ON or 1
  for (int bodyPart = 0; bodyPart <= snakeSize; bodyPart++)
  {
    matrix[snakeRow[bodyPart]][snakeColumn[bodyPart]] = 1;
  }

  //Update the singular position that the apple spawns in
  matrix[appleRow][appleColumn] = 1;
  
  //The entirety of this section below will now update the second matrix or array "matrixTwo"
 
  //Checking the tens digit of the score to update
  
  //Displays a 0 in tens digit
  if ((score/10) == 0)
  {
    matrixTwo[2][1] = 1;
    matrixTwo[3][1] = 1;
    matrixTwo[4][1] = 1;
    matrixTwo[5][1] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[5][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[3][3] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][2] = 1;
  }
  //Displays a 1 in tens digit (10s)
  else if ((score/10) == 1)
  {
    matrixTwo[5][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[5][2] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[3][2] = 1;
    matrixTwo[2][2] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][1] = 1;

  }
  //Displays a 2 in tens digit (20s)
  else if ((score/10) == 2)
  {
    matrixTwo[2][1] = 1;
    matrixTwo[3][1] = 1;
    matrixTwo[4][1] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[5][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][2] = 1;
  }
  //Displays a 3 in tens digit (30s)
  else if ((score/10) == 3)
  {
    matrixTwo[2][1] = 1;
    matrixTwo[4][1] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[5][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[3][3] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][2] = 1;
  }
  //Displays a 4 in tens digit (40s)
  else if ((score/10) == 4)
  {
    matrixTwo[4][1] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[5][1] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[5][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[3][3] = 1;
    matrixTwo[2][3] = 1;
  }
  //Displays a 5 in tens digit (50s)
  else if ((score/10) == 5)
  {
    matrixTwo[2][1] = 1;
    matrixTwo[4][1] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[5][1] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[3][3] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][2] = 1;
  }
  //Displays a 6 in tens digit (60s)
  else if ((score/10) == 6)
  {
    matrixTwo[2][1] = 1;
    matrixTwo[3][1] = 1;
    matrixTwo[4][1] = 1;
    matrixTwo[4][2] = 1;
    matrixTwo[5][1] = 1;
    matrixTwo[6][1] = 1;
    matrixTwo[6][2] = 1;
    matrixTwo[6][3] = 1;
    matrixTwo[4][3] = 1;
    matrixTwo[3][3] = 1;
    matrixTwo[2][3] = 1;
    matrixTwo[2][2] = 1;
  }

  //Checking the tens digit of the score to update
  
  //Displays a 0 in ones digit
  if ((score % 10) == 0)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[3][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
  //Displays a 1 in ones digit
  if ((score % 10) == 1)
  {
    matrixTwo[6][6] = 1;
    matrixTwo[5][6] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[3][6] = 1;
    matrixTwo[2][6] = 1;
    matrixTwo[2][5] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[5][5] = 1;
  }
  //Displays a 2 in ones digit
  if ((score % 10) == 2)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[3][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }

  //Displays a 3 in ones digit
  if ((score % 10) == 3)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
  //Displays a 4 in ones digit
  if ((score % 10) == 4)
  {
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
  }
  //Displays a 5 in ones digit
  if ((score % 10) == 5)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
  //Displays a 6 in ones digit
  if ((score % 10) == 6)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[3][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
  //Displays a 7 in ones digit
  if ((score % 10) == 7)
  {
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
  }
  //Displays a 8 in ones digit
  if ((score % 10) == 8)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[3][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
  //Displays a 9 in ones digit
  if ((score % 10) == 9)
  {
    matrixTwo[2][5] = 1;
    matrixTwo[4][5] = 1;
    matrixTwo[4][6] = 1;
    matrixTwo[5][5] = 1;
    matrixTwo[6][5] = 1;
    matrixTwo[6][6] = 1;
    matrixTwo[6][7] = 1;
    matrixTwo[5][7] = 1;
    matrixTwo[4][7] = 1;
    matrixTwo[3][7] = 1;
    matrixTwo[2][7] = 1;
    matrixTwo[2][6] = 1;
  }
}


void updatePosition()
{
  //Update the snake's tail parts, not including the snake's head
  for (int bodyPart = (snakeSize+1); bodyPart > 0; bodyPart--)
  {
    snakeRow[bodyPart] = snakeRow[bodyPart-1];
    snakeColumn[bodyPart] = snakeColumn[bodyPart-1];
  }

  

  /*
  Independently moves the snake's head based on where it can move

  If the snake's head is moving horizontally, it can only move up or down
  If the snake's head is moving vertically, it can only go left or right

  Moves over one LED on the matrix per 750 ms (.75 seconds)
  */

  if (inputUp == true) //When the latest user input before the next update is UP
  {
    verticalMove = true;
    horizontalMove = false;
    snakeRow[0]++;
  }
  else if (inputDown == true) //When the latest user input before the next update is DOWN
  {
    verticalMove = true;
    horizontalMove = false;
    snakeRow[0]--;
  }
  else if (inputLeft == true) //When the latest user input before the next update is LEFT
  {
    verticalMove = false;
    horizontalMove = true;
    snakeColumn[0]--;
  }
  else if (inputRight == true) //When the latest user input before the next update is RIGHT
  {
    verticalMove = false;
    horizontalMove = true;
    snakeColumn[0]++;
  }
}

void updateFrame()
{
  //Matrix 1 & 2 have their own independent columns but both share the same rows to multiplex with
    for (int rowNumber = 0; rowNumber<8; rowNumber++)
  {
    //Resets "column" variables to every LED being ON
    column = B00000000; //Matrix 1 columns
    columnTwo = B00000000; //Matrix 2 columns
    for (int columnNumber = 0; columnNumber<8; columnNumber++)
    {
      //For columns, 0 means on and 1 means off, but on the matrix grid array it's the opposite
      //If the part of the matrix grid that is scanned is 0, then the column LED for that singular row turns off
      if (matrix[rowNumber][columnNumber] == 0) //Matrix 1
        bitSet(column, columnNumber);
      if (matrixTwo[rowNumber][columnNumber] == 0) //Matrix 2
        bitSet(columnTwo, columnNumber);
    }
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, LSBFIRST, columnTwo);  //Turn on/off columns (matrix 2)
      shiftOut(dataPin, clockPin, LSBFIRST, column);  //Turn on/off columns (matrix 1)
      shiftOut(dataPin, clockPin, MSBFIRST, row[rowNumber]);  //Turn on/off rows (matrix 1 & 2)
      digitalWrite(latchPin, HIGH);
  }
}

void buttonInput() {
  //Sends signal that the user pushed the up button on the D-Pad
  if (digitalRead(upPin) == LOW) 
  {
    if (horizontalMove == true)
    {
     // Serial.println("Up");
      inputUp = true; 
      inputDown = false; 
      inputLeft = false; 
      inputRight = false;
    }
  }

  //Sends signal that the user pushed the down button on the D-Pad
  if (digitalRead(downPin) == LOW) 
  {
    if (horizontalMove == true)
    {
      //Serial.println("Down");
      inputUp = false; 
      inputDown = true; 
      inputLeft = false; 
      inputRight = false;
    }
  }

  //Sends signal that the user pushed the left button on the D-Pad
  if (digitalRead(leftPin) == LOW)
  {
    if (verticalMove == true)
    {
      //Serial.println("Left");
      inputUp = false; 
      inputDown = false; 
      inputLeft = true; 
      inputRight = false;
    }
  }

  //Sends signal that the user pushed the right button on the D-Pad
  if (digitalRead(rightPin) == LOW) 
  {
    if (verticalMove == true)
    {
      //Serial.println("Right");
      inputUp = false; 
      inputDown = false; 
      inputLeft = false; 
      inputRight = true;
    }
  }

  //Only useful for allowing the user to select game options or pause their game
  if (digitalRead(selectPin) == LOW) 
  {
    //Serial.println("Select");
    newGame();
  }
}

void newGame()
{
  //Visually clears the matrix, every LED is set to LOW
  matrixOff();

  //Snake size is set to default (tail length of 2, not including the head)
  snakeSize = 2;

  //Score is reset to 0
  score = 0;

  //Default starting positions for rows
  snakeRow[0] = 3;
  snakeRow[1] = 3;
  snakeRow[2] = 3;

  //Default starting positions for columns
  snakeColumn[0] = 2;
  snakeColumn[1] = 1;
  snakeColumn[2] = 0;

  //Influences the inputs early to make sure that the snake is moving rightwards by default at the start of game
  horizontalMove = true;
  verticalMove = false;
  inputUp = false; 
  inputDown = false; 
  inputLeft = false; 
  inputRight = true;

  //Allows the apple to spawn in a singular time
  newApple = true;
}

void checkDeath()
{
  //Checks if the snake hits its own tail
  for (int snakeTail = 1; snakeTail < (snakeSize+1); snakeTail++)
  {
    if (snakeRow[0] == snakeRow[snakeTail] && snakeColumn[0] == snakeColumn[snakeTail])
    {
      newGame(); //Resets the game
    }
  }

  //Checks if the snake hits the edge of the LED matrix (out of bounds)
  if (snakeRow[0] > 7 || snakeColumn[0] > 7 || snakeRow[0] < 0 || snakeColumn[0] < 0)
  {
    newGame(); //Resets the game
  }
  
}
void setup() {
  //Shift Register Matrix Display
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  //D-Pad Buttons
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);  
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP); 
  pinMode(selectPin, INPUT_PULLUP); 

  //For debugging using serial monitor
  Serial.begin(9600);

  //Reserves a pin in order to have rand() truly work with no set seeds
  randomSeed(analogRead(A0));

  newGame();
}

/*
A function for randomly spawning in and deciding the position of an apple in snake game
When the apple is eaten, it will spawn in a new location and the snake's size will grow +1
An apple will only spawn in parts of the LED matrix that are off
*/
void appleSpawn()
{
  if (newApple == true) //When the apple just spawns in general, whether initially during startup or after the apple is eaten
  {
    Serial.println("\nhi"); 

    newApple = false; //A new apple can no longer spawn in after one has just spawned

    openSpace = (64-(snakeSize+1)); //Update the number of LEDs currently off in the matrix
    appleNum = random(openSpace); //Assign random number for apple location between 0 to (# of LEDs off)
    
    /*
    Resets the counter for the apple to spawn in

    This counter only goes up after sorting through each of the LEDs for the rows and columns from left to right
    starting at the LED at the very top left
    */
    appleCount = 0;

    int testcounter = 0;
    /*
    Updates the counter by sorting through the matrix[8][8] array from the top left LED
    
    If the position for matrix[row][col] is 0, then the counter will go up
    If the position is 1, then the counter will skip it

    This way ensures that only the blank spaces are checked and that it still goes by a
    random number system for spawning the apple by using the variable appleNum's random() function
    */
    for (int rowNumber = 0; rowNumber < 8; rowNumber++)
    {
      for (int columnNumber = 0; columnNumber < 8; columnNumber++)
      {
        if(matrix[rowNumber][columnNumber] == 0) //An LED within the matrix that is OFF or 0 is detected
        {
          appleCount++; //The counter for the apple to spawn in goes up 
          if (appleCount == appleNum) //When the counter reaches its desired number and matches with appleNum, assign the new row and column spawn positions
          {
            appleRow = rowNumber;
            appleColumn = columnNumber;
            
            //Testing
            /*
                        Serial.println("-------------------------------------------------------------"); 
                        testcounter++;
                        Serial.println("Apples Collected: " + String(testcounter-1)); 

            Serial.println("appleCount: " + String(appleCount)); 
            Serial.println("rowNumber: " + String(rowNumber));    
            Serial.println("columnNumber: " + String(columnNumber));    
            
            */
          }
        }
      }
    }


  }

}


void loop() 
{
  appleSpawn();
  updateGrid();
  updateFrame();
  buttonInput();
  checkDeath();

    if (appleRow == snakeRow[0] && appleColumn == snakeColumn[0]) //For when the snake actually eats the apple
    {
      snakeSize++; //When the snake eats the apple, its tail grows by 1 LED length
      score++;
      newApple = true; //For appleSpawn() function to know to spawn in a new apple
    }
  /* 
  Independently delays a part of the program that deals with frame time
  without interfering with the functions for actually updating the grid or multiplexing

  This if-statement serves to update the snake's position every .75 seconds,
  */
  if (millis() - travelTime >= 750)
  {
    travelTime = millis(); //Once the snake travel time reaches 750 ms, reset to 0 to count a new frame.
    updatePosition(); //Updates the positions of the body parts for the snakeColumn and snakeRow arrays as the snake moves
    

  }
}



//STUFF DONE & informal change log:

//(DONE 7/15/2025) finished wiring, debugging wires, and learning how parts work 
//(DONE 7/16/2025) Learned & mapped out on a pdf & paint.net where the register's pins connect so that I can have more efficient binary inputs for easier time coding 
//(DONE 7/17/2025) Created a function for condensed use when changing frames 
//(DONE 7/18/2025) Coded and wired button control inputs for select, up, down, left, right 
//(DONE 7/19/2025) Navigate & control a single LED (head of the snake in snake game) using d-pad buttons 
//(DONE 7/21/2025) Added "death" boundaries for snake game when the head touches the border to prevent the matrix lights from displaying weird patterns 
//(DONE 7/22/2025) Brainstorming and revising new methods for storing the position of the snake length, will redo some parts like input in future 
//(DONE 7/23/2025) Reimplemented inputs and added new logic for rows and columns to be stored on two different arrays for positions 
//(DONE 7/23/2025) Added a death barrier that triggers the game reset function if the head of the attempts to go out of bounds or beyond positions 0-7 on the LED matrix 
//(DONE 7/24/2025) Added snake-game-like movement where the snake moves in a certain direction until a button is actually pressed to go in another direction, and the snake can only turn 2 ways 
/*(BIG REDESIGN 7/27/2025), just putting the stuff that I did here that I did to keep it more concise: 
    - Added matrix[8][8] that works like coordinates on a grid, shifting out columns is now by LSB instead of MSB because of structure of the way the grid interprets column bits
    - "column" variable is no longer an array, it updates based on the matrix[8][8] position
    - Frame updating now incorporates less sloppy multiplexing where it instead changes each row from the row[8]
    - A loop is now used when multiplexing to cycle through each row one by one and use setBit() to update the "column" variable based on matrix[8][8]'s  grid-like setup for bits
*/
//(DONE 7/28/2025) Implemented an if-statement using millis() that substitutes with delay() and no longer interferes with multiplexing and doesn't cause flickering or stops inputs
//(DONE 7/29/2025) Reimplemented basic omnidirectional LED movement based on user inputs for basic input and movement testing
//(DONE 7/30/2025) Finally added the snake's tail copying the previous movements of the snake's head without any LED flickering
//(DONE 7/31/2025) Reworked movement to be like snake game again, where you can only go up & down or left & right depending if the snake moves horizontally or vertically
//(DONE 8/1/2025) Added a separate function for starting a new game, temporarily made it so the "Select" push button now triggers a restart for testing
//(DONE 8/2/2025) Made it so when the snake goes out of bounds it triggers the game to automatically restart for now
//(DONE 8/3/2025) When the snake collides with its own tail the game will also restart
//(DONE 8/6/2025) The snake now grows when touching a stationary apple, random spawning still being worked on
//(DONE 8/7/2025) The apple is now fully capable of randomly spawning in on unfilled LED spaces (when an LED is off) but doesn't play nice when a new apple needs to spawn (need fix tmrw)
//(DONE 8/9/2025) fixed previous bug for apple not spawning in a new location, i still dislike the visual abnormalties of a random LED flickering that likely occurs because two positions are considered "ON" at the same time and are shifted out twice so will fix tmrw
//(DONE 8/10/2025) replaced push buttons with more breadboard compatible ones that arent as annoying to deal with and dont constantly pop out
//(DONE 8/11/2025) Fixed a random LED appearing bottom left every time an apple was eaten caused by snake length not being updated fast enough
/*(BIG REDESIGN 8/18/2025)
    - Rewired the entirety of the breadboard circuit for easier time understanding where the wires actually go
    - Added a third shift register 
    - Added a second LED matrix that uses the new third shift register for separate column shifting
    - The second LED matrix uses the same shift register that multiplexes the display, turning on one row at a time really fast
    - Added a matrixTwo[8][8]; variable for now
*/
//(DONE 8/19/2025) Learned and added third shift register foundations for turning on and off LEDs for now, will work on the "grid" array updating for second matrix next time
/*
(FINAL UPDATE 8/30/2025)
Mini break because of new semester classes
- Finished the counter system and added a variable called "score" that keeps track of the score every time an apple is eaten
- The tens place of the display is updated using (score/10) to check what integer the tens place should be
- The ones place of the display is updated using (score%modulo) to see what integer the ones place should be from the score

Fun project :)
Joshua Johnson
*/


//TO-DO LIST/brainstorming/throw pasta at the wall:

//nothing left for now

