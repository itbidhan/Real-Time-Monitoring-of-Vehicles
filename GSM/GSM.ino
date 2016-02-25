#include <SoftwareSerial.h>
#include <EasyTransfer.h>
#define switchPin 10 
SoftwareSerial cell(2,3);  // We need to create a serial port on D2/D3 to talk to the GSM module
char mobilenumber[] = "xxxxxxxxxx";  // Replace xxxxxxxx with the recipient's mobile number

const int blue = 7;
const int pink = 11; //seat belt only
EasyTransfer ET;

struct DATA_STRUCTURE{
 char float_data;
};
 
DATA_STRUCTURE myData;
char area;
void setup()
{  //Initialize serial ports for communication.

Serial.begin(9600);
ET.begin(details(myData), &Serial);
cell.begin(9600);
delay(35000); // give the GSM module time to initialise, locate network etc.
// this delay time varies. 
pinMode(blue,INPUT);
pinMode(switchPin,INPUT);
digitalWrite(switchPin,HIGH);
pinMode(pink,INPUT);
}
void loop()
{
  Serial.println (digitalRead(blue));
  if(ET.receiveData())
{
  area=myData.float_data;
}
  if(digitalRead(blue)==HIGH)
 { 
   

   Serial.println("Entered");
cell.println("AT+CMGF=1"); // set SMS mode to text
cell.print("AT+CMGS=");  // now send message...
cell.write((byte)34);  // ASCII equivalent of "
cell.print(mobilenumber);
cell.write((byte)34);   // ASCII equivalent of "
cell.println();
delay(500); // give the module some thinking time
cell.print("Alert : overspeeding ");// our message to send
if(digitalRead(switchPin)== LOW)
      {
        cell.println("and violated seatbelt rule at");
      }

switch (area)
{
case 'a' :cell.println("ZONE 2");
        break;
case 'b' :cell.println("ROAD A");
        break;
case 'c':cell.println("ROAD B");
        break;
case 'd':cell.println("FEET ROAD");
        break;
case 'e':cell.println("Broadway ROAD");
        break;
case 'f':cell.println("ZONE 3");
        break;
case 'g':cell.println("GOTHAM ZONE");
        break;
default:cell.println("default");
}

cell.write((byte)26); // ASCII equivalent of Ctrl-Z
cell.println();
delay(15000); // the SMS module needs time to return to OK status
Serial.println("gsm");
 }
 
 
 if(digitalRead(pink)==HIGH)
 { 
   
if(digitalRead(switchPin)== LOW)
{
   Serial.println("Entered");
cell.println("AT+CMGF=1"); // set SMS mode to text
cell.print("AT+CMGS=");  // now send message...
cell.write((byte)34);  // ASCII equivalent of "
cell.print(mobilenumber);
cell.write((byte)34);   // ASCII equivalent of "
cell.println();
delay(500); // give the module some thinking time
cell.print("Alert : violating seat belt rule at");// our message to send

switch (area)
{
case 'a' :cell.println("ZONE 2");
        break;
case 'b' :cell.println("ROAD A");
        break;
case 'c':cell.println("ROAD B");
        break;
case 'd':cell.println("FEET ROAD");
        break;
case 'e':cell.println("Broadway ROAD");
        break;
case 'f':cell.println("ZONE 3");
        break;
case 'g':cell.println("GOTHAM ZONE");
        break;
default:cell.println("default");
}


cell.write((byte)26); // ASCII equivalent of Ctrl-Z
cell.println();
delay(15000); // the SMS module needs time to return to OK status
Serial.println("gsm");
 }
}
}

