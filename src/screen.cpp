#include "screen.hpp"
#include "configuration.hpp"
#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// layout UI variables
const int COL_LEFT = 10;
const int COL_RIGHT = 250;
const int BTN_WIDTH = 210;
const int BTN_HEIGHT = 60;

const int ROW_1 = 30;
const int ROW_2 = 120;

// track & pendulum visualizer math
const int TRACK_Y = 280;
const int TRACK_X_START = 40;
const int TRACK_X_END = 440;
const int TRACK_CENTER = 240;

// 1.5ft total physical track length = 457.2mm (Half is 228.6mm)
const float MAX_PHYSICAL_LIMIT_MM = 228.6f;

int mapCartToPixels(float pos_mm) {
  // clamp visually so it doesn't draw off the screen
  if (pos_mm < -MAX_PHYSICAL_LIMIT_MM)
    pos_mm = -MAX_PHYSICAL_LIMIT_MM;
  if (pos_mm > MAX_PHYSICAL_LIMIT_MM)
    pos_mm = MAX_PHYSICAL_LIMIT_MM;

  // map the mm position to screen pixels
  return (int)(TRACK_CENTER + (pos_mm / MAX_PHYSICAL_LIMIT_MM) *
                                  (TRACK_CENTER - TRACK_X_START));
}

void initScreen() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // draw static UI elements
  tft.drawRect(COL_RIGHT, ROW_1, BTN_WIDTH, BTN_HEIGHT, TFT_WHITE);
  tft.fillRect(COL_RIGHT, ROW_2, BTN_WIDTH, BTN_HEIGHT, TFT_DARKGREY);
  tft.drawRect(COL_RIGHT, ROW_2, BTN_WIDTH, BTN_HEIGHT, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("RESET FAULT", COL_RIGHT + (BTN_WIDTH / 2), ROW_2 + 18,
                       4);

  // draw static track
  tft.drawLine(TRACK_X_START, TRACK_Y, TRACK_X_END, TRACK_Y, TFT_DARKGREY);
  tft.drawFastVLine(TRACK_CENTER, TRACK_Y - 5, 10, TFT_WHITE); // center mark

  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("-1.5ft", 5, TRACK_Y - 30, 2);
  tft.drawRightString("+1.5ft", 475, TRACK_Y - 30, 2);
}

void updateScreen(float angle, float speed, long encoder_counts,
                  uint32_t freeRam, uint32_t loopTime, float current_setpoint,
                  bool is_balancing) {
  static int counter = 0;
  static bool prev_balancing = !is_balancing;

  static int prev_cart_x = -1;
  static int prev_pend_x = -1;
  static int prev_pend_y = -1;

  float pos_mm = encoder_counts * MM_PER_COUNT;

  tft.setTextSize(2);

  // update left column (metrics)
  tft.setCursor(COL_LEFT, ROW_1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.printf("Ang: %6.1f deg \n", angle);

  tft.setCursor(COL_LEFT, ROW_1 + 40);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.printf("Pos: %6.1f mm  \n", pos_mm);

  tft.setCursor(COL_LEFT, ROW_1 + 80);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.printf("Spd: %6.1f pwm \n", speed);

  tft.setCursor(COL_LEFT, ROW_1 + 120);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("Tgt: %6.1f deg \n", current_setpoint);

  // update right column (buttons)
  if (is_balancing != prev_balancing) {
    if (is_balancing) {
      tft.fillRect(COL_RIGHT + 1, ROW_1 + 1, BTN_WIDTH - 2, BTN_HEIGHT - 2,
                   TFT_DARKGREEN);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("SYSTEM: ON", COL_RIGHT + (BTN_WIDTH / 2),
                           ROW_1 + 18, 2);
    } else {
      tft.fillRect(COL_RIGHT + 1, ROW_1 + 1, BTN_WIDTH - 2, BTN_HEIGHT - 2,
                   TFT_MAROON);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("SYSTEM: OFF", COL_RIGHT + (BTN_WIDTH / 2),
                           ROW_1 + 18, 2);
    }
    prev_balancing = is_balancing;
  }

  // live pendulum visualizer
  int cart_x = mapCartToPixels(pos_mm);

  // convert angle to radians (Assuming ~90 deg is straight up)
  float rad = angle * PI / 180.0f;
  int pend_len = 70; // 70 pixels long
  int pend_x = cart_x + (pend_len * cos(rad));
  int pend_y = TRACK_Y - (pend_len * sin(rad));

  // erase and redraw logic to prevent flickering
  if (prev_cart_x != cart_x || prev_pend_x != pend_x || prev_pend_y != pend_y) {

    if (prev_cart_x != -1) {
      // erase old pendulum line and bob
      tft.drawLine(prev_cart_x, TRACK_Y, prev_pend_x, prev_pend_y, TFT_BLACK);
      tft.fillCircle(prev_pend_x, prev_pend_y, 7, TFT_BLACK);

      // erase old cart
      tft.fillRect(prev_cart_x - 16, TRACK_Y - 11, 32, 22, TFT_BLACK);

      // repair the static track lines that got erased
      tft.drawLine(TRACK_X_START, TRACK_Y, TRACK_X_END, TRACK_Y, TFT_DARKGREY);
      tft.drawFastVLine(TRACK_CENTER, TRACK_Y - 5, 10, TFT_WHITE);

      int limit_pos_x = mapCartToPixels(MAX_DISTANCE_MM);
      int limit_neg_x = mapCartToPixels(-MAX_DISTANCE_MM);
      tft.drawFastVLine(limit_pos_x, TRACK_Y - 10, 20, TFT_RED);
      tft.drawFastVLine(limit_neg_x, TRACK_Y - 10, 20, TFT_RED);
    }

    // draw new cart
    tft.fillRect(cart_x - 15, TRACK_Y - 10, 30, 20, TFT_BLUE);

    // draw new pendulum
    tft.drawLine(cart_x, TRACK_Y, pend_x, pend_y, TFT_YELLOW);
    tft.fillCircle(pend_x, pend_y, 6, TFT_ORANGE);

    prev_cart_x = cart_x;
    prev_pend_x = pend_x;
    prev_pend_y = pend_y;
  }
}

ButtonID checkButtons(uint16_t touchX, uint16_t touchY) {
  if (touchX >= COL_RIGHT && touchX <= COL_RIGHT + BTN_WIDTH) {
    if (touchY >= ROW_1 && touchY <= ROW_1 + BTN_HEIGHT)
      return BTN_TOGGLE_BALANCE;
    if (touchY >= ROW_2 && touchY <= ROW_2 + BTN_HEIGHT)
      return BTN_RESET;
  }
  return BTN_NONE;
}
