#pragma once

extern int privChanBufferPtr;

void InitialiseTibiaHooks();
int baseAdjust(int addr);

void INmyDrawRect();
void INmyDrawBlackRect();
void myDrawRect(int ebp, int ecx, int nSurface, int nX, int nY, int nWeight, int nHeight, int nRed, int nGreen, int nBlue, DWORD drawRectPtr);
int OUTmyDrawRect(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, DWORD drawRectPtr);

void INmyPrintText();
int myPrintText(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign);
int OUTmyPrintText(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9);

void INmyPlayerNameText();
int myPlayerNameText(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char* str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17);
int OUTmyPlayerNameText(int alignvar, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *strvar, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17);

void INmyInterceptInfoMiddleScreen();
void myInterceptInfoMiddleScreen(int type, char *s);
void OUTmyInterceptInfoMiddleScreen(int typevar, char* s);

void INmyInterceptInfoMessageBox();
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

void INmyInterceptEncrypt();
int myInterceptEncrypt(int v1, int v2);
int OUTmyInterceptEncrypt(int v1, int v2);

void INmyInterceptDecrypt();
int myInterceptDecrypt(int v1, int v2);
int OUTmyInterceptDecrypt(int v1, int v2);


void trapFun(HANDLE dwHandle, int addr, unsigned int targetFun);
void hookDrawRect(HANDLE dwHandle, int addr, unsigned int targetFun);