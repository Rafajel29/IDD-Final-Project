
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
/********************************************************************/

char ID[] = "Boogie";

int status = WL_IDLE_STATUS;
char ssid[] = "RedRover"; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;// your network key Index number (needed only for WEP)

unsigned int PCPort = 51885;
char PCIP[] = "10.148.132.251";    //PC IP

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[4096] = "acknowledged";       // a string to send back

WiFiUDP Udp;

const int relay1 = 5;
const int relay2 = 18;
const int relay3 = 19;
const int relay4 = 21;

const int CT2R = 32;
const int CT2W = 33;
const int SupplyVoltage = 1650;
const int ADC_COUNTS = 4096;
const float ICAL = 100;
float CURRES[1];
const int StatLED = 22;

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
  pinMode(relay4, OUTPUT);

  digitalWrite(relay1, HIGH); 
  digitalWrite(relay2, HIGH); 
  digitalWrite(relay3, HIGH); 
  digitalWrite(relay4, HIGH); 
  
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
  
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
  pinMode(StatLED, OUTPUT);
  digitalWrite(StatLED, HIGH);

  //digitalWrite(relay1, LOW); 
  //digitalWrite(relay2, LOW); 
  //digitalWrite(relay3, LOW); 
  //digitalWrite(relay4, LOW); 
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
    sprintf(ReplyBuffer, "%s,%f" , ID, CURRES[0]);
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
  double filteredI2 = 0;
  double offsetI2 = (ADC_COUNTS/2);
  double sqI2 = 0;
  double sumI2 = 0;
  int sampleI2 = analogRead(CT2R);
  int gndsampleI2 = analogRead(CT2W);
  
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {

    sampleI2 = analogRead(CT2R);
    gndsampleI2 = analogRead(CT2W);
   
    filteredI2 = sampleI2 - gndsampleI2 - ((sampleI2-offsetI2)/1024);
    offsetI2 = (offsetI2 + (sampleI2 - offsetI2)/1024);

    sqI2 = filteredI2 * filteredI2;
    sumI2 += sqI2;

    delay(1);
    
  }
  double I_RATIO2 = ICAL *((SupplyVoltage/1000.0) / (ADC_COUNTS));
  double Irms2 = I_RATIO2 * sqrt(sumI2 / Number_of_Samples) - 1.5;

  if (Irms2 < 0){
    Irms2 = 0;
  }

  CURRES[0] = 4 * Irms2;
  
}
