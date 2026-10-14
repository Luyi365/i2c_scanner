# i2c_scanner

平台无关的 I2C 设备扫描库，4 个 API 扫描总线上的设备及地址。灵感来源于 Nordic 的 TWI scanner 示例。

## API

```c
// 探测回调类型：向 addr 探测，设备应答(ACK)返回 true
typedef bool (*i2c_scanner_probe_fn)(uint16_t addr);

// 注册平台 I2C 探测接口，须在扫描前调用
void i2c_scanner_init(i2c_scanner_probe_fn probe);

// 扫描全部可分配地址，命中项经 printf 打印，返回设备总数
// 未先 init 时返回 0
uint16_t i2c_scanner_scan(void);

// 验证一组已知设备是否都在：首参为数量，其后为期望地址（7/10 位可混填）
// 全部找到返回 0；否则返回按传入顺序第一个未找到的地址
uint16_t i2c_scanner_verify(uint16_t count, ...);
```

两种场景：

- **不知道有哪些设备** → `i2c_scanner_scan()`，全扫一遍。
- **验证已知设备是否都在** → `i2c_scanner_verify(n, addr1, addr2, ...)`。

## 平台移植

**移植到新平台，唯一要做的就是实现 `probe` 回调。** 它向给定地址发起一次最小 I2C 事务：
从机应答(ACK)返回 `true`，无应答 / 超时 / 出错返回 `false`。通常用「读 1 字节」或
「只含地址的空写事务」实现。

各平台典型实现：

| 平台       | probe 内部大致调用                                                    |
| ---------- | --------------------------------------------------------------------- |
| Arduino    | `Wire.beginTransmission(addr); return Wire.endTransmission() == 0;`   |
| STM32 HAL  | `HAL_I2C_IsDeviceReady(&hi2c, addr << 1, 1, 2) == HAL_OK`              |
| ESP-IDF    | 组一个只含地址的 write 事务，看 `i2c_master_cmd_begin` 是否返回 `ESP_OK` |
| Linux      | `ioctl(fd, I2C_SLAVE, addr)` 后尝试一次读/写                           |
| Nordic nRF | `nrf_drv_twi_rx(&m_twi, addr, &b, 1) == NRF_SUCCESS`（见示例）         |

两点注意：

- **地址对齐**：库传入的是 7 位地址。STM32 HAL 等用 8 位地址（含读写位），需在 probe 内
  左移一位（`addr << 1`）。
- **10 位地址**：帧封装（首字节 `11110xx` 前导 + 次字节）同样在 probe 内按平台 API 处理。

## 示例

- `examples/generic_example.c`：纯标准 C，用模拟设备表运行，无需硬件，可在 PC 上验证。
- `examples/nrf52_example.c`：Nordic nRF5 SDK 适配示例。

PC 上编译运行：

```sh
cc -I. examples/generic_example.c i2c_scanner.c -o scanner_demo
./scanner_demo
```
