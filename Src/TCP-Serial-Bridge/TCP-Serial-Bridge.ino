#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SoftwareSerial.h>

// ***
// *** The name of the service offered. SoE => Serial Over Ethernet
// ***
#define MDNS_SERVICE "SoE"

// ***
// *** Wi-Fi Manager is used to get Wi-Fi credentials setup using another
// *** computer or mobile phone. Local initialization. Once its business
// *** is done, there is no need to keep it around
// ***
WiFiManager _wifiManager;

// ***
// *** TCP Port for Bridge Server
// ***
int _port = 3001;

// ***
// *** Baud rate for Serial port
// ***
int _baudRate = 9600;

// ***
// ***
// ***
WiFiServer* _server = NULL;

// ***
// *** Setup a software based serial port.
// ***
#define S_RX D1
#define S_TX D2
SoftwareSerial _softwareSerial(S_RX, S_TX);

// ***
// *** Define an alias for the serial port. This makes
// *** the code esier to follow by showing where in code we are
// *** writing to the RS232 device versus statements meant to
// *** go to the debugging device.
// ***
#define COMPORT Serial

// ***
// *** Define an alias for the DEBUG output. This allows
// *** the devie to be changed with minimal or no code
// *** changes.
// ***
#define DEBUG _softwareSerial

void setup()
{
  // ***
  // *** Start the debugging device.
  // ***
  DEBUG.begin(9600);

  while (!DEBUG)
  {
    // ***
    // *** Wait for the device to connect.
    // ***
  }

  // ***
  // *** Let the system stabilize. This prevents (or minimizes)
  // *** garbage output on the debugging device.
  // ***
  delay(2000);

  // ***
  // *** Create a unique device name.
  // ***
  String deviceName = MDNS_SERVICE + String("-") + String(ESP.getFlashChipId(), HEX);

  // ***
  // *** Set some Wi-Fi Manager properties.
  // ***
  _wifiManager.setConfigPortalTimeout(180);
  _wifiManager.setMinimumSignalQuality(50);
  _wifiManager.setSaveParamsCallback(saveParamCallback);

  // ***
  // *** Add custom properties.
  // ***
  WiFiManagerParameter custom_port("port", "TCP Listening Port", String(_port).c_str(), 5);
  _wifiManager.addParameter(&custom_port);
  WiFiManagerParameter custom_baud_rate("baud_rate", "Serial Port Baud Rate", String(_baudRate).c_str(), 6);
  _wifiManager.addParameter(&custom_baud_rate);

  // ***
  // *** Initiate the configuration.
  // ***
  _wifiManager.autoConnect(deviceName.c_str());

  // ***
  // *** Register the device name on the network.
  // ***
  DEBUG.print("Device name is "); DEBUG.println(deviceName);

  if (MDNS.begin(deviceName, WiFi.localIP()))
  {
    DEBUG.println("Adding service to mDNS.");
    MDNS.addService(MDNS_SERVICE, "tcp", _port);
    DEBUG.println("mDNS service setup successfully.");
    QueryMdns();
  }
  else
  {
    DEBUG.println("mDNS service setup failed.");
  }

  // ***
  // *** Start the server.
  // ***
  _server = new WiFiServer(_port);
  _server->begin();
  DEBUG.print("TCP to Serial Port bridge is listening on IP address "); DEBUG.print(WiFi.localIP()); DEBUG.print(" and port "); DEBUG.println(_server->port());

  // ***
  // *** Start the serial port for the bridge.
  // ***
  COMPORT.begin(_baudRate);
  DEBUG.print("Serial port ready at "); DEBUG.print(_baudRate); DEBUG.println(" baud");

  // ***
  // *** The system is initialized and ready to go.
  // ***
  DEBUG.println("TCP to Serial Port Bridge is ready.");
}

String getParam(String name)
{
  // ***
  // *** Read parameter from server, for custom HTML input.
  // ***
  String value;

  if (_wifiManager.server->hasArg(name))
  {
    value = _wifiManager.server->arg(name);
  }

  return value;
}

void saveParamCallback()
{
  DEBUG.println("[CALLBACK] saveParamCallback fired");

  String portValue = getParam("port");
  DEBUG.println("PARAM Port = " + portValue);
  _port = portValue.toInt();

  String baudRateValue = getParam("baud_rate");
  DEBUG.println("PARAM Baud Rate = " + baudRateValue);
  _baudRate = baudRateValue.toInt();
}

void loop()
{
  // ***
  // *** Check for a client connection.
  // ***
  WiFiClient client = _server->available();

  // ***
  // *** If there is a client connection then start to read from the client. Only
  // *** one client connection is allowed at a time.
  // ***
  if (client)
  {
    DEBUG.println("Connection requested.");

    // ***
    // *** Display a message when a connection occurrs.
    // ***
    if (client.connected())
    {
      DEBUG.println("Client connected successfully.");

      // ***
      // *** While connected, send all incoming data to the serial
      // *** port an send any incoming serial port data back to
      // *** the client.
      // ***
      while (client.connected())
      {
        // ***
        // *** Read all of the data available and forward it
        // *** to the serial port.
        // ***
        while (client.available() > 0)
        {
          // ***
          // *** Read data from the connected client.
          // ***
          char c = client.read();

          // ***
          // *** Forward it to the serial port.
          // ***
          COMPORT.write(c);

          // ***
          // *** Display it on the debugging device.
          // ***
          DEBUG.write(c);
        }

        // ***
        // *** Read all available data on the serial port.
        // ***
        while (COMPORT.available() > 0)
        {
          // ***
          // *** Read the data from the serial port.
          // ***
          char c = COMPORT.read();

          // ***
          // *** Forward the data to the client.
          // ***
          client.write(c);

          // ***
          // *** Display it on the debugging device.
          // ***
          DEBUG.write(c);
        }
      }

      DEBUG.println();

      // ***
      // *** The client disconnected so stop the connection from this end.
      // ***
      client.stop();
      DEBUG.println("Client disconnected");
    }
    else
    {
      DEBUG.println("Client connection failed.");
    }
  }

  // ***
  // *** Yield to the microcontroller.
  // ***
  yield();
}

void QueryMdns()
{
  // ***
  // *** Send out query for esp tcp services.
  // ***
  DEBUG.print("Sending mDNS query for "); DEBUG.print(MDNS_SERVICE); DEBUG.println(" services.");
  int n = MDNS.queryService(MDNS_SERVICE, "tcp");
  DEBUG.println("mDNS query completed.");

  if (n == 0)
  {
    DEBUG.print("No "); DEBUG.print(MDNS_SERVICE); DEBUG.println(" services were found.");
  }
  else
  {
    DEBUG.print(n); DEBUG.println(" service(s) found");

    for (int i = 0; i < n; ++i)
    {
      // ***
      // *** Print details for each service found.
      // ***
      DEBUG.print(i + 1);
      DEBUG.print(": ");
      DEBUG.print(MDNS.hostname(i));
      DEBUG.print(" (");
      DEBUG.print(MDNS.IP(i));
      DEBUG.print(":");
      DEBUG.print(MDNS.port(i));
      DEBUG.println(")");
    }
  }

  DEBUG.println();
}
