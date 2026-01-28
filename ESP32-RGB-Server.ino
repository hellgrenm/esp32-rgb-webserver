#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "SSID";
const char* password = "PASSWORD";

#define PIN_RED    23 // GPIO23
#define PIN_GREEN  22 // GPIO22
#define PIN_BLUE   21 // GPIO21


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP RGB</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <div style="display: flex; justify-content: center; flex-wrap: wrap;">
  <h1 style="width: 100%; text-align: center;">ESP32 Web Server</h1>
  <label for="redColor" id="redValue">Red: 0</label> <input type="range" autocomplete="off" min="0" max="255" value="0" name="redColor" id="redColor" onchange="updateColor(this.id, this.value)"> 
  <label for="greenColor" id="greenValue">Green: 0</label> <input type="range" autocomplete="off" min="0" max="255" value="0" name="greenColor" id="greenColor" onchange="updateColor(this.id, this.value)"> 
  <label for="blueColor" id="blueValue">Blue: 0</label> <input type="range" autocomplete="off" min="0" max="255" value="0" name="blueColor" id="blueColor" onchange="updateColor(this.id, this.value)"> 
  <span style="width: 100%; height: 0;"></span>
  <div id="example" style="padding: 50px; margin-top: 10px; width: 25%; border: 1px solid black; background-color: black;"></div>
  </div>

  <script> 

  function updateColor(id, value){
    if (id == "redColor"){
      document.getElementById("redValue").textContent = "Red: " +  value;
    } else if (id == "greenColor"){
      document.getElementById("greenValue").textContent = "Green: " + value;
    } else if ( id == "blueColor"){
      document.getElementById("blueValue").textContent = "Blue: " + value;
    }
    updateBackground();

    fetch('/update?color=' + id + '&val=' + value)
    .then(response => console.log("Skickat " + id + ": " + value))
    .catch(err => console.error("Fel:", err));
  }

  
  function updateBackground(){
    let red = document.getElementById("redColor").value;
    let green = document.getElementById("greenColor").value;
    let blue = document.getElementById("blueColor").value;
    document.getElementById("example").style.backgroundColor = `rgb(${red}, ${green}, ${blue})`;
  }

  </script>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  // Send web page to client and set all lights to 0
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    analogWrite(PIN_RED,   0);
    analogWrite(PIN_GREEN, 0);
    analogWrite(PIN_BLUE,  0);
    request->send_P(200, "text/html", index_html);
  });


  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
    String chosenColor;
    String colorValue;
      if (request->hasParam("color") && request->hasParam("val")) {
        chosenColor = request->getParam("color")->value();
        colorValue = request->getParam("val")->value();
        if (chosenColor == "redColor"){
           analogWrite(PIN_RED, colorValue.toInt() );
        } else if (chosenColor == "greenColor"){
            analogWrite(PIN_GREEN, colorValue.toInt() );
        } else if (chosenColor == "blueColor"){
            analogWrite(PIN_BLUE, colorValue.toInt() );
        }
          
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Error: no params");
      }
  });

  
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) { //send the html
    request->send_P(200, "text/html", index_html);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  
}
