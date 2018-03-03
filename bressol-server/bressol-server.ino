

#include <Servo.h>
#include <Adafruit_CC3000.h>
#include <ArduinoJson.h>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "hackathongi"   // cannot be longer than 32 characters!
#define WLAN_PASS       "tarla2018"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80
#define MAX_ACTION            10
#define MAX_PATH              64
#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20
#define TIMEOUT_MS            500    

Adafruit_CC3000_Server httpServer(LISTEN_PORT);
uint8_t buffer[BUFFER_SIZE+1];
int bufindex = 0;
char action[MAX_ACTION+1];
char path[MAX_PATH+1];

Servo myServo;  // create a servo object

int availableMemory() {
  int size = 1024; // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}

void moveCrib()
{
  myServo.attach(9); // attaches the servo on pin 9 to the servo object
  for (int i = 0; i < 5; i++)
  {
    myServo.write(0);
    Serial.println(F("angle: 0"));
    delay(3000);
    myServo.write(180);
    Serial.println(F("angle: 180"));
    delay(3000);
  }
  myServo.detach();
}

void prepareServer()
{
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }

  uint32_t ipAddress = cc3000.IP2U32(192, 168, 4, 28);
  uint32_t netMask = cc3000.IP2U32(255, 255, 0, 0);
  uint32_t defaultGateway = cc3000.IP2U32(192, 168, 4, 254);
  uint32_t dns = cc3000.IP2U32(8, 8, 4, 4);
  if (!cc3000.setStaticIPAddress(ipAddress, netMask, defaultGateway, dns)) {
    Serial.println(F("Failed to set static IP!"));
    while(1);
  }
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
   
/*
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  
*/

  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }
  
  // Start listening for connections
  httpServer.begin();
  Serial.println(F("Connected! Listening for connections..."));
}

void setup(void)
{
  Serial.begin(115200);
  prepareServer();
}

void loop(void)
{
  // Try to get a client which is connected.
  Adafruit_CC3000_ClientRef client = httpServer.available();
  if (client) {
    Serial.println(F("Client connected."));

    // Clear the incoming data buffer and point to the beginning of it.
    bufindex = 0;
    memset(&buffer, 0, sizeof(buffer));
    
    // Clear action and path strings.
    memset(&action, 0, sizeof(action));
    memset(&path,   0, sizeof(path));

    // Set a timeout for reading all the incoming data.
    unsigned long endtime = millis() + TIMEOUT_MS;
    
    // Read all the incoming data until it can be parsed or the timeout expires.
    bool parsed = false;
    while (!parsed && (millis() < endtime) && (bufindex < BUFFER_SIZE)) {
      if (client.available()) {
        buffer[bufindex++] = client.read();
      }
      parsed = parseRequest(buffer, bufindex, action, path);
    }

    // Handle the request if it was parsed.
    if (parsed) {

      // Skip HTTP headers
      char endOfHeaders[] = "\r\n\r\n";
      if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        return;
      }

      // Allocate JsonBuffer
      // Use arduinojson.org/assistant to compute the capacity.
      
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 128;
      
      DynamicJsonBuffer jsonBuffer(capacity);
      
      // Parse JSON object
      JsonObject& root = jsonBuffer.parseObject(client);
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }
    
      // Extract values
      Serial.println(availableMemory());
      const char* value = root["data"][0]["estat"]["value"];
      if (strcmp(value, "on") == 0)
      {
        Serial.println(F("Moving crib"));
        moveCrib();
        Serial.println(F("End moving crib"));
      }
      else
        Serial.println(F("Not moving"));
        
      //Serial.println(root["data"][0]["estat"]["value"].as<char*>());
      
      //Serial.println();
      //Serial.println("closing connection");      
      
      if (strcmp(action, "POST") == 0) {
        Serial.println(F("Entra a POST"));
        client.fastrprintln(F("HTTP/1.1 200 OK"));
        Serial.println(F("Entra a POST 1"));
        client.fastrprintln(F("Content-Type: text/plain"));
        Serial.println(F("Entra a POST 2"));
        client.fastrprintln(F("Connection: close"));
        client.fastrprintln(F("Server: Adafruit CC3000"));
        client.fastrprintln(F(""));
        Serial.println(F("DINS a POST"));

        // Now send the response data.
        client.fastrprintln(F("Hello world!"));
        client.fastrprint(F("You accessed path: ")); client.fastrprintln(path);
      }
      else {
                Serial.println(F("Entra a ELSE POST"));

        // Unsupported action, respond with an HTTP 405 method not allowed error.
        client.fastrprintln(F("HTTP/1.1 405 Method Not Allowed"));
        client.fastrprintln(F(""));
      }

              Serial.println(F("FINAL"));

    }

    // Wait a short period to make sure the response had time to send before
    // the connection is closed (the CC3000 sends data asyncronously).
    delay(100);

    // Close the connection when done.
    Serial.println(F("Client disconnected"));
    client.close();
  }
}

// Return true if the buffer contains an HTTP request.  Also returns the request
// path and action strings if the request was parsed.  This does not attempt to
// parse any HTTP headers because there really isn't enough memory to process
// them all.
// HTTP request looks like:
//  [method] [path] [version] \r\n
//  Header_key_1: Header_value_1 \r\n
//  ...
//  Header_key_n: Header_value_n \r\n
//  \r\n
bool parseRequest(uint8_t* buf, int bufSize, char* action, char* path) {

  // Serial.println(F(buf));

  // Check if the request ends with \r\n to signal end of first line.
  if (bufSize < 2)
    return false;
  if (buf[bufSize-2] == '\r' && buf[bufSize-1] == '\n') {
    parseFirstLine((char*)buf, action, path);
    return true;
  }
  return false;

}

// Parse the action and path from the first line of an HTTP request.
void parseFirstLine(char* line, char* action, char* path) {
  // Parse first word up to whitespace as action.
  char* lineaction = strtok(line, " ");
  if (lineaction != NULL)
    strncpy(action, lineaction, MAX_ACTION);
  // Parse second word up to whitespace as path.
  char* linepath = strtok(NULL, " ");
  if (linepath != NULL)
    strncpy(path, linepath, MAX_PATH);
}

// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.println();
    return true;
  }
}
