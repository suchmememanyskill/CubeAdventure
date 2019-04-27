#pragma once

int IsInsideRange(int location, int difference, int referencepoint);
int IsInsideBox(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY);
int boxcoll(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY);
int keysmove(int select);
void ladder(float topleftX, float topleftY, int boxhorzlength, int boxvertlengh, float LocX, float LocY, float offsetY);
float calcVmomentum(float locY, float offsetY, int season, int apress);
float calcHmomentum(float locX, float offsetX, int season);
