# general

- [x] figure out how to flash code to [the generic pi pico](https://a.co/d/8O0cdaL)
  - [x] make template files (cmake like Kyle had for robot 2?)
- [ ] find or write display driver for [the touchscreen](https://a.co/d/4rDwqDx)
- [ ] decide on a gui library to use, probably [lvgl](https://lvgl.io/)
  - [ ] implement a very simple touchscreen ui

# robot 1

- [ ] tweak code to run on the pico
  - rewrite in C++, change pin variables, add cmake stuff
- [ ] write code to make the magnetic encoder work
- [ ] work through PID math to figure out how to convert our inputted angle to output a speed

# robot 2

- we need to decide on which sensor to use
- [x] implement the new sensor in code
  - probably the ToF sensor, but could also be the camera
- [ ] work through physics equations to check if the code appears roughly correct
