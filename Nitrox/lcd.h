
#ifndef LCD_H_
#define LCD_H_

#include "libraries/glcd/glcd.h"
#include "libraries/glcd/fonts/font5x7.h"
#include "libraries/glcd/fonts/Liberation_Sans15x21_Numbers.h"

#define LCD_CONTRAST_LEVEL 10

#define MIN(A,B) (((A)<(B)) ? (A) : (B))
#define MAX(A,B) (((A)>(B)) ? (A) : (B))

void lcd_init();
void lcd_display_oxygen(double oxygen_level, int holding);
void lcd_display_calibration(double value);

#endif /* LCD_H_ */
