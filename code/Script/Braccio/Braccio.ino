#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <Braccio.h>
#define TRESHOLD 30
Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;
int m1 = 90;int m2 = 90; int m3 = 90; int m4 = 90;
int m5 = 90;
int m6 = 0;

float positions[6]={m1,m2,m3,m4,m5,m6};
// Inserisci le informazioni della tua rete
char ssid[] = "WiFi-LabIoT";  // il tuo network SSID (nome)
char pass[] = "s1jzsjkw5b";   // la tua password di rete

// Indirizzo IP e porta del server MQTT
const char* mqtt_server = "192.168.1.21";

WiFiClient espClient;
PubSubClient client(espClient);


const int commandDelay = 500; // Ritardo tra i comandi in millisecondi
unsigned long lastCommandTime = 0;
bool joystickInMotion = false;
int lastJoystickX = 512; // Posizione neutra del joystick X
int lastJoystickY = 512; // Posizione neutra del joystick Y

void setup() {
  // Inizializza la comunicazione seriale
  Serial.begin(9600);

  // Connetti alla rete WiFi
  setup_wifi();

  // Imposta il server MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Inizializza Braccio
  Braccio.begin();
  Braccio.ServoMovement(15, m1, m2, m3, m4, m5, m6);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup_wifi() {
  delay(10);
  // Connetti alla rete WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Converti il payload in una stringa
  String data;
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }

  // Separa i valori del joystick e dei pulsanti
  int separatorIndex1 = data.indexOf(',');
  int separatorIndex2 = data.indexOf(',', separatorIndex1 + 1);
  int separatorIndex3 = data.indexOf(',', separatorIndex2 + 1);
  int separatorIndex4 = data.indexOf(',', separatorIndex3 + 1);
  int separatorIndex5 = data.indexOf(',', separatorIndex4 + 1);
  String strJoystickValueX = data.substring(0, separatorIndex1);
  String strJoystickValueY = data.substring(separatorIndex1 + 1, separatorIndex2);
  String strButtonStateA = data.substring(separatorIndex2 + 1, separatorIndex3);
  String strButtonStateB = data.substring(separatorIndex3 + 1, separatorIndex4);
  String strButtonStateC = data.substring(separatorIndex4 + 1, separatorIndex5);

  // Converte le stringhe in interi
  int joystickValueX = strJoystickValueX.toInt();
  int joystickValueY = strJoystickValueY.toInt();
  int buttonStateA = strButtonStateA.toInt();
  int buttonStateB = strButtonStateB.toInt();
  int buttonStateC = strButtonStateC.toInt();

 /* int deltaX = map(joystickValueX, 0, 707, - (707 / 2), (707 / 2));
  if(deltaX >= -10 && deltaX <= 10) {
    deltaX = 0;
  }
  Serial.println(deltaX);*/



  if (buttonStateC == 0) {

moveServo(4,joystickValueX);
      //m5 = map(joystickValueX, 0, 1023, 0, 180);
    } else {
moveServo(0,joystickValueX);
     // m1 = map(joystickValueX, 0, 1023, 0, 180);
    }

    if (buttonStateB == 0) {
      moveServo(2,joystickValueY);
     // m3 = map(joystickValueY, 0, 1023, 0, 180);
    } else {
      moveServo(1,joystickValueY);
      //m2 = map(joystickValueY, 0, 1023, 0, 180);
    }

    // Muove destra/sinistra
    if (buttonStateA == 0) {
      m6 = 73;
    } else {
      m6 = 10;
    }

    // Aggiorna i servomotori solo dopo aver processato il comando
    /*Braccio.ServoMovement(15, m1, m2, m3, m4, m5, m6);
    delay(30);*/
}

void reconnect() {
  // Loop fino a quando non siamo connessi
  while (!client.connected()) {
    Serial.println(WiFi.localIP());
    Serial.print("Server non connesso...");
    // Prova a connetterti
    if (client.connect("ClientBraccio")) {
      Serial.println("connected");
      // Una volta connesso, sottoscrivi al topic
      client.subscribe("joystickCommands",0);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Aspetta 5 secondi prima di riprovare
      delay(5000);
    }
  }  
}
void moveServo(int index,int value)
{
  if(abs(value-510)>TRESHOLD)
  {
     float delta=(float)(value-512)/100;
   
    
          positions[index]+=delta; 
     positions[index]=constrain(positions[index],0,180);
      }
      int t=0;
      t=positions[index];
      positions[index]=map(positions[index],0,179,0,180);
 Braccio.ServoMovement(1, positions[0], positions[1], positions[2], positions[3], positions[4], m6);
 positions[index]=t;

     
 
}
