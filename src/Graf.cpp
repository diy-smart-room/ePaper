#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "GxEPD2_BW.h"
#include "SPI.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include "Zigbee.h"
#include "stdlib.h"

#define DEFAULT_X_CRT 24
#define DEFAULT_Y_CRT 10
#define VELIKOST_CRTIC 5

class Graf{
    private:
        int posX;           //to so specifikacije celotnega grafikona
        int posY;
        int width;
        int height;
        int grafX;          //tole pa je za dobesedno graf (crta)
        int grafY;
        int grafW;
        int grafH;
        GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> *display;
        float arr[DEFAULT_X_CRT];

    public:
        Graf(int x,int y, int w, int h, GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> *display):
            posX(x), posY(y), width(w), height(h), display(display) {
                for(int i = 0; i < DEFAULT_X_CRT; i++){
                    arr[i] = -1;
                }
            }
        
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

            display->setPartialWindow(0,posY-5,display->width(),display->height());
            display->setTextSize(1);
            display->setTextColor(GxEPD_BLACK);
            display->firstPage();
            int x,y;
            int stX, stY;

            int16_t x1,y1;
            uint16_t stW,stH;
            char str[5];

            do{
                display->drawLine(posX, posY, posX, posY + height, GxEPD_BLACK);
                display->drawLine(posX, posY + height , posX + width, posY + height, GxEPD_BLACK);
                for(int i = 1; i  < DEFAULT_X_CRT +1 ; i++){
                    x = posX + i * razmakX;
                    y = posY + height;
                    display->drawLine( x , y + VELIKOST_CRTIC, x, y - VELIKOST_CRTIC, GxEPD_BLACK);

                    stY = y + (8 - (y%8));
                    stX = x;
                    snprintf(str,sizeof(str),"%d",DEFAULT_X_CRT - i);
                    display->getTextBounds(str,0,0,&x1,&y1,&stW,&stH);
                    display->setCursor(stX +1- stW/2,stY);
                    display->print(str);
                }



                for(int i = 1;  i < DEFAULT_Y_CRT +1 ; i++){
                    x = posX;
                    y = posY + height - i * razmakY;
                    display->drawLine(x - VELIKOST_CRTIC, y , x+ VELIKOST_CRTIC, y , GxEPD_BLACK);

                    stY = y;
                    stX = x - VELIKOST_CRTIC - 8;
                    snprintf(str,sizeof(str),"%d",i + 19);
                    display->getTextBounds(str,0,0,&x1,&y1,&stW,&stH);
                    display->setCursor(stX + 1- stW/2,stY + 1 - stH/2);
                    display->print(str);
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

        void d(){                   // za testiranje kje so bo grafikon risal!!!!!
            int gX = posX + VELIKOST_CRTIC;
            gX = gX + (8 - gX%8);
            int gY = posY - VELIKOST_CRTIC;
            gY = gY + (8 - gY%8);
            int gH = height - VELIKOST_CRTIC;
            gH = gH - gH%8;
            int gW = width - VELIKOST_CRTIC;
            gW = gW +(8 - gW%8);

            /*
            display->setPartialWindow(grafX,grafY,grafW,grafH);

            do{
                display->fillRect(grafX,grafY,grafW,grafH, GxEPD_BLACK);
            }while(display->nextPage());
            */
            grafX = gX;
            grafY = gY;
            grafH = gH;
            grafW = gW;
        }

        void e(float stevilo){
            int razmakX = width / DEFAULT_X_CRT;
            int razmakY = height / DEFAULT_Y_CRT;

            float temp;

            for(int i = 0; i < DEFAULT_X_CRT; i++){
                temp = arr[i];
                arr[i] = stevilo;
                stevilo = temp;
            }

            display->setPartialWindow(grafX,grafY,grafW,grafH);
            do{
                display->fillRect(grafX,grafY,grafW,grafH,GxEPD_WHITE);
                for(int i = 0; arr[i+1] > -1 && i < DEFAULT_X_CRT - 1 ; i++){
                    display->drawLine(posX + width - i * razmakX, posY + (int)(height - (arr[i]-20.0)/9.0 * height),
                                      posX + width - (i+1) * razmakX, posY + (int)(height - (arr[i+1]-20.0)/9.0*height), GxEPD_BLACK);
                }
            }while(display->nextPage());
        }
};
