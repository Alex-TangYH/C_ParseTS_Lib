/*
 * GetPsiTableInfo.h
 *
 *  Created on: 2017��9��5��
 *      Author: Administrator
 */

#ifndef GETPSITABLEINFO_H_
#define GETPSITABLEINFO_H_
#include "Parse_PAT.h"
#include "Parse_CAT.h"


int GetPatTable(FILE *pfTsFile, TS_PAT_T *pstPat);
int GetCatTable(FILE *pfTsFile, TS_CAT_T *pstCAT);

#endif /* GETPSITABLEINFO_H_ */
