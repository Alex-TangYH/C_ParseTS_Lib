/****************************************************************
 *
 * FLIENAME: TsParser.c
 *
 * DESCRIPTION: Parse TS
 *
 * AUTHOR: tangyh
 *
 * DATE : 04/08/2002
 *
 * HISTORY //not need if controlled by ClearCase
 *
 *    Date	 Rev.	    Who	       Details
 *  ==========	=======   =========   =========
 *  03/08/2017     0.01     Tangyh    First Release
 *****************************************************************/

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

#define PROGRAM_MAX 128
#define CA_SYSTEM_MAX 64

#define EIT_PF_ACTUAL_TABLE_ID 0x4e
#define EIT_EIT_SCHEDULE_TABLE_ID_ONE 0x50
#define EIT_EIT_SCHEDULE_TABLE_ID_TWO 0x51

/******************************************
 *
 *获取所有PMT信息
 *
 ******************************************/

int ParseAllProgramPMT(FILE *pfTsFile, int iTsPosition, int iTsLength, PAT_INFO_T *pstPAT_Info, int iProgramCount, PMT_INFO_T *pstPMT_Info)
{
	int iProgramIndex = 0;
	unsigned int uiPMT_PID = 0;
	PMT_INFO_T stOnePMT_Info = { 0 };
	TS_PMT_T stTS_PMT = { 0 };
	DUBUGPRINTF("ParseAllProgramPMT\nProgramCount：%d\n", iProgramCount);

	for (iProgramIndex = 0; iProgramIndex < iProgramCount; iProgramIndex++)
	{
		uiPMT_PID = pstPAT_Info[iProgramIndex].uiPMT_PID;
		if (-1 == ParsePMT_Table(pfTsFile, iTsPosition, iTsLength, uiPMT_PID, &stOnePMT_Info, &stTS_PMT))
		{
			DUBUGPRINTF("Parse PMT error, the ProgramIndex is %d, PMT_PID is 0x%0x", iProgramIndex, uiPMT_PID);
			return -1;
		}
		pstPMT_Info[iProgramIndex] = stOnePMT_Info;
	}
	return 0;
}

/******************************************
 *
 * 获取所有EIT信息
 *
 ******************************************/

int ParseAllEIT_Table(FILE *pfTsFile, int iTsPosition, int iTsLength)
{
	//TODO 去掉40这个参数，动态分配
	TS_EIT_T *astEitArray = (TS_EIT_T *) malloc(40 * sizeof(TS_EIT_T));
	int iEitCount = 0;
	ParseEIT_Table(pfTsFile, iTsPosition, iTsLength, EIT_PF_ACTUAL_TABLE_ID, astEitArray, &iEitCount);
//	ParseEIT_Table(pfTsFile, iTsPosition, iTsLength, EIT_EIT_SCHEDULE_TABLE_ID_ONE);
//	ParseEIT_Table(pfTsFile, iTsPosition, iTsLength, EIT_EIT_SCHEDULE_TABLE_ID_TWO);
//	 TODO 对返回值进行判断
	free(astEitArray);
	astEitArray = NULL;
	return 0;
}

/****************************************************************
 * funtion: ParseTransportStream
 * param:
 *	 FILE *pfTsFile		pointer of transport stream
 * feature: Parse transport stream
 *
 ****************************************************************/
