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

then build with CMAKE:

```
    mdkir build
    cd build
    cmake ..
    make
```

# notes to self

## DRV8871 motor driver

| IN1 | IN2 |  OUT1  |  OUT2  | DESCRIPTION |
| :-: | :-: | :----: | :----: | :---------- |
|  0  |  0  | high-z | high-z | coast       |
|  0  |  1  |   L    |   H    | reverse     |
|  1  |  0  |   H    |   L    | forward     |
|  1  |  1  |   L    |   L    | brake       |

## GM3865-520 dc motor encoder

- the encoder outputs two data signals phased 100deg apart

# submodules

- [dancesWithMachines/dwm_pico_as45600](https://github.com/dancesWithMachines/dwm_pico_as5600)
- [raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [jtof-dev/pico-pid-library](https://github.com/jtof-dev/pico-pid-library)

# to-do

- [ ] add code to self-correct robot when far away from origin
- [ ] change code to use a digital encoder instead of a potentiometer
- [ ] migrate code to use a different board
- [ ] implement touch screen and UI
