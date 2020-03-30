#ifndef MAIN_H
#define	MAIN_H

//#include <xc.h> // include processor files - each processor file is guarded.  
void Inter_adxl();
void Inter_timer();

bool ReadGPS();
bool LoraConnect();
void SleepNow();

#endif	/* XC_HEADER_TEMPLATE_H */

