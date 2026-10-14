/**
 * @file    i2c_scanner.h
 * @author Luyi365 (luyi.hub@foxmail.com)
 * @brief 平台无关的 I2C 总线设备扫描库
 * @date 2026-10-14
 *
 * @copyright Copyright (c) 2026
 *
 * @note
 * 平台相关的 I2C 探测操作通过 probe 回调注入，扫描细节经 printf 输出，
 * 需在目标平台将 printf 重定向到串口等。
 */

#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 7 位地址取值范围。保留地址由 I2C 总线规范规定：
 *   0x00~0x07  广播/CBUS/其它总线格式/Hs-mode 主机码
 *   0x78~0x7F  10 位寻址前导与保留
 * 故通用可分配区间为 0x08~0x77，为扫描默认范围。设备若占用保留地址，可在包含本头文件前定义同名宏覆盖。
 */
#ifndef I2C_SCANNER_ADDR_MIN_7BIT
#define I2C_SCANNER_ADDR_MIN_7BIT   0x08u
#endif
#ifndef I2C_SCANNER_ADDR_MAX_7BIT
#define I2C_SCANNER_ADDR_MAX_7BIT   0x77u
#endif
#ifndef I2C_SCANNER_ADDR_MIN_10BIT
#define I2C_SCANNER_ADDR_MIN_10BIT  0x000u
#endif
#ifndef I2C_SCANNER_ADDR_MAX_10BIT
#define I2C_SCANNER_ADDR_MAX_10BIT  0x3FFu
#endif

/**
 * @brief 平台 I2C 探测回调：向 addr 探测，设备应答(ACK)返回 true。
 *
 * @param[in] addr  7 位(0x01~0x7F)或 10 位(0x000~0x3FF)从机地址。
 */
typedef bool (*i2c_scanner_probe_fn)(uint16_t addr);

/**
 * @brief 注册平台 I2C 探测接口，须在扫描前调用。
 *
 * @param[in] probe  探测回调，不可为 NULL。
 */
void i2c_scanner_init(i2c_scanner_probe_fn probe);

/**
 * @brief 扫描全部可分配地址，命中项经 printf 打印。
 *
 * 7 位段扫 [I2C_SCANNER_ADDR_MIN_7BIT, I2C_SCANNER_ADDR_MAX_7BIT]，10 位段
 * 扫 7 位上限之上至 I2C_SCANNER_ADDR_MAX_10BIT，跳过重叠区间使每个设备只计一次。
 *
 * @retval 设备总数；未调用 i2c_scanner_init() 时返回 0。
 */
uint16_t i2c_scanner_scan(void);

/**
 * @brief 验证一组已知设备是否都在总线上。
 *
 * 例：i2c_scanner_verify(3, 0x3C, 0x50, 0x68)。地址 7 位、10 位可混填。
 *
 * @param[in] count  期望地址个数，为 0 视为空列表。
 * @param[in] ...    count 个期望地址。
 * @retval 0   全部找到，或 count 为 0，或未调用 i2c_scanner_init()。
 * @retval !0  按传入顺序第一个未找到的地址。
 */
uint16_t i2c_scanner_verify(uint16_t count, ...);

#ifdef __cplusplus
}
#endif

#endif /* I2C_SCANNER_H */
