#ifndef _BLEADC_H_
#define _BLEADC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Sensor Data */
/* e761d2af-1c15-4fa7-af80-b5729002b340 */
static const ble_uuid128_t gatt_svr_svc_adc_uuid =
        BLE_UUID128_INIT(0x40, 0xb3, 0x20, 0x90, 0x72, 0xb5, 0x80, 0xaf,
                         0xa7, 0x4f, 0x15, 0x1c, 0xaf, 0xd2, 0x61, 0xe7);
#define ADC_SNS_TYPE          0xDEAD
#define ADC_SNS_STRING "eTape Liquid Level Sensor"
#define ADC_SNS_VAL           0xBEAD
extern uint16_t gatt_adc_val; 

#ifdef __cplusplus
}
#endif

#endif /* _BLEADC_H_*/
