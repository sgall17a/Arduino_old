
/*

 */


#include <ESP8266WiFi.h>

const char* ssid = "ii33FD43primary";
const char* pass = "5df5be4e";
const char* host = "api.thingspeak.com"; 
WiFiClient client;



void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  yield();
  WiFi.begin(ssid, pass  );
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  yield();
  Serial.println(WiFi.localIP());
}
int value = 0;
void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  yield();
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return; 
    }
  yield();
  
  String url = "/talkbacks/2868/commands?api_key=GXB2F76D1YTB0XYG";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  String line;
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    line = client.readStringUntil('\r');
    yield();
    Serial.print(line);
    line= "";
  }
  Serial.print (line);
  Serial.println();
  yield();
  Serial.println("closing connection");
  client.stop();
}





