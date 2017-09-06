#ifndef _PARSE_CAT_H_
#define _PARSE_CAT_H_

#define CAT_DESCRIPTOR_MAX 256
#include "Parse_Descriptor.h"

typedef struct CAT_INFO_T
{
	unsigned int uiCA_system_id :16;
	unsigned int uiCA_PID :13;
} CAT_INFO_T;

typedef struct TS_CAT_T
{
	unsigned int uiTable_id :8;
	unsigned int uiSection_syntax_indicator :1;
	unsigned int uiZero :1;
	unsigned int uiReserved_first :2;
	unsigned int uiSection_length :12;
	unsigned int uiReserved_second :18;
	unsigned int uiVersion_number :5;
	unsigned int uiCurrent_next_indicator :1;
	unsigned int uiSection_number :8;
	unsigned int uiLast_section_number :8;
	unsigned char aucDescriptor[CAT_DESCRIPTOR_MAX];
	unsigned long uiCRC_32 :32;
} TS_CAT_T;

void ParseCAT_Section(TS_CAT_T *pstTS_CAT, unsigned char *pucSectionBuffer);
void GetCAT_Info(CA_DESCRIPTOR_T *pstCA_Descriptor, int iDescriptorCount, CAT_INFO_T *pstCAT_Info, int *iInfoCount);
void PrintCAT(TS_CAT_T *pstTS_CAT);
int ParseCAT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength, CAT_INFO_T *pstCAT_Info, TS_CAT_T *pstTS_CAT);
#endif
