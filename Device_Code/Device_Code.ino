#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

typedef enum {
    SOLDER_IT,
    CODE_IT,
    MULTIMETER_IT
} COMMAND;

//16x2 display, I2C address
LiquidCrystal_I2C lcd(0x27,16,2);

//Set up SD mp3 player
SoftwareSerial mySoftwareSerial(10, 11); //RX TX
DFRobotDFPlayerMini myDFPlayer;

//Define the pins
#define SOLDER_PIN 2
#define CODE_PIN 3
#define MULTIMETER_PIN A0
#define ON_BUTTON 4

//Create the global variables
int score, time, playing;
int starttime, endtime;
int timeLimit;
bool gameOver, correctChoice;
COMMAND currentTask;

void setup() {
  // set up LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Score: ");
  delay(2000);
  
  //Connect to the serial monitor
  Serial.begin(115200);
  Serial.println("Startup");

  //Connect to the DFPlayer
  mySoftwareSerial.begin(9600);

  //Start the MP3 player
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");
  while (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(100);
  }
  Serial.println("DFPlayer Mini online.");

  //Set the DFPlayer Volume (range is 0 to 30)
  myDFPlayer.volume(10);

  //Set the global variables
  playing = false;
  gameOver = false;

  //Set the pins as inputs
  pinMode(SOLDER_PIN, INPUT);
  pinMode(CODE_PIN, INPUT);
  pinMode(MULTIMETER_PIN, INPUT);

}

void displaynumber(int num){
  // Move cursor to the second row
  lcd.setCursor(0,1);
  //clear previous number
  lcd.print("  ");

  lcd.setCursor(0,1);
  lcd.print(num);
}

COMMAND generateCommand(){
  //Generate a random number (1, 2, or 3) to choose a random behavior
  long randNum = random(1, 4);

  //Choose the command based on the randomly generated number. 
  //Play the associated sound and return the command
  switch(randNum){
    case 1:
      Serial.println("Solder It");
      myDFPlayer.playMp3Folder(/*File Number = */1);
      return SOLDER_IT;
    case 2:
      Serial.println("Code It");
      myDFPlayer.playMp3Folder(/*File Number = */2);
      return CODE_IT;
    case 3:
      Serial.println("Multimeter It");
      myDFPlayer.playMp3Folder(/*File Number = */3);
      return MULTIMETER_IT;
    default:
      Serial.println("ERROR: random number out of range");
      myDFPlayer.playMp3Folder(/*File Number = */4);
      return SOLDER_IT;
  }
}

void loop() {

  //Play the file No.1, the numbers are arranged according to the sequence of the files copied into the U-disk 
  //Serial.println("Playing song");
  
  //If the on button is pressed, turn the playing variable on so that the game starts and reset gameOver
  if(digitalRead(ON_BUTTON) == HIGH){
    gameOver = false;
    playing = true;
    timeLimit = 12000;
    score = 0;
  }

  //Loop through the main code if we are in the play condition
  if(playing){
    displaynumber(score);
    myDFPlayer.playMp3Folder(/*File Number = */1);
    delay(5000);
    Serial.println("Game Started");
    while(!gameOver){
      //We will generate a random command here
      currentTask = MULTIMETER_IT;
      Serial.println("Issued Command");
      displaynumber(score);

      //Initialize the times for the countdown
      starttime = millis();
      endtime = starttime;
      //Check the input while the time is counting down
      while((starttime-endtime) < timeLimit){
        //Once we receive a command check if it matches the randomly generated command
        //If it does, break out of the loop and continue. If it doesn't break out of the loop and end the game
        Serial.println("Waiting for input");
        //If the solder pin is high, the solder action has been completed
        if(digitalRead(SOLDER_PIN) == HIGH){
          Serial.println("Soldering is being done");
          //If this is the correct action, increment the score
          if(currentTask == SOLDER_IT){
            Serial.println("Correct Action");
            correctChoice = true;
          //Otherwise, set gameOver to true
          }else{
            gameOver = true;
          }
          break;
        }

        //If the code pin is high, the code action has been completed
        if(digitalRead(CODE_PIN) == HIGH){
          Serial.println("Coding is being done");
          //If this is the correct action, increment the score
          if(currentTask == CODE_IT){
            Serial.println("Correct Action");
            correctChoice = true;
          //Otherwise, set gameOver to true
          }else{
            gameOver = true;
          }
          //Exit the loop waiting for an action since an action was complete
          break;
        }

        //If the anologue pin is not close to 0 (being pulled down), the multimeter action is being complete
        if(analogRead(MULTIMETER_PIN) > 10){
          //If this is the correct action, check to make sure the correct resistor is being probed (the analogue input is in the right range)
          if(currentTask == MULTIMETER_IT){
            Serial.println("Correct Action");
            int resistorVal = analogRead(MULTIMETER_PIN);
            Serial.println(resistorVal);
            //If the correct resistor is being probed, increment the score
            if(resistorVal > 500 && resistorVal < 550){
              correctChoice = true;
            //Otherwise, set game over to true
            }else{
              gameOver = true;
            }
          //Otherwise, set gameOver to true
          }else{
            gameOver = true;
          }
          //Exit the loop waiting for an action since an action was complete
          break;
        }
      }

      //Check to see if the correct selection was made
      if(correctChoice){
        //If the user made the correct choice, increment the score and decrease the available time by 0.1s
        score++;
        timeLimit -= 100;
      }else{
        //If the user ran out of time, set gameOver to true
        gameOver = true;
      }
    }

    //If the game already ran and we now are at gameOver, play the end message
    if(gameOver){

      //Set playing to false
      playing = false;
    }

  }

}
