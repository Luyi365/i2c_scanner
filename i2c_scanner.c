/**
 * @file i2c_scanner.c
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 平台无关的 I2C 总线设备扫描库
 * @date 2026-10-14
 * 
 * @copyright Copyright (c) 2026
 */

#include <stdio.h>
#include <stdarg.h>
#include "i2c_scanner.h"

static i2c_scanner_probe_fn s_probe = NULL;

/** @brief 扫描 [start, end] 并打印命中项，返回命中数量。 */
static uint16_t scan_range(uint16_t start, uint16_t end)
{
    uint16_t count = 0u;
    uint32_t addr  = 0u;   /* 32 位循环变量避免 end == 0x3FF 时溢出 */

    for (addr = start; addr <= end; addr++)
    {
        uint16_t cur = (uint16_t)addr;

        if (s_probe(cur))
        {
            printf("  I2C device found at 0x%03X\n", (unsigned int)cur);
            count++;
        }
    }

    return count;
}

void i2c_scanner_init(i2c_scanner_probe_fn probe)
{
    s_probe = probe;
}

uint16_t i2c_scanner_scan(void)
{
    uint16_t total = 0u;

    if (s_probe == NULL)
    {
        return 0u;
    }

    printf("I2C scan: 7-bit addresses\n");
    total += scan_range(I2C_SCANNER_ADDR_MIN_7BIT, I2C_SCANNER_ADDR_MAX_7BIT);

    /* 10 位段从 7 位上限之上起扫，跳过重叠区间以免重复计数 */
    printf("I2C scan: 10-bit addresses\n");
    total += scan_range(I2C_SCANNER_ADDR_MAX_7BIT + 1u, I2C_SCANNER_ADDR_MAX_10BIT);

    printf("I2C scan done: %u device(s) found.\n", (unsigned int)total);

    return total;
}

uint16_t i2c_scanner_verify(uint16_t count, ...)
{
    va_list  ap;
    uint16_t missing = 0u;
    uint16_t i       = 0u;

    if (s_probe == NULL)
    {
        return 0u;
    }

    va_start(ap, count);

    for (i = 0u; i < count; i++)
    {
        /* 可变参数中 uint16_t 被提升为 int */
        uint16_t addr = (uint16_t)va_arg(ap, int);

        if (!s_probe(addr))
        {
            missing = addr;
            break;
        }
    }

    va_end(ap);

    return missing;
}
