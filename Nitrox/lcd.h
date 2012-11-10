
#ifndef LCD_H_
#define LCD_H_

#include "libraries/glcd/glcd.h"
#include "libraries/glcd/fonts/font5x7.h"
#include "libraries/glcd/fonts/Liberation_Sans15x21_Numbers.h"

#define LCD_CONTRAST_LEVEL 10

void lcd_init();
void lcd_display_oxygen(double oxygen_level);

#endif /* LCD_H_ */
