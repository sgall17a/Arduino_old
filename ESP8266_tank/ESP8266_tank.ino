/* 
  This a simple example of the aREST Library for the ESP8266 WiFi chip. 
  See the README file for more details.
 
  Written in 2015 by Marco Schwartz under a GPL license. 
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <aREST.h>

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "ii33FD43primary";
const char* password = "5df5be4e";
IPAddress ip(10, 1, 1, 40);
IPAddress gateway(10,1,1,1);
IPAddress subnet(255,255,255,0);

// The port to listen for incoming TCP connections 

#define LISTEN_PORT           8080

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
int temperature;
int humidity;

void setup(void)
{  
  // Start Serial
  Serial.begin(115200);
  
  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;
  rest.variable("temperature",&temperature);
  rest.variable("humidity",&humidity);

  // Function to be exposed
  rest.function("led",ledControl);
  
  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("esp8266");
  
  // Connect to WiFi

  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
  
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
 
}

// Custom function accessible by the API
int ledControl(String command) {
  
  // Get state from command
  int state = command.toInt();
  
  digitalWrite(6,state);
  return 1;
}
