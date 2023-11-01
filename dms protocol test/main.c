#include "./inc/dms.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include <time.h>

#define UPDATE_APP_MODE	"UPDATA_APP"

#define UPDATE_BOOTLOADER_MODE	"UPDATA_BOOTLOADER"

#define SPIFC_BOOTLOADER_SIZE	65536

#define BUFFER_SIZE 1536
#define RETRY_COUNT 5
char *cdms_cmd = NULL;
char *gps_cmd = NULL;

HANDLE hDevID = 0;
char *sourceFile = NULL;
char *gpsFile = NULL;
char *configFile = NULL;
unsigned int dms_baudrate = 9600;
unsigned int gps_baudrate = 9600;
unsigned int ota_baudrate = 921600;//230400
unsigned char *update_mode = UPDATE_APP_MODE;

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
			//printf("Receive timeout\r\n");
			return -1;
		}
	} while(size > 0);
	if(printable)
	{
		printf("%s", data);
	}
	return 0;
}

char print_buf[BUFFER_SIZE] = {0};
char read_buf[BUFFER_SIZE] = {0};
void print_string(HANDLE hDevID, CHAR *fmt, ...)
{
	va_list v_list;
	int ret;

	CHAR *pt;
	va_start(v_list, fmt);
	memset(print_buf, 0, BUFFER_SIZE);
	ret = vsnprintf(print_buf, BUFFER_SIZE, fmt, v_list);
	if(ret < 0)
	{
		return;
	}
	va_end(v_list);

	serialWrite(hDevID, print_buf, strlen(print_buf));
}

