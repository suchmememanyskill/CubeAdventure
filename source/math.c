#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>
#include "L1.h"

bool grounded = false;
float Vmomajust = 0, Vmomentum = 0, ajustpY = 0, ajustoY = 0, Hmomentum = 0, ajustpX = 0, ajustoX = 0;

int IsInsideRange(int location, int difference, int referencepoint){
	int result = 0;
	if (location > referencepoint && location - difference < referencepoint) result = 1;
	else result = 0;
	return result;
}

int IsInsideBox(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY){
	int result = 0;
	if (IsInsideRange(LocX + 20, boxhorzlength + 20, topleftX) == 1 && IsInsideRange(LocY + 40, boxvertlengh + 40, topleftY) == 1) result = 1;
	return result;
}

int boxcoll(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY){
		int result = 0;
		if (IsInsideBox(topleftX, topleftY, boxhorzlength, boxvertlengh, LocX, LocY) == 1){
			if (IsInsideRange(LocY + 40, 10, topleftY) == 1 && IsInsideRange(LocX + 20, boxhorzlength + 14, topleftX + 3) == 1) grounded = true;
			else if (IsInsideRange(LocY + 39, 10, topleftY) == 1) grounded = true, ajustpY = ajustpY - 1;
			//if (IsInsideRange(LocY + 35, 10, topleftY) == 1) result = 7;  //up
			else if (IsInsideRange(LocX, 5, topleftX + boxhorzlength - 4) == 1) ajustpX = ajustpX + 1, Hmomentum = 0; //right
			else if (IsInsideRange(LocY, 10, topleftY + boxvertlengh - 10) == 1 && IsInsideRange(LocX + 20, boxhorzlength + 14, topleftX + 3) == 1) Vmomentum = 0; //down
			else if (IsInsideRange(LocX + 20, 5, topleftX) == 1) ajustpX = ajustpX - 1, Hmomentum = 0; //left 
			else ajustpY = ajustpY - 5;
		}
		return result;
}


void ladder(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY, float offsetY){
if (IsInsideBox(topleftX, topleftY, boxhorzlength, boxvertlengh, LocX, LocY) != 0){
	if (IsInsideBox(topleftX, topleftY - 38, boxhorzlength, 7, LocX, LocY) != 0) grounded = true;
	else Vmomentum = -0.15f;
	if (keysmove(1) == 1) { ajustoY = ajustoY - 2; }
	if (keysmove(1) == 5 && offsetY < -1) { ajustoY = ajustoY + 2; }
	else if (keysmove(1) == 5 && offsetY > -1) { ajustpY = ajustpY + 1; }
}
}

float calcVmomentum(float locY, float offsetY, int season, int apress){
	if (Vmomajust != 0) Vmomentum = Vmomentum + Vmomajust;

 	if (locY < 50 && Vmomentum < 0 ) { ajustpY = ajustpY - Vmomentum; ajustoY = ajustoY + Vmomentum; }
    else if (locY < 50 && Vmomentum == 0) { ajustpY = ajustpY + 1; ajustoY = ajustoY - 1; }
    if (locY > 160 && offsetY < 0 && Vmomentum > 0) { ajustpY = ajustpY - Vmomentum; ajustoY = ajustoY + Vmomentum; }
	else if (locY > 160 && offsetY < 0 && Vmomentum == 0) { ajustpY = ajustpY - 1; ajustoY = ajustoY + 1; }

	if (grounded == true && apress == 1 && season == 4) Vmomentum = Vmomentum - 3.0f;
    else if (grounded == true && apress == 1 && season == 1) Vmomentum = Vmomentum - 5.0f;
    else if (grounded == true && apress == 1) Vmomentum = Vmomentum - 4.0f;
    else if ((keysmove(2) == 1 || keysmove(1) == 1 || keysmove(1) == 2 || keysmove(1) == 8) && season == 3 && Vmomentum > 0)  Vmomentum = Vmomentum - 0.12f;

    if (offsetY > 0) ajustoY = ajustoY - offsetY;

 	if (grounded == true && Vmomentum > 0) Vmomentum = 0;
 	if (grounded == false) Vmomentum = Vmomentum + 0.15f;
 	grounded = false;

    Ajustoffset("Y", ajustoY);
    Ajustoffset("pY", ajustpY);
    ajustpY = 0, ajustoY = 0, Vmomajust = 0;

    float tempvmom = 0;
    tempvmom = Vmomentum;
    return tempvmom;
}

