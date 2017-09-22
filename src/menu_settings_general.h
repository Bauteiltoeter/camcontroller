#ifndef __MENU_SETTINGS_GENERAL_H
#define __MENU_SETTINGS_GENERAL_H

#include "rotary.h"

void setup_menu_next(void);
void setup_menu_prev(void);
void setup_menu_enter(void);
void setup_menu_show(void);
void setup_reset(void);

extern rotary_config_t rotary_menu_settings_general;
#endif
