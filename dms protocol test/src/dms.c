#include "../inc/dms.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define AT_SEND_HEADER		"AT$PD"
#define AT_RECEIVE_HEADER	"AT+"
#define DMS_HEADER			"CDMS"
#define CMD_END				"\r\n"

#define DMS_SMOKE			"SMOKE"
#define DMS_YAWN			"YAWN"
#define DMS_PHONE_CALL		"PHONE"
#define DMS_NO_FACE			"NOPEO"
#define DMS_DISTRACTION		"DIS"
#define DMS_EYE_CLOSED		"SLEEP"
#define DMS_ADJUST			"ADJ"
#define DMS_CAMERA_BLOCKED	"UNLENS"
#define DMS_FACE_MASK		"MASK"
#define DMS_OBJECT_FRAME	"FRAME"
#define DMS_LOOK_DOWN		"PHUBBING"
#define DMS_SEAT_BELT		"SEATBELT"

#define DMS_AT_VERSION		"VER"
#define DMS_AT_UPDATE		"UPDATA"

#define STATIC_CODE			"310"
#define MANUFACTURERS		"EUP"
#define FRIMWARE_VERSION	"147"


#define OTA_HEADER		  "GPOTA"

static int smoke_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *smoke)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (smoke->enable) ? '1' : '0'
		, smoke->top.trigger_speed, smoke->top.trigger_time, smoke->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int yawn_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *yawn)
{
	int len;
	if(length < 9)
	{
		return 0;
	}
	len = snprintf(str, 9, ";%c:%u:%u"
		, (yawn->enable) ? '1' : '0'
		, yawn->top.trigger_speed, yawn->top.warn_interval
	);
	if(len < 9)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int eye_closed_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *eye_closed)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (eye_closed->enable) ? '1' : '0'
		, eye_closed->top.trigger_speed, eye_closed->top.trigger_time, eye_closed->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int phone_call_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *phone_call)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (phone_call->enable) ? '1' : '0'
		, phone_call->top.trigger_speed, phone_call->top.trigger_time, phone_call->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int no_face_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *no_face)
{
	int len;
	if(length < 33)
	{
		return 0;
	}
	len = snprintf(str, 33, ";%c:%.1f:%u:%u:%.1f:%u"
		, (no_face->enable) ? '1' : '0'
		, no_face->button.trigger_time, no_face->button.warn_interval
		, no_face->top.trigger_speed, no_face->top.trigger_time, no_face->top.warn_interval
	);
	return len;
	if(len < 33)
	{
	}
	else
	{
		return 0;
	}
}

static int distraction_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *distraction)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (distraction->enable) ? '1' : '0'
		, distraction->top.trigger_speed, distraction->top.trigger_time, distraction->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int camera_blocked_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *camera_blocked)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (camera_blocked->enable) ? '1' : '0'
		, camera_blocked->top.trigger_speed, camera_blocked->top.trigger_time, camera_blocked->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int face_mask_config_to_str(char *str, unsigned int length, char face_mask)
{
	int len;
	if(length < 3)
	{
		return 0;
	}
	len = snprintf(str, 3, ";%c"
		, (face_mask) ? '1' : '0'
	);
	if(len < 3)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int object_frame_config_to_str(char *str, unsigned int length, char object_frame)
{
	int len;
	if(length < 3)
	{
		return 0;
	}
	len = snprintf(str, 3, ";%c"
		, (object_frame) ? '1' : '0'
	);
	if(len < 3)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int look_down_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *look_down)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (look_down->enable) ? '1' : '0'
		, look_down->top.trigger_speed, look_down->top.trigger_time, look_down->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

static int seat_belt_config_to_str(char *str, unsigned int length, DMS_BASE_CONFIG *seat_belt)
{
	int len;
	if(length < 14)
	{
		return 0;
	}
	len = snprintf(str, 14, ";%c:%u:%.1f:%u"
		, (seat_belt->enable) ? '1' : '0'
		, seat_belt->top.trigger_speed, seat_belt->top.trigger_time, seat_belt->top.warn_interval
	);
	if(len < 14)
	{
		return len;
	}
	else
	{
		return 0;
	}
}

int dms_all_config_to_cmd(char *str, unsigned int length, DMS_CONSIG *config)
{
	unsigned int len = snprintf(str, length, "%s=%s;%s;%s"
		, AT_SEND_HEADER
		, STATIC_CODE
		, MANUFACTURERS
		, FRIMWARE_VERSION
	);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}
	len = smoke_config_to_str(str, length, &config->smoke);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = yawn_config_to_str(str, length, &config->yawn);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = eye_closed_config_to_str(str, length, &config->eye_closed);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = phone_call_config_to_str(str, length, &config->phone_call);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = no_face_config_to_str(str, length, &config->no_face);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = distraction_config_to_str(str, length, &config->distraction);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = camera_blocked_config_to_str(str, length, &config->camera_blocked);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = face_mask_config_to_str(str, length, config->face_mask);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = object_frame_config_to_str(str, length, config->object_frame);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = look_down_config_to_str(str, length, &config->look_down);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = seat_belt_config_to_str(str, length, &config->seat_belt);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}

	len = snprintf(str, length, CMD_END);
	if(length <= len || len == 0)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}
	return 1;
}

