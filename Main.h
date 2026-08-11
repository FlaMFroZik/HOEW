// Main.h
#pragma once
#include <windows.h>

extern short g_objectCount;

char getSymbol();
bool init();
bool newFrame();
void getPressedKeys(char* keys, int* count);

struct Object;
Object* getObjects();
short getObjectCount();
void addObject(Object obj);
void clearAllObjects();
HWND getWindow();