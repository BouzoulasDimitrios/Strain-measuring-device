#include <SD.h>     //SD CARD BREAKOUT BOARD LIBRARY
  
long sample = 0 ;   //TEMPORARY STORAGE VALUE FOR CALIBRARION
long sample_temp;   //TEMPORARY VALUE THAT STORES 
int x;              //VALUE REPRESENTING THE HX USED (1,2,3)
int e1,e2,e3;       //variables that represent the 3 strain outputs
//
long count = 0;     // Instant Reading value
//
long sample1 = 0;   // Sample for calibrating
long sample2 = 0;   // Sample for calibrating
long sample3 = 0;   // Sample for calibrating
//
unsigned long time_from_reading_start; //time point that the first recorded reading took place
unsigned long general_time;
char Incoming_value = 0;               //VALUE INPUTED TO THE ARDUINO FROM PHONE, RESPONSIBLE FOR WITING SO SD CARD AND RECALIBRATING THE DEVICE
int memory_controller;                 //VARIABLE USED TO STORE THE VALUE THAT IS INPUTED FROM THE PHONE APPLICATION
int readings_counter;                  //USED TO STORE THE AMOUNT OF TIMES WE WROTE DATA TO THR SD CARD
File myFile;
//
byte DT1=14;  //
byte SCK1=15; //SPI Clock
byte DT2=16;  //
byte SCK2=17; //
byte DT3=18;  //  
byte SCK3=19; //
//
//### READING ONCE FROM HX711###//
unsigned long readrealcount(int x){ 
                  //## SELECTION OF INPUT PINS BASED ON FUNCTION INPUT
  if(x==1){       //## FIRST HX711  
    SCK1=15;
    DT1=14;    
  }
  if(x==2){       //## SECOND HX711
    SCK1=SCK2;
    DT1=DT2; 
  }
  if(x==3){       //## THIRD HX711
    SCK1=SCK3;
    DT1=DT3; 
  }
  
   /*pulse reading function to avoid using the standart hx711 library
    * as we want to get the voltage diffrence in order to calculate strain 
    * and not the weight   */
    
   unsigned long Count;
   unsigned char i;
   pinMode(DT1, OUTPUT);      //SETTING DT PIN TO OUTPUT MODE 
   digitalWrite(DT1,HIGH);    //SETTING DT HIGH 
   digitalWrite(SCK1,LOW);    //SETTING SCK TO LOW 
   Count=0;
   pinMode(DT1, INPUT); 
   while(digitalRead(DT1));   //WAIT UNTILL DT(DATA LINE) GOES LOW
   for (i=0;i<24;i++)         //PULSE THE CLOCK PIN(SCK) 24 TIMES TO READ THE DATA(24BIT SIGNED INTEGER) FROM HX711 
   {
   digitalWrite(SCK1,HIGH);
   Count=Count<<1;
   digitalWrite(SCK1,LOW);
   if(digitalRead(DT1))
   Count++;
   }
   digitalWrite(SCK1,HIGH);
   Count=Count^0x800000;
   digitalWrite(SCK1,LOW);
   return(Count);             //RETURN THE FINAL READING
}
//
void setup()
{
readings_counter = 0;         //NUMBER OF TIMES WE WROTE OUR READINGS TO THE SD CARD IS SET TO 0  
Serial.begin(9600);           //SERIAL PORT INITIALIZATION  
pinMode(10, OUTPUT);          //SETTING PIN 10 AS OUTPUT IN ORDER TO SET UP THE SD CARD BREAKOUT BOARD 

pinMode(15, OUTPUT);          // CLOCK PIN HX711(1ST)
pinMode(17, OUTPUT);          // CLOCK PIN HX711(2ND)
pinMode(19, OUTPUT);          // CLOCK PIN HX711(3RD)

//#### CHECK IF SD CARD HAS BEEN INITIALIZED ####
if (!SD.begin(10)) {          
  Serial.print("initialization failed!");
  Serial.print("|");
  Serial.print("initialization failed!");
  Serial.print("|");
  Serial.print("initialization failed!");
  Serial.print("|");
  return;
}
  Serial.print("initialization of sd done");
  Serial.print("|");
  Serial.print("initialization of sd done");
  Serial.print("|");
  Serial.print("initialization of sd done");
  Serial.print("|");
//
myFile = SD.open("test.txt", FILE_WRITE); // OPEN THE SD CARD FILE
//

//### CHECK THAT THE OUR FILE OPENED IN THE SD CARD ###
if (myFile) {  
  Serial.print("Ready to record");
  Serial.print("|");
  Serial.print("Ready to record");
  Serial.print("|");
  Serial.print("Ready to record");
  Serial.print("|");
} else {
  Serial.println("unable to record");
  Serial.print("|");
  Serial.println("unable to record");
  Serial.print("|");
  Serial.println("unable to record");
  Serial.print("|");
}


  //### CALIBRATION SEQUENCE FOR ALL THE HX711 ###
  //the " " and the "|" are needed in order to print them in order and have them centered in the app 
  Serial.print("calibrating 1"); 
  Serial.println(" "); 
  Serial.println(" "); 
  Serial.print("|"); 
  Serial.print("calibrating 2"); 
  Serial.println(" "); 
  Serial.println(" "); 
  Serial.print("|"); 
  Serial.print("calibrating 3"); 
  Serial.println(" "); 
  Serial.println(" "); 
  Serial.print("|"); 
  delay(500);
  
  Incoming_value = 0;       //THIS VALUE IS SET TO ZERO SO THE DEVICE STARTS OFF WITHOUT WRITING TO THE SD CARD

  sample1 = calibration(1); // calibration of the 1ST hx711
  sample2 = calibration(2); // calibration of the 2ND hx711
  sample3 = calibration(3); // calibration of the 3RD hx711
  
}

