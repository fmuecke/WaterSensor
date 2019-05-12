#include <U8g2lib.h>

class OledDisplay
{
public:
  OledDisplay()
  {}

  void Init()
  {
    oled.begin(); 
    oled.setFont(u8g2_font_t0_11_tr); // 6x11
    //oled.setFont(u8g2_font_t0_12_tr); // 6x11
    oled.setContrast(128);
  }
  
  void DrawLine(int nr, const char* str)
  {
    auto height = oled.getMaxCharHeight();
    oled.drawStr(0, height * (nr+1) -1, str);  // write something to the internal memory
  }

  void Clear() { oled.clearBuffer(); }

  void DrawDots()
  {
    oled.drawPixel(0,0);
    oled.drawPixel(oled.getDisplayWidth() - 1, 0);
    oled.drawPixel(0, oled.getDisplayHeight() - 1);
    oled.drawPixel(oled.getDisplayWidth() -1, oled.getDisplayHeight() -1);
  }

  void DrawBorder()
  {
    oled.drawLine(0, 0, oled.getDisplayWidth() - 1, 0);
    oled.drawLine(0, 0, 0, oled.getDisplayHeight() - 1);
    oled.drawLine(oled.getDisplayWidth() - 1, 0, oled.getDisplayWidth() - 1, oled.getDisplayHeight() - 1);
    oled.drawLine(0, oled.getDisplayHeight() - 1, oled.getDisplayWidth() - 1, oled.getDisplayHeight() - 1);
  }

  void Flush()
  {
    oled.sendBuffer();          // transfer internal memory to the display
  }
  
private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled {U8G2_R0, /* reset=*/ U8X8_PIN_NONE};
};
