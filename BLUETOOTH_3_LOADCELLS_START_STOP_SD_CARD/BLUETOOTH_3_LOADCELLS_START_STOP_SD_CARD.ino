#include <SD.h>
//will be used in final 
long sample=0; // Sample for calibrating
long count=0; // Instant Reading value
//
long sample2=0; // Sample for calibrating
long count2=0; // Instant Reading value
//
long sample3=0; // Sample for calibrating
long count3=0; // Instant Reading value
//
unsigned long time_from_reading_start;
unsigned long general_time;
char Incoming_value = 0;
int memory_controller;
int readings_counter;
File myFile;
//
byte DT1=14;
byte SCK1=15;
byte DT2=16;
byte SCK2=17;
byte DT3=18;
byte SCK3=19;
//
unsigned long readrealcount(int x){
  if(x==1){
    SCK1=15;
    DT1=14;    
  }
  if(x==2){
    SCK1=SCK2;
    DT1=DT2; 
  }
  if(x==3){
    SCK1=SCK3;
    DT1=DT3; 
  }
   unsigned long Count;
   unsigned char i;
   pinMode(DT1, OUTPUT);
   digitalWrite(DT1,HIGH);
   digitalWrite(SCK1,LOW);
   Count=0;
   pinMode(DT1, INPUT);
   while(digitalRead(DT1));
   for (i=0;i<24;i++)
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
   return(Count);
}
//
void setup()
{
readings_counter = 0;

Serial.begin(9600); // Serial port init

pinMode(10, OUTPUT);//SD card    

pinMode(15, OUTPUT); // Clock pin set up
pinMode(17, OUTPUT); // Clock pin set up
pinMode(19, OUTPUT); // Clock pin set up
//
if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
}
//opnings sd file
myFile = SD.open("test.txt", FILE_WRITE);
//checking file
if (myFile) {
  Serial.print("initialization of sd done");
  Serial.print("|");
  Serial.print("initialization of sd done");
  Serial.print("|");
  Serial.print("initialization of sd done");
  Serial.print("|");
} else {
  Serial.println("error opening test.txt");
  Serial.print("|");
  Serial.println("error opening test.txt");
  Serial.print("|");
  Serial.println("error opening test.txt");
  Serial.print("|");
}

//calibration
calibrate();
calibrate1();
//calibrate2();
//
Serial.print("calibrating 1");
Serial.print("|");
Serial.print("calibrating 2");
Serial.print("|");
Serial.print("calibrating 3");
Serial.print("|");
delay(2000);
Incoming_value = 0;
general_time = millis();
}
//***********************************************************************************************
void loop()
{
myFile = SD.open("test.txt", FILE_WRITE);
//used in order to constantly recieve values from phone
Serial.setTimeout(100);


//1ST
count = readrealcount(1); // reciveing the sensor value from the rutine 
long val1=(count-sample); // error deleting 
float vout=(val1*0.43); // Vout calculating (in nVolts ) 1=0.43nV
int e=(vout/3.77)*0.002; //microstrain 
delay(1000);

//2ND
count2 = readrealcount(2); // reciveing the sensor value from the rutine 
long val2=(count2-sample2); // error deleting 
float vout2=(val2*0.43); // Vout calculating (in nVolts ) 1=0.43nV
int e2=(vout2/3.77)*0.002; //microstrain 
delay(1000);

//3RD
/*
count3 = readrealcount(3); // reciveing the sensor value from the rutine 
long val3=(count3-sample3); // error deleting 
float vout3=(val3*0.43); // Vout calculating (in nVolts ) 1=0.43nV
int e3=(vout3/3.77)*0.002; //microstrain 
delay(1000);*/

//

Incoming_value = Serial.parseInt();      
    
if(Incoming_value == 1) {   
  memory_controller=1;         
  }  
else if(Incoming_value == 2){ 
  time_from_reading_start = general_time;
  readings_counter+=1;      
  memory_controller=2;
  myFile.print("set of readings: ");
  myFile.println(readings_counter);
} else if(Incoming_value == 3){
  memory_controller=3;
}
   
   
if(memory_controller == 2){

  time_from_reading_start = time_from_reading_start - general_time;
  myFile.print(time_from_reading_start);
  myFile.print(" ");
  myFile.print(e);
  myFile.print(" ");
  myFile.print(e2);
  myFile.print(" ");
  myFile.print(e2);
  myFile.print(" ");
  myFile.println(e2);
}else if(memory_controller == 3){ 

}else if(memory_controller == 1){
  
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
  //calibration
  calibrate();
  calibrate1();
  //calibrate2();
  delay(2000);
  //Incoming_value = 0;
  memory_controller = 3;

}
//print1
Serial.print("Measured microstrain 1:  ");
Serial.print(int(e));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");

//print2
Serial.print("Measured Strain 2:  ");
Serial.print(int(e2));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");

//print3
Serial.print("third strain 3:  ");
Serial.print(int(e2));
Serial.println(" ");
Serial.println(" ");
Serial.print("|");
delay(100);

myFile.close();
}


void calibrate()
{ 
for(int i=0;i<100;i++) // Start sampling for calibrating
{
count=readrealcount(1); // Add sample values 
sample+=count;
}
sample/=100;
count=0;
}

void calibrate1()
{ 
for(int i=0;i<100;i++) // Start sampling for calibrating
{
count2=readrealcount(2); // Add sample values 
sample2+=count2;
}
sample2/=100;
count2=0; 
}

/*
void calibrate2()
{ 
 for(int i=0;i<100;i++) // Start sampling for calibrating
 {
 count2=readrealcount(3); // Add sample values 
 sample2+=count2;
 }
 sample2/=100;
 count2=0;
}
*/