int dms_event_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, unsigned short speed)
{
	if(cmd_enum == DMS_CMD_ADJUST || cmd_enum == DMS_CMD_FACE_MASK || cmd_enum == DMS_CMD_OBJECT_FRAME)
	{
		return 0;
	}
	unsigned int len = snprintf(str, length, "%s=%u;%u" CMD_END
		, AT_SEND_HEADER
		, cmd_enum
		, speed
	);
	if(length <= len)
	{
		return 0;
	}
	else
	{
		return len;
	}
}

int dms_ack_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, char ok, char *random_key)
{
	char *cmd;
	unsigned int len;
	switch(cmd_enum)
	{
	case DMS_CMD_SMOKE:
		cmd = DMS_SMOKE;
		break;
	case DMS_CMD_YAWN:
		cmd = DMS_YAWN;
		break;
	case DMS_CMD_PHONE_CALL:
		cmd = DMS_PHONE_CALL;
		break;
	case DMS_CMD_NO_FACE:
		cmd = DMS_NO_FACE;
		break;
	case DMS_CMD_DISTRACTION:
		cmd = DMS_DISTRACTION;
		break;
	case DMS_CMD_EYE_CLOSED:
		cmd = DMS_EYE_CLOSED;
		break;
	case DMS_CMD_ADJUST:
		cmd = DMS_ADJUST;
		break;
	case DMS_CMD_CAMERA_BLOCKED:
		cmd = DMS_CAMERA_BLOCKED;
		break;
	case DMS_CMD_FACE_MASK:
		cmd = DMS_FACE_MASK;
		break;
	case DMS_CMD_OBJECT_FRAME:
		cmd = DMS_OBJECT_FRAME;
		break;
	case DMS_CMD_LOOK_DOWN:
		cmd = DMS_LOOK_DOWN;
		break;
	case DMS_CMD_SEAT_BELT:
		cmd = DMS_SEAT_BELT;
		break;
	default:
		return 0;
	}
	len = snprintf(str, length, "%s=%s$S%s;%u:%s" CMD_END
		, AT_SEND_HEADER
		, DMS_HEADER
		, cmd
		, ok
		, random_key
	);
	if(length <= len)
	{
		return 0;
	}
	else
	{
		return len;
	}
}

