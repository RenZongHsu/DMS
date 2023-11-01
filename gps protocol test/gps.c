#include "stdio.h"
#include "string.h"
#include "windows.h"
#include <time.h>
#include "./inc/nmea.h"
#include <conio.h>
#define BUFFER_SIZE 1024
#define SERIAL_BUFFER_SIZE (1024 * 4)
#define RETRY_COUNT 5

HANDLE hDevID = 0;
unsigned int gps_baudrate = 9600;


#define UART_RX_WAIT_TIME 100
HANDLE serialInit(char *port)
{
	HANDLE hDevID = CreateFile( // open the serial port
		port,
		GENERIC_READ | GENERIC_WRITE,
		0,			   // Must be opened with exclusive-access.
		NULL,		   // No security attributes.
		OPEN_EXISTING, // Must use OPEN_EXISTING.
		0,			   // Not overlapped I/O.
		NULL		   // hTemplate must be NULL for comm devices.
	);

	if(hDevID == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile() failed\n");
		return INVALID_HANDLE_VALUE;
	}
	else
	{
		printf("CreateFile() succeeded\n");
		return hDevID;
	}
}

int serialSetup(HANDLE hDevID, DWORD baudrate, BYTE parity, BYTE byteSize, BYTE stopBits)
{
	DCB dcb;
	COMMTIMEOUTS timeouts;
	if(hDevID == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	memset(&dcb, 0, sizeof(DCB));
	if(!GetCommState(hDevID, &dcb))
	{
		printf("GetCommState() failed\n");
		CloseHandle(hDevID);
		return -1;
	}
	dcb.BaudRate = baudrate;
	dcb.Parity = parity;
	dcb.ByteSize = byteSize;
	dcb.StopBits = stopBits;

	if(!SetCommState(hDevID, &dcb))
	{
		printf("SetCommState() failed\n");
		CloseHandle(hDevID);
		return -1;
	}

	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(!SetCommTimeouts(hDevID, &timeouts))
	{
		printf("SetCommTimeouts() failed\n");
		CloseHandle(hDevID);
		return -1;
	}
	return 0;
}

int serialWrite(HANDLE hDevID, char *data, unsigned int size)
{
	int writtenbytes = 0;
	if(WriteFile(hDevID, data, size, (LPDWORD)&writtenbytes, NULL))
	{
		if(writtenbytes != size)
		{
			printf("WriteFile() timed out\n");
			return -1;
		}
	}
	else
	{
		printf("WriteFile() failed\n");
		return -1;
	}
	return 0;
}

int serialRead(HANDLE hDevID, char *data, unsigned int size, char printable, unsigned long time_out)
{
	char *start_read = data;
	unsigned int nbbytes;
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	// do stuff

	memset(start_read, 0, size);
	do
	{
		if(!ReadFile(hDevID, start_read, size, (LPDWORD)&nbbytes, NULL))
		{
			printf("Read fail\r\n");
			CloseHandle(hDevID);
			return -1;
		}
		else
		{
			static int start = 0;
			if(nbbytes)
			{
				int need_break = 0;
				if(strstr(start_read, "\n"))
				{
					need_break = 1;
				}
				start_read += nbbytes;
				size -= nbbytes;
				if(need_break)
				{
					*start_read = 0;
					break;
				}
			}
		}
		clock_gettime(CLOCK_REALTIME, &end);
		unsigned long delta_us = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
		if(delta_us > time_out)
		{
			printf("Receive timeout\r\n");
			return -1;
		}
	} while(size > 0);
	if(printable)
	{
		printf("%s", data);
	}
	return 0;
}

void argProcess(char *argv)
{
	if(strstr(argv, "COM"))
	{
		unsigned int comNum;
		if(sscanf(argv, "COM%u", &comNum) == 1)
		{
			char str[16] = {0};
			snprintf(str, sizeof(str), "\\\\.\\COM%u", comNum);
			hDevID = serialInit(str);
		}
	}
}

typedef struct
{
	double latitude;
	char latitude_hemisphere;
	double longitude;
	char longitude_hemisphere;
	UTC_DATE date;
	UTC_TIME time;
	double ground_speed_km;
}GPS_GLOBAL_DATA;

int main(int argc, char *argv[])
{
	if(argc == 1)
	{
		return -1;
	}
	for(int axis = 1; axis < argc; axis++)
	{
		argProcess(argv[axis]);
	}
	if(serialSetup(hDevID, gps_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
	{
		printf("COM port setup fail\r\n");
		system("pause");
		return -1;
	}
	printf("Baudrate:%d\r\n", gps_baudrate);
	while(!_kbhit())
	{
		char str[BUFFER_SIZE] = {0};
		char serial[SERIAL_BUFFER_SIZE] = {0};
		unsigned int serial_index = 0;
		unsigned print_length = 0;
		time_t current_time = time(NULL);
		if(current_time == ((time_t)-1))
		{
			printf("current_time error\r\n");
			system("pause");
			return 0;
		}
		struct tm tm = *gmtime(&current_time);
		srand(time(NULL));

		GPS_GLOBAL_DATA global = {
			.latitude = (9000) * rand() / (RAND_MAX + 1.0),
			.latitude_hemisphere = (rand() & 0x1) ? 'N' : 'S',
			.longitude = (18000) * rand() / (RAND_MAX + 1.0),
			.longitude_hemisphere = (rand() & 0x1) ? 'E' : 'W',
			.date.day = tm.tm_mday,
			.date.month = tm.tm_mon + 1,
			.date.year = tm.tm_year,
			.time.hour = tm.tm_hour,
			.time.min = tm.tm_min,
			.time.sec = tm.tm_sec,
			.time.hsec = rand() % 1000,
			.ground_speed_km = (200) * rand() / (RAND_MAX + 1.0),
		};

		GPGGA gga = {
			.time = global.time,
			.latitude = global.latitude,
			.latitude_hemisphere = global.latitude_hemisphere,
			.longitude = global.longitude,
			.longitude_hemisphere = global.longitude_hemisphere,
			.position_valid = 2,
			.inuse_satellite_count = rand() % 32 + 1,
			.hdop = (10) * rand() / (RAND_MAX + 1.0),
			.alt = (10000) * (rand() / (RAND_MAX + 1.0) * 2 - 1),
			.alt_unit = 'M',
			.undulation = (10000) * (rand() / (RAND_MAX + 1.0) * 2 - 1),
			.undulation_unit = 'M',
			.age = rand() % 100,
			.station_id = {rand() % 26 + 'A', rand() % 26 + 'A', rand() % 26 + 'A', rand() % 26 + 'A', 0},
		};
		print_length = nmeaEncodeGGA(str, sizeof(str), &gga);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);
		GPGSA gsa = {
			.mode = 'A',
			.position_type = 1,
			.PDOP = (10) * rand() / (RAND_MAX + 1.0),
			.HDOP = (10) * rand() / (RAND_MAX + 1.0),
			.VDOP = (10) * rand() / (RAND_MAX + 1.0)
		};
		unsigned prn_total = 0;
		for(unsigned int axis = 0; axis < sizeof(gsa.prn) / sizeof(gsa.prn[0]); axis++)
		{
			gsa.prn[prn_total] = rand() % 100;
			if(gsa.prn[prn_total] <= 50 && gsa.prn[prn_total] > 0)
			{
				++prn_total;
			}
		}

		print_length = nmeaEncodeGSA(str, sizeof(str), &gsa);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);

		GPGSV gsv = {
			.inview_satellite_count = rand() % SATTLITE_MAX_NUMS + 1,
		};
		gsv.cable_counts = gsv.inview_satellite_count / NMEA_SATINPACK + ((gsv.inview_satellite_count % NMEA_SATINPACK) ? 1 : 0);
		for(unsigned axis = 0; axis < gsv.inview_satellite_count; axis++)
		{
			gsv.sattlite[axis].satellite_num = axis;
			gsv.sattlite[axis].elevation = rand() % 100;
			gsv.sattlite[axis].azimuth = rand() % 1000;
			gsv.sattlite[axis].db = rand() % 100;
		}

		print_length = nmeaEncodeGSV(str, sizeof(str), &gsv);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);

		GPRMC rmc = {
			.time = global.time,
			.position_valid = 'A',
			.latitude = global.latitude,
			.latitude_hemisphere = global.latitude_hemisphere,
			.longitude = global.longitude,
			.longitude_hemisphere = global.longitude_hemisphere,
			.ground_speed = global.ground_speed_km * 0.621371192,
			.ground_course = (180) * rand() / (RAND_MAX + 1.0),
			.date = global.date,
			.magnetic_declination = 0,
			.magnetic_declination_ew = 'E',
			.position_type = 'D'
		};
		print_length = nmeaEncodeRMC(str, sizeof(str), &rmc);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);

		GPVTG vtg = {
			.true_north_ground_course = 224.592,
			.true_north_t = 'T',
			.mag_north_ground_course = 224.592,
			.mag_north_m = 'M',
			.ground_speed = global.ground_speed_km * 0.621371192,
			.ground_speed_n = 'N',
			.ground_speed_km_hour = global.ground_speed_km,
			.ground_speed_k = 'K',
			.position_type = 'D'
		};
		print_length = nmeaEncodeVTG(str, sizeof(str), &vtg);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);

		GPGLL gll = {
			.latitude = global.latitude,
			.latitude_hemisphere = global.latitude_hemisphere,
			.longitude = global.longitude,
			.longitude_hemisphere = global.longitude_hemisphere,
			.time = global.time,
			.position_valid = 'A',
			.position_type = 'D',
		};
		print_length = nmeaEncodeGLL(str, sizeof(str), &gll);
		if(print_length)
		{
			if(serial_index < SERIAL_BUFFER_SIZE)
			{
				memcpy(&serial[serial_index], str, print_length);
				serial_index += print_length;
			}
		}
		memset(str, 0, print_length);

		printf("%s", serial);
		serialWrite(hDevID, serial, serial_index);
		Sleep(500);
	}


	if(!CloseHandle(hDevID))
	{
		printf("CloseHandle() failed\n");
	}
	return 0;
}
