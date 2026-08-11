// Main.cpp - ДВИЖОК
#define _CRT_SECURE_NO_WARNINGS
#include "Main.h"
#include "Render.h"
#include "Types.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdio>

// ============================================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ============================================================================

static HWND g_hwnd = NULL;
static bool g_initialized = false;

#define MAX_OBJECTS 100
static Object g_objects[MAX_OBJECTS];
short g_objectCount = 0;  // Глобальная переменная

void clearObjects() {
    for (int i = 0; i < g_objectCount; i++) {
        g_objects[i].texture.freeData();
    }
    g_objectCount = 0;
}

// ============================================================================
// ФУНКЦИИ ИЗ MAIN.H
// ============================================================================

char getSymbol() {
    for (int key = 8; key < 256; key++) {
        if (GetAsyncKeyState(key) & 0x8000) {
            if (key == VK_ESCAPE) return 27;
            if (key == VK_RETURN) return 13;
            if (key == VK_SPACE) return 32;
            if (key == VK_BACK) return 8;
            if (key == VK_TAB) return 9;

            if ((key >= 'A' && key <= 'Z') ||
                (key >= '0' && key <= '9') ||
                (key >= 'a' && key <= 'z')) {
                return (char)key;
            }
        }
    }
    return 0;
}

void getPressedKeys(char* keys, int* count) {
    *count = 0;

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) keys[(*count)++] = 27;
    if (GetAsyncKeyState(VK_RETURN) & 0x8000) keys[(*count)++] = 13;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) keys[(*count)++] = 32;
    if (GetAsyncKeyState(VK_BACK) & 0x8000) keys[(*count)++] = 8;
    if (GetAsyncKeyState(VK_TAB) & 0x8000) keys[(*count)++] = 9;
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) keys[(*count)++] = VK_LEFT;
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) keys[(*count)++] = VK_RIGHT;
    if (GetAsyncKeyState(VK_UP) & 0x8000) keys[(*count)++] = VK_UP;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) keys[(*count)++] = VK_DOWN;

    for (char c = 'A'; c <= 'Z'; c++) {
        if (GetAsyncKeyState(c) & 0x8000) keys[(*count)++] = c;
    }
    for (char c = 'a'; c <= 'z'; c++) {
        if (GetAsyncKeyState(c) & 0x8000) keys[(*count)++] = c;
    }
    for (char c = '0'; c <= '9'; c++) {
        if (GetAsyncKeyState(c) & 0x8000) keys[(*count)++] = c;
    }
}

bool init() {
    if (g_initialized) return true;

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GameWindow";

    if (!RegisterClassA(&wc)) {
        return false;
    }

    g_hwnd = CreateWindowA("GameWindow", "2D Game",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_hwnd) {
        return false;
    }

    initRender(g_hwnd);

    srand((unsigned)time(NULL));
    g_initialized = true;
    return true;
}

bool newFrame() {
    if (!g_initialized) return false;

    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
            return false;
        }
    }

    return true;
}

Object* getObjects() {
    return g_objects;
}

short getObjectCount() {
    return g_objectCount;
}

void addObject(Object obj) {
    if (g_objectCount < MAX_OBJECTS) {
        g_objects[g_objectCount] = obj;
        g_objectCount++;
    }
}

void clearAllObjects() {
    clearObjects();
}

HWND getWindow() {
    return g_hwnd;
}