#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <WiFi.h>

// Inserisci le informazioni della tua rete
// Inserisci le informazioni della tua rete
char ssid[] = "WiFi-LabIoT";  // il tuo network SSID (nome)
char pass[] = "s1jzsjkw5b";   // la tua password di rete

// Indirizzo IP e porta del server MQTT
const char* mqtt_server = "192.168.1.21";

// Definisci i pin per il joystick e i pulsanti
const int joystickPinX = A0;
const int joystickPinY = A1;
const int buttonPinA = 2;
const int buttonPinB = 3;
const int buttonPinC = 4;
const int buttonPinD = 5;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Inizializza la comunicazione seriale
  Serial.begin(9600);

  // Connetti alla rete WiFi
  setup_wifi();

  // Imposta il server MQTT
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leggi i valori del joystick
  int joystickValueX = analogRead(joystickPinX);
  int joystickValueY = analogRead(joystickPinY);

  // Leggi i valori dei pulsanti
  int buttonStateA = digitalRead(buttonPinA);
  int buttonStateB = digitalRead(buttonPinB);
  int buttonStateC = digitalRead(buttonPinC);
  int buttonStateD = digitalRead(buttonPinD);

  // Crea il messaggio da inviare
  String message = String(joystickValueX) + "," + String(joystickValueY) + "," + String(buttonStateA) + "," + String(buttonStateB) + "," + String(buttonStateC) + "," + String(buttonStateD);
   Serial.println("MESSAGGIO BOTTONI PREMUTI:" + message);
  // Invia il messaggio al topic "joystickCommands"
  client.publish("joystickCommands", message.c_str());

  delay(16); // Ritardo per rendere più leggibili i dati sulla seriale
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

void reconnect() {
  // Loop fino a quando non siamo connessi
  while (!client.connected()) {
    Serial.println(WiFi.localIP());
   
    Serial.print("Server non connesso...");
    // Prova a connetterti
    if (client.connect("ClientJoystick")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Aspetta 5 secondi prima di riprovare
      delay(5000);
    }
  }
}
