# robotics traveling van inverted pendulum robot

NOTE: this is an early rewrite to use `arduino-cli` and run a touchscreen, but the code is messy and has a fair amount of technical debt. while this code may work for now, it will be a pain to work on

everything needed to power an inverted pendulum robot (ipr) with four motors, including the code, parts lists, CAD model, and instructions. when you're done, check out our [ball and beam balance robot](https://github.com/jtof-dev/robotics-traveling-van-bbb) and our [website](https://sce.nau.edu/capstone/projects/EE/2026/RoboVan/)

```ascii flowchart
                                ┌─────────┐
                                │         │
                                │ initial │
                                │  boot   │
                                │         │
                                └────┬────┘
                                     │
          ┌───────────────────┐      │      ┌─────────────┐
          │                   │      │      │             │
          │ send brake signal │◄─────┼─────►│ set up pins │
          │     to motors     │      │      │             │
          │                   │      │      └─────────────┘
          └───────────────────┘      │
                                     ▼
                                ┌────────┐
                                │        │
                                │ main() │
                                │        │
                                └────┬───┘
                                     │
                                     │
                                     │
                                     ▼
   ┌─────────────────┐      ┌──────────────────┐      ┌────────────────┐
   │                 │      │                  │      │                │
   │ send speed PWM  ├─────►│ write a frame to ├─────►│ read magnetic  │
   │ value to motors │      │ the touchscreen  │      │ encoder values │
   │                 │      │                  │      │                │
   └─────────────────┘      └──────────────────┘      └─────────────┬──┘
      ▲                                                             │
      │                                                             │
      │                                                             │
      │                                                             │
      │                                                             │
      │     ┌─────────────────────┐      ┌─────────────────────┐    │
      │     │                     │      │                     │    │
      └─────┤ adjust output value │◄─────┤ run PID calculation │◄───┘
            │                     │      │                     │
            └─────────────────────┘      └─────────────────────┘
```

# notes

## directory structure

