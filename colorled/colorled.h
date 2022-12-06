#ifndef _COLOR_H_
#define _COLOR_H_

int pwmActiveAll();
int pwmInactiveAll();
int pwmSetDuty(int dutyCycle, int pwmIndex);
int pwmSetPeriod(int Period, int pwmIndex);
int pwmSetPercent(int percent, int ledColor);
int pwmStartAll();
int pwmLedInit();

#endif
