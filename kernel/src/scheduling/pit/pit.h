#pragma once
#include <stdint.h>
#include "../../IO.h"

extern double TimeSinceBoot;

void Sleepd(double seconds);
void Sleep(uint64_t milliseconds);

void SetDivisor(uint16_t divisor);
uint64_t GetFrequency();
void SetFrequency(uint64_t frequency);
void Tick();
