/****************************************************************************************************/
/*This lab is to design a controller for traffic lights that face one traffic direction.
  There are three colors of lights, represented with Red, Yellow, Green LEDs. 
  The traffic lights operate in the following patterns:

 (1) At the start of the system (power up), the Red light flashes (1-second on then 1-second
     off) until a button is pressed.
     
 (2) The Red light stays on for 20 seconds before the Green light is turned on
 
 (3) The Green light stays on for 20 seconds before switching the Yellow light on.
 
 (4) The Yellow light stays on for 3 seconds (Y1); then the Yellow light blinks (on/off 200
     msec) for 3 seconds (Y2) before switching back to Red light.
     
 (5) The R-G-Y1-Y2 pattern continues until the system is powered off.
 
 (6) An active buzzer beeps for 3 seconds before a light is changed.

  --------------------Additional requirements for EECE.5520 students--------------------------------
 (7) A four-digit 7-Segment display shows the time (in seconds) remaining for each light.
     Number is displayed in hex-decimal. For 0 .. 9, A .. F. For the 7-segment, number A is
    (ABC, EFG), B,“b”, is (CDEFG), C is (ADEF), D “d” (BCDEG), E is (ADEFG) and F is
    (AEFG)*/


#include <GOTStateMachine.h>  //Header file used that contains state machine library

int red = 10;                //Variable Decleration for Red LED 
int yellow = 9;              //Variable Decleration for Yellow LED
int green = 8;               //Variable Decleration for Green LED
int buzzer = 3;              //Variable Decleration for Buzzer
int buttonState = 0;         //Variable Decleration for the state/postion of push button
const int buttonPin = 2;     //Variable Decleration for push button

//***** State Machine *****
GOTStateMachine stateMachine(50); // execute every 50 milliseconds

// States
void Flash_Start();  //Initial State State 0
void Red();          //State 1
void Green();        //State 2
void Yellow1();      //State 3
void Yellow2();      //State 4


int latch = 11;  //74HC595 pin 11STCP/R_CLK
int clock = 13;  //74HC595 pin 13 SHCP/SR_CLK
int data = 12;   //74HC595 pin 12 DS/SER

const int digit0   = 7;   // 7-Segment pin Digit 0
const int digit1   = 6;   // 7-Segment pin Digit 1
const int digit2   = 5;   // 7-Segment pin Digit 2
const int digit3   = 4;   // 7-Segment pin Digit 3 

byte controlDigits[] = { digit0, digit1, digit2, digit3 };  // pins to turn off & on digits

unsigned long onTime = 0;             // Tracks time
bool switchView = false;              // Switch between HexCounter (table array) and RawDisplay (raw bytes)
                                      // False = HexCounter
                                      // True = RawDisplay
                                      
unsigned int counter = 0;             // RawDisplay counter

////// 4 Digit 7 Segment LED Display Table //////
byte table[]=
{
        0x5F,  // = 0                 
        0x44,  // = 1                
        0x9D,  // = 2
        0xD5,  // = 3
        0xC6,  // = 4
        0xD3,  // = 5
        0xDB,  // = 6
        0x45,  // = 7
        0xDF,  // = 8
        0xC7,  // = 9
        0xCF,  // = A = 10
        0xDA,  // = b = 11
        0x1B,  // = C = 12
        0xDC,  // = d = 13
        0x9B,  // = E = 14
        0x8B,  // = F = 15
        0x00   // blank
  
  };
//////////////////////////////////////////////////


void setup() 
{
  
  /////////////////////////////// LED OUTPUTS /////////////////////////////////////
  pinMode(red,OUTPUT);       //Initialize digital pin 10 (red) as an output.
  pinMode(yellow,OUTPUT);    //Initialize digital pin 9 (yellow) as an output.
  pinMode(green,OUTPUT);     //Initialize digital pin 8 (green) as an output.
  pinMode(buzzer,OUTPUT);    //Initialize digital pin 3 (buzzer) as an output.
  pinMode(buttonPin,INPUT);  //Initialize digital pin 2 (buttonPin) as an output.

  //////////////////////////// 8 Bit Shift Register ////////////////////////////////
  pinMode(latch,OUTPUT);    //Initialize digital pin 11 (latch)as an output.
  pinMode(clock,OUTPUT);    //Initialize digital pin 13 (clock) as an output.
  pinMode(data,OUTPUT);     //Initialize digital pin 12 (data) as an output.
  
  //////////////////////// Initialize Digit Places as Outputs ///////////////////////
  pinMode(digit0,OUTPUT);    //Initialize digital pin 7 (digit0) as an output.
  pinMode(digit1,OUTPUT);    //Initialize digital pin 6 (digit1) as an output.
  pinMode(digit2,OUTPUT);    //Initialize digital pin 5 (digit2) as an output.
  pinMode(digit3,OUTPUT);    //Initialize digital pin 4 (digit) as an output.

  //////////////////////// State Machine Initialization ///////////////////////////
  stateMachine.setStartState(Flash_Start); // Initialise state machine
}

 
void Display(byte num)
  ///////////// Data Shitt Register Procsss ///////////////////
{
  digitalWrite(latch,LOW);
  shiftOut(data,clock,MSBFIRST,table[num]);
  digitalWrite(latch,HIGH);
}


