#ifndef _EPAPER_FUNC_
#define _EPAPER_FUNC_

#include "stdint.h"

void Epaper_Init(void);
void Epaper_Clear(void);
void Epaper_Clear_Black(void);
void Epaper_Sleep(void);
void Epaper_Init_Fast(void);
void Epaper_Display_Fast(uint8_t *Image);



#endif