int dms_encode_cmd(char *str, unsigned int length, DMS_CMD cmd_enum, DMS_CONSIG *config, char *random_key)
{
	char *cmd;
	char data[48];
	switch(cmd_enum)
	{
	case DMS_CMD_SMOKE:
		cmd = DMS_SMOKE;
		if(!smoke_config_to_str(data, sizeof(data), &config->smoke))
		{
			return 0;
		}
		break;
	case DMS_CMD_YAWN:
		cmd = DMS_YAWN;
		if(!yawn_config_to_str(data, sizeof(data), &config->yawn))
		{
			return 0;
		}
		break;
	case DMS_CMD_PHONE_CALL:
		cmd = DMS_PHONE_CALL;
		if(!phone_call_config_to_str(data, sizeof(data), &config->phone_call))
		{
			return 0;
		}
		break;
	case DMS_CMD_NO_FACE:
		cmd = DMS_NO_FACE;
		if(!no_face_config_to_str(data, sizeof(data), &config->no_face))
		{
			return 0;
		}
		break;
	case DMS_CMD_DISTRACTION:
		cmd = DMS_DISTRACTION;
		if(!distraction_config_to_str(data, sizeof(data), &config->distraction))
		{
			return 0;
		}
		break;
	case DMS_CMD_EYE_CLOSED:
		cmd = DMS_EYE_CLOSED;
		if(!eye_closed_config_to_str(data, sizeof(data), &config->eye_closed))
		{
			return 0;
		}
		break;
	case DMS_CMD_CAMERA_BLOCKED:
		cmd = DMS_CAMERA_BLOCKED;
		if(!camera_blocked_config_to_str(data, sizeof(data), &config->camera_blocked))
		{
			return 0;
		}
		break;
	case DMS_CMD_FACE_MASK:
		cmd = DMS_FACE_MASK;
		if(!face_mask_config_to_str(data, sizeof(data), config->face_mask))
		{
			return 0;
		}
		break;
	case DMS_CMD_OBJECT_FRAME:
		cmd = DMS_OBJECT_FRAME;
		if(!object_frame_config_to_str(data, sizeof(data), config->object_frame))
		{
			return 0;
		}
		break;
	case DMS_CMD_LOOK_DOWN:
		cmd = DMS_LOOK_DOWN;
		if(!look_down_config_to_str(data, sizeof(data), &config->look_down))
		{
			return 0;
		}
		break;
	case DMS_CMD_SEAT_BELT:
		cmd = DMS_SEAT_BELT;
		if(!seat_belt_config_to_str(data, sizeof(data), &config->seat_belt))
		{
			return 0;
		}
		break;
	default:
		return 0;
	}
	unsigned int len = snprintf(str, length, "%s=%s$G%s%s;%s" CMD_END
		, AT_SEND_HEADER
		, DMS_HEADER
		, cmd
		, data
		, random_key
	);
	if(length <= len)
	{
		return 0;
	}
	else
	{
		length -= len;
		str += len;
	}
	return len;
}

int dms_receive_smoke_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%3d:%f:%3d;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}


static int dms_receive_yawn_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.warn_interval
		, random_key
	) == 4)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_phone_call_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_no_face_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%f:%u:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.button.trigger_time
		, &buf.button.warn_interval
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 7)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_distraction_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_eye_closed_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_adjust_cmd(char *str, char *data, char *random_key)
{
	char buf;
	if(str == 0)
	{
		return 0;
	}

	if(sscanf(str, ";%c;%4s" CMD_END
		, &buf
		, random_key
	) == 2)
	{
		buf = buf - '0';
		memcpy(data, &buf, sizeof(char));
		return 1;
	}
	return 0;
}

static int dms_receive_camera_blocked_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_face_mask_cmd(char *str, char *data, char *random_key)
{
	char buf;
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c;%4s" CMD_END
		, &buf
		, random_key
	) == 2)
	{
		buf = buf - '0';
		memcpy(data, &buf, sizeof(char));
		return 1;
	}
	return 0;
}

static int dms_receive_object_frame_cmd(char *str, char *data, char *random_key)
{
	char buf;
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c;%4s" CMD_END
		, &buf
		, random_key
	) == 2)
	{
		buf = buf - '0';
		memcpy(data, &buf, sizeof(char));
		return 1;
	}
	return 0;
}

static int dms_receive_look_down_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_seat_belt_cmd(char *str, DMS_BASE_CONFIG *config, char *random_key)
{
	DMS_BASE_CONFIG buf = {0};
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%c:%u:%f:%u;%4s" CMD_END
		, &buf.enable
		, &buf.top.trigger_speed
		, &buf.top.trigger_time
		, &buf.top.warn_interval
		, random_key
	) == 5)
	{
		buf.enable = buf.enable - '0';
		memcpy(config, &buf, sizeof(DMS_BASE_CONFIG));
		return 1;
	}
	return 0;
}