int ParseTransportStream(FILE *pfTsFile)
{
	int iTsPosition = 0;
	int iTsLength = 0;
	int iProgramCount = 0;
	int iEmmPosition = 0;
	int iEmmCount = 0;

	TS_PAT_T stPAT = { 0 };
	TS_CAT_T stCAT = { 0 };
	PAT_INFO_T stPAT_Info[PROGRAM_MAX] = { 0 };
	PMT_INFO_T stPMT_Info[PROGRAM_MAX] = { 0 };
	CAT_INFO_T stCAT_Info[CA_SYSTEM_MAX] = { 0 };
	TS_NIT_T stNIT = { 0 };
	TS_TDT_T stTS_TDT = { 0 };
	TS_TOT_T stTS_TOT = { 0 };
	TS_SDT_T stTS_SDT = { 0 };
	TS_ST_T stTS_ST = { 0 };
	TS_DIT_T stTS_DIT = { 0 };
	TS_SIT_T stTS_SIT = { 0 };
	TS_RST_T stTS_RST = { 0 };
	TS_BAT_T stTS_BAT = { 0 };
	int nitTransportStreamDescriptorCount = 0;
	int sitInfoCount = 0;
	int iSdtInfoCount = 0;
	int iBatInfoCount = 0;

	iTsLength = ParseTsLength(pfTsFile, &iTsPosition);
	if (-1 == iTsLength)
	{
		DUBUGPRINTF("The file is not the transport stream file\n");
		return -1;
	}
	DUBUGPRINTF("The position is %d\n", iTsPosition);
	DUBUGPRINTF("The package length is %d\n", iTsLength);

	iEmmCount = ParseCAT_Table(pfTsFile, iTsPosition, iTsLength, stCAT_Info, &stCAT);
	if (-1 == iEmmCount)
	{
		DUBUGPRINTF("ParseCAT_Table error \n");
		return -1;
	}
	if (0 == iEmmCount)
	{
		DUBUGPRINTF("This file had not be encrypted\n");
	}
	else
	{
		for (iEmmPosition = 0; iEmmPosition < iEmmCount; iEmmPosition++)
		{
			if (-1 == ParseEMM_Table(pfTsFile, iTsPosition, iTsLength, &stCAT_Info[iEmmPosition]))
			{
				return -1;
			}
			else
			{
				/*TODO: ParseECM  */
			}
		}
	}

	iProgramCount = ParsePAT_Table(pfTsFile, iTsPosition, iTsLength, stPAT_Info, &stPAT);
	if (iProgramCount <= 0)
	{
		return -1;
	}

	if (-1 == ParseSDT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_SDT, &iSdtInfoCount))
	{
		return -1;
	}

	if (-1 == ParseAllProgramPMT(pfTsFile, iTsPosition, iTsLength, stPAT_Info, iProgramCount, stPMT_Info))
	{
		return -1;
	}

	if (-1 == ParseNIT_Table(pfTsFile, iTsPosition, iTsLength, &stNIT, &nitTransportStreamDescriptorCount))
	{
		return -1;
	}

	if (-1 == ParseTDT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_TDT))
	{
		return -1;
	}

	if (-1 == ParseTOT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_TOT))
	{
		return -1;
	}

	if (-1 == ParseBAT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_BAT, &iBatInfoCount))
	{
		return -1;
	}

	if (-1 == ParseAllEIT_Table(pfTsFile, iTsPosition, iTsLength))
	{
		//return -1;
	}

	if (-1 == ParseRST_Table(pfTsFile, iTsPosition, iTsLength, &stTS_RST))
	{
		//return -1;
	}

	if (-1 == ParseST_Table(pfTsFile, iTsPosition, iTsLength, &stTS_ST))
	{
		//return -1;
	}

	if (-1 == ParseDIT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_DIT))
	{
		//return -1;
	}

	if (-1 == ParseSIT_Table(pfTsFile, iTsPosition, iTsLength, &stTS_SIT, &sitInfoCount))
	{
		//return -1;
	}

	return 0;
}

/****************************************************************
 * funtion: parseStream()
 * param:
 * feature: Parse transport stream
 *
 ****************************************************************/
int parseStream(char *pcFilePath)
{
	FILE *pfTsFile = NULL;

	pfTsFile = fopen(pcFilePath, "rb");
	if (NULL == pfTsFile)
	{
		pfTsFile = fopen(pcFilePath, "rb");
		if (NULL == pfTsFile)
		{
			DUBUGPRINTF("file does not exist \n");
			return 0;
		}
	}

	ParseTransportStream(pfTsFile);
	DUBUGPRINTF("完成\n");
	fclose(pfTsFile);
	return 1;
}

/****************************************************************
 * funtion: main()
 * param:
 * feature: Parse transport stream
 *
 ****************************************************************/
int main()
{
	char cTestFilePath[] = "D:\\test\\standent2.ts";
	parseStream(cTestFilePath);
	Test();
	return 1;
}
