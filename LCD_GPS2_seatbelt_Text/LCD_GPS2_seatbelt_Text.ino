#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <EasyTransfer.h>
EasyTransfer ET;

#define NUM_OFF 2
#define DELAY 50
#define TEST_BUTTON 9
#define DEFAULT_SPEED_LIMIT 50
#define NSPEEDZONES 6

struct DATA_STRUCTURE{
 char float_data;
};
 
DATA_STRUCTURE myData;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
boolean debugMode = true;

boolean speeding = false;
int blue;
int red;
int pink;


class Vertex {
public:
  Vertex(float llat, float llng) {
    lat = llat;
    lng = llng;
  }
  float lat;
  float lng;
};

class SpeedZone {
public:
  SpeedZone(int s) {
    speedLimit = s;
  }
  void setVertices(int n, Vertex *v) {
    nVertices = n;
    vertices = v;
  }
  int nVertices;
  Vertex *vertices;
  int speedLimit;
};


SpeedZone *speedZones[NSPEEDZONES];
TinyGPS gps;
SoftwareSerial nss(7, 8);
boolean buttonPressed = false;


void setup() {
  Serial.begin(9600);
  ET.begin(details(myData), &Serial);
  setupSpeedZones();
  blue = 13;
  red=6;
  pink=10;
    lcd.begin(16, 2);
   // lcd.print("WELCOME TO RAMTS");
 // lcd.setCursor(0,1);
 // lcd.print("BOB,SANDY");
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(pink, OUTPUT);
  pinMode(TEST_BUTTON, INPUT);
  digitalWrite(TEST_BUTTON, HIGH);


  // Allow EM-406a to power up
  delay(3000);

  // Establish serial connection to EM-406a
  nss.begin(9600);
}



void setupSpeedZones() {


//Pesit zone 1
speedZones[0] = &SpeedZone(35);
speedZones[0]->setVertices(4, (Vertex[4]){
Vertex(12.934822019841514,77.53277063369751),
Vertex(12.93051386533952,77.5354528427124),
Vertex(12.934131883590146,77.53944396972656),
Vertex(12.935763111652912,77.53628969192505)});

//zone 2
speedZones[1] = &SpeedZone(40);
speedZones[1]->setVertices(6, (Vertex[6]){
Vertex(12.9352821096151,77.53560304641724),
Vertex(12.936620547767827,77.53371477127075),
Vertex(12.938147195017908,77.53315687179565),
Vertex(12.938523627150266,77.53352165222168),
Vertex(12.936766939005077,77.5344443321228),
Vertex(12.935846764086554,77.53607511520386)});

//road A
speedZones[2] = &SpeedZone(45);
speedZones[2]->setVertices(4, (Vertex[4]){
Vertex(12.939642463745145,77.5328403711319),
Vertex(12.944632727518822,77.53350019454956),
Vertex(12.944792184274341,77.53727674484253),
Vertex(12.939485617796052,77.53697097301483)});
 
//ROAD B
speedZones[3] = &SpeedZone(20);
speedZones[3]->setVertices(6,(Vertex[6]){
Vertex(12.933379005502244,77.53882169723511),
Vertex(12.929959655598589,77.5435209274292),
Vertex(12.930430211117871,77.54373550415039),
Vertex(12.933891381114584,77.53921866416931)});
      
//Broadway Road
speedZones[4] = &SpeedZone(25);
speedZones[4]->setVertices(6, (Vertex[6]){
Vertex(12.944044566473055,77.53880023956299),
Vertex(12.943040768416472,77.55424976348877),
Vertex(12.94103316017857,77.55420684814453),
Vertex(12.942036966318216,77.5389289855957)});
      
//GOTHAM ZONE
speedZones[5] = &SpeedZone(30);
speedZones[5]->setVertices(4, (Vertex[4]){
Vertex(12.930712544003562,77.54338145256042),
Vertex(12.937593002233534,77.54916429519653),
Vertex(12.9374361549955,77.54949688911438),
Vertex(12.930545235665402,77.54373550415039)});
	
      
        if (debugMode) {
    printSpeedZones();
  }

}

/*
 * This is the point-in-polygon algorithm adapted
 */
