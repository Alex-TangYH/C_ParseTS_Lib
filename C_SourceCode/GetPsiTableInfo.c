/*
 * GetPsiTableInfo.c
 *
 *  Created on: 2017Äê9ÔÂ5ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>

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

int GetPatTable(FILE *pfTsFile, TS_PAT_T *pstPat)
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
	int i = ParsePAT_Table(pfTsFile, iTsPosition, iTsLength, stPAT_Info, pstPat);
	fclose(pfTsFile);
	return i;
}

