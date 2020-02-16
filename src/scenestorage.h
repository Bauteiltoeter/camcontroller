#define SCENE_COUNT 32

void scenestorage_saveTo(uint8_t index);
void scenestorage_load(uint8_t index);

void scenestorage_delete(uint8_t index);
uint8_t scenestorage_hasData(uint8_t index);
uint8_t scenestorage_getTiming(uint8_t index);
uint8_t scenestorage_getCrossfade(uint8_t index);
void scenestorage_setTiming(uint8_t index, uint8_t timing);
void scenestorage_setCrossfade(uint8_t index, uint8_t crossfade);