#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const byte mac[] = {0x00,0x01,0x02,0x03,0x04,0x05};
const byte myIp[] = {192,168,0,10};

const IPAddress mqttIp(192,168,0,1);
const uint16_t mqttPort = 1883;
const char mqttID[] = {"KesselRaum"};
const char mqttUser[] = {"local"};
const char mqttPasswd[] = {"local"};
const char mqttPubTopic[] = {"keller/heizung"} ;

EthernetClient client;
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}
PubSubClient mqtt(mqttIp,1883,callback,client);

int8_t sensorCount;
OneWire ow(A4);
DallasTemperature ds(&ow);

uint8_t sendStatus=0;

void setup() {
  ds.begin();
  Ethernet.begin(mac,myIp);
  //mqtt.connect(mqttID);  
  }



void loop() {
  sensorCount = ds.getDS18Count();
  mqtt.loop();
  Ethernet.maintain();
    if(sensorCount>0){
      if(sendStatus>=sensorCount)sendStatus=0;
      ds.requestTemperatures();
        DeviceAddress address;
        ds.getAddress(address,sendStatus);
         
        char addrStr[50];
        sprintf(addrStr,"%s/%s/%x%x%x%x%x%x%x%x",mqttPubTopic,"temperatures",address[0],address[1],address[2],address[3],address[4],address[5],address[6],address[7]);
        float temp = ds.getTempCByIndex(sendStatus);
        char tempStr[10];
        String buff = String(temp);
        buff.toCharArray(tempStr,10);
        if(mqtt.connect(mqttID)){
        mqtt.publish(addrStr,tempStr);
        mqtt.disconnect();
        }
        sendStatus++;
      delay(2000);
    }

}