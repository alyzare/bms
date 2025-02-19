#include "DisplayManager.h"
#include "Constants.h"

const uint8_t *DisplayManager::FONT_LARGE = u8g2_font_helvR18_tf;
const uint8_t *DisplayManager::FONT_NORMAL = u8g2_font_helvR10_tf;
const uint8_t *DisplayManager::FONT_NORMAL_BOLD = u8g2_font_ncenB10_tr;
const uint8_t *DisplayManager::FONT_SMALL = u8g2_font_ncenR08_tf;
const uint8_t *DisplayManager::FONT_SMALL_BOLD = u8g2_font_ncenB08_tr;

DisplayManager::DisplayManager() : lcd(U8G2_R0, /* reset=*/U8X8_PIN_NONE) {}

void DisplayManager::begin()
{
    lcd.begin();
    drawFramedText("Starting...", FONT_SMALL_BOLD, 2, 2);
    lcd.sendBuffer();
}

void DisplayManager::updateNetwork(wifi_mode_t wifiMode, bool status)
{
    lcd.setDrawColor(0);
    lcd.drawBox(0, 0, 128, 16);
    lcd.setDrawColor(1);

    Rect rect = drawFramedText(wifiMode == WIFI_MODE_AP ? "AP" : "Wi-Fi", FONT_SMALL_BOLD, 2, 2);

    if (wifiMode == WIFI_MODE_STA)
    {
        if (status)
        {
            lcd.drawXBMP(rect.width + 8, 4, 8, 8, checkmarkBitmap);
            
            lcd.drawXBMP(rect.width + 20, 4, 10, 10, broadcastBitmap);
        }
        else
        {
            drawLoadingDots(rect.width + 10, 8);
        }
    }

    lcd.sendBuffer();
}

void DisplayManager::showIP(String ip)
{
    lcd.setDrawColor(0);
    lcd.drawBox(0, 20, 128, 20);
    lcd.setDrawColor(1);
    drawFramedText(ip.c_str(), FONT_NORMAL_BOLD, 2, 20);

    lcd.sendBuffer();
}

void DisplayManager::drawLoadingDots(uint16_t x, uint16_t y)
{
    static uint8_t counter = 1;
    for (int i = 0; i < (counter / 2 + 1); ++i)
    {
        lcd.drawDisc(x + i * 10, y, 2, U8G2_DRAW_ALL);
    }

    counter = (counter + 1) % 6 + 1;
}

void DisplayManager::drawLights(uint8_t state)
{
    lcd.setDrawColor(0);
    lcd.drawBox(0, 40, 128, 20);
    lcd.setDrawColor(1);
    
    for (int i = 0; i < 4; ++i)
    {
        if (state & (1 << i))
        {
            lcd.drawDisc(10 + i * 30, 50, 5, U8G2_DRAW_ALL);
        }
        else
        {
            lcd.drawCircle(10 + i * 30, 50, 5, U8G2_DRAW_ALL);
        }
    }
    lcd.sendBuffer();
}

Rect DisplayManager::drawFramedText(const char *text, const uint8_t *font, int16_t x, int16_t y)
{
    // Set the font
    lcd.setFont(font);

    // Get the width and height of the text
    uint8_t textWidth = lcd.getStrWidth(text);
    uint8_t textHeight = lcd.getAscent() - lcd.getDescent();

    // Draw the frame
    lcd.drawFrame(x, y, textWidth + 4, textHeight + 3);

    lcd.drawStr(x + 2, y + textHeight, text);

    return {uint16_t(textWidth + 4), uint16_t(textHeight + 3)};
}
