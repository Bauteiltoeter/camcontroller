#ifndef TALLY_H__
#define TALLY_H__

typedef enum 
{
    tally_auto,
    tally_blue,
    tally_yellow
} tally_mode_t;

typedef enum 
{
    rf_ok,
    rf_error_neko,
    rf_error_com,
    rf_wait

} tally_rf_status_t;

void tally_init();
void tally_tick();
void tally_setAcleauto();
void tally_setYellow();
void tally_setBlue();
tally_mode_t tally_getMode();
#endif