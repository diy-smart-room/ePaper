#include "Arduino.h"
#include <stdio.h>
#include <string.h>
#include "GxEPD2_BW.h"
#include "SPI.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Zigbee.h"
#include "stdlib.h"

#define DEFAULT_X_CRT 30
#define DEFAULT_Y_CRT 15
#define VELIKOST_CRTIC 5

class Graf{
    private:
        int posX;
        int posY;
        int width;
        int height;
        GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> *display;
        int minTemp = 20;
        int maxTemp = 30;       // in Celsius 
        int deltaTime = 12;     // in hours 
    public:
        Graf(int x,int y, int w, int h, GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> *display):
            posX(x), posY(y), width(w), height(h), display(display) {}
        
        void a(void){           //za preverjanje template-a za grafikon
            display->setPartialWindow(posX, posY, width, height);
            display->firstPage();
            do{
                display->fillRect(posX,posY,width,height, GxEPD_BLACK);
            }while(display->nextPage());
        }

        void b(void){           //za narisanje x in y osi
            int razmakX = width / DEFAULT_X_CRT;
            int razmakY = height / DEFAULT_Y_CRT;

            display->setPartialWindow(0,posY,display->width(),display->height());
            display->firstPage();
            do{
                display->drawLine(posX, posY, posX, posY + height, GxEPD_BLACK);
                display->drawLine(posX, posY + height , posX + width, posY + height, GxEPD_BLACK);
                for(int i = 1; i * razmakX < width; i++){
                   display->drawLine(posX + i * razmakX, posY + height + VELIKOST_CRTIC,posX +  i* razmakX,
                                     posY + height - VELIKOST_CRTIC, GxEPD_BLACK);
                }

                for(int i = 1; height - razmakY * i > 0; i++){
                    display->drawLine(posX - VELIKOST_CRTIC, posY + height - i * razmakY, posX + VELIKOST_CRTIC,
                                      posY + height - i * razmakY, GxEPD_BLACK);
                }


            }while(display->nextPage());

        }

        void c(void){               //brisanje grafa
            display->setPartialWindow(posX+2,posY, width - 2, height - 1);
            display->firstPage();
            do{
                display->fillRect(posX+2,posY,width-2,height - 1, GxEPD_WHITE);
            }while(display->nextPage());
        }

        void d(void){
            int y = posY + height + VELIKOST_CRTIC;
            display->setTextSize(1);
            display->setTextColor(GxEPD_BLACK);
            display->setPartialWindow(0, posY + height + VELIKOST_CRTIC, 32,20);
            display->firstPage();
            do{
                display->setCursor(posX + 23, posY + 12 + height + VELIKOST_CRTIC);
                display->print("12");


            }while(display->nextPage());
        }
};
