
#include <WiFiS3.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <WiFiServer.h>

#include "RelaySwitch.h"

#define DEVICE_NAME "Robo_Crawler_v1.1"

#define RELAY_ON 0
#define RELAY_OFF 1

//PINS

//Arduino Pin Definitions
#define RELAY_1 3
#define RELAY_2 4
#define RELAY_3 5
#define RELAY_4 6
#define PWMDRIVE 10

#define SPEED_PIN_LEFT    26
#define SPEED_PIN_RIGHT    25
  
// WiFi credentials for AP mode
const char* ap_ssid = DEVICE_NAME;
const char* ap_password = "password";

//Web Socket and Server
WebSocketsServer webSocket = WebSocketsServer(8765);
WiFiServer server(80);

//JSON
JsonDocument doc_send;
JsonDocument doc_recv;
String temp_send;

RelaySwitch relay;



void setup() 
{

  // the string in the input  will be duplicated in the JsonDocument.
  //  char temp_data[300]; 
  String stringone = "{\"Name\":\"";
  String stringtwo = "\", \"Type\":\"Test\", \"Check\":\"SunFounder Controller\"}";
  temp_send = stringone + DEVICE_NAME + stringtwo;

  //relay_init();//initialize the relay

  Serial.begin(115200);
  Serial.println("Starting WiFi in AP mode...");  
  WiFi.beginAP(ap_ssid, ap_password);
  server.begin();
  Serial.print("AP IP address: ");  
  Serial.println(WiFi.softAPIP());
  //Start WebSocket server and assign callback
  webSocket.begin(); 
  webSocket.onEvent(onWebSocketEvent);

  //Relay setup
  relay.init(RELAY_1,RELAY_2,RELAY_3,RELAY_4);
  relay.print();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    // Wait until client sends a request
    while (!client.available()) {
      delay(1);
    }
    String request = client.readStringUntil('\r');
    client.flush();

    //Sends a basic HTTP response to the client
    client.println("HTTP/1.1 200 OK");
    //client.println("Content-Type: application/json");
    //client.println("Connection: close");
    //client.println();
    //client.println("{\"NAME\":\"Test\", \"Type\":\"Test\", \"Check\":\"SunFounder Controller\"}");
    //delay(1);
    client.stop();
  }
  
  webSocket.loop();
}


//------------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------//

//---------------------------------------WebSocket---------------------------------------//

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
  String output;
  String temp;
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      analogWrite(PWMDRIVE, 0);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        //IPAddress ip = webSocket.remoteIP(client_num);
        //Serial.println("[%u] Connection from ");
        //Serial.println(ip.toString());
        Serial.println("Client Connected");
        Serial.println(temp_send);
        webSocket.sendTXT(client_num, temp_send);
      }
      break;

    case WStype_TEXT:
    {
      deserializeJson(doc_recv, payload);

      //-------------------Robot Control-------------------//

      //Directional control for seperated drive
      relay.setRelay(!doc_recv["S"], !doc_recv["T"], !doc_recv["I"], !doc_recv["J"]);

      //Directional control for joined drive
      if (doc_recv["Q"]) {
        relay.setRelay(1, RELAY_ON);
        relay.setRelay(3, RELAY_ON);
      }
      
      if (doc_recv["R"]) {
        relay.setRelay(2, RELAY_ON);
        relay.setRelay(4, RELAY_ON);
      }

      relay.update();

      //Drive limiter
      float driveLim = 0.01 * map(doc_recv["A"].as<int>(),0,100,50,100);

      //Drive Control
      float driveInput = doc_recv["K"].as<int>();

      //the function here is specially tuned to provide the desired output
      //base function: 191 + 128/pi * arctan(x)
      float driveSpeed = 191 + 45*atan(driveInput/15) * driveLim;

      //lagacy linear output. functioned poorly around zero
      //int driveSpeed = map(doc_recv["K"].as<int>(),-100,100,0,255);
      Serial.println(driveSpeed);
      analogWrite(PWMDRIVE, driveSpeed);


      //Sends information back to the controller
      serializeJson(doc_recv, temp);
      //Serial.println(temp);
      serializeJson(doc_send, output);
      //Serial.println(output);
      webSocket.sendTXT(client_num, output);     
      break;
    }

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
    
  }
}

//---------------------------------------RELAY---------------------------------------//

//initialize the relay


//---------------------------------------PWM Drive---------------------------------------//
// Manually generate RC PWM signal
void sendPulse(int pulseWidthMicros) {
    digitalWrite(PWMDRIVE, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(PWMDRIVE, LOW);
    delayMicroseconds(20000 - pulseWidthMicros);
}






