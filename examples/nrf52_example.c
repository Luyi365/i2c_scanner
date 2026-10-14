/**
 * @file    nrf52_example.c
 * @brief   nRF5 SDK 平台适配示例：把 nRF TWI 驱动包装成 i2c_scanner 的 probe 回调。
 *
 * 扫描细节由库通过 printf 输出，需在工程中把 printf 重定向到串口 / RTT。
 * 编译需把 nRF SDK 组件（nrf_drv_twi 等）加入工程。
 */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"

#include "i2c_scanner.h"

#define ARDUINO_SCL_PIN     24
#define ARDUINO_SDA_PIN     25

#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#else
#define TWI_INSTANCE_ID     0
#endif

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

static void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
        .scl                = ARDUINO_SCL_PIN,
        .sda                = ARDUINO_SDA_PIN,
        .frequency          = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

/** @brief 向地址读 1 字节，收到应答即认为该地址有设备。 */
static bool nrf_twi_probe(uint16_t addr)
{
    uint8_t    sample_data;
    ret_code_t err_code;

    err_code = nrf_drv_twi_rx(&m_twi, (uint8_t)addr, &sample_data, sizeof(sample_data));
    return (err_code == NRF_SUCCESS);
}

int main(void)
{
    uint16_t count;

    twi_init();

    i2c_scanner_init(nrf_twi_probe);

    count = i2c_scanner_scan();
    (void)count;

    /* 验证已知设备：uint16_t missing = i2c_scanner_verify(2, 0x3C, 0x50); */

    while (true)
    {
    }
}
