#ifndef _PARSE_EIT_H_
#define _PARSE_EIT_H_

#define EIT_INFO_MAX 100
#define EIT_DESCRIPTOR_MAX 100

typedef struct EIT_INFO_T
{
	unsigned int uiEvent_id :16;
	unsigned int auiStart_time[5];
	unsigned int uiDuration[3];
	unsigned int uiRunning_status :3;
	unsigned int uiFree_CA_mode :1;
	unsigned int uiDescriptors_loop_length :12;
	unsigned char aucDescriptor[EIT_DESCRIPTOR_MAX];
} EIT_INFO_T;

typedef struct EIT_IDENTIFICATION_INFO_T
{
	unsigned int uiTable_id :8;
	unsigned int uiService_id :16;
	unsigned int uiTransport_stream_id :16;
	unsigned int uiVersion_number :5;
	unsigned int uiSection_number :8;
} EIT_IDENTIFICATION_INFO_T;

typedef struct TS_EIT_T
{
	unsigned int uiTable_id :8;
	unsigned int uiSection_syntax_indicator :1;
	unsigned int uiReserved_future_use_first :1;
	unsigned int uiReserved_first :2;
	unsigned int uiSection_length :12;
	unsigned int uiService_id :16;
	unsigned int uiReserved_second :2;
	unsigned int uiVersion_number :5;
	unsigned int uiCurrent_next_indicator :1;
	unsigned int uiSection_number :8;
	unsigned int uiLast_section_number :8;
	unsigned int uiTransport_stream_id :16;
	unsigned int uiOriginal_network_id :16;
	unsigned int uiSegment_last_section_number :8;
	unsigned int uiLast_table_id :8;
	EIT_INFO_T astEIT_info[EIT_INFO_MAX];
	unsigned long uiCRC_32 :32;
	int eitInfoCount;
} TS_EIT_T;

int ParseEIT_Section(TS_EIT_T *pstTS_EIT, unsigned char *pucSectionBuffer);
int ParseEIT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength, int iEIT_table_id, TS_EIT_T *astEitArray, int *piEitTableCount);
#endif

