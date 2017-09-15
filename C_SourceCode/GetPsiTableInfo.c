/*
 * GetPsiTableInfo.c
 *
 *  Created on: 2017年9月5日
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include "ParseTS_Length.h"

#include "TsParser.h"
#include "Parse_EIT.h"
#include "Parse_EMM.h"
#include "Parse_PAT.h"
#include "Parse_PMT.h"
#include "Parse_CAT.h"
#include "Parse_SDT.h"
#include "Parse_BAT.h"
#include "Parse_NIT.h"
#include "Parse_TDT.h"
#include "Parse_TOT.h"
#include "Parse_RST.h"
#include "Parse_ST.h"
#include "Parse_DIT.h"
#include "Parse_SIT.h"
#include "TestFuction.h"
#include "GetPsiTableInfo.h"

#define PROGRAM_MAX 128
#define CA_SYSTEM_MAX 64

#define EIT_PF_ACTUAL_TABLE_ID 0x4e
#define EIT_EIT_SCHEDULE_TABLE_ID_ONE 0x50
#define EIT_EIT_SCHEDULE_TABLE_ID_TWO 0x51

//获取所有PMT表数组
int GetAllPmtTable(FILE *pfTsFile, int pmtCount, TS_PAT_PROGRAM_T *patInfoArray, TS_PMT_T *pmtArray)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);

	int iProgramIndex = 0;
	unsigned int uiPMT_PID = 0;
	PMT_INFO_T stOnePMT_Info = { 0 };
	for (iProgramIndex = 0; iProgramIndex < pmtCount; iProgramIndex++)
	{
		uiPMT_PID = patInfoArray[iProgramIndex].uiProgram_map_PID;
		if (-1 == ParsePMT_Table(pfTsFile, iTsPosition, iTsLength, uiPMT_PID, &stOnePMT_Info, &pmtArray[iProgramIndex]))
		{
			DUBUGPRINTF("Parse PMT error, the ProgramIndex is %d, PMT_PID is 0x%0x", iProgramIndex, uiPMT_PID);
			return -1;
		}
	}
	return 1;
}

//获取NIT
int GetNitTable(FILE *pfTsFile, TS_NIT_T *pstNIT, int *piTransportStreamCount)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseNIT_Table(pfTsFile, iTsPosition, iTsLength, pstNIT, piTransportStreamCount);
}

//获取CAT
int GetCatTable(FILE *pfTsFile, TS_CAT_T *pstCAT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	CAT_INFO_T stCAT_Info[CA_SYSTEM_MAX] = { 0 };
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseCAT_Table(pfTsFile, iTsPosition, iTsLength, stCAT_Info, pstCAT);
}

//获取PAT
int GetPatTable(FILE *pfTsFile, TS_PAT_T *pstPAT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	PAT_INFO_T stPAT_Info[PROGRAM_MAX] = { 0 };
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParsePAT_Table(pfTsFile, iTsPosition, iTsLength, stPAT_Info, pstPAT);
}

//获取TDT
int GetTDTTable(FILE *pfTsFile, TS_TDT_T *pstTDT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseTDT_Table(pfTsFile, iTsPosition, iTsLength, pstTDT);
}

//获取TOT
int GetTOTTable(FILE *pfTsFile, TS_TOT_T *pstTOT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseTOT_Table(pfTsFile, iTsPosition, iTsLength, pstTOT);
}

//获取SDT
int GetSdtTable(FILE *pfTsFile, TS_SDT_T *pstSDT, int *piSdtInfoCount)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseSDT_Table(pfTsFile, iTsPosition, iTsLength, pstSDT, piSdtInfoCount);
}

//获取ST
int GetStTable(FILE *pfTsFile, TS_ST_T *pstDIT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseST_Table(pfTsFile, iTsPosition, iTsLength, pstDIT);
}

//获取DIT
int GetDitTable(FILE *pfTsFile, TS_DIT_T *pstDIT)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseDIT_Table(pfTsFile, iTsPosition, iTsLength, pstDIT);
}

//获取RST
int GetRstTable(FILE *pfTsFile, TS_RST_T *pstRST)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	return ParseRST_Table(pfTsFile, iTsPosition, iTsLength, pstRST);
}
