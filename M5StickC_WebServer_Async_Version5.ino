
// Import required libraries
#include <Arduino.h>
#include <M5StickC.h>
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h> 
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1016.00)

Adafruit_BME280 bme;

// Replace with your network credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";


// current temperature, humidity, pressure and altitude updated in loop()
float t = 0.0;
float h = 0.0;
float p = 0.0;
float a = 0.0;

// Create AsyncWebServer object on port ......
AsyncWebServer server(48848);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time BME was updated

// Updates BME280 readings every 5 seconds
const long interval = 4000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: 'Open Sans', sans-serif;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    body { background-image: url("\background_image.jpg");}
    h1 { font-family: Helvetica; font-size: 4.0rem; }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; padding: 4px}
    .units { font-size: 1.2rem; }
    .bme-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
    
    .leftdiv 
     { 
         float: left; 
         } 
    .middlediv1 
     { 
         float: left; 
 
         } 
    .middlediv2
     { 
         float: left; 
  
         } 
    .rightdiv 
     { 
         float: left; 
         } 
     div{ 
         padding : 2%; 
         color: white;
         background-color: 009900;
         width: 30%; 
         margin: 8px;
         border-color: #a2944e;
         border-style: solid; 
         } 

  </style>
</head>
<body>
    <h1><u>M5-STickC Weather Station</u></h1>
    <div class="leftdiv">
      <h2 style="color: #f29c1f">&nbsp; Temperature &nbsp;</h2>
      <p>
        <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
        <span id="temperature">%TEMPERATURE%</span>
        <sup class="units">&deg;C</sup>
      </p>
    </div>
    <div class="middlediv1">   
      <h2 style="color:#3b97d3">&nbsp;&nbsp; Humidity &nbsp;&nbsp;</h2>
      <p>
         <i class="fas fa-tint" style="color:#00add6;"></i> 
         <span id="humidity">%HUMIDITY%</span>
         <sup class="units">Pct</sup>
      </p>
    </div>
    <div class="middlediv2">
      <h2 style="color:#26b99a">&nbsp;&nbsp; Pressure &nbsp;&nbsp;</h2>
       <p>
        <i class="fas fa-tachometer-alt" style="color:#ff0000;"></i>
        <span id="pressure">%PRESSURE%</span>
        <sup class="units">hPa</sup>
      </p>
    </div>
    <div class="rightdiv">
      <h2 style="color:#955ba5">&nbsp;&nbsp; Altitude &nbsp;&nbsp;</h2>
      <p>
        <i class="fas fa-mountain" style="color:#1b688f;"></i> 
        <span id="altitude">%ALTITUDE%</span>
        <sup class="units">m</sup>
      </p>
    </div>
</body>

<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 4000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 4000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("pressure").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/pressure", true);
  xhttp.send();
}, 4000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("altitude").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/altitude", true);
  xhttp.send();
}, 4000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with BME values
String processor(const String& var){
  Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "PRESSURE"){
    return String(p);
  }
  else if(var == "ALTITUDE"){
    return String(a);
  }
  else{}
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  M5.begin();
  Wire.begin(0, 26);
  

  while(!bme.begin()){
    M5.Lcd.println("Error");
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(p).c_str());
  });
  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(a).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the BME280 values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = bme.readTemperature();
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    
    // Read Humidity
    float newH = bme.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from BME280 sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }

    // Read Pressure
    float newP = bme.readPressure() / 100.0F;
    // if pressure read failed, don't change p value 
    if (isnan(newP)) {
      Serial.println("Failed to read from BME280 sensor!");
    }
    else {
      p = newP;
      Serial.println(p);
    }

    // Read Altitude
    float newA = bme.readAltitude(SEALEVELPRESSURE_HPA);
    // if altitude read failed, don't change a value 
    if (isnan(newA)) {
      Serial.println("Failed to read from BME280 sensor!");
    }
    else {
      a = newA;
      Serial.println(a);
    }
  }
}
