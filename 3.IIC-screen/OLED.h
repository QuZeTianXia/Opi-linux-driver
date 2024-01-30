#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(char Line, char Column, char Char);
void OLED_ShowString(char Line, char Column, char *String);
void OLED_ShowNum(char Line, char Column, int Number, char Length);
void OLED_ShowSignedNum(char Line, char Column, int Number, char Length);
void OLED_ShowHexNum(char Line, char Column, int Number, char Length);
void OLED_ShowBinNum(char Line, char Column, int Number, char Length);

#endif
