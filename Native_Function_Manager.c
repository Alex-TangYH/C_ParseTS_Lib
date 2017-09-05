/*
 * Jni.c
 *
 *  Created on: 2017年9月1日
 *      Author: Administrator
 */

#include <jni.h>
#include <string.h>
#include "com_alex_ts_parser_native_function_NativeFunctionManager.h"
#include "C_SourceCode/ParseTS_Length.h"
#include "C_SourceCode/TsParser.h"
#include "C_SourceCode/Parse_EIT.h"
#include "C_SourceCode/Parse_EMM.h"
#include "C_SourceCode/Parse_PAT.h"
#include "C_SourceCode/Parse_PMT.h"
#include "C_SourceCode/Parse_CAT.h"
#include "C_SourceCode/Parse_SDT.h"
#include "C_SourceCode/Parse_BAT.h"
#include "C_SourceCode/Parse_NIT.h"
#include "C_SourceCode/Parse_TDT.h"
#include "C_SourceCode/Parse_TOT.h"
#include "C_SourceCode/Parse_RST.h"
#include "C_SourceCode/Parse_ST.h"
#include "C_SourceCode/Parse_DIT.h"
#include "C_SourceCode/Parse_SIT.h"
#include "C_SourceCode/TestFuction.h"
#include "C_SourceCode/GetPsiTableInfo.h"

//解析PAT
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parsePAT(JNIEnv *env, jclass obj)
{
	jclass patBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_Table");
	jmethodID patConstrocMID = (*env)->GetMethodID(env, patBeanClass, "<init>", "(IIIIIIIIIII[Lcom/alex/ts_parser/bean/psi/PAT_ProgramInfo;II)V");
	jclass patInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_ProgramInfo");
	jmethodID patInfoConstrocMID = (*env)->GetMethodID(env, patInfoBeanClass, "<init>", "(III)V");

	char cFilePath[] = "D:\\test\\test.ts";
	FILE *pfTsFile = NULL;
	pfTsFile = fopen(cFilePath, "rb");
	if (NULL == pfTsFile)
	{
		pfTsFile = fopen(cFilePath, "rb");
		if (NULL == pfTsFile)
		{
			DUBUGPRINTF("file does not exist \n");
			return 0;
		}
	}
	int i = 0;
	TS_PAT_T stPat = { 0 };
	int iProgramCount = GetPatTable(pfTsFile, &stPat);
	jobjectArray patInfoArray = (*env)->NewObjectArray(env, iProgramCount, patInfoBeanClass, NULL);
	for (i = 0; i < iProgramCount; i++)
	{
		jobject patinfoBean = (*env)->NewObject(env, patInfoBeanClass, patInfoConstrocMID, stPat.stPAT_Program[i].uiProgram_number, stPat.stPAT_Program[i].uiReserved, stPat.stPAT_Program[i].uiProgram_map_PID);
		(*env)->SetObjectArrayElement(env, patInfoArray, i, patinfoBean);
	}
	jobject patBean = (*env)->NewObject(env, patBeanClass, patConstrocMID, stPat.uiTable_id, stPat.uiSection_syntax_indicator, stPat.uiZero, stPat.uiReserved_first, stPat.uiSection_length, stPat.uiTransport_stream_id, stPat.uiReserved_second,
			stPat.uiVersion_number, stPat.uiCurrent_next_indicator, stPat.uiSection_number, stPat.uiLast_section_number, patInfoArray, stPat.uiNetwork_PID, stPat.uiCRC_32); //需要增加参数
	return patBean;
}

//传如文件路径，进行解析
JNIEXPORT jint JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseTSFileNative(JNIEnv *env, jclass clazz, jstring filePath)
{
	const char *str;
	char pcFilePath[256];
	str = (*env)->GetStringUTFChars(env, filePath, NULL);
	strcpy(pcFilePath, str);
	(*env)->ReleaseStringUTFChars(env, filePath, str);
	parseStream(pcFilePath);
	return 1;
}

//传对象例子
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseAge(JNIEnv *env, jclass obj)
{
	jclass testClass = (*env)->FindClass(env, "com/alex/ts_parser/native_function/TestClass");
	jmethodID constrocMID = (*env)->GetMethodID(env, testClass, "<init>", "(IZCFDBSJLjava/lang/String;)V");
	jstring str = (*env)->NewStringUTF(env, "hwq");
	unsigned char bl = 0;
	unsigned short us = 0x47;
	char c = 0xff;
	float f = 0x7fffffff;
	double db = 0x7fffffffffffffff;
	long l = 0x80000000;
	long long ll = 0x8fffffffffff;
	short s = 0xffff;
	jobject testClass_obj = (*env)->NewObject(env, testClass, constrocMID, l, bl, us, f, db, c, s, ll, str);
	return testClass_obj;
}

