/*
 * GetPsiTableInfo.h
 *
 *  Created on: 2017Äê9ÔÂ5ÈÕ
 *      Author: Administrator
 */

#ifndef GETPSITABLEINFO_H_
#define GETPSITABLEINFO_H_
#include "Parse_PAT.h"
#include "Parse_CAT.h"
#include "Parse_NIT.h"
#include "Parse_PMT.h"
#include "Parse_TDT.h"
#include "Parse_TOT.h"
#include "Parse_SDT.h"
#include "Parse_ST.h"
#include "Parse_DIT.h"

int GetDitTable(FILE *pfTsFile, TS_DIT_T *pstDIT);
int GetStTable(FILE *pfTsFile, TS_ST_T *pstST);
int GetSDTTable(FILE *pfTsFile, TS_SDT_T *pstSDT, int *piSdtInfoCount);
int GetTOTTable(FILE *pfTsFile, TS_TOT_T *pstTOT);
int GetTDTTable(FILE *pfTsFile, TS_TDT_T *pstTDT);
int GetPatTable(FILE *pfTsFile, TS_PAT_T *pstPat);
int GetCatTable(FILE *pfTsFile, TS_CAT_T *pstCAT);
int GetNitTable(FILE *pfTsFile, TS_NIT_T *pstNIT,int *piTransportStreamCount);
int GetAllPmtTable(FILE *pfTsFile, int pmtCount, TS_PAT_PROGRAM_T patInfoArray[], TS_PMT_T patArray[]);

#endif /* GETPSITABLEINFO_H_ */
