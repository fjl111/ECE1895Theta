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
#define SOLDER_LIGHT 7
#define CODE_LIGHT 6
#define MULTIMETER_LIGHT 5

//Create the global variables
int score, time, playing;
int starttime, endtime;
int timeLimit;
bool gameOver, correctChoice;
COMMAND currentTask;

void setup() {
  //Connect to the serial monitor
  Serial.begin(115200);
  Serial.println("Startup");

  // set up LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Score: ");
  delay(2000);

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
  myDFPlayer.volume(15);

  //Create a seed for the random number using a measurement from analog pin 1 which is floating (so will just have noise on it)
  randomSeed(analogRead(A1));

  //Set the global variables
  playing = false;
  gameOver = false;

  //Set the pins as inputs
  pinMode(SOLDER_PIN, INPUT);
  pinMode(CODE_PIN, INPUT);
  pinMode(MULTIMETER_PIN, INPUT);

  //Set the light pins as outputs
  pinMode(SOLDER_LIGHT, OUTPUT);
  pinMode(CODE_LIGHT,  OUTPUT);
  pinMode(MULTIMETER_LIGHT, OUTPUT);

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
      myDFPlayer.playMp3Folder(/*File Number = */3);
      delay(1000);
      return SOLDER_IT;
    case 2:
      Serial.println("Code It");
      myDFPlayer.playMp3Folder(/*File Number = */4);
      delay(1000);
      return CODE_IT;
    case 3:
      Serial.println("Multimeter It");
      myDFPlayer.playMp3Folder(/*File Number = */5);
      delay(1000);
      return MULTIMETER_IT;
    default:
      Serial.println("ERROR: random number out of range");
      delay(1000);
      myDFPlayer.playMp3Folder(/*File Number = */3);
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
    timeLimit = 5000;
    score = 0;
  }

  //Loop through the main code if we are in the play condition
  if(playing){
    displaynumber(score);
    myDFPlayer.playMp3Folder(/*File Number = */1);
    delay(3000);
    Serial.println("Game Started");
    while(!gameOver && score < 99){
      //We will generate a random command here
      currentTask = generateCommand();
      Serial.print("Issued Command: ");
      Serial.println(currentTask);
      Serial.print("Time Limit: ");
      Serial.println(timeLimit);
      displaynumber(score);
      //Reset correct choice to false
      correctChoice = false;
      //Turn off the correct choice light right before we are looking for the next input
      digitalWrite(SOLDER_LIGHT, LOW);
      digitalWrite(CODE_LIGHT, LOW);
      digitalWrite(MULTIMETER_LIGHT, LOW);

      //Initialize the times for the countdown
      starttime = millis();
      endtime = starttime;
      //Check the input while the time is counting down
      while((starttime-endtime) < timeLimit){
        //Once we receive a command check if it matches the randomly generated command
        //If it does, break out of the loop and continue. If it doesn't break out of the loop and end the game
        //Serial.println("Waiting for input");
        //If the solder pin is high, the solder action has been completed
        if(digitalRead(SOLDER_PIN) == HIGH){
          Serial.println("Soldering is being done");
          //If this is the correct action, increment the score and light up the correct choice light for this action
          if(currentTask == SOLDER_IT){
            Serial.println("Correct Action");
            correctChoice = true;
            digitalWrite(SOLDER_LIGHT, HIGH);
          //Otherwise, set gameOver to true
          }else{
            gameOver = true;
          }
          break;
        }

        //If the code pin is high, the code action has been completed
        if(digitalRead(CODE_PIN) == HIGH){
          Serial.println("Coding is being done");
          //If this is the correct action, increment the score and light up the correct choice light for this action
          if(currentTask == CODE_IT){
            Serial.println("Correct Action");
            correctChoice = true;
            digitalWrite(CODE_LIGHT, HIGH);
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
            //Poll the analog pin for 500ms (and until the resistorVal read is not 0) and take the highest reading to make sure we are getting an accurate reading
            int resistorVal = analogRead(MULTIMETER_PIN);
            Serial.println(resistorVal);
            int start_time = millis();
            while(millis() - start_time < 500 || resistorVal == 0){
              int tempVal = analogRead(MULTIMETER_PIN);
              if(tempVal > resistorVal){
                resistorVal = tempVal;
              }
            }
            Serial.println(resistorVal);
            //If the correct resistor is being probed, increment the score and light up the correct choice light for this action
            // //If the reading is 0, something is wrong so keep reading the analog input until we get a valid input
            // while(resistorVal == 0){
            //   resistorVal = analogRead(MULTIMETER_PIN);
            //   Serial.println(resistorVal);
            // }
            if(resistorVal > 100 && resistorVal < 800){
              correctChoice = true;
              digitalWrite(MULTIMETER_LIGHT, HIGH);
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
        //Update the current time to continue counting down
        starttime = millis();
      }

      //Check to see if the correct selection was made
      if(correctChoice){
        //If the user made the correct choice, increment the score and decrease the available time by 0.1s
        score++;
        timeLimit *= 0.98;
        if(timeLimit < 1000){
          timeLimit = 1000;
        }
        Serial.println("Score increased");
      }else{
        Serial.println("Time Ran out or wrong choice");
        //Turn off all the correct lights
        digitalWrite(SOLDER_LIGHT, LOW);
        digitalWrite(CODE_LIGHT, LOW);
        digitalWrite(MULTIMETER_LIGHT, LOW);
        //If the user ran out of time, set gameOver to true
        gameOver = true;
      }
    }

    //If the game already ran and we now are at gameOver, play the end message
    if(gameOver){
      Serial.println("Game Over Initiated");
      myDFPlayer.playMp3Folder(/*File Number = */2);
      delay(2000);
      //Set playing to false
      playing = false;
    //If the game has already run and we got a perfect score, play win message.
    //Display score because the score is shown at the beginning of the loop, before it is updated
    }else if(score == 99){
      displaynumber(score);
      Serial.println("Win Condition Met");
      myDFPlayer.playMp3Folder(/*File Number = */6);
      playing = false;
    }

  }

}
