
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/

char ID[] = "EVE";

#define ONE_WIRE_BUS 18
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

int status = WL_IDLE_STATUS;
char ssid[] = "S301"; //  your network SSID (name)
char pass[] = "S301S301";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;// your network key Index number (needed only for WEP)

unsigned int PCPort = 51885;
char PCIP[] = "192.168.1.105";

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[4096] = "acknowledged";       // a string to send back

WiFiUDP Udp;

const int relay1 = 5;
const int relay2 = 18;
const int relay3 = 19;

const int CT1R = 34;
const int CT1W = 35;
const int CT2R = 32;
const int CT2W = 33;
const int CT3R = 36;
const int CT3W = 39;
const int SupplyVoltage = 3300;
const int ADC_COUNTS = 4096;
const float ICAL = 100;
float CURRES[3];

float Temp = 0;
float GotTemp = 0;

char hold1[] = "";
char hold2[] = "";


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");

   sensors.begin(); 
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    ControlRelays(packetBuffer);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.printf(ReplyBuffer);
    Udp.endPacket();
  }

    calcIrms(1480, CURRES); 
    sprintf(ReplyBuffer, "%s,%f,%f,%f" , ID, CURRES[0], CURRES[1], CURRES[2]);
    Serial.println(ReplyBuffer);
      
    Udp.beginPacket(PCIP, PCPort);
    Udp.printf(ReplyBuffer);
    Udp.endPacket();
    delay(1000);
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void ControlRelays(char recieved[]){
  Serial.print(recieved[0]);
  if (strcmp(recieved, "1") == 0){
    digitalWrite(relay1, HIGH);
  }
  if (strcmp(recieved, "2") == 0){
    digitalWrite(relay2, HIGH);
  }
  if (strcmp(recieved, "3") == 0){
    digitalWrite(relay3, HIGH);
  }
  if (strcmp(recieved, "4") == 0){
    digitalWrite(relay1, LOW);
  }
  if (strcmp(recieved, "5") == 0){
    digitalWrite(relay2, LOW);
  }
  if (strcmp(recieved, "6") == 0){
    digitalWrite(relay3, LOW);
  }
}

void calcIrms(unsigned int Number_of_Samples, float CURRES[])
{
  double filteredI = 0;
  double offsetI = (ADC_COUNTS/2);
  double sqI = 0;
  double sumI = 0;
  double filteredI2 = 0;
  double offsetI2 = (ADC_COUNTS/2);
  double sqI2 = 0;
  double sumI2 = 0;
  double filteredI3 = 0;
  double offsetI3 = (ADC_COUNTS/2);
  double sqI3 = 0;
  double sumI3 = 0;
  int sampleI3 = analogRead(CT3R);
  int gndsampleI3 = analogRead(CT3W);
  int sampleI2 = analogRead(CT2R);
  int gndsampleI2 = analogRead(CT2W);
  int sampleI1 = analogRead(CT1R);
  int gndsampleI1 = analogRead(CT1W);
  
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI1 = analogRead(CT1R);
    gndsampleI1 = analogRead(CT1W);
   
    filteredI = sampleI1 - gndsampleI1 - ((sampleI1-offsetI)/1024);
    offsetI = (offsetI + (sampleI1-offsetI)/1024);

    sqI = filteredI * filteredI;
    sumI += sqI;

    sampleI2 = analogRead(CT2R);
    gndsampleI2 = analogRead(CT2W);
   
    filteredI2 = sampleI2 - gndsampleI2 - ((sampleI2-offsetI2)/1024);
    offsetI2 = (offsetI2 + (sampleI2 - offsetI2)/1024);

    sqI2 = filteredI2 * filteredI2;
    sumI2 += sqI2;

    sampleI3 = analogRead(CT3R);
    gndsampleI3 = analogRead(CT3W);
   
    filteredI3 = sampleI3 - gndsampleI3 - ((sampleI3-offsetI3)/1024);
    offsetI3 = (offsetI3 + (sampleI3 - offsetI3)/1024);

    sqI3 = filteredI3 * filteredI3;
    sumI3 += sqI3;

    delay(1);
    
  }

  double I_RATIO = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  double Irms = I_RATIO * sqrt(sumI / Number_of_Samples);

  double I_RATIO2 = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  double Irms2 = I_RATIO2 * sqrt(sumI2 / Number_of_Samples);

  double I_RATIO3 = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  double Irms3 = I_RATIO3 * sqrt(sumI3 / Number_of_Samples);

  CURRES[0] = Irms;
  CURRES[1] = Irms2;
  CURRES[2] = Irms3;
  
}
