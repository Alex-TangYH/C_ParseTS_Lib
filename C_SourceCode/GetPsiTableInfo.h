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

int GetPatTable(FILE *pfTsFile, TS_PAT_T *pstPat);
int GetCatTable(FILE *pfTsFile, TS_CAT_T *pstCAT);
int GetNitTable(FILE *pfTsFile, TS_NIT_T *pstNIT,int *piTransportStreamCount);

#endif /* GETPSITABLEINFO_H_ */
