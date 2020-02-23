#include <avr/eeprom.h>
#include "globals.h"
#include "scenestorage.h"
#include <string.h>


scene_t scens[SCENE_COUNT] EEMEM;

scene_t currentScene;


void scenestorage_saveTo(uint8_t index)
{

    memcpy(&currentScene.imagescan, &imageScan_data, sizeof(imagescan_data_t)*MOVING_FIXTURE_COUNT);
    memcpy(&currentScene.cameotri, &cameotri_data, sizeof(cameotri_data_t)*CAMEOTRI_FIXTURE_COUNT);
    memcpy(&currentScene.cameoflood, &cameoflood_data, sizeof(cameoflood_data_t)*CAMEOFLOOD_FIXTURE_COUNT );
    memcpy(&currentScene.citycolor, &citycolor_data, sizeof(citycolor_data_t)*CITYCOLOR_FIXTURE_COUNT);


    eeprom_write_block(&currentScene, &scens[index],sizeof(scene_t));


    eeprom_write_byte(&scens[index].scenInactive,0);
}


void scenestorage_load(uint8_t index)
{

    eeprom_read_block(&currentScene, &scens[index],sizeof(scene_t));

    uint8_t backup_imagescan_addr[MOVING_FIXTURE_COUNT];
    uint8_t backup_cameotri_addr[CAMEOTRI_FIXTURE_COUNT];
    uint8_t backup_cameoflood_addr[CAMEOFLOOD_FIXTURE_COUNT];
    uint8_t backup_citycolor_addr[CITYCOLOR_FIXTURE_COUNT];

    for(uint8_t i=0; i < MOVING_FIXTURE_COUNT; i++)
        backup_imagescan_addr[i] = imageScan_data[i].dmx_addr;
    for(uint8_t i=0; i < CAMEOTRI_FIXTURE_COUNT; i++)
        backup_cameotri_addr[i] = cameotri_data[i].dmx_addr;
    for(uint8_t i=0; i < CAMEOFLOOD_FIXTURE_COUNT; i++)
        backup_cameoflood_addr[i] = cameoflood_data[i].dmx_addr;
    for(uint8_t i=0; i < CITYCOLOR_FIXTURE_COUNT; i++)
        backup_citycolor_addr[i] = citycolor_data[i].dmx_addr;

    memcpy(&imageScan_data, &currentScene.imagescan, sizeof(imagescan_data_t)*MOVING_FIXTURE_COUNT);
    memcpy(&cameotri_data, &currentScene.cameotri, sizeof(cameotri_data_t)*CAMEOTRI_FIXTURE_COUNT);
    memcpy(&cameoflood_data, &currentScene.cameoflood, sizeof(cameoflood_data_t)*CAMEOFLOOD_FIXTURE_COUNT );
    memcpy(&citycolor_data, &currentScene.citycolor, sizeof(citycolor_data_t)*CITYCOLOR_FIXTURE_COUNT);


    for(uint8_t i=0; i < MOVING_FIXTURE_COUNT; i++)
        imageScan_data[i].dmx_addr = backup_imagescan_addr[i];
    for(uint8_t i=0; i < CAMEOTRI_FIXTURE_COUNT; i++)
        cameotri_data[i].dmx_addr = backup_cameotri_addr[i];
    for(uint8_t i=0; i < CAMEOFLOOD_FIXTURE_COUNT; i++)
        cameoflood_data[i].dmx_addr = backup_cameoflood_addr[i];
    for(uint8_t i=0; i < CITYCOLOR_FIXTURE_COUNT; i++)
        citycolor_data[i].dmx_addr = backup_citycolor_addr[i];
}


void scenestorage_delete(uint8_t index)
{
    eeprom_write_byte(&scens[index].scenInactive,1);
}

uint8_t scenestorage_hasData(uint8_t index)
{
    if(eeprom_read_byte(&scens[index].scenInactive))
        return 0;
    return 1;
}


uint16_t scenestorage_getTiming(uint8_t index)
{
    return eeprom_read_word(&scens[index].timing);
}

uint16_t scenestorage_getCrossfade(uint8_t index)
{

    return eeprom_read_word(&scens[index].crossfade);
}


void scenestorage_setTiming(uint8_t index, uint16_t timing)
{
    eeprom_write_word(&scens[index].timing,timing);
}

void scenestorage_setCrossfade(uint8_t index, uint16_t crossfade)
{
    eeprom_write_word(&scens[index].crossfade,crossfade);
}

void scenestorage_getScene(uint8_t index, scene_t* scene)
{
    eeprom_read_block(scene, &scens[index],sizeof(scene_t));
}