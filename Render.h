// Render.h
#pragma once
#include "Types.h"
#include <windows.h>

void initRender(HWND hwnd);
void renderObject(Object object);
void renderLoop(Object* objects, short objectNum);
void resizeRender(int newWidth, int newHeight);
void cleanupRender();