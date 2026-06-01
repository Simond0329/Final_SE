#include "adc_driver.h"
 
void adc_driver_init(adc_unit_t unit, adc_oneshot_unit_handle_t *handle) {
    adc_oneshot_unit_init_cfg_t cfg = { .unit_id = unit, .ulp_mode = ADC_ULP_MODE_DISABLE };
    adc_oneshot_new_unit(&cfg, handle);
}
void adc_driver_add_channel(adc_oneshot_unit_handle_t handle,
                            adc_channel_t ch, adc_atten_t atten) {
    adc_oneshot_chan_cfg_t cfg = { .atten = atten, .bitwidth = ADC_BITWIDTH_12 };
    adc_oneshot_config_channel(handle, ch, &cfg);
}
float adc_driver_read_raw(adc_oneshot_unit_handle_t handle, adc_channel_t ch) {
    int raw = 0; adc_oneshot_read(handle, ch, &raw); return raw;
}