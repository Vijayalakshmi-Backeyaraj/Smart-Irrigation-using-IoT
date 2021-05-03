#include <ESP8266WiFi.h>
String apiKey = "Write_API_Key";     //  Enter your Write API key here
const char* server = "api.thingspeak.com";    // URL of ThingSpeak server
const char *ssid =  "WiFi_Name";     // Enter your WiFi Name
const char *pass =  "WiFi_Password"; // Enter your WiFi Password


WiFiClient client;

const int moisturePin = A0;             // moisteure sensor pin
const int motorPin = 16;            // 16 is the alias of D0 pin in ESP
unsigned long interval = 10000;
unsigned long previousMillis = 0;
unsigned long interval1 = 1000;
unsigned long previousMillis1 = 0;
float moisturePercentage;              //variable to store moisture reading


void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(motorPin, OUTPUT);   // set pinMode of motorPin to output
  digitalWrite(motorPin, LOW); // keep the motor off initally
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) // Check the WiFi connection
  {
    delay(500);
    Serial.print(".");              // Wait until WiFi gets connected
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  unsigned long currentMillis = millis(); // Grab the current time since when the board began running the program.

  moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) / 1023.00) * 100.00 ) ); //Calculating moisture in terms of percentage // For zero moisture, we get a maximum value of 1023. 
                                                                                                                                    // For Maximum moisture, we get a minimum value of 0 that corresponds to 100% moisture.

  if ((unsigned long)(currentMillis - previousMillis1) >= interval1) {   // print mositure percentage to the serial monitor at an interval of 1000ms
    Serial.print("Soil Moisture is  = ");
    Serial.print(moisturePercentage);
    Serial.println("%");
    previousMillis1 = millis();
  }

if (moisturePercentage < 50) {
  digitalWrite(motorPin, HIGH);         // Turn ON motor
}
if (moisturePercentage > 50 && moisturePercentage < 55) {
  digitalWrite(motorPin, HIGH);        // Turn ON motor 
}
if (moisturePercentage > 56) {
  digitalWrite(motorPin, LOW);          // Turn OFF motor
}

if ((unsigned long)(currentMillis - previousMillis) >= interval) {

  sendThingspeak();           //Function Call to Send data to thingspeak server at an interval of 10secs
  previousMillis = millis();
  client.stop();
}

}

void sendThingspeak() {
  if (client.connect(server, 80))  //(client.connect(URL of server, port))
  {
    String postStr = apiKey;              // Add API key in the postStr string
    postStr += "&field1=";
    postStr += String(moisturePercentage);    // Add mositure reading
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());           //Send the length of the string
    client.print("\n\n");
    client.print(postStr);                      //Send the complete string
    Serial.print("Moisture Percentage: ");
    Serial.print(moisturePercentage);
    Serial.println("%. Sent to Thingspeak.");
  }
}
