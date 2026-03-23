# robotics traveling van inverted pendulum robot

the code needed to power an inverted pendulum robot with four motors. written for a generic RP2040 board

- I referenced [Ian Carey's inverted pendulum code](https://gist.github.com/careyi3/d087f707b33c665915bd611e5514a355) for help with organizing the code, thank you!!

```ascii flowchart
      ┌───────────────────┐          ┌─────────┐          ┌──────────────────┐
      │                   │          │         │          │                  │
      │ send brake signal │◄─────────┤ setup() ├─────────►│ initialize pins  │
      │     to motors     │          │         │          │                  │
      │                   │          └────┬────┘          └──────────────────┘
      └───────────────────┘               │
                                          │
                                          │
                                          │
                                          │
                                          ▼
┌─────────────────────────┐          ┌────────┐           ┌───────────────────────────────┐
│                         │          │        │           │                               │
│ send specific speed PWM ├─────────►│ loop() ┼──────────►│ read magnetic encoder values  │
│     value to motors     │          │        │           │                               │
│                         │          └────────┘           └──────────────────┬────────────┘
└─────────────────────────┘                                                  │
       ▲                                                                     │
       │                                                                     │
       │                                                                     │
       │                                                                     │
       │                                                                     │
       │     ┌─────────────────────┐            ┌─────────────────────┐      │
       │     │                     │            │                     │      │
       └─────┤ adjust output value │◄───────────┤ run PID calculation │◄─────┘
             │                     │            │                     │
             └─────────────────────┘            └─────────────────────┘
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

| **pin** | **connection**              |
| ------- | --------------------------- |
| VCC     | 3.3v on pico                |
| OUT     | HIGH or LOW (currently low) |
| GND     | GND on pico                 |
| DIR     |                             |
| SCL     | 5 on pico                   |
| SDA     | 4 on pico                   |
| GPO     |                             |

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
