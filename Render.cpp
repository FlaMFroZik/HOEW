// Render.cpp - ОПТИМИЗИРОВАННЫЙ (без SetPixel)
#define _CRT_SECURE_NO_WARNINGS
#include "Render.h"
#include <windows.h>

static HDC g_hdc = NULL;
static HDC g_hdcMem = NULL;
static HBITMAP g_hbmMem = NULL;
static int g_winW = 0;
static int g_winH = 0;

// ============================================================================
// БУФЕР ПИКСЕЛЕЙ (ОЧЕНЬ БЫСТРО!)
// ============================================================================

static int* g_pixelBuffer = NULL;
static int g_bufferSize = 0;
static BITMAPINFO g_bmi = { 0 };

inline BYTE getR(COLORREF color) { return (BYTE)((color >> 24) & 0xFF); }
inline BYTE getG(COLORREF color) { return (BYTE)((color >> 16) & 0xFF); }
inline BYTE getB(COLORREF color) { return (BYTE)((color >> 8) & 0xFF); }

inline COLORREF makeCOLORREF(COLORREF color) {
    return RGB(getR(color), getG(color), getB(color));
}

void initRender(HWND hwnd) {
    g_hdc = GetDC(hwnd);

    RECT rect;
    GetClientRect(hwnd, &rect);
    g_winW = rect.right;
    g_winH = rect.bottom;

    // === СОЗДАЁМ БУФЕР ПИКСЕЛЕЙ ===
    g_bufferSize = g_winW * g_winH;
    g_pixelBuffer = (int*)malloc(g_bufferSize * sizeof(int));
    memset(g_pixelBuffer, 0, g_bufferSize * sizeof(int));

    // === НАСТРАИВАЕМ BITMAPINFO ===
    g_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    g_bmi.bmiHeader.biWidth = g_winW;
    g_bmi.bmiHeader.biHeight = -g_winH;  // Отрицательное = сверху вниз
    g_bmi.bmiHeader.biPlanes = 1;
    g_bmi.bmiHeader.biBitCount = 32;
    g_bmi.bmiHeader.biCompression = BI_RGB;
    g_bmi.bmiHeader.biSizeImage = 0;
    g_bmi.bmiHeader.biXPelsPerMeter = 0;
    g_bmi.bmiHeader.biYPelsPerMeter = 0;
    g_bmi.bmiHeader.biClrUsed = 0;
    g_bmi.bmiHeader.biClrImportant = 0;

    // HDC для совместимости
    g_hdcMem = CreateCompatibleDC(g_hdc);
    g_hbmMem = CreateCompatibleBitmap(g_hdc, g_winW, g_winH);
    SelectObject(g_hdcMem, g_hbmMem);
}

// ============================================================================
// БЫСТРАЯ ОТРИСОВКА В БУФЕР
// ============================================================================

void drawTextureFast(HDC hdc, Texture* texture, int destX, int destY, int destW, int destH) {
    if (texture == NULL || texture->data == NULL) return;
    if (texture->width <= 0 || texture->height <= 0) return;

    int* texData = texture->data;
    float scaleX = (float)destW / texture->width;
    float scaleY = (float)destH / texture->height;

    // Ограничиваем область рисования
    int startX = destX;
    int startY = destY;
    int endX = destX + destW;
    int endY = destY + destH;

    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > g_winW) endX = g_winW;
    if (endY > g_winH) endY = g_winH;

    if (startX >= endX || startY >= endY) return;

    // Рисуем прямо в буфер (ОЧЕНЬ БЫСТРО!)
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            int srcX = (int)((x - destX) / scaleX);
            int srcY = (int)((y - destY) / scaleY);

            if (srcX >= texture->width) srcX = texture->width - 1;
            if (srcY >= texture->height) srcY = texture->height - 1;

            int texIndex = srcY * texture->width + srcX;
            int screenIndex = y * g_winW + x;

            g_pixelBuffer[screenIndex] = texData[texIndex];
        }
    }
}

void renderObject(Object object) {
    if (g_pixelBuffer == NULL) return;
    drawTextureFast(g_hdcMem, &object.texture, object.x, object.y, object.width, object.height);
}

// ============================================================================
// ГЛАВНЫЙ РЕНДЕР (ОДИН ВЫЗОВ SetDIBitsToDevice!)
// ============================================================================

void renderLoop(Object* objects, short objectNum) {
    if (objects == NULL || objectNum <= 0) return;
    if (g_pixelBuffer == NULL) return;

    // 1. Очищаем буфер (очень быстро!)
    memset(g_pixelBuffer, 0, g_bufferSize * sizeof(int));

    // 2. Рисуем все объекты в буфер
    for (short i = 0; i < objectNum; i++) {
        renderObject(objects[i]);
    }

    // 3. Копируем буфер на экран (ОДИН ВЫЗОВ!)
    SetDIBitsToDevice(
        g_hdc,           // HDC
        0, 0,            // x, y
        g_winW, g_winH,  // ширина, высота
        0, 0,            // srcX, srcY
        0, g_winH,       // start scan, scan lines
        g_pixelBuffer,   // данные
        &g_bmi,          // BITMAPINFO
        DIB_RGB_COLORS
    );
}

void resizeRender(int newWidth, int newHeight) {
    if (g_hdcMem) {
        DeleteDC(g_hdcMem);
        DeleteObject(g_hbmMem);
    }

    g_winW = newWidth;
    g_winH = newHeight;
    g_bufferSize = g_winW * g_winH;

    // Пересоздаём буфер
    if (g_pixelBuffer) {
        free(g_pixelBuffer);
    }
    g_pixelBuffer = (int*)malloc(g_bufferSize * sizeof(int));
    memset(g_pixelBuffer, 0, g_bufferSize * sizeof(int));

    // Обновляем BITMAPINFO
    g_bmi.bmiHeader.biWidth = g_winW;
    g_bmi.bmiHeader.biHeight = -g_winH;

    g_hdcMem = CreateCompatibleDC(g_hdc);
    g_hbmMem = CreateCompatibleBitmap(g_hdc, g_winW, g_winH);
    SelectObject(g_hdcMem, g_hbmMem);
}

void cleanupRender() {
    if (g_pixelBuffer) {
        free(g_pixelBuffer);
        g_pixelBuffer = NULL;
        g_bufferSize = 0;
    }
    if (g_hdcMem) {
        DeleteDC(g_hdcMem);
        DeleteObject(g_hbmMem);
        g_hdcMem = NULL;
        g_hbmMem = NULL;
    }
    if (g_hdc) {
        ReleaseDC(NULL, g_hdc);
        g_hdc = NULL;
    }
}