boolean inSpeedZone(int speedZone, float lat, float lng) {
  SpeedZone *s = speedZones[speedZone];

  int i, j;
  boolean inside = false;
  for (i = 0, j = s->nVertices-1; i < s->nVertices; j = i++) {
    if ( ((s->vertices[i].lat > lat) != (s->vertices[j].lat > lat)) &&
         (lng < (s->vertices[j].lng - s->vertices[i].lng) * (lat - s->vertices[i].lat) / (s->vertices[j].lat - s->vertices[i].lat) + s->vertices[i].lng) )
       inside = !inside;
  }

  return inside;
}


boolean inSpeedZone(int speedZone) {
  float lat, lng;
  unsigned long  fix_age;

  // retrieves +/- lat/long in 100,000ths of a degree
  gps.f_get_position(&lat, &lng, &fix_age);

  return inSpeedZone(speedZone, lat, lng);
}
void loop() {
  
 if (gps.f_speed_kmph()>=20)
 {
digitalWrite(pink, HIGH); }

  if (readGPS()) {
    if (debugMode) {
      debug();
    }
    speeding = isSpeeding();
  }

  if (digitalRead(TEST_BUTTON) == LOW) {
    buttonPressed = true;
  } else {
    buttonPressed = false;
  }
  if ((speeding || buttonPressed) && (!(speeding && buttonPressed))) {
    policeLights();
  } else {
    allOff();
  }

}

bool readGPS() {
  while (nss.available()) {
    if (gps.encode(nss.read())) {
      return true;
    }
  }
  return false;
}

  
int getSpeedLimit() {
  boolean isInSpeedZone;

  for(int i=0;i<NSPEEDZONES;i++) {
    isInSpeedZone = inSpeedZone(i);
    if (isInSpeedZone) {
      return speedZones[i]->speedLimit;
    }
   
     
  }
  return DEFAULT_SPEED_LIMIT;
}

boolean isSpeeding() {
  int speed = (int)(gps.f_speed_kmph() + 0.5);
  int speedLimit = getSpeedLimit();

  if (speed > speedLimit) {
    return true;
  }
  return false;
}

boolean policeLights() {

digitalWrite(blue, HIGH);
digitalWrite(red, HIGH);
Serial.println("Message Sent");
}


boolean allOff() {
    digitalWrite(blue,LOW);
    digitalWrite(red, LOW);
    }

void printSpeedZones() {

  for(int i=0;i<NSPEEDZONES;i++) {
    SpeedZone *s = speedZones[i];
    Serial.println(s->speedLimit);
    for(int v=0;v<s->nVertices;v++) {
      Serial.print("(");
      Serial.print(s->vertices[v].lat,15);
      Serial.print(", ");
      Serial.print(s->vertices[v].lng,15);
      Serial.println(")");
    }
  }
}

void debug() {
  float lat, lng;
  unsigned long fix_age, time, date, speed, course;

  // retrieves +/- lat/long in 100000ths of a degree
  gps.f_get_position(&lat, &lng, &fix_age);

  Serial.println(getSpeedLimit());
  lcd.setCursor(0, 0);
  lcd.print("Limit:");
  switch(getSpeedLimit())
  {
    case 40 : lcd.print("40 zone2");
              Serial.println("Zone 2");
              myData.float_data= 'a';
              ET.sendData();
              delay(1000);
             break; 
    case 45 : lcd.print("45 ROAD A");
              Serial.println("Road A");
              myData.float_data= 'b';
              ET.sendData();
              delay(1000);
             break;
             
    case 25: lcd.print("25 ROAD B");
             Serial.println("ROAD B");
             myData.float_data= 'c';
              ET.sendData();
              delay(1000);
             break;
             
    case 30 : lcd.print("30 Broadway road");
             Serial.println("Broadway ROAD");
             myData.float_data= 'd';
              ET.sendData();
              delay(1000);
             break;
             
    case 20 : lcd.print("20 ZONE 1");
             Serial.println("ZONE 1");myData.float_data= 'e';
              ET.sendData();
              delay(1000);
             break;       
             
    case 35 : lcd.print("35 Pzone1");
             Serial.println("P ZONE 1");
             myData.float_data= 'f';
              ET.sendData();
              delay(1000);
             break;        
    default:lcd.print("50 GOTHAM");
            Serial.println("GOTHAM");
            myData.float_data= 'g';
              ET.sendData();
              delay(1000);
  }  
  
 
  Serial.print("lat: ");
  Serial.print(lat,15);
  Serial.print("    lng: ");
  Serial.print(lng,15);
  Serial.print("    speed: ");
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.print(gps.f_speed_kmph());
  lcd.print(" KMPH ");
  Serial.println(gps.f_speed_kmph());
}