float calcHmomentum(float locX, float offsetX, int season){

	if (locX < 0) ajustoX = ajustoX - locX;

    if ((locX > 270 && Hmomentum > 0) || (locX < 80 && offsetX > 0 && Hmomentum < 0)) { 
    	ajustpX = ajustpX - Hmomentum; 
    	ajustoX = ajustoX + Hmomentum;
    }

    int seasajusted = season - 1;
    float acc[4] = {1, 0.6f, 0.5f, 0.3f}; //spring acceleration, summer acceleration, fall acceleration, winter acceleration.
    float dec[4] = {2, 2.8f, 2.4f, 3.75f}; //max speed spring, summer, fall, winter.
    bool LRpress = false;

    if (keysmove(1) == 3 || keysmove(1) == 2 || keysmove(1) == 4) Hmomentum = Hmomentum + acc[seasajusted], LRpress = true;
    if (keysmove(1) == 7 || keysmove(1) == 6 || keysmove(1) == 8) Hmomentum = Hmomentum - acc[seasajusted], LRpress = true;

    if (LRpress == true){
    	if (Hmomentum > dec[seasajusted]) Hmomentum = Hmomentum - acc[seasajusted];
    	if (Hmomentum < (-1 * dec[seasajusted])) Hmomentum = Hmomentum + acc[seasajusted];
    	if (season == 4 && grounded == false && (Hmomentum > dec[seasajusted] - 0.75f)) Hmomentum = Hmomentum - acc[seasajusted];
    	if (season == 4 && grounded == false && (Hmomentum < -1 * dec[seasajusted] + 0.75f)) Hmomentum = Hmomentum + acc[seasajusted];
    }
    else {
    	if (Hmomentum > 0.7f) Hmomentum = Hmomentum - 0.5f;
    	if (Hmomentum < -0.7f) Hmomentum = Hmomentum + 0.5f;
    	if (Hmomentum > -0.7f && Hmomentum < 0.7f) Hmomentum = 0;
    }



	Ajustoffset("X", ajustoX);
    Ajustoffset("pX", ajustpX);


    ajustpX = 0, ajustoX = 0;
    LRpress = false;

    float temphmom = 0;
    temphmom = Hmomentum;
    return temphmom;
}

int keysmove(int select){
	circlePosition pos;
	hidCircleRead(&pos);
	int directX = 0, directY = 0, result = 0;
	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	if (select == 1){

	if (pos.dx > 80) directX = 1;
	else if (pos.dx < -80) directX = -1;

	if (pos.dy > 80) directY = 1;
	else if (pos.dy < -80) directY = -1;

	if (kHeld & KEY_DUP) directY = 1;
	else if (kHeld & KEY_DDOWN) directY = -1;
	
	if (kHeld & KEY_DRIGHT) directX = 1;
	else if (kHeld & KEY_DLEFT) directX = -1;

	if (directX == -1 && directY == 0) result = 7; //left
	else if (directX == 0 && directY == -1) result = 5; //down
	else if (directX == 1 && directY == 0) result = 3; //right
	else if (directX == 0 && directY == 1) result = 1; //up
	else if (directX == -1 && directY == -1) result = 6; //downleft
	else if (directX == -1 && directY == 1) result = 8; //upleft
	else if (directX == 1 && directY == -1) result = 4; //downright
	else if (directX == 1 && directY == 1) result = 2; //upright
}

	if (select == 2){
		if (kHeld && KEY_A) result = 1;
	}
	return result;
}