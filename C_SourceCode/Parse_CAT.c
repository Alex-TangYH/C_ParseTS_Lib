#include <string.h>
#include <stdio.h>

#include "Parse_CAT.h"
#include "Parse_Descriptor.h"
#include "Parse_DesciptorStream.h"
#include "Get_Section.h"
#include "TsParser.h"
#include "DVB_CRC32.h"

#define CAT_PID 0x0001
#define CAT_TABLE_ID 0x01
#define INITIAL_VERSION 0xff
#define SECTION_COUNT_256 256
#define SECTION_MAX_LENGTH_4096 1024 * 4
#define OUTPUT_PREFIX_SIZE 256

/******************************************
 *
 * 重置CAT内存
 *
 ******************************************/
void CleanCAT_Info(CAT_INFO_T *pstCAT_info, int *piCA_systemCount)
{
	(*piCA_systemCount) = 0;
	memset(pstCAT_info, 0, sizeof(CAT_INFO_T));
}

/******************************************
 *
 * 解析CAT Section Head数据
 *
 ******************************************/

void ParseCAT_Head(TS_CAT_T *pstTS_CAT, unsigned char *pucSectionBuffer)
{
	int iCAT_length = 0;
	pstTS_CAT->uiTable_id = pucSectionBuffer[0];
	pstTS_CAT->uiSection_syntax_indicator = pucSectionBuffer[1] >> 7;
	pstTS_CAT->uiZero = (pucSectionBuffer[1] >> 6) & 0x01;
	pstTS_CAT->uiReserved_first = (pucSectionBuffer[1] >> 4) & 0x03;
	pstTS_CAT->uiSection_length = ((pucSectionBuffer[1] & 0x0f) << 8) | pucSectionBuffer[2];
	pstTS_CAT->uiReserved_second = (pucSectionBuffer[3] << 10) | (pucSectionBuffer[4] << 2) | ((pucSectionBuffer[5] >> 6) & 0x03);
	pstTS_CAT->uiVersion_number = (pucSectionBuffer[5] >> 1) & 0x1f;
	pstTS_CAT->uiCurrent_next_indicator = pucSectionBuffer[5] >> 7;
	pstTS_CAT->uiSection_number = pucSectionBuffer[6];
	pstTS_CAT->uiLast_section_number = pucSectionBuffer[7];

	iCAT_length = 3 + pstTS_CAT->uiSection_length;
	pstTS_CAT->uiCRC_32 = (pucSectionBuffer[iCAT_length - 4] << 24) | (pucSectionBuffer[iCAT_length - 3] << 16) | (pucSectionBuffer[iCAT_length - 2] << 8) | (pucSectionBuffer[iCAT_length - 1]);
}

/******************************************
 *
 * 解析CAT Section数据
 *
 ******************************************/

void ParseCAT_Section(TS_CAT_T *pstTS_CAT, unsigned char *pucSectionBuffer)
{
	ParseCAT_Head(pstTS_CAT, pucSectionBuffer);
	if (pstTS_CAT->uiSection_length - 4 - 5 > 0)
	{
		memcpy(pstTS_CAT->aucDescriptor, pucSectionBuffer + 8, pstTS_CAT->uiSection_length - 4 - 5);
	}
	else
	{
	}
}

/******************************************
 *
 *输出CAT 数据
 *
 ******************************************/
void PrintCAT(TS_CAT_T *pstTS_CAT)
{
	char acOutputPrefix[OUTPUT_PREFIX_SIZE] = { 0 };
	DUBUGPRINTF("\n-------------CAT info start-------------\n");
	DUBUGPRINTF("CAT->Table_id: 0x%02x\n", pstTS_CAT->uiTable_id);
	DUBUGPRINTF("CAT->Section_syntax_indicatior: 0x%02x\n", pstTS_CAT->uiSection_syntax_indicator);
	DUBUGPRINTF("CAT->Zero: 0x%02x\n", pstTS_CAT->uiZero);
	DUBUGPRINTF("CAT->Resrved_first: 0x%02x\n", pstTS_CAT->uiReserved_first);
	DUBUGPRINTF("CAT->Section_length: 0x%02x\n", pstTS_CAT->uiSection_length);
	DUBUGPRINTF("CAT->Resrved_second: 0x%02x\n", pstTS_CAT->uiReserved_second);
	DUBUGPRINTF("CAT->Version_number: 0x%02x\n", pstTS_CAT->uiVersion_number);
	DUBUGPRINTF("CAT->Current_next_indicator: 0x%02x\n", pstTS_CAT->uiCurrent_next_indicator);
	DUBUGPRINTF("CAT->Section_number: 0x%02x\n", pstTS_CAT->uiSection_number);
	DUBUGPRINTF("CAT->Last_section_number: 0x%02x\n", pstTS_CAT->uiLast_section_number);
	DUBUGPRINTF("CAT->CRC_32: 0x%08lx\n", pstTS_CAT->uiCRC_32);

	int iDescriptorLength = pstTS_CAT->uiSection_length - 4 - 5;
	if (0 < iDescriptorLength)
	{
		memset(acOutputPrefix, 0, OUTPUT_PREFIX_SIZE);
		sprintf(acOutputPrefix, "CAT->Descriptor.");
		ParseAndPrintDescriptor(pstTS_CAT->aucDescriptor, iDescriptorLength, acOutputPrefix);
	}
	else
	{
	}
	DUBUGPRINTF("-------------CAT info end---------------\n");
}

