# HomeKit ESP Components

ESPHome 的 HomeKit 外部组件库，支持 ESP32 和 ESP8266。

## 使用方法

在 ESPHome 配置中添加 `external_components`：

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/txhtxh11/homekit
      path: homekit-esp32    # ESP32 选此路径
    refresh: 7d
    components: [homekit]

  - source:
      type: git
      url: https://github.com/txhtxh11/homekit
      path: homekit-esp8266  # ESP8266 选此路径
    refresh: 7d
    components: [homekit]
```

两个 source 可以同时保留，ESPHome 会根据开发板自动编译对应的组件。

## 组件说明

| 路径 | 平台 | 组件名 | 说明 |
|------|------|--------|------|
| `homekit-esp32` | ESP32 | `homekit` | HomeKit Accessory，支持灯、锁、传感器、风扇、开关、空调 |
| `homekit-esp8266` | ESP8266 | `homekit` | HomeKit Bridge，支持灯、风扇、开关、传感器、二进制传感器 |

## 配置示例

### ESP32

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/txhtxh11/homekit
      path: homekit-esp32
    refresh: 7d
    components: [homekit]

esp32:
  board: esp32dev
  framework:
    type: esp-idf

homekit:
  light:
    - id: my_light
```

### ESP8266

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/txhtxh11/homekit
      path: homekit-esp8266
    refresh: 7d
    components: [homekit]

esp8266:
  board: esp01_1m

homekit:
  setup_code: "111-11-111"
  switches:
    - relay_switch
```

## 许可证

Apache-2.0
