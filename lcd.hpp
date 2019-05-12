#include <U8g2lib.h>

class Lcd
{
public:
  Lcd()
  {}

  void Init()
  {
    lcd.begin(); 
    //lcd.setFont(u8g2_font_t0_11_tr); // 6x11
    lcd.setFont(u8g2_font_t0_12_tr); // 6x11
    lcd.setContrast(128);
  }
  
  void DrawLine(int nr, const char* str)
  {
    auto height = lcd.getMaxCharHeight();
    lcd.drawStr(0, height * (nr+1) -1, str);  // write something to the internal memory
  }

  void Clear() { lcd.clearBuffer(); }

  void DrawDots()
  {
    lcd.drawPixel(0,0);
    lcd.drawPixel(lcd.getDisplayWidth() - 1, 0);
    lcd.drawPixel(0, lcd.getDisplayHeight() - 1);
    lcd.drawPixel(lcd.getDisplayWidth() -1, lcd.getDisplayHeight() -1);
  }

  void DrawBorder()
  {
    lcd.drawLine(0, 0, lcd.getDisplayWidth() - 1, 0);
    lcd.drawLine(0, 0, 0, lcd.getDisplayHeight() - 1);
    lcd.drawLine(lcd.getDisplayWidth() - 1, 0, lcd.getDisplayWidth() - 1, lcd.getDisplayHeight() - 1);
    lcd.drawLine(0, lcd.getDisplayHeight() - 1, lcd.getDisplayWidth() - 1, lcd.getDisplayHeight() - 1);
  }

  void Flush()
  {
    lcd.sendBuffer();          // transfer internal memory to the display
  }
  
private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd {U8G2_R0, /* reset=*/ U8X8_PIN_NONE};
};