/******************************************
 *
 *读取所需要的CAT数据
 *
 ******************************************/
void GetCAT_Info(CA_DESCRIPTOR_T *pstCA_Descriptor, int iDescriptorCount, CAT_INFO_T *pstCAT_Info, int *iInfoCount)
{
	int iLoopTime = 0;

	for (iLoopTime = 0; iLoopTime < iDescriptorCount; iLoopTime++)
	{
		pstCAT_Info[*iInfoCount].uiCA_PID = pstCA_Descriptor->uiCA_PID;
		pstCAT_Info[*iInfoCount].uiCA_system_id = pstCA_Descriptor->uiCA_system_ID;
		(*iInfoCount)++;
	}
}

/******************************************
 *
 * 解析CAT数据
 *
 ******************************************/
int ParseCAT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength, CAT_INFO_T *pstCAT_Info, TS_CAT_T *pstTS_CAT)
{
	DUBUGPRINTF("\n\n=================================ParseCAT_Table Start================================= \n");
	int iTemp = 0;
	int iInfoCount = 0;
	unsigned int uiVersion = INITIAL_VERSION;
	unsigned int uiRecordSectionNumber[SECTION_COUNT_256] = { 0 };
	unsigned char ucSectionBuffer[SECTION_MAX_LENGTH_4096] = { 0 };

	if (-1 == fseek(pfTsFile, iTsPosition, SEEK_SET))
	{
		DUBUGPRINTF("Parse CAT error\n");
		return -1;
	}
	else
	{
		memset(pstCAT_Info, 0, sizeof(CAT_INFO_T));
		while (!feof(pfTsFile))
		{
			iTemp = GetOneSection(pfTsFile, iTsLength, ucSectionBuffer, CAT_PID, CAT_TABLE_ID, &uiVersion);
			switch (iTemp)
			{
				case 0:
					DUBUGPRINTF("Enter if (0 == iTemp) in PARSE_CAT\n");
					uiVersion = INITIAL_VERSION;
					memset(uiRecordSectionNumber, 0, sizeof(char) * SECTION_COUNT_256);
					fseek(pfTsFile, 0 - iTsLength, SEEK_CUR);
					CleanCAT_Info(pstCAT_Info, &iInfoCount);
					break;
				case 1:
					if (0 == IsSectionGetBefore(ucSectionBuffer, uiRecordSectionNumber))
					{
						DUBUGPRINTF("Enter if (0 == IsSectionGetBefore) in PARSE_CAT\n");
						ParseCAT_Section(pstTS_CAT, ucSectionBuffer);
//						if (pstTS_CAT->uiSection_syntax_indicator != 0x1)
//						{
//							memset(pstTS_CAT, 0, sizeof(TS_CAT_T));
//							uiRecordSectionNumber[0]--;
//						}
//						else
//						{
						//OPT:提取CA信息
						//GetCAT_Info(&stTS_CAT, iDescriptorCount, pstCAT_Info, &iInfoCount);
						if (1 == PRINTFCAT_INFO)
						{
							PrintCAT(pstTS_CAT);
						}
//						}
					}
					if (1 == IsAllSectionOver(ucSectionBuffer, uiRecordSectionNumber))
					{
						DUBUGPRINTF("Enter if (1 == IsAllSectionOver) in PARSE_CAT\n");
						DUBUGPRINTF("\n=================================ParseCAT_Table END=================================== \n\n");
						return 1;
					}
					else
					{
					}
					break;
				case 2:
					break;
				case -1:
					DUBUGPRINTF("Enter if (-1 == iTemp) in PARSE_CAT\n");
					DUBUGPRINTF("return 0\n");
					DUBUGPRINTF("\n=================================ParseCAT_Table END=================================== \n\n");
					return 0;
					break;
				default:
					LOG("PARSE_CAT switch (iTemp) default\n");
					break;
			}
		}
		DUBUGPRINTF("return 0\n");
		DUBUGPRINTF("\n=================================ParseCAT_Table END=================================== \n\n");
		return 0;
	}

}

