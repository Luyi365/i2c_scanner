/**
 * @file    generic_example.c
 * @brief   纯标准 C 示例：用模拟设备表充当 I2C 总线，无需硬件即可运行。
 *
 * 真实平台请把 simulated_probe 换成实际的 I2C 探测操作。
 * 编译：cc -I.. generic_example.c ../i2c_scanner.c -o scanner_demo
 */

#include <stdio.h>
#include "i2c_scanner.h"

/* 模拟设备：0x3C OLED、0x50 EEPROM、0x68 RTC/IMU */
static const uint16_t k_simulated_devices[] = { 0x3C, 0x50, 0x68 };

static bool simulated_probe(uint16_t addr)
{
    size_t i;
    for (i = 0; i < sizeof(k_simulated_devices) / sizeof(k_simulated_devices[0]); i++)
    {
        if (k_simulated_devices[i] == addr)
        {
            return true;
        }
    }
    return false;
}

int main(void)
{
    uint16_t count;
    uint16_t missing;

    i2c_scanner_init(simulated_probe);

    count = i2c_scanner_scan();
    printf("scan() returned %u\n\n", (unsigned int)count);

    missing = i2c_scanner_verify(3, 0x3C, 0x50, 0x68);
    if (missing == 0u)
    {
        printf("verify: all expected devices present.\n");
    }
    else
    {
        printf("verify: device 0x%03X missing.\n", (unsigned int)missing);
    }

    missing = i2c_scanner_verify(3, 0x3C, 0x20, 0x68);
    printf("verify with 0x20: returned 0x%03X\n", (unsigned int)missing);

    return 0;
}
