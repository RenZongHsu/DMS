#include "nmea_data_type.h"

unsigned int nmeaEncodeGGA(char *str, unsigned int length, GPGGA *gga);

unsigned int nmeaEncodeVTG(char *str, unsigned int length, GPVTG *vtg);

unsigned int nmeaEncodeGSA(char *str, unsigned int length, GPGSA *gsa);

unsigned int nmeaEncodeGSV(char *str, unsigned int length, GPGSV *gsv);

unsigned int nmeaEncodeRMC(char *str, unsigned int length, GPRMC *rmc);

unsigned int nmeaEncodeGLL(char *str, unsigned int length, GPGLL *gll);