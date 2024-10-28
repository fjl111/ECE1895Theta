#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>

typedef enum {
    SOLDER_IT,
    CODE_IT,
    MULTIMETER_IT
} COMMAND;

//16x2 display, I2C address
LiquidCrystal_I2C lcd(0x27,16,2);
//Define SD pins
#define SD_ChipSelectPin 10
TMRcpm tmrcpm;

//Define the pins
#define SOLDER_PIN 4
#define CODE_PIN 5
#define MULTIMETER_PIN 23
#define ON_BUTTON 6

//Create the global variables
int score, time, playing;
bool gameOver;
COMMAND currentTask;

void setup() {
  // set up LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Score: ");
  delay(2000);

  //Connect to the serial monitor
  Serial.begin(9600);

  Serial.println("Startup");

  //Set the global variables
  score = 0;
  time = 0;
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

void loop() {

  displaynumber(99);

  // //If the on button is pressed, turn the playing variable on so that the game starts and reset gameOver
  // if(digitalRead(ON_BUTTON) == HIGH){
  //   gameOver = false;
  //   playing = true;
  // }

  // //Loop through the main code if we are in the play condition
  // if(playing){
  //   while(!gameOver){
  //     //We will generate a random command here
  //     currentTask = SOLDER_IT;

  //     //Check the input while the time is counting down
  //     while(time > 0){
  //       //Once we receive a command check if it matches the randomly generated command
  //       //If it does, break out of the loop and continue. If it doesn't break out of the loop and end the game

  //       //If the solder pin is high, the solder action has been completed
  //       if(digitalRead(SOLDER_PIN) == HIGH){
  //         Serial.println("Soldering is being done");
  //         //If this is the correct action, increment the score
  //         if(currentTask == SOLDER_IT){
  //           Serial.println("Correct Action");
  //           score++;
  //         //Otherwise, set gameOver to true
  //         }else{
  //           gameOver = true;
  //         }
  //         break;
  //       }

  //       //If the code pin is high, the code action has been completed
  //       if(digitalRead(CODE_PIN) == HIGH){
  //         Serial.println("Coding is being done");
  //         //If this is the correct action, increment the score
  //         if(currentTask == CODE_IT){
  //           Serial.println("Correct Action");
  //           score++;
  //         //Otherwise, set gameOver to true
  //         }else{
  //           gameOver = true;
  //         }
  //         //Exit the loop waiting for an action since an action was complete
  //         break;
  //       }

  //       //If the anologue pin is not close to 0 (being pulled down), the multimeter action is being complete
  //       if(analogRead(MULTIMETER_PIN) > 10){
  //         //If this is the correct action, check to make sure the correct resistor is being probed (the analogue input is in the right range)
  //         if(currentTask == MULTIMETER_IT){
  //           Serial.println("Correct Action");
  //           int resistorVal = analogRead(MULTIMETER_PIN);
  //           Serial.println(resistorVal);
  //           //If the correct resistor is being probed, increment the score
  //           if(resistorVal > 0 && resistorVal < 1023){
  //             score++;
  //           //Otherwise, set game over to true
  //           }else{
  //             gameOver = true;
  //           }
  //         //Otherwise, set gameOver to true
  //         }else{
  //           gameOver = true;
  //         }
  //         //Exit the loop waiting for an action since an action was complete
  //         break;
  //       }
  //     }
      
  //   }
  // }

  //If the game already ran and we now are at gameOver, play the end message
  if(gameOver){

  }
}
