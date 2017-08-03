#ifndef _LCD_CTRL_H_
#define _LCD_CTRL_H_

#define LCD_RESOLUTION_X		480
#define LCD_RESOLUTION_Y		272

void InitLCD();
void SetLCDBackLight(bool bOnOff);
void SetLCDContrast(U8 nVal);
void SetLCDBrightness(double dBrightRate);
void SetLCDBrightnessByLevel(U8 nLvl);
#endif	// END OF _LCD_CTRL_H_
