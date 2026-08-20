



#include <WiFi.h> //Provides the functionality to set the Esp32 up as a wifi Access Point
#include <WebSocketsServer.h> //Enables persistent real time communication via a WebSocket Connection
#include <ArduinoJson.h> //Handles the encoding and decoding of JSON data to and from the websocket connection

#define DEVICE_NAME "Robo_Crawler_v1.1"




//PINS
//SERVO Pins
#define MOTOR1_IN1 32  //Servo 1 IN1
#define MOTOR1_IN2 33  //Servo 1 IN2
#define MOTOR2_IN1 26  //Servo 2 IN1
#define MOTOR2_IN2 27  //Servo 2 IN2

#define DRIVE_EN 23
#define DRIVE_CH1 18
#define DRIVE_CH2 19

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

//WebSocket event funciton definition
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length);



void sendPulse(int pulseWidthMicros);

void setup() 
{

  // the string in the input  will be duplicated in the JsonDocument.
  //  char temp_data[300]; 
  String stringone = "{\"Name\":\"";
  String stringtwo = "\", \"Type\":\"Test\", \"Check\":\"SunFounder Controller\"}";
  temp_send = stringone + DEVICE_NAME + stringtwo;

  Serial.begin(115200);

  //Start WiFi broadcast
  Serial.println("Starting WiFi in AP mode...");  
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress ip = WiFi.softAPIP();
  server.begin();
  Serial.print("AP IP address: ");  
  Serial.println(ip);

  //Start WebSocket server
  Serial.println("Starting websocket");
  webSocket.begin(); 

  //Assign callback Function
  Serial.println("Starting websocket");
  webSocket.onEvent(onWebSocketEvent);


  analogWriteFrequency(15000);

  //set drive motors to output
  Serial.println("Initializing motor pairs");

  pinMode(DRIVE_CH1, OUTPUT);
  pinMode(DRIVE_CH1, OUTPUT);
  pinMode(DRIVE_EN, OUTPUT);

  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);

  digitalWrite(DRIVE_EN, LOW);

  Serial.println("Initialization Complete");
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
  Serial.println("Websocket event triggered");
  String output;
  String temp;
  // Figure out the type of WebSocket event
  switch(type) {

    // Client has disconnected
    case WStype_DISCONNECTED:

      digitalWrite(DRIVE_EN, LOW);
      digitalWrite(MOTOR1_IN2, LOW);
      digitalWrite(MOTOR2_IN1, LOW);

      // Servos
      digitalWrite(MOTOR1_IN1, LOW);
      digitalWrite(MOTOR1_IN2, LOW);
      digitalWrite(MOTOR2_IN1, LOW);
      digitalWrite(MOTOR2_IN2, LOW);

      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        Serial.println("Client Connected");
        Serial.println(temp_send);
        webSocket.sendTXT(client_num, temp_send);
      }
      break;

    case WStype_TEXT:
    {
      deserializeJson(doc_recv, payload);

      //-------------------Directional Control-------------------//

      if (doc_recv["Q"]) {

        digitalWrite(MOTOR1_IN1, HIGH);
        digitalWrite(MOTOR1_IN2, LOW);

        digitalWrite(MOTOR2_IN1, LOW);
        digitalWrite(MOTOR2_IN2, HIGH);

      }else if (doc_recv["R"]) {

        digitalWrite(MOTOR1_IN1, LOW);
        digitalWrite(MOTOR1_IN2, HIGH);

        digitalWrite(MOTOR2_IN1, HIGH);
        digitalWrite(MOTOR2_IN2, LOW);

      }else {

        digitalWrite(MOTOR1_IN1, doc_recv["I"]);
        digitalWrite(MOTOR1_IN2, doc_recv["J"]);

        digitalWrite(MOTOR2_IN1, doc_recv["S"]);
        digitalWrite(MOTOR2_IN2, doc_recv["T"]);

      }

      // Enable driver
      digitalWrite(DRIVE_EN, HIGH);

      //Drive limiter
      float driveLim = 0.01 * map(doc_recv["A"].as<int>(),0,100,50,100);

      //Drive Control
      float driveInput = doc_recv["K"].as<int>();

      //the function here is specially tuned to provide the desired output
      //base function: 50 + 100/pi * arctan(x)
      //This function is tuned so that f(100) and f(-100) equal 100 and 0 respectfully
      //float driveSpeed = 50 + 35.164*atan(driveInput/15) * driveLim;

      //Linear curve
      //float driveSpeed = 2.55 * driveInput * driveLim;

      // Old function 179.34*atan(driveInput/15)

      // Other old function: 50 * tan(0.01377 * driveInput) * driveLim;

      float driveSpeed = 2.55 * driveInput * driveLim;

      float driveMag = abs(driveSpeed);
      bool direction = (driveSpeed >= 0); //1 is forward, 0 is backwards
      
      analogWrite(DRIVE_CH1, driveMag * direction);
      analogWrite(DRIVE_CH2, driveMag * !direction);

      Serial.print("Current driveSpeed: ");
      Serial.println(driveSpeed);

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




