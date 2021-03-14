#ifndef TALLY_H__
#define TALLY_H__

typedef enum 
{
    rf_ok,
    rf_error_neko,
    rf_error_com,
    rf_wait

} tally_rf_status_t;

void tally_init(void);
void tally_tick(void);
void tally_setAuto(void);
void tally_setAcleauto(void);
void tally_setYellow(void);
void tally_setBlue(void);
//tally_mode_t tally_getMode();
#endif