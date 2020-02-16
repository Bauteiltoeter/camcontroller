#include "globals.h"

imagescan_data_t imageScan_data[MOVING_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
cameotri_data_t cameotri_data[CAMEOTRI_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
cameoflood_data_t cameoflood_data[CAMEOFLOOD_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
citycolor_data_t citycolor_data[CITYCOLOR_FIXTURE_COUNT];	//!< global array to store the configuration data for each fixture
uint8_t active_fixture; 		//!< global variable that contains the number of the currently manipulated fixture
uint8_t active_rotary_funtion;
