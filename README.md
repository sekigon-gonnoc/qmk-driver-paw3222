# PAW3222 Optical Sensor Driver Module for QMK

This module provides QMK support for the PAW3222 optical sensor, commonly used in mice and trackballs. It implements the pointing device interface for QMK, allowing you to use this sensor in your keyboard projects.

## Features

- Full integration with QMK's pointing device framework
- Configurable CPI (Counts Per Inch) settings
- Simple SPI-like communication protocol
- Easy to integrate with custom keyboard PCBs

## Installation

1. Copy paw3222.c/h into your keyboard directory

2. Add the following to your `rules.mk`:
   ```
   SRC += paw3222.c
   ```

## Pin Configuration

You must define the following pins in your `config.h`:

```c
// Option 1: Using specific PAW3222 pin definitions
#define PAW3222_SCLK_PIN B0
#define PAW3222_SDIO_PIN B1
#define PAW3222_CS_PIN   B2

// Option 2: Using generic pointing device pin definitions
#define POINTING_DEVICE_SCLK_PIN B0
#define POINTING_DEVICE_SDIO_PIN B1
#define POINTING_DEVICE_CS_PIN   B2
```

## Usage

1. Enable pointing device in `keyboard.json`:

```json
{
    "features": {
        "pointing_device": true
    }
}
```

2. Add the PAW3222 driver to your pointing device configuration in `config.h`:

```c
#define POINTING_DEVICE_DRIVER paw3222_pointing_device_driver
```

## Credits

- Original implementation by Gompa (for PAW3204)
- Modified by sekigon-gonnoc (adaptation for PAW3222)

## License

This driver is released under the GPL-2.0 license, as per QMK's licensing.

---

# PAW3222 光学センサー QMKドライバモジュール

このモジュールは、マウスやトラックボールでよく使用されるPAW3222光学センサー用のQMKサポートを提供します。QMKのポインティングデバイスインターフェースを実装しており、このセンサーをキーボードプロジェクトで使用することができます。

## 特徴

- QMKのポインティングデバイスフレームワークと完全に統合
- 設定可能なCPI（Counts Per Inch）値
- シンプルなSPI風通信プロトコル
- カスタムキーボードPCBへの容易な統合

## インストール方法

1. paw3222.c/hファイルをキーボードディレクトリにコピーします

2. `rules.mk`ファイルに以下を追加します：
   ```
   SRC += paw3222.c
   ```

## ピン設定

`config.h`ファイルで以下のピンを定義する必要があります：

```c
// オプション1: PAW3222専用ピン定義を使用
#define PAW3222_SCLK_PIN B0
#define PAW3222_SDIO_PIN B1
#define PAW3222_CS_PIN   B2

// オプション2: 一般的なポインティングデバイスピン定義を使用
#define POINTING_DEVICE_SCLK_PIN B0
#define POINTING_DEVICE_SDIO_PIN B1
#define POINTING_DEVICE_CS_PIN   B2
```

## 使用方法

1. `keyboard.json`でポインティングデバイスを有効にします：

```json
{
    "features": {
        "pointing_device": true
    }
}
```

2. `config.h`にPAW3222ドライバーを設定します：

```c
#define POINTING_DEVICE_DRIVER paw3222_pointing_device_driver
```

## クレジット

- オリジナルの実装：Gompa（PAW3204向け）
- 修正：sekigon-gonnoc（PAW3222向けに改変）

## ライセンス

このドライバはQMKのライセンスに従い、GPL-2.0ライセンスの下でリリースされています。