void argProcess(char *argv)
{
	if(strstr(argv, "COM"))
	{
		unsigned int comNum;
		if(sscanf(argv, "COM%u", &comNum) == 1)
		{
			char str[14];
			snprintf(str, sizeof(str), "\\\\.\\COM%u", comNum);
			hDevID = serialInit(str);
		}
	}
	else if(strstr(argv, "sfile:"))
	{
		if(sourceFile == NULL)
		{
			sourceFile = calloc(1, 2048);
		}
		if(sourceFile)
		{
			argv += strlen("sfile:");
			memcpy(sourceFile, argv, strlen(argv));
			update_mode = UPDATE_APP_MODE;
		}
		else
		{
			printf("System no space\r\n");
		}
	}
	else if(strstr(argv, "bfile:"))
	{
		if(sourceFile == NULL)
		{
			sourceFile = calloc(1, 2048);
		}
		if(sourceFile)
		{
			argv += strlen("bfile:");
			memcpy(sourceFile, argv, strlen(argv));
			update_mode = UPDATE_BOOTLOADER_MODE;
		}
		else
		{
			printf("System no space\r\n");
		}
	}
	else if(strstr(argv, "cfile:"))
	{
		if(configFile == NULL)
		{
			configFile = calloc(1, 2048);
		}
		if(configFile)
		{
			argv += strlen("cfile:");
			memcpy(configFile, argv, strlen(argv));
		}
		else
		{
			printf("System no space\r\n");
		}
	}
	else if(strstr(argv, "gfile:"))
	{
		if(gpsFile == NULL)
		{
			gpsFile = calloc(1, 2048);
		}
		if(gpsFile)
		{
			argv += strlen("gfile:");
			memcpy(gpsFile, argv, strlen(argv));
		}
		else
		{
			printf("System no space\r\n");
		}
	}
}

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

	FILE *fptr;
	if(configFile)
	{
		printf("Config start\r\n");
		if(serialSetup(hDevID, dms_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("COM port setup fail\r\n");
			return -1;
		}
		printf("Baudrate:%d\r\n", dms_baudrate);

		if(cdms_cmd == NULL)
		{
			cdms_cmd = calloc(1, 64);
		}
		if(cdms_cmd)
		{
			if(fopen_s(&fptr, configFile, "r") == 0)
			{
				while(fgets(cdms_cmd, 64, fptr) != NULL)
				{
					char *start = cdms_cmd;
					if((start = strstr(start, "CDMS$")) != NULL)
					{
						char *end = strstr(start, "\r\n");
						if(end == NULL)
						{
							end = strchr(start, '\n');
							if(end == NULL)
							{
								end = strchr(start, '\r');
								if(end == NULL)
								{
									break;
								}
							}
						}
						char temp[128] = {0};
						if(end - start > sizeof(temp))
						{
							break;
						}
						memcpy(temp, start, end - start);
						memcpy(temp + (end - start), "\r\n", 2);

						/*
						printf("%s", temp);
						serialWrite(hDevID, temp, strlen(temp));
						start = end;
						*/

						do
						{
							serialWrite(hDevID, temp, strlen(temp));
							if(serialRead(hDevID, read_buf, sizeof(read_buf), 1, UART_RX_WAIT_TIME) == 0)
							{
								start = end;
								break;
							}
							Sleep(10);
						} while(1);
					}
				}
				Sleep(1000);
				fclose(fptr);
			}
			else
			{
				printf("Can't open config file %s\r\n", configFile);
			}
		}
	}
	if(gpsFile)
	{
		printf("GPS start\r\n");
		if(serialSetup(hDevID, gps_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("serialSetup fail\r\n");
			return -1;
		}
		printf("Baudrate:%d\r\n", gps_baudrate);
		if(fopen_s(&fptr, gpsFile, "r") == 0)
		{
			unsigned int file_size;
			fseek(fptr, 0, SEEK_END); // seek to end of file
			file_size = ftell(fptr);  // get current file pointer
			fseek(fptr, 0, SEEK_SET); // seek back to beginning of file
			gps_cmd = calloc(1, file_size);
			if(gps_cmd)
			{
				if(fread(gps_cmd, 1, file_size, fptr) == 0)
				{
					printf("fread fail\r\n");
					return 0;
				}
			}
			fclose(fptr);
			while(gps_cmd)
			{
				printf(gps_cmd);
				serialWrite(hDevID, gps_cmd, file_size);
				Sleep(500);
			}
		}
		else
		{
			printf("Can't open source file\r\n");
			return -1;
		}
	}
	else if(sourceFile)
	{
		unsigned int retry = RETRY_COUNT;
		printf("%s start\r\n", update_mode);
		if(serialSetup(hDevID, dms_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("COM port setup fail\r\n");
			return -1;
		}
		printf("Baudrate:%d\r\n", dms_baudrate);
		do
		{
			print_string(hDevID, "AT+%s\r\n", update_mode);
			if(0 == serialRead(hDevID, read_buf, sizeof(read_buf), 1, 5000))
			{
				break;
			}
			Sleep(10);
		} while(--retry);
		if(retry == 0)
		{
			return -1;
		}
		if(serialSetup(hDevID, ota_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("serialSetup fail\r\n");
			return -1;
		}
		printf("Baudrate:%d\r\n", ota_baudrate);
		if(serialRead(hDevID, read_buf, sizeof(read_buf), 1, 5000) != 0)
		{
			printf("Wait OTA boot timeout\r\n");
			return -1;
		}

		unsigned int file_size;
		float total_file_size;
		unsigned int total_index;
		char payload[1024] = {0};
		if(fopen_s(&fptr, sourceFile, "r") == 0)
		{
			unsigned int offset = 0;
			if(strstr(update_mode, "UPDATA_APP"))
			{
				offset = SPIFC_BOOTLOADER_SIZE;
			}
			fseek(fptr, 0, SEEK_END); // seek to end of file
			file_size = ftell(fptr) - offset;
			fseek(fptr, offset, SEEK_SET); // seek back to beginning of file

			total_index = file_size / sizeof(payload);
			if(file_size % sizeof(payload))
			{
				++total_index;
			}

			sourceFile = calloc(1, file_size);
			if(fread(sourceFile, 1, file_size, fptr) == 0)
			{
				printf("fread fail\r\n");
				return 0;
			}
			fclose(fptr);
		}
		else
		{
			printf("Can't open source file\r\n");
			return -1;
		}
		retry = RETRY_COUNT;

		unsigned int write_length = ota_encode_total_index_cmd(print_buf, sizeof(print_buf), total_index);
		do
		{
			serialWrite(hDevID, print_buf, write_length);
			if(serialRead(hDevID, read_buf, sizeof(read_buf), 0, UART_RX_WAIT_TIME) == 0)
			{
				break;
			}
			Sleep(10);
		} while(--retry);
		if(retry == 0)
		{
			return -1;
		}
		total_file_size = file_size;

		int percent = -1;
		unsigned int index = 0;
		while(file_size)
		{
			unsigned int crc;
			unsigned int payload_size;

			if(file_size >= 1024)
			{
				memcpy(payload, sourceFile, 1024);
				payload_size = 1024;
			}
			else
			{
				memcpy(payload, sourceFile, file_size);
				payload_size = file_size;
			}
			retry = RETRY_COUNT;
			write_length = ota_encode_payload_cmd(print_buf, sizeof(print_buf), index, payload_size, payload);
			do
			{
				serialWrite(hDevID, print_buf, write_length);
				if(serialRead(hDevID, read_buf, sizeof(read_buf), 0, UART_RX_WAIT_TIME) == 0)
				{
					break;
				}
				Sleep(10);
			} while(--retry);
			if(retry == 0)
			{
				return -1;
			}
			unsigned int read_index;
			if(ota_decode_request_cmd(read_buf, &read_index))
			{
				if(read_index == (index + 1))
				{
					sourceFile += payload_size;
					file_size -= payload_size;
					index = read_index;
				}
				int now_per = ((1 - file_size / total_file_size) * 100);
				if(percent != now_per)
				{
					printf("%d%%\r\n", now_per);

					percent = now_per;
				}
			}
			else if(strstr(read_buf, "END_BOOT"))
			{
				if(read_index != (total_index - 1))
				{
					printf("Upgrade fail\r\n");
				}
				else
				{
					printf("100%%\r\nUpgrade success\r\n");
				}
				file_size = 0;
			}
		}
	}

	if(!CloseHandle(hDevID))
	{
		printf("CloseHandle() failed\n");
		return -1;
	}
	return 0;
}
