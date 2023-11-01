#ifndef __DMS_H__
#define __DMS_H__
#include "dms_data_type.h"

//encode
int dms_all_config_to_cmd(char *str, unsigned int length, DMS_CONSIG *config);
int dms_event_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, unsigned short speed);
int dms_ack_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, char ok, char *random_key);
int dms_encode_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, DMS_CONSIG *config, char *random_key);

int ota_encode_request_cmd(char *str, unsigned int length, unsigned int index);
int ota_encode_payload_cmd(char *str, unsigned int length, unsigned int index, unsigned int payload_length, void *payload);
int ota_encode_total_index_cmd(char *str, unsigned int length, unsigned int total_index_count);
//decode
char *dms_decode_line(char *str, DMS_CONSIG *dms, int *status, DMS_CMD *get_cmd, char *random_key);
int ota_decode_request_cmd(char *str, unsigned int *index);

#endif
