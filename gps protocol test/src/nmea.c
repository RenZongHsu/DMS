#include "../inc/nmea.h"
#include "stdio.h"
#include "string.h"



static char nmea_crc(char *str, unsigned short str_length)
{
	unsigned short i;
	char crc = 0;
	for(i = 0; i < str_length; i++)
	{
		crc ^= str[i];
	}
	return crc;
}

unsigned int nmeaEncodeGGA(char *str, unsigned int length, GPGGA *gga)
{
	unsigned int len = snprintf(str, length, "$GPGGA,%02d%02d%02d.%03d,%04.4f,%c,%05.4f,%c,%1d,%02d,%.1f,%.1f,%c,%.1f,%c,%.1f,%4s"
		, gga->time.hour, gga->time.min, gga->time.sec, gga->time.hsec
		, gga->latitude, gga->latitude_hemisphere, gga->longitude, gga->longitude_hemisphere
		, gga->position_valid, gga->inuse_satellite_count, gga->hdop, gga->alt, gga->alt_unit
		, gga->undulation, gga->undulation_unit, gga->age, gga->station_id);
	if(length - len > 5)
	{
		return (snprintf(str + len, length - len, "*%02X\r\n", nmea_crc(str + 1, len - 1)) == 5) ? len + 5 : 0;
	}
	return 0;
}


unsigned int nmeaEncodeVTG(char *str, unsigned int length, GPVTG *vtg)
{
	unsigned int len = snprintf(str, length, "$GPVTG,%03.1f,%c,%03.1f,%c,%03.1f,%c,%03.1f,%c,%c"
		, vtg->true_north_ground_course, vtg->true_north_t
		, vtg->mag_north_ground_course, vtg->mag_north_m
		, vtg->ground_speed, vtg->ground_speed_n
		, vtg->ground_speed_km_hour, vtg->ground_speed_k
		, vtg->position_type);
	if(length - len > 5)
	{
		return (snprintf(str + len, length - len, "*%02X\r\n", nmea_crc(str + 1, len - 1)) == 5) ? len + 5 : 0;
	}
	return 0;
}


unsigned int nmeaEncodeGSA(char *str, unsigned int length, GPGSA *gsa)
{
	char *crc_start = str + 1;
	unsigned int len = snprintf(str, length, "$GPGSA,%c,%1d", gsa->mode, gsa->position_type);
	if(length <= len)
	{
		printf("str buffer not enough\r\n");
		return 0;
	}
	length -= len;
	str += len;

	for(unsigned int prn_index = 0; prn_index < 12; ++prn_index)
	{
		if(gsa->prn[prn_index])
		{
			len = snprintf(str, length, ",%02d", gsa->prn[prn_index]);
		}
		else
		{
			len = snprintf(str, length, ",");
		}
		if(length <= len)
		{
			printf("str buffer not enough\r\n");
			return 0;
		}
		length -= len;
		str += len;
	}

	len = snprintf(str, length, ",%1.1f,%1.1f,%1.1f", gsa->PDOP, gsa->HDOP, gsa->VDOP);
	if(length <= len)
	{
		printf("str buffer not enough\r\n");
		return 0;
	}
	length -= len;
	str += len;
	if(length - len > 5)
	{
		len = snprintf(str, length, "*%02X\r\n", nmea_crc(crc_start, str - crc_start));
		if(length <= len)
		{
			printf("str buffer not enough\r\n");
			return 0;
		}
		length -= len;
		str += len;
	}
	return str - crc_start + 1;
}


unsigned int nmeaEncodeGSV(char *str, unsigned int length, GPGSV *gsv)
{
	unsigned char count = 0, statement_num;
	char *start = str;
	char *crc_start;
	statement_num = gsv->inview_satellite_count / NMEA_SATINPACK + ((gsv->inview_satellite_count % NMEA_SATINPACK) ? 1 : 0);
	if(gsv->inview_satellite_count > SATTLITE_MAX_NUMS || gsv->inview_satellite_count == 0 || gsv->cable_counts != statement_num || gsv->cable_counts == 0)
	{
		return 0;
	}

	while(count < gsv->cable_counts * NMEA_SATINPACK)
	{
		unsigned int len = 0;
		if(count % NMEA_SATINPACK == 0)
		{
			statement_num = count / NMEA_SATINPACK + 1;
			len = snprintf(str, length, "$GPGSV,%1d,%1d,%02d", gsv->cable_counts, statement_num, gsv->inview_satellite_count);
			if(length <= len)
			{
				printf("str buffer not enough\r\n");
				return 0;
			}
			crc_start = str + 1;
			length -= len;
			str += len;
		}

		if(count < gsv->inview_satellite_count)
		{
			len = snprintf(str, length, ",%02d,%02d,%03d,%02d", gsv->sattlite[count].satellite_num, gsv->sattlite[count].elevation, gsv->sattlite[count].azimuth, gsv->sattlite[count].db);
			if(length <= len)
			{
				printf("str buffer not enough\r\n");
				return 0;
			}
		}
		else
		{
			len = snprintf(str, length, ",,,,");
			if(length <= len)
			{
				printf("str buffer not enough\r\n");
				return 0;
			}
		}

		length -= len;
		str += len;
		++count;

		if(count % NMEA_SATINPACK == 0)
		{
			len = snprintf(str, length, "*%02X\r\n", nmea_crc(crc_start, str - crc_start));
			if(length <= len)
			{
				printf("str buffer not enough\r\n");
				return 0;
			}
			length -= len;
			str += len;
		}
	}

	return str - start;
}


unsigned int nmeaEncodeRMC(char *str, unsigned int length, GPRMC *rmc)
{
	unsigned int len = snprintf(str, length,
		"$GPRMC,%02d%02d%02d.%03d,%c,%04.4f,%c,%05.4f,%c,%03.1f,%03.1f,%02d%02d%02d,%03.1f,%c,%c"
		, rmc->time.hour, rmc->time.min, rmc->time.sec, rmc->time.hsec
		, rmc->position_valid, rmc->latitude, rmc->latitude_hemisphere
		, rmc->longitude, rmc->longitude_hemisphere
		, rmc->ground_speed, rmc->ground_course
		, rmc->date.day, rmc->date.month, rmc->date.year
		, rmc->magnetic_declination, rmc->magnetic_declination_ew, rmc->position_type);
	if(length - len > 5)
	{
		return (snprintf(str + len, length - len, "*%02X\r\n", nmea_crc(str + 1, len - 1)) == 5) ? len + 5 : 0;
	}
}


unsigned int nmeaEncodeGLL(char *str, unsigned int length, GPGLL *gll)
{
	unsigned int len = snprintf(str, length,
		"$GPGLL,%04.8f,%c,%05.8f,%c,%02d%02d%02d.%02d,%c,%c"
		, gll->latitude, gll->latitude_hemisphere
		, gll->longitude, gll->longitude_hemisphere
		, gll->time.hour, gll->time.min, gll->time.sec, gll->time.hsec
		, gll->position_valid
		, gll->position_type);
	if(length - len > 5)
	{
		return (snprintf(str + len, length - len, "*%02X\r\n", nmea_crc(str + 1, len - 1)) == 5) ? len + 5 : 0;
	}
}
