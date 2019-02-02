#pragma once

int IsInsideRange(int location, int difference, int referencepoint);
int IsInsideBox(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY);
int boxcoll(int topleftX, int topleftY, int boxhorzlength, int boxvertlengh, int LocX, int LocY);
int keysmove(void);