- **datasheets/**: datasheets for the specific parts used in this robot
- **lib/**: contains all submodules
- **scripts/**: all scripts commonly used while writing code
- **sims/**: any python scripts used to model the behavior of the inverted pendulum
- **src/**: our own written code, split into a `configuration.hpp` and `main.cpp`, along with some extra functions split off into individual files


# software

because there are no good screen libraries written for the pi pico, we are instead using one written for arduino, and then compiling a mixture of arduino and pico code

## building

- first, fetch all submodules with `git submodule update --init --recursive` or delete and re-clone all submodules with `scripts/submoduleSetup.sh`

- second, install two `arduino-cli` (and `arduino-cli` if needed) dependencies that get used in `CMakeLists.txt`:

```bash
arduino-cli core update-index --additional-urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
arduino-cli core install rp2040:rp2040 --additional-urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```

then build with CMAKE (or with `scripts/buildFresh.sh`):

```bash
    mdkir build
    cd build
    cmake ..
    make
```

(note: updating configurations in `src/configuration.hpp` does not trigger a proper re-build, so only running `make` will often not be enough. instead re-run all of the commands or use `scripts/buildFresh.sh`)

## uploading

- to flash the compiled `.uf2`, either reboot the pico into BOOTSEL mode (hold the BOOTSEL button and plug in the pico), or use `picotool` (or `scripts/upload.sh`):

```bash
picotool load -f -x flash.uf2
```

## submodules

- [bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_espi)
- [dancesWithMachines/dwm_pico_as45600](https://github.com/dancesWithMachines/dwm_pico_as5600)
- [jtof-dev/pico-pid-library](https://github.com/jtof-dev/pico-pid-library)

## sims

- located in `sims/`, and models the expected behavior of the robot. theoretically, it should produce similar gain values to what will be used on the physical robot
- the python environment is managed with `uv`:

```bash
uv sync
uv run main.py
```

or, install `matplotlib` and run normally

# hardware

![circuit diagram](./datasheets/ipr_circuit_diagram.webp)

- for CAD model assembly and instructions, check out [`CAD/`](https://github.com/jtof-dev/robotics-traveling-van-ipr/tree/main/CAD)
- for PCB models, check out [`PCB/`](https://github.com/jtof-dev/robotics-traveling-van-ipr/tree/main/PCB)

## parts list

- [AS5600](./datasheets/AS5600_datasheet.pdf) magnetic encoder
- [DRV8871](./datasheets/DRV8871_datasheet.pdf) motor driver
- [GM3865-520](https://web.archive.org/web/20260407004016/https://www.amazon.com/dp/B0F1N9VZSK) dc motor
- raspberry pi pico or [RP2040](./datasheets/RP2040_datasheet.pdf) compatible board
- touchscreen with [ST7796S](./datasheets/ST7796S_datasheet.pdf) display driver

### generic parts

- 4-pack of 3.3V batteries (in series for 13V total), matching BMS, and charger
  - be sure to match max voltage, current, and battery chemistry type between all three components
- step-down voltage converter to 3.3V
- 470uF 25V electrolytic capacitor

## pin configuration

### AS5600 magnetic encoder

| **pin** | **connection**              | **color** |
| ------- | --------------------------- | --------- |
| VCC     | 3.3v on pico                | red       |
| OUT     | --                          | --        |
| GND     | GND on pico                 | black     |
| DIR     | HIGH or LOW (currently low) | black     |
| SCL     | 5 on pico                   | blue      |
| SDA     | 4 on pico                   | white     |
| GPO     | --                          | --        |

### DRV8871 motor driver

| **pin** | **connection**                |
| ------- | ----------------------------- |
| motor+  | 12v+ to motor                 |
| motor-  | 12v- to motor                 |
| power+  | 12v+ from PSU                 |
| power-  | 12v- from PSU                 |
| IN1     | 6 on pico                     |
| IN2     | 7 on pico                     |
| VM      | passthrough 12v (don't use!!) |
| GND     | passthrough ground            |

| **IN1** | **IN2** | **OUT1** | **OUT2** | **description** |
| :-----: | :-----: | :------: | :------: | :-------------- |
|    0    |    0    |  high-z  |  high-z  | coast           |
|    0    |    1    |    L     |    H     | reverse         |
|    1    |    0    |    H     |    L     | forward         |
|    1    |    1    |    L     |    L     | brake           |

### GM3865-520 DC motor

| **pin** | **connection**         | **wire color** |
| :------ | :--------------------- | :------------- |
| M+      | 12v+ from motor driver | white          |
| M-      | 12v- from motor driver | blue           |
| GND     | 3.3v GND from pico     | green          |
| VCC     | 3.3v from pico         | yellow         |
| A       | 2 on pico              | red            |
| B       | 3 on pico              | black          |

### ST7796S touchscreen
| display pin | pi pico pin | notes / function | wire color |
| :--- | :--- | :--- | :--- |
| VCC | external 3.3V | power for the display | red |
| GND | common GND | ground | black |
| LCD_CS | GP20 | TFT chip select (`TFT_CS`) | orange |
| LCD_RST | GP21 | TFT reset (`TFT_RST`) | yellow |
| LCD_RS | GP22 | TFT data/command (`TFT_DC`) | green |
| SDI (MOSI) | GP19 | SPI data input (`TFT_MOSI`) | blue |
| SCK | GP18 | SPI clock (`TFT_SCLK`) | purple |
| LED | external 3.3V | backlight power. connect to 3.3V external power, it will overdraw the pico if connected to a data pin | white |
| SDO (MISO) | GP0 | SPI data output (`TFT_MISO`) | gray |
| CTP_SCL | GP27 | I2C1 clock (capacitive touch) | orange |
| CTP_RST | GP10 | touch reset | yellow |
| CTP_SDA | GP26 | I2C1 data (capacitive touch) | green |
| CTP_INT | GP11 | touch interrupt | blue |
| SD_CS | -- | SD card chip select (shares SPI bus with TFT) | -- |

# contributors

### [Andy Babcock](https://github.com/jtof-dev)

- developed the electronics and balancing software for the inverted pendulum robot

### [Kaden Zaremba](https://github.com/kadenisuhhh)

- developed the touchscreen software

### [David Jimenez](https://www.linkedin.com/in/davidkjimenez)

- assisted with the electronics design and assembly

### [Kyle Draper](https://github.com/Kdra-bit)

- assisted with the electronics design and bug-fixing

### [Andres Gonzales](https://github.com/AndresGonzales-hub)

- developed the CAD model and helped with assembly

### [Colin Parsinia](https://www.linkedin.com/in/colin-parsinia-9b55a7176/)

- developed the CAD model and helped with assembly
