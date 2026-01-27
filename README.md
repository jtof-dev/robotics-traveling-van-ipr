# robotics traveling van inverted pendulum robot

the code needed to power an inverted pendulum robot with four (or two) motors. currently written for an arduino uno, but will later be rewritten for a difference board (unsure on which one yet)

- this code is made much simpler thanks to [this PID controller library](https://github.com/br3ttb/Arduino-PID-Library)
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
┌─────────────────────────┐          ┌────────┐           ┌────────────────────────────┐
│                         │          │        │           │                            │
│ send specific speed PWM ├─────────►│ loop() ┼──────────►│ read potentiometer values  │
│     value to motors     │          │        │           │                            │
│                         │          └────────┘           └──────────────────┬─────────┘
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

## pin configuration

- potentiometer: A0
- encoder output A: D2
- encoder output B: D3

- left motor 1: D5 (PWM)
- left motor 1: D6 (PWM)

# to-do

- [ ] add code to self-correct robot when far away from origin
- [ ] change code to use a digital encoder instead of a potentiometer
- [ ] migrate code to use a different board
- [ ] implement touch screen and UI
