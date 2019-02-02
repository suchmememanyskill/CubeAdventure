#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>

int IsInsideRange(int location, int difference, int referencepoint){
	int result = 0;
	if (location > referencepoint && location - difference < referencepoint) result = 1;
	else result = 0;
	return result;
}

int IsInsideBox(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY){
	int result = 0;
	if (IsInsideRange(LocX + 20, boxhorzlength + 20, topleftX) == 1 && IsInsideRange(LocY + 20, boxvertlengh + 20, topleftY) == 1) result = 1;
	return result;
}

int boxcoll(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY){
		int result = 0;
		if (IsInsideBox(topleftX, topleftY, boxhorzlength, boxvertlengh, LocX, LocY) == 1){
			if (IsInsideRange(LocY + 20, 10, topleftY) == 1 && IsInsideRange(LocX + 20, boxhorzlength + 14, topleftX + 3) == 1) {
			result = 1;
			if (IsInsideRange(LocY + 19, 10, topleftY) == 1) result = 6;
			if (IsInsideRange(LocY + 15, 10, topleftY) == 1) result = 7; } //up
			else if (IsInsideRange(LocX, 5, topleftX + boxhorzlength - 4) == 1) result = 2; //right
			else if (IsInsideRange(LocY, 10, topleftY + boxvertlengh - 10) == 1 && IsInsideRange(LocX + 20, boxhorzlength + 14, topleftX + 3) == 1) result = 3; //down
			else if (IsInsideRange(LocX + 20, 5, topleftX) == 1) result = 4; //left 
			else result = 5;
		}
		return result;
}

int keysmove(){
	circlePosition pos;
	hidCircleRead(&pos);
	int directX = 0, directY = 0, result = 0;

	if (pos.dx > 80) directX = 1;
	else if (pos.dx < -80) directX = -1;

	if (pos.dy > 80) directY = 1;
	else if (pos.dy < -80) directY = -1;

	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();

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

	return result;
}