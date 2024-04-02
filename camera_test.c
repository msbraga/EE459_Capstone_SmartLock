#include "memorysaver.h"

#include "Arduino.h"
#include "ArduCAM.h"
#include <Wire.h>
#include <SPI.h>
#include "HardwareSerial.h"

#if defined(__SAM3X8E__)
	#define Wire  Wire1
#endif