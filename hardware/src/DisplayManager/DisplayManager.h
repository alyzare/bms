#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Wire.h>
#include <U8g2lib.h>
#include <WiFi.h>

struct Rect
{
    uint16_t width;
    uint16_t height;
};

class DisplayManager
{
public:
    DisplayManager();
    void begin();
    void updateNetwork(wifi_mode_t wifiMode, bool status);
    void showIP(String ip);
    void drawLoadingDots(uint16_t x, uint16_t y);
    void drawLights(uint8_t state);

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd;
    Rect drawFramedText(const char *text, const uint8_t *font, int16_t x, int16_t y);

    static const uint8_t *FONT_LARGE;
    static const uint8_t *FONT_NORMAL;
    static const uint8_t *FONT_NORMAL_BOLD;
    static const uint8_t *FONT_SMALL;
    static const uint8_t *FONT_SMALL_BOLD;
};

#endif