void loop()
{
buttonState = digitalRead(buttonPin);        //Reads the state of the pushbutton
digitalWrite(digit0,HIGH);                   // Turns off power to digit place 0
digitalWrite(digit1,HIGH);                   // Turns off power to digit place 1
digitalWrite(digit2,HIGH);                   // Turns off power to digit place 2
digitalWrite(digit3,HIGH);                   // Turns off power to digit place 3
stateMachine.execute();                      // Process the states as required and starts the state machine
}


//***** States *****
void Flash_Start()
{
////////// State 0 /////////////   RED LED Flasshes
if (buttonState == LOW)
  {
    // State logic
    // turn Red LED on:
    digitalWrite(red,HIGH);
    digitalWrite(yellow,LOW);
    digitalWrite(green,LOW);
    delay(1000);
    // turn Red LED off:
    digitalWrite(red,LOW);
    digitalWrite(yellow,LOW);
    digitalWrite(green,LOW);
    delay(1000);
   }
   
else
  {
  stateMachine.changeState(Red);  //Transitions to next state if button state false
  return;
  }
}

void Red()
{
////////// State 1 /////////////   RED LED Solid (20 seconds)
  // State logic
    digitalWrite(red,HIGH);
    digitalWrite(yellow,LOW);
    digitalWrite(green,LOW);


if (stateMachine.isDelayComplete(5000))   // Starts Countdown of 4 Digit 7 Segment Disply (Last 15 seconds) after first 5 seconds
  {
    for (int count=15; count>=0; count--)
            {
             digitalWrite(digit0,LOW);    //Turns on power to digit0
             digitalWrite(digit1,LOW);    //Turns on power to digit1
             digitalWrite(digit2,LOW);    //Turns on power to digit2
             digitalWrite(digit3,LOW);    //Turns on power to digit3
             Display(count);              //Prints out table data to 4 Digit 7 Segment Disply
                   if (count < 4 && count > 0)
                      {
                      ///////// Activates Buzzer for last 3 seconds//////////
                       digitalWrite(buzzer,HIGH);
                       delay(125);
                       digitalWrite(buzzer,LOW);
                       delay(125);
                      }

                   if (count>3)
                      {
                       delay(500);
                      }
                   else
                      {
                        delay(250);
                      }
                            
                   delay(500);
             }
                              
   digitalWrite(digit0,HIGH);  //Turns off power to digit0
   digitalWrite(digit1,HIGH);  //Turns off power to digit1
   digitalWrite(digit2,HIGH);  //Turns off power to digit2
   digitalWrite(digit3,HIGH);  //Turns off power to digit3
   stateMachine.changeState(Green);   //Transitions to next state
   return;
  }
}

void Green()
{
////////// State 2 ///////////// GREEN LED Solid (20 seconds)
  // State logic
    digitalWrite(red,LOW);
    digitalWrite(yellow,LOW);
    digitalWrite(green,HIGH);

if (stateMachine.isDelayComplete(5000)) // Starts Countdown of 4 Digit 7 Segment Disply (Last 15 seconds) after first 5 seconds
   {
        for (int count=15; count>=0; count--)
           {
            digitalWrite(digit0,LOW);     //Turns on power to digit0
            digitalWrite(digit1,LOW);     //Turns on power to digit1
            digitalWrite(digit2,LOW);     //Turns on power to digit2
            digitalWrite(digit3,LOW);     //Turns on power to digit3
            Display(count);               //Prints out table data to 4 Digit 7 Segment Disply
                 if (count < 4 && count > 0)
                    {
                    ///////// Activates Buzzer for last 3 seconds//////////
                     digitalWrite(buzzer,HIGH);
                     delay(125);
                     digitalWrite(buzzer,LOW);
                     delay(125);
                    }

                  if (count>3)
                    {
                     delay(500);
                    }
                  else
                    {
                     delay(250);
                    }

                     delay(500);
                          }
  digitalWrite(digit0,HIGH);    //Turns on power to digit0
  digitalWrite(digit1,HIGH);    //Turns on power to digit0
  digitalWrite(digit2,HIGH);    //Turns on power to digit0
  digitalWrite(digit3,HIGH);    //Turns on power to digit0

  stateMachine.changeState(Yellow1);   //Transitions to next state
  return;
 }
}

void Yellow1(){
////////// State 3 ///////////// YELLOW LED Solid (3 seconds)
  // State logic
    digitalWrite(red,LOW);
    digitalWrite(yellow,HIGH);
    digitalWrite(green,LOW);

 
 if (stateMachine.isDelayComplete(3000))  // Transitions after (3 Seconds)
  {
    stateMachine.changeState(Yellow2); //Transitions to next state
    return;
  }
}

void Yellow2(){
////////// State 4 ///////////// YELLOW LED Flashing (3 seconds)
  // State logic
    digitalWrite(red,LOW);
    digitalWrite(yellow,LOW);
    digitalWrite(green,LOW);
    delay(200);
    digitalWrite(red,LOW);
    digitalWrite(yellow,HIGH);
    digitalWrite(green,LOW);
    delay(200);

  // Transitions
 if (stateMachine.isDelayComplete(3000))  // Transitions after (3 Seconds)
  {
    stateMachine.changeState(Red);  //Transitions to next state
    return;
  }

}