//***********************************************************************************************

void loop()
{
myFile = SD.open("test.txt", FILE_WRITE); //OPENING THE TEXT FILE THAT WILL STORE OUR READINGS

Serial.setTimeout(100);                   //THE TIMOUT FUNCTIONS STOPS WAITING FOR AN INPUT AFTER A SET AMOUNT OF TIME 
                                          //IN OUR CASE WE NEED IT BECAUSE WE DONT HAVE A CONSTANT INPUT STRAM FROM THE USER
                                          
//### TAKING A READING FROM EVERY HX711 ###
e1 = reading(1);
delay(50);
e2 = reading(2);
delay(50);
e3 = reading(3);
delay(50);
//

Incoming_value = Serial.parseInt();       //READING VALUE FROM PHONE(WAITS FOR 100 MS)

// the incoming value resets to zero after every loop because if the phone doesnt input anything it takes it as a zero
// so we save the value of the input in a variable:

// THE INCOMING VALUE RESETS TO ZERO IF THE IS NO INPUT FROM THE PHONE 
// AS A RESULT WE NEED TO SAVE THE INCOMING VALUE INTO A SEPARATE VALUE

if(Incoming_value == 1) {              //RECALIBRATION SEZUENCE
  memory_controller=1;                 //SAVING THE INCOMING VALUE SO IT DOESNT RESET TO 0 IN THE NEXT LOOP
}  
else if(Incoming_value == 2){          //START WRITING TO SD CARD
  time_from_reading_start = millis();  //saves time of start to a value
  readings_counter+=1;                 //INCREACE THE NUMBER OF READING CYCLES BY ONE
  memory_controller=2;                 //SAVING THE INCOMING VALUE SO IT DOESNT RESET TO 0 IN THE NEXT LOOP
  myFile.print("set of readings: ");   //WRITING TO THE SD CARD THE NUM OF THE CURRENT READING SYCLE 
  myFile.println(readings_counter);
}
else if(Incoming_value == 3){           //STOP WRITING TO THE SD CARD
  memory_controller=3;                  //SAVING THE INCOMING VALUE SO IT DOESNT RESET TO 0 IN THE NEXT LOOP
}


//### READ/WRITE TO SD AND RECALIBRATE ###

if(memory_controller == 2){                        //PRINT TO SD CARD

  general_time = millis()-time_from_reading_start; //SAVING THE TIME OF THE READING 
  myFile.print(general_time/1000);                 //RESETING THE TIME FROM SECONDS TO MS
  myFile.print(" ");                               //WRITING THE READINGS
  myFile.print(e1);
  myFile.print(" ");
  myFile.print(e2);
  myFile.print(" ");
  myFile.print(e3);
  myFile.println(" ");

}else if(memory_controller == 3){                  //STOP PRINTING READINGS(DO NOTHING IF INPUTEED VAL == 3)

}else if(memory_controller == 1){                  //START RECALIBRATION SEQUENCE

  delay(100);
  Serial.print("calibrating 1");                   //PRINT TO THE PHONE THAT THE DEVICE IS CALIBRATING
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("|");
  Serial.print("calibrating 2");
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("|");
  Serial.print("calibrating 3");
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("|");
  
  sample1 = calibration(1);                         //CALIBRATION OF 1ST HX711
  sample2 = calibration(2);                         //CALIBRATION OF 2ND HX711
  sample3 = calibration(3);                         //CALIBRATION OF 1RD HX711
  delay(100);
  memory_controller = 3;                            //stoping the recordings on the sd card (DO NOTHING)    
}

//###PRINTING TO PHONE###
  
//### PRINTING VALUES OF THE FIRST GAUGE ###
Serial.print("1: ");
Serial.print(int(e1));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");

//### PRINTING VALUES OF THE SECOND GAUGE ###
Serial.print("2: ");
Serial.print(int(e2));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");

//### PRINTING VALUES OF THE THIRD GAUGE ###
Serial.print("3: ");
Serial.print(int(e3));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");
delay(500);

myFile.close(); //CLOSING THE SD CARD FILE
}


unsigned long reading(int x){   //function for taking a reading from the HX711
  
  if(x==1){sample_temp = sample1;
  }else if(x==2){sample_temp = sample2;
  }else if(x==3){sample_temp = sample3;}
  
  count = readrealcount(x);     // SAVING THE READING VALUE INTO A VRIABLE
  
  long val=(count-sample_temp); // DELETING THE ERROR THAT WE DETECTED IN THE CALIBRATION SEQUENCE
  float vout=(val*0.43);        // CONVRETING THE VAKUE INTO VOLTS (in nVolts ) 1=0.43nV
  int e=4*(vout/3.77)*0.002;    // CONVERTION TO MICROSTRAIN 
  return e;                     // RETURNING THE MICROSTRAIN 
}

unsigned long calibration(int x){   //CALIBRAION FUNCTION
  sample = 0;                       //RSET TEMPORARY DATA STORAGE VALUE TO ZERO
  for(int i=0;i<100;i++)            //SAVING 100 READINGS INTO THE TEMPORARY VALUE
    {
    count = readrealcount(x);       
    sample+=count;                  
    }
  sample/=100;                      //AVERAGING THEM OUT
  count = 0;                        //RESETING THE VALUES TO ZERO
  return sample;                    
}
