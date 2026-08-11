// Types.h
#pragma once
#include <windows.h>

struct Texture {
    short width;
    short height;
    int* data;

    Texture() : width(0), height(0), data(NULL) {}

    void setData(int* newData, short w, short h);
    void freeData();
};

struct Object {
    short x, y;
    short width, height;
    Texture texture;
};