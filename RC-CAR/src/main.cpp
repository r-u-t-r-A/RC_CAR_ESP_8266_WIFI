#include <Arduino.h>

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>

Servo DriveServo1;
Servo DriveServo2;
Servo SteerServo;

bool LState = 0;
bool RState = 0;
bool BState = 0;
bool FState = 0;
const int DrivePin1 = 13;
const int DrivePin2 = 14; //D7 on LoLin(NodeMCU v3)
const int SteerPin = 12; //D6 on LoLin(NodeMCU v3)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char *ssid = "CAR";
const char *password = "123456";
IPAddress apIP(42, 42, 42, 42);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
    max-width: 500px;
  }
  
  .buttonR {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.buttonR:hover {background-color: #0f8b8d}*/
   .buttonR:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   
   .buttonL {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    margin-top: 50px;
   }
   /*.buttonL:hover {background-color: #0f8b8d}*/
   .buttonL:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   .buttonB {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    margin:10px;
   }
   /*.buttonB:hover {background-color: #0f8b8d}*/
   .buttonB:active {
    background-color: #0a6163;
    box-shadow: 2 2px #CDCDCD;
    transform: translateX(-2px);
   }
   
   .buttonF {
    padding: 50px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    margin:10px;
   }
   .buttonF:active {
     background-color: #0a6163;
     box-shadow: 2 2px #CDCDCD;
     transform: translateX(-2px);
   }
   .driveB button {
  cursor: pointer; 
  float: left; 
}
   .driveB:after {
   content: "";
   clear: both;
   display: table;
}

  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
<div class="driveB"><p><button id="buttonB" class="buttonB">B</button></p><p><button id="buttonF" class="buttonF">F</button></p></div>  
<p><button id="buttonL" class="buttonL">L</button></p>
<p><button id="buttonR" class="buttonR">R</button></p>  
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onLoad(event) {
    initWebSocket();
    initButtonF();
    initButtonB();
    initButtonL();
    initButtonR();
  }
  function initButtonF() {
    document.getElementById('buttonF').addEventListener('mousedown', FButton);
    document.getElementById('buttonF').addEventListener('mouseup', FButton);
    document.getElementById('buttonF').addEventListener('touchstart', FButton);
    document.getElementById('buttonF').addEventListener('touchend', FButton);
  }
   function initButtonB() {
    document.getElementById('buttonB').addEventListener('mousedown', BButton);
    document.getElementById('buttonB').addEventListener('mouseup', BButton);
    document.getElementById('buttonB').addEventListener('touchstart', BButton);
    document.getElementById('buttonB').addEventListener('touchend', BButton);
  }
  function initButtonL() {
    document.getElementById('buttonL').addEventListener('mousedown', LButton);
    document.getElementById('buttonL').addEventListener('mouseup', LButton);
    document.getElementById('buttonL').addEventListener('touchstart', LButton);
    document.getElementById('buttonL').addEventListener('touchend', LButton);
  }
   function initButtonR() {
    document.getElementById('buttonR').addEventListener('mousedown', RButton);
    document.getElementById('buttonR').addEventListener('mouseup', RButton);
    document.getElementById('buttonR').addEventListener('touchstart', RButton);
    document.getElementById('buttonR').addEventListener('touchend', RButton);
  }
  function RButton(){
    websocket.send('RButton');
  }
  function LButton(){
    websocket.send('LButton');
  }
   function FButton(){
    websocket.send('FButton');
  }
  function BButton(){
    websocket.send('BButton');
  }
</script>
</body>
</html>
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "FButton") == 0) {
      FState = !FState;
    }
    if (strcmp((char*)data, "BButton") == 0) {
      BState = !BState;
    }
     if (strcmp((char*)data, "LButton") == 0) {
      LState = !LState;
    }
    if (strcmp((char*)data, "RButton") == 0) {
      RState = !RState;
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  DriveServo1.attach(DrivePin1);
  DriveServo2.attach(DrivePin2);
  SteerServo.attach(SteerPin);
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  if (LState == 0 && RState == 0){
    SteerServo.write(82);
  } else if (LState != 0) {
    SteerServo.write(112);
  } else if (RState != 0) {
    SteerServo.write(52);
  }

  if (FState == 0 && BState == 0) {
    DriveServo1.write(92);
    DriveServo2.write(91);
  } else if (FState != 0) {
    DriveServo1.write(0);
    DriveServo2.write(180);
  } else if (BState != 0) {
    DriveServo1.write(180);
    DriveServo2.write(0); 
  }
  
}