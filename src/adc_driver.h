#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
 
void adc_driver_init(adc_unit_t unit, adc_oneshot_unit_handle_t *handle);
void adc_driver_add_channel(adc_oneshot_unit_handle_t handle, adc_channel_t ch, adc_atten_t atten);
float adc_driver_read_raw(adc_oneshot_unit_handle_t handle, adc_channel_t ch);
#endif
 
