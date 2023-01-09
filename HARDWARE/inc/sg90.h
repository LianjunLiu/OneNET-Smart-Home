#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H

#include "sys.h"

void servo_init(u16 arr, u16 psc);
void servo_on_45(void);
void servo_on_90(void);
void servo_on_135(void);
void servo_on_180(void);
void servo_off(void);

#endif
