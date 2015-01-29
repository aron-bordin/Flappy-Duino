/**
 *     
 	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "LedMatrixObject.h"
#include "TimerObject.h"


LedMatrixObject *led = new LedMatrixObject(2, 3, 4, 5, 6, 7, 8, 9);

TimerObject *fps = new TimerObject(1/30); //so we can have 30 FPS 

float heroY = 5, vSpeed = 0.1;//go down 0.1 LED per FPS;

float towerX = 15; //pos to the enemy tower

float floorOffset = 1;
float hSpeed = 0.05; //scene move 0.1 led/fps

int gapY = 3;

int buttonPin = 11;

unsigned long deadTime = millis();


unsigned char  Home[16][16] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

bool isHome = true;
void setup(){
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);
    fps->setOnTimer(&gameUpdate);
    fps->Start();
}

void drawHero(){
    /*
        hero looks like:

            *
           ***  <- the center * is the Y position
            * 
     */
    
    led->setLedOn((int)heroY + 1, 2);
    led->setLedOn((int)heroY, 1);
    led->setLedOn((int)heroY, 2);
    led->setLedOn((int)heroY, 3);
    led->setLedOn((int)heroY - 1, 2);
}

void drawFloor(){
    /**
      the floor is basic, like:
      **** **** **** ****
     */
    
    int x = (int)floorOffset;

    for(int i = 0; i < 16; i++){
        if((i + x) % 4 != 0)
            led->setLedOn(15, i);
    }
}

void drawTower(){
    /**
      The tower is a vertical line, with a gap of 5 leds
     */
    towerX -= hSpeed;
    int x = (int)towerX;
    if(x < 0){
        x = 15;
        towerX = 15;
        gapY = random(0, 8);
    }

    for(int i = 0; i < 16; i++){
        if(i <= gapY || i >= gapY + 7){
            led->setLedOn(i, x);    
            led->setLedOn(i, x + 1);
        }
    }

    if(x == 2)
        checkCollisions();
}

void checkCollisions(){
    if(heroY < gapY | heroY > gapY + 7)
        heroDie();
}

void gameUpdate(){
    heroY += vSpeed; // update hero position

    if(heroY >= 15) //if hero in the bottom
        heroDie();

    floorOffset += hSpeed; //move the screen offset
    if(floorOffset > 4)
        floorOffset = 0;

    // drawFloor(); //show the floor in the scene
    drawTower(); // show the tower in the screen
    drawHero(); //show the hero in the scene

    led->draw(); //draw the scene
}

void heroDie(){
    restartGame();
    isHome = true;
    deadTime = millis();
}

void restartGame(){
    heroY = 5;
    towerX = 15;
    floorOffset = 1;
    gapY = 3;
}

void jump(){
    if(heroY > 0)
        heroY -= 0.2;
}

void loop(){
    int buttonState = digitalRead(buttonPin);
    if(isHome){
        led->setScene(Home);
        led->draw();
        if(buttonState && millis() - deadTime > 1000)
            isHome = false;
    } else {
        if(buttonState){
            jump();
        }
        fps->Update();
    }
	
}