#include "Types.h"
#include <cstring>
#include <cstdlib>

void Texture::setData(int* newData, short w, short h) {
    if (newData == NULL || w <= 0 || h <= 0) return;

    if (data) free(data);

    width = w;
    height = h;
    data = (int*)malloc(width * height * sizeof(int));
    memcpy(data, newData, width * height * sizeof(int));
}

void Texture::freeData() {
    if (data) {
        free(data);
        data = NULL;
    }
    width = 0;
    height = 0;
}