static int dms_receive_search_cmd(char *str, char *random_key)
{
	if(str == 0)
	{
		return 0;
	}
	if(sscanf(str, ";%4s" CMD_END
		, random_key
	) == 1)
	{
		return 1;
	}
	return 0;
}

void dms_data_decode(char *str, DMS_CONSIG *dms, int *status, DMS_CMD *get_cmd, char *random_key)
{

	if(str == strstr(str, "$S"))
	{
		str += strlen("$S");
		*status = DMS_SET_STATUS;
	}
	else if(str == strstr(str, "$G"))
	{
		str += strlen("$G");
		*status = DMS_SEARCH_STATUS;
	}
	else
	{
		*status = DMS_ERR_STATUS;
		return;
	}
	if(str == strstr(str, DMS_SMOKE))
	{
		*get_cmd = DMS_CMD_SMOKE;
		str += strlen(DMS_SMOKE);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_smoke_cmd(str, &dms->smoke, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_YAWN))
	{
		*get_cmd = DMS_CMD_YAWN;
		str += strlen(DMS_YAWN);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_yawn_cmd(str, &dms->yawn, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_PHONE_CALL))
	{
		*get_cmd = DMS_CMD_PHONE_CALL;
		str += strlen(DMS_PHONE_CALL);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_phone_call_cmd(str, &dms->phone_call, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_NO_FACE))
	{
		*get_cmd = DMS_CMD_NO_FACE;
		str += strlen(DMS_NO_FACE);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_no_face_cmd(str, &dms->no_face, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_DISTRACTION))
	{
		*get_cmd = DMS_CMD_DISTRACTION;
		str += strlen(DMS_DISTRACTION);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_distraction_cmd(str, &dms->distraction, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_EYE_CLOSED))
	{
		*get_cmd = DMS_CMD_EYE_CLOSED;
		str += strlen(DMS_EYE_CLOSED);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_eye_closed_cmd(str, &dms->eye_closed, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_ADJUST))
	{
		*get_cmd = DMS_CMD_ADJUST;
		str += strlen(DMS_ADJUST);
		if(*status == DMS_SET_STATUS)
		{
			char adjust = 0;
			if(!dms_receive_adjust_cmd(str, &adjust, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_CAMERA_BLOCKED))
	{
		*get_cmd = DMS_CMD_CAMERA_BLOCKED;
		str += strlen(DMS_CAMERA_BLOCKED);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_camera_blocked_cmd(str, &dms->camera_blocked, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_FACE_MASK))
	{
		*get_cmd = DMS_CMD_FACE_MASK;
		str += strlen(DMS_FACE_MASK);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_face_mask_cmd(str, &dms->face_mask, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_OBJECT_FRAME))
	{
		*get_cmd = DMS_CMD_OBJECT_FRAME;
		str += strlen(DMS_OBJECT_FRAME);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_object_frame_cmd(str, &dms->object_frame, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_LOOK_DOWN))
	{
		*get_cmd = DMS_CMD_LOOK_DOWN;
		str += strlen(DMS_LOOK_DOWN);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_look_down_cmd(str, &dms->look_down, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else if(str == strstr(str, DMS_SEAT_BELT))
	{
		*get_cmd = DMS_CMD_SEAT_BELT;
		str += strlen(DMS_SEAT_BELT);
		if(*status == DMS_SET_STATUS)
		{
			if(!dms_receive_seat_belt_cmd(str, &dms->seat_belt, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
		else
		{
			if(!dms_receive_search_cmd(str, random_key))
			{
				*get_cmd = DMS_CMD_DATA_ERR;
			}
		}
	}
	else
	{
		*get_cmd = DMS_CMD_ERR;
	}
}
void at_cmd_decode(char *str, int *status, DMS_CMD *get_cmd)
{
	if(strstr(str, DMS_AT_VERSION) == str)
	{
		*status = DMS_AT_STATUS;
		*get_cmd = DMS_CMD_VER;
	}
	else if(strstr(str, DMS_AT_UPDATE) == str)
	{
		*status = DMS_AT_STATUS;
		*get_cmd = DMS_CMD_UPDATE;
	}
	else
	{
		*status = DMS_ERR_STATUS;
		*get_cmd = DMS_CMD_ERR;
	}
}

char *dms_decode_line(char *str, DMS_CONSIG *dms, int *status, DMS_CMD *get_cmd, char *random_key)
{
	char *end;
	if(str == 0)
	{
		return 0;
	}
	end = strstr(str, CMD_END);
	if(end != 0)
	{
		char *start = strstr(str, DMS_HEADER);
		if(start != 0 && end > start)
		{
			start += strlen(DMS_HEADER);
			dms_data_decode(start, dms, status, get_cmd, random_key);
		}
		else
		{
			start = strstr(str, AT_RECEIVE_HEADER);
			if(start != 0 && end > start)
			{
				start += strlen(AT_RECEIVE_HEADER);
				at_cmd_decode(start, status, get_cmd);
			}
			else
			{
				*status = DMS_ERR_STATUS;
			}
		}
		return end + strlen(CMD_END);
	}
	return 0;
}

static unsigned int ota_crc(char *payload, unsigned short length)
{
	unsigned int crc = 0;

	while(length > 3)
	{
		crc ^= *((unsigned int *)payload);
		++payload;
		--length;
	}

	return crc;
}

int ota_encode_request_cmd(char *str, unsigned int length, unsigned int index)
{
	int len, crc_len;
	len = snprintf(str, length, "%s%s;%u", OTA_HEADER, "$R", index);
	if(len == length)
	{
		return 0;
	}
	else
	{
		length -= len;
		crc_len = snprintf(str + len, length, "*%x" CMD_END, ota_crc(str + strlen(OTA_HEADER), len - strlen(OTA_HEADER)));
		if(crc_len == length)
		{
			return 0;
		}
		else
		{
			return len + crc_len;
		}
	}
}

int ota_decode_request_cmd(char *str, unsigned int *index)
{
	unsigned int crc, index_buf;
	str = strstr(str, OTA_HEADER);
	if(str)
	{
		str += strlen(OTA_HEADER);
		if(sscanf(str, "$R;%u*%x", &index_buf, &crc) == 2)
		{
			if(ota_crc(str, (unsigned int)(strchr(str, '*') - str)) == crc)
			{
				*index = index_buf;
				return 1;
			}
		}
	}
	return 0;
}

int ota_encode_payload_cmd(char *str, unsigned int length, unsigned int index, unsigned int payload_length, void *payload)
{
	int len;
	int total_len = 0;
	len = snprintf(str, length, "%s%s;%u;%u;", OTA_HEADER, "$W", index, payload_length);
	if(len == length)
	{
		return 0;
	}
	else
	{
		char *data_start = 0;
		length -= len;
		data_start = str + strlen(OTA_HEADER);
		str += len;
		total_len += len;
		if(length < payload_length)
		{
			return 0;
		}
		memcpy(str, payload, payload_length);
		str += payload_length;
		length -= payload_length;
		total_len += payload_length;
		len = snprintf(str, length, "*%x" CMD_END, ota_crc(data_start, total_len - strlen(OTA_HEADER)));
		if(len == length)
		{
			return 0;
		}
		else
		{
			total_len += len;
			return total_len;
		}
	}
}

int ota_encode_total_index_cmd(char *str, unsigned int length, unsigned int total_index_count)
{
	int len, crc_len;
	len = snprintf(str, length, "%s%s;%u", OTA_HEADER, "$A", total_index_count);
	if(len == length)
	{
		return 0;
	}
	else
	{
		length -= len;
		crc_len = snprintf(str + len, length, "*%x" CMD_END, ota_crc(str + strlen(OTA_HEADER), len - strlen(OTA_HEADER)));
		if(crc_len == length)
		{
			return 0;
		}
		else
		{
			return len + crc_len;
		}
	}
}
