#include "Arduino.h"
#include <stdio.h>
#include <string.h>
#include "GxEPD2_BW.h"
#include "SPI.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Zigbee.h"
#include "stdlib.h"
#include "Graf.cpp"

#define EPD_SCK   6
#define EPD_MOSI  7
#define EPD_CS    18  
#define EPD_DC    10
#define EPD_RES   11
#define EPD_BUSY  12

#define ST_MER_ZA_SKOK 2
#define BUTTON 2

struct _podatek{
  const char* enota;
  const char* stvar;
  float stevilo;
  float vsotaMer = 0.0;
  int stPrebranihMer = 0;
};

_podatek** arrPodatkov = (_podatek**)calloc(sizeof(_podatek*),2);


GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(
    GxEPD2_370_GDEY037T03(EPD_CS, EPD_DC, EPD_RES, EPD_BUSY)
);


#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee Coordinator/Router mode is not enabled!"
#endif

#define ZIGBEE_ENDPOINT 1

void izpis(int);
void receiveTemperature(float);
void receiveHumidity(float);
char* frankenSteinnanjeStringov(int, float);
void test(void);

Graf graf(40,50,360, 150, &display);

ZigbeeThermostat zbThermostat(ZIGBEE_ENDPOINT);
bool bol = false;
bool lol = false;

void receiveTemperature(float temperature){
  bol = true;
  arrPodatkov[0]->stevilo = temperature;
  arrPodatkov[0]->vsotaMer += temperature;
  arrPodatkov[0]->stPrebranihMer++;
  graf.e(temperature);
}


void receiveHumidity(float humidity){
  lol = true;
  arrPodatkov[1]->stevilo = humidity;
  arrPodatkov[1]->vsotaMer += humidity;
  arrPodatkov[1]->stPrebranihMer++;

  if(arrPodatkov[1]->stPrebranihMer == ST_MER_ZA_SKOK){
  }
}


void setup(){
  SPI.begin(EPD_SCK,-1,EPD_MOSI,EPD_CS);      //na zalost je treba napisati custom configuracijo SPI bus-a, saj
  Serial.begin(115200);                 //esp32-c6 nima tistih standardnih pinov namenjene za SPI (23=/= MOSI)
  display.init(115200,true,50,false);
  display.setRotation(3);
  display.clearScreen();
  
  arrPodatkov[0] = (_podatek*)calloc(sizeof(_podatek),1);
  arrPodatkov[0]->enota = "C";
  arrPodatkov[0]->stvar = "Temp: ";
  arrPodatkov[0]->stevilo = -1; 
  
  arrPodatkov[1] = (_podatek*)calloc(sizeof(_podatek),1);
  arrPodatkov[1]->enota = "%";
  arrPodatkov[1]->stvar = "Vlaga: ";
  arrPodatkov[1]->stevilo = -1; 

  display.setTextSize(3);
  
  log_i("ESP32-C6 Zigbee Thermostat starting");

  zbThermostat.onTempReceive(receiveTemperature);
  zbThermostat.onHumidityReceive(receiveHumidity);

  zbThermostat.setManufacturerAndModel(
      "DIY Smart Room",
      "ESP32-C6 Thermostat"
  );

  zbThermostat.allowMultipleBinding(true);

  Zigbee.addEndpoint(&zbThermostat);

  log_i("Starting Zigbee");

  if (!Zigbee.begin(ZIGBEE_ROUTER)){
      log_e("Failed to start Zigbee");
      ESP.restart();
  }

  log_i("Waiting for Zigbee network");

  /*
  while (!Zigbee.connected()){
      delay(100);
  }
  */

  log_i("Connected to Zigbee network");
  log_i("Waiting for temperature sensor to join or rejoin");

  pinMode(BUTTON, INPUT);

  izpis(0);
  //test();

  graf.b();
  graf.d();
  delay(1000);
  graf.e(23.41);
}


void izpis(int id){
  display.setPartialWindow(20,10,display.width(),25);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  char* str = frankenSteinnanjeStringov(id,arrPodatkov[id]->stevilo);
  display.firstPage();
  do{
    display.setCursor(20,10);
    display.print(str);
  }while(display.nextPage());
  Serial.println(str);
  free(str);
}


char* frankenSteinnanjeStringov(int id, float st){        //naredi EN velik string za izpis (seveda ga pol se free-am)
  char* string = (char*)calloc(sizeof(char),(strlen(arrPodatkov[id]->enota) + strlen(arrPodatkov[id]->stvar) + 10 ));
  strcat(string,arrPodatkov[id]->stvar);
  char* temp = (char*)calloc(sizeof(char),10);
  sprintf(temp,"%2.1f ",st);
  strcat(string,temp);
  strcat(string,arrPodatkov[id]->enota);
  free(temp);
  return string;
}

float num;

void test(){
    num = 20 + rand()%9 + (float)((rand()%10))/10;
    receiveTemperature(num);
}

int gumb;
int next = 0;

void loop(){
  //Serial.println(graf.izracunaj());
    gumb = digitalRead(BUTTON);
    //Serial.println(gumb);

    if(gumb == 1 && millis() > next){
        next = millis() + 500;
        gumb = 0;
        test();
    }

    if(bol){
        bol = false;
        izpis(0);
    }

  //graf.e();
  
  if(lol){
    lol = false;
    //izpis(1);
  }
}
