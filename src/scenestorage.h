#define SCENE_COUNT 27

#include "globals.h"

typedef struct {
    imagescan_data_t imagescan[MOVING_FIXTURE_COUNT];
    cameotri_data_t cameotri[CAMEOTRI_FIXTURE_COUNT];
    cameoflood_data_t cameoflood[CAMEOFLOOD_FIXTURE_COUNT];
    citycolor_data_t citycolor[CITYCOLOR_FIXTURE_COUNT];

    uint8_t scenInactive;
    uint8_t crossfade;
    uint8_t timing;
} scene_t;

void scenestorage_saveTo(uint8_t index);
void scenestorage_load(uint8_t index);

void scenestorage_delete(uint8_t index);
uint8_t scenestorage_hasData(uint8_t index);
uint8_t scenestorage_getTiming(uint8_t index);
uint8_t scenestorage_getCrossfade(uint8_t index);
void scenestorage_setTiming(uint8_t index, uint8_t timing);
void scenestorage_setCrossfade(uint8_t index, uint8_t crossfade);
void scenestorage_getScene(uint8_t index, scene_t* scene);