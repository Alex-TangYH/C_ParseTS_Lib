#ifndef _PARSE_SIT_H_
#define _PARSE_SIT_H_

#define SIT_DESCRIPTOR_INFO_MAX 1024 * 4
#define SIT_INFO_MAX 1024 * 4
#define SIT_INFO_DESCRIPTOR_MAX 1024 * 4

typedef struct SIT_INFO_T
{
	unsigned int uiService_id :16;
	unsigned int uiDVB_reserved_future_use :1;
	unsigned int uiRunning_status :3;
	unsigned int uiService_loop_length :12;
	unsigned char aucDescriptor[SIT_INFO_DESCRIPTOR_MAX];
} SIT_INFO_T;

typedef struct TS_SIT_T
{
	unsigned int uiTable_id :8;
	unsigned int uiSection_syntax_indicator :1;
	unsigned int uiDVB_Reserved_future_use_first :1;
	unsigned int uiISO_Reserved_first :2;
	unsigned int uiSection_length :12;
	unsigned int uiDVB_Reserved_future_use_second :16;
	unsigned int uiISO_Reserved_second :2;
	unsigned int uiVersion_number :5;
	unsigned int uiCurrent_next_indicator :1;
	unsigned int uiSection_number :8;
	unsigned int uiLast_section_number :8;
	unsigned int uiDVB_Reserved_future_use_third :4;
	unsigned int uiTransmission_info_loop_length :12;
	unsigned char aucSIT_info_descriptor[SIT_DESCRIPTOR_INFO_MAX];
	SIT_INFO_T astSIT_info[SIT_INFO_MAX];
	unsigned long uiCRC32 :32;
} TS_SIT_T;

int ParseSIT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength);
#endif
