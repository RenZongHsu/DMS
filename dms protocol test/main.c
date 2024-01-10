#include "./inc/dms.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include <time.h>

#define OTA_DEBUG_EN	0

#if OTA_DEBUG_EN
#define OTA_DEBUG_COM	9
#define OTA_DEBUG_FILE	"D:/GP22C_DMS_code/tool/G+ Code Packer V1.9.9.17/OutputBin/GPA7XXXA_SPIFC/Code.bin"
#endif

#define UPDATE_APP_MODE	"UPDATE_APP"

#define UPDATE_BOOTLOADER_MODE	"UPDATE_BOOTLOADER"

#define SPIFC_BOOTLOADER_SIZE	65536

#define DEFAULT_OTA_BAUDRATE	921600

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
unsigned int ota_baudrate = DEFAULT_OTA_BAUDRATE;
unsigned char *update_mode = UPDATE_APP_MODE;

#define UART_RX_WAIT_TIME 500
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

int serialGetState(HANDLE hDevID, DWORD *baudrate, BYTE *parity, BYTE *byteSize, BYTE *stopBits)
{
	DCB dcb;
	if(hDevID == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	memset(&dcb, 0, sizeof(DCB));
	if(!GetCommState(hDevID, &dcb))
	{
		printf("GetCommState() failed\n");
		CloseHandle(hDevID);
		return 0;
	}

	*baudrate = dcb.BaudRate;
	*parity = dcb.Parity;
	*byteSize = dcb.ByteSize;
	*stopBits = dcb.StopBits;
	return 1;
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

int serialRead(HANDLE hDevID, char *data, unsigned int size, char printable, unsigned long long time_out)
{
	char *start_read = data;
	unsigned int nbbytes;
	struct timespec start, end;
	// do stuff

	memset(start_read, 0, size);
	clock_gettime(CLOCK_REALTIME, &start);
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
		unsigned long long delta_us = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
		if(delta_us > time_out)
		{
			//printf("Receive timeout : %ul, %ul\r\n", end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
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
	else if(strstr(argv, "baudrate:"))
	{
		sscanf(argv, "baudrate:%u", &ota_baudrate);
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

int dmsGetVersion(HANDLE handle)
{
	int retry = RETRY_COUNT;
	do
	{
		print_string(handle, "AT+VER\r\n");
		if(0 == serialRead(handle, read_buf, sizeof(read_buf), 1, 5000))
		{
			break;
		}
		Sleep(20);
	} while(--retry);
	return retry;
}

int dmsSetBaudrate(HANDLE handle, unsigned int target_baudrate)
{
	int retry = RETRY_COUNT;
	do
	{
		print_string(handle, "AT+BAUDRATE:%u\r\n", target_baudrate);
		if(serialRead(handle, read_buf, sizeof(read_buf), 1, 5000) == 0)
		{
			char *start = strstr(read_buf, "BAUDRATE=");
			if(start)
			{
				unsigned int baudrate;
				start += strlen("BAUDRATE=");
				baudrate = atoi(start);
				if(target_baudrate != baudrate)
				{
					if(serialSetup(handle, target_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
					{
						printf("Baudrate setup fail\r\n");
						return 0;
					}
					else
					{
						return dmsSetBaudrate(handle, target_baudrate);
					}
				}
				else
				{
					break;
				}
			}
		}
		Sleep(20);
	} while(--retry);
	return retry;
}

int dmsSendUpdateMode(HANDLE handle, unsigned char *update_mode)
{
	int retry = RETRY_COUNT;
	do
	{
		print_string(handle, "AT+%s\r\n", update_mode);
		if(0 == serialRead(handle, read_buf, sizeof(read_buf), 0, 5000))
		{
			break;
		}
		Sleep(20);
	} while(--retry);
	return (retry && strstr(read_buf, "Reboot") != 0);
}

int main(int argc, char *argv[])
{

#if OTA_DEBUG_EN == 0
	if(argc == 1)
	{
		return -1;
	}
	for(int axis = 1; axis < argc; axis++)
	{
		argProcess(argv[axis]);
	}
#else

	{
		char str[14];
		snprintf(str, sizeof(str), "\\\\.\\COM%d", OTA_DEBUG_COM);
		hDevID = serialInit(str);
		sourceFile = OTA_DEBUG_FILE;
	}
#endif
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
							Sleep(20);
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
		unsigned int file_size;
		float total_file_size;
		unsigned int total_index;
		char payload[1024] = {0};
		unsigned int write_length;
		unsigned int retry = RETRY_COUNT;
		int percent = -1;
		unsigned int index = 0;
		char *write;
		if(serialSetup(hDevID, dms_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("COM port setup fail\r\n");
			return -1;
		}
		printf("Baudrate:%d\r\n", dms_baudrate);

		if(!dmsGetVersion(hDevID))
		{
			printf("Get version fail\r\n");
			return -1;
		}

		printf("%s start\r\n", update_mode);

		if(!dmsSetBaudrate(hDevID, ota_baudrate))
		{
			printf("Baudrate setup fail\r\n");
			return -1;
		}

		if(!dmsSendUpdateMode(hDevID, update_mode))
		{
			printf("Wait Reboot fail\r\n");
			return -1;
		}
		printf("Reboot\r\n");

		if(serialSetup(hDevID, ota_baudrate, NOPARITY, 8, ONESTOPBIT) == -1)
		{
			printf("serialSetup fail\r\n");
			return -1;
		}

		if(serialRead(hDevID, read_buf, sizeof(read_buf), 0, 5000) != 0)
		{
			printf("Wait OTA boot timeout\r\n");
			return -1;
		}
		if(strstr(read_buf, "OTA_BOOT") == 0)
		{
			printf("Wait OTA boot fail\r\n");
			return -1;
		}
		printf("OTA_BOOT\r\n");

		if(fopen_s(&fptr, sourceFile, "r") == 0)
		{
			unsigned int offset = 0;
			if(strstr(update_mode, UPDATE_APP_MODE))
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
			--total_index;
			printf("Total index:%d\r\n", total_index);
			sourceFile = calloc(1, file_size);
			if(fread(sourceFile, 1, file_size, fptr) == 0)
			{
				printf("fread fail\r\n");
				return 0;
			}
			write = sourceFile;
			fclose(fptr);
		}
		else
		{
			printf("Can't open source file\r\n");
			return -1;
		}



		write_length = ota_encode_total_index_cmd(print_buf, sizeof(print_buf), total_index);
		retry = RETRY_COUNT;
		do
		{
			serialWrite(hDevID, print_buf, write_length);
			if(serialRead(hDevID, read_buf, sizeof(read_buf), 0, UART_RX_WAIT_TIME) == 0)
			{
				break;
			}
			Sleep(20);
		} while(--retry);
		if(retry == 0)
		{
			printf("Wait total index timeout\r\n");
			return -1;
		}
		total_file_size = file_size;

		while(file_size)
		{
			unsigned int crc;
			unsigned int payload_size;
			int err = 0;

			if(file_size >= 1024)
			{
				memcpy(payload, write, 1024);
				payload_size = 1024;
			}
			else
			{
				memcpy(payload, write, file_size);
				payload_size = file_size;
			}
			retry = RETRY_COUNT;
			write_length = ota_encode_payload_cmd(print_buf, sizeof(print_buf), index, payload_size, payload);
			do
			{
				serialWrite(hDevID, print_buf, write_length);
				if(serialRead(hDevID, read_buf, sizeof(read_buf), 0, UART_RX_WAIT_TIME) == 0)
				{
					unsigned int read_index;
					if(ota_decode_request_cmd(read_buf, &read_index))
					{
						if(read_index == (index + 1))
						{
							write += payload_size;
							file_size -= payload_size;
							index = read_index;
							{
								int now_per = ((1 - file_size / total_file_size) * 100);
								if(percent != now_per)
								{
									printf("%d%%\r\n", now_per);

									percent = now_per;
								}
							}
						}
					}
					else if(strstr(read_buf, "END_BOOT"))
					{
						write += payload_size;
						file_size -= payload_size;
						++index;
						{
							int now_per = ((1 - file_size / total_file_size) * 100);
							if(percent != now_per)
							{
								printf("%d%%\r\n", now_per);

								percent = now_per;
							}
						}
						printf("END_BOOT\r\n");
					}
					else if(strstr(read_buf, "ERROR_UPGRADE"))
					{
						err = 1;
						printf("ERROR_UPGRADE\r\n");
					}
					else
					{
						printf("Receive cmd error\r\n");
					}
					break;
				}
				Sleep(20);
			} while(--retry && !err);
			if(retry == 0)
			{
				printf("Wait payload ack timeout\r\n");
				break;
			}
			else if(err)
			{
				break;
			}
		}
		free(sourceFile);
	}
	if(!CloseHandle(hDevID))
	{
		printf("CloseHandle() failed\n");
		return -1;
	}
	return 0;
}
