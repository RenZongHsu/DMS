#ifndef __NMEA_DATA_TYPE_H__
#define __NMEA_DATA_TYPE_H__

#define SATTLITE_MAX_NUMS	32
#define NMEA_SATINPACK 4

typedef struct
{
	unsigned int hour;
	unsigned int min;
	unsigned int sec;
	unsigned int hsec;
}UTC_TIME;

typedef struct
{
	unsigned int year;
	unsigned int month;
	unsigned int day;
}UTC_DATE;

typedef struct
{
	UTC_TIME time;
	double latitude;
	char latitude_hemisphere;
	double longitude;
	char longitude_hemisphere;
	char position_valid;
	unsigned int inuse_satellite_count;
	double hdop;
	double alt;
	char alt_unit;
	double undulation;
	char undulation_unit;
	double age;
	char station_id[5];
}GPGGA;

typedef struct
{
	char mode;
	char position_type;
	unsigned int prn[12];
	double PDOP;
	double HDOP;
	double VDOP;
}GPGSA;

typedef struct
{
	unsigned int satellite_num;
	unsigned int elevation;
	unsigned int azimuth;
	unsigned int db;
}SATTLITE;

typedef struct
{
	unsigned int cable_counts;
	unsigned int inview_satellite_count;
	SATTLITE sattlite[SATTLITE_MAX_NUMS];
}GPGSV;

typedef struct
{
	UTC_TIME time;
	char position_valid;
	double latitude;
	char latitude_hemisphere;
	double longitude;
	char longitude_hemisphere;
	double ground_speed;
	double ground_course;
	UTC_DATE date;
	double magnetic_declination;
	char magnetic_declination_ew;
	char position_type;
}GPRMC;

typedef struct
{
	double true_north_ground_course;
	char true_north_t;
	double mag_north_ground_course;
	char mag_north_m;
	double ground_speed;
	char ground_speed_n;
	double ground_speed_km_hour;
	char ground_speed_k;
	char position_type;
}GPVTG;

typedef struct
{
	double latitude;
	char latitude_hemisphere;
	double longitude;
	char longitude_hemisphere;
	UTC_TIME time;
	char position_valid;
	char position_type;
}GPGLL;

#endif
