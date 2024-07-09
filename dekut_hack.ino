#include <PubSubClient.h>
#include <WiFi.h>
#include <ESP32Firebase.h>


#define ssid "dekut"
#define password "dekut@ict2023"
#define mqtt_server "mqtt.eclipseprojects.io"

#define REFERENCE_URL "https://dhack-62204-default-rtdb.firebaseio.com/"  

Firebase firebase(REFERENCE_URL);

WiFiClient espClient;//creating object in wifi client called client but you can change it
PubSubClient client(espClient);////creating an object in pubsub client


#include <Multichannel_Gas_GMXXX.h>

// if you use the software I2C to drive the sensor, you can uncommnet the define SOFTWAREWIRE which in Multichannel_Gas_GMXXX.h.
#ifdef SOFTWAREWIRE
    #include <SoftwareWire.h>
    SoftwareWire myWire(3, 2);
    GAS_GMXXX<SoftwareWire> gas;
#else
    #include <Wire.h>
    GAS_GMXXX<TwoWire> gas;
#endif

static uint8_t recv_cmd[8] = {};


     // Creating MQTT callback
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  
  
    //converting the message received in bytes to char 
    for (int i = 0; i < length; i++) {
      Serial.print((char)message[i]);
      messageTemp += (char)message[i];
    }
    Serial.println();
  
    // Feel free to add more if statements to control more GPIOs with MQTT
  
    // If a message is received on the topic home/office/esp1/gpio2, you check if the message is either 1 or 0. Turns the ESP GPIO according to the message
    if(topic=="PUMP"){
        Serial.print("Switching PUMP");
        if(messageTemp == "on"){
          
          Serial.print("On");
        }
        else if(messageTemp == "off"){
          ;
          Serial.print("Off");
        }
    }
  }



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect to a specific id which is esp8266client
   
  if (client.connect("ESP321")) {
    Serial.println("connected");  
    // Subscribe or resubscribe to a topic
    // You can subscribe to more topics (to control more LEDs in this example)
    client.subscribe("NO2");//pump is the topic which then the client will be able to publish to in the callback looop
   
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(5000);
  }
  }

}
void setup() {
    Serial.begin(9600);
    // If you have changed the I2C address of gas sensor, you must to be specify the address of I2C.
    //The default addrss is 0x08;
    gas.begin(Wire, 0x08); // use the hardware I2C
    //gas.begin(MyWire, 0x08); // use the software I2C
    //gas.setAddress(0x64); change thee I2C address
      WiFi.mode(WIFI_STA);//to intiate it as a listener butcomes automatic so this line of code is not very much useful
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);


  //wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  //Serial.println("IP address: " + WiFi.localIP().toString());//viewing ip address

  client.setServer(mqtt_server, 1883);//creating mqtt
  client.setCallback(callback);//for the client to publish
}  





void loop() {
    uint8_t len = 0;
    uint8_t addr = 0;
    uint8_t i;
    uint32_t val = 0;
    uint32_t val1 = 0;
    uint32_t val2 = 0;
    uint32_t val3 = 0;

        //to reconnect whenever not connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();//for continuous streaming of data


    val = gas.getGM102B(); Serial.print("NO2: "); Serial.println(val); //Serial.print("  =  ");
    //Serial.print(gas.calcVol(val)); Serial.println("V");
    val1 = gas.getGM302B(); Serial.print("ETHYL ALCOHOL: "); Serial.println(val1); //Serial.print("  =  ");
    //Serial.print(gas.calcVol(val1)); Serial.println("V");
    val2 = gas.getGM502B(); Serial.print("VOC: "); Serial.println(val2); //Serial.print("  =  ");
    //Serial.print(gas.calcVol(val2)); Serial.println("V");
    val3 = gas.getGM702B(); Serial.print("CO: "); Serial.println(val3); //Serial.print("  =  ");
    //Serial.print(gas.calcVol(val3)); Serial.println("V");

    delay(500);

    ///publishing data.....the first tpart in the arguement is the topic subscribe in string format while the second part is the message in string format
    if (val3>=400){
    client.publish("NO2", "5");
    while (val>=590){
      client.publish("NO2","1");
    }
    }
    else(val3<400);{
      client.publish("NO2" ,"1");
    }

  firebase.setFloat("Sensor/NO2", val);
  delay(200);
   firebase.setFloat("Sensor/ETHYL_ALCOHOL", val1);
  delay(200);
   firebase.setFloat("Sensor/VOC", val2);
  delay(200);
   firebase.setFloat("Sensor/CO", val3);
  delay(200);
  
    

    // to subscribe data.........we will have to draw the callback funtion so that it can work as a communication channel

}


  
