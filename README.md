# robotics traveling van inverted pendulum robot

everything needed to power an inverted pendulum robot with four motors, including the code, parts lists, CAD model, and instructions.

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

## building

- first, fetch all submodules with `git submodule update --init --recursive`

then build with CMAKE (or with `scripts/buildFresh.sh`):

```

    mdkir build
    cd build
    cmake ..
    make

```

# pins

## AS5600 magnetic encoder

| **pin** | **connection**              | **color** |
| ------- | --------------------------- | --------- |
| VCC     | 3.3v on pico                | red       |
| OUT     | --                          | --        |
| GND     | GND on pico                 | black     |
| DIR     | HIGH or LOW (currently low) | black     |
| SCL     | 5 on pico                   | blue      |
| SDA     | 4 on pico                   | white     |
| GPO     | --                          | --        |

## GM3865-520 DC motor

| **pin** | **connection**         |
| ------- | ---------------------- |
| M+      | 12v+ from motor driver |
| M-      | 12v- from motor driver |
| GND     | 3.3v GND from pico     |
| VCC     | 3.3v from pico         |
| A       | 2 on pico              |
| B       | 3 on pico              |

on new motors:

- M+: white
- M-: blue
- GND: green
- VCC: yellow
- B: black
- A: red

## DRV8871 motor driver

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

| IN1 | IN2 |  OUT1  |  OUT2  | DESCRIPTION |
| :-: | :-: | :----: | :----: | :---------- |
|  0  |  0  | high-z | high-z | coast       |
|  0  |  1  |   L    |   H    | reverse     |
|  1  |  0  |   H    |   L    | forward     |
|  1  |  1  |   L    |   L    | brake       |

# submodules

- [dancesWithMachines/dwm_pico_as45600](https://github.com/dancesWithMachines/dwm_pico_as5600)
- [raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [jtof-dev/pico-pid-library](https://github.com/jtof-dev/pico-pid-library)

# to-do

- [ ] add code to self-correct robot when far away from origin
- [ ] change code to use a digital encoder instead of a potentiometer
- [ ] migrate code to use a different board
- [ ] implement touch screen and UI

```

```
