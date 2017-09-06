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

//解析CAT
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseCAT(JNIEnv *env, jclass obj)
{
	jclass catBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/CAT_Table");
	jmethodID defalutConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "()V");
	jmethodID catConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "(IIIIIIIIIII)V");
//	jmethodID catConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "(IIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor;I)V");

	FILE *pfTsFile = NULL;
	char cFilePath[] = "D:\\test\\test_ca.ts";
	pfTsFile = fopen(cFilePath, "rb");
	if (NULL == pfTsFile)
	{
		pfTsFile = fopen(cFilePath, "rb");
		if (NULL == pfTsFile)
		{
			DUBUGPRINTF("file does not exist \n");
			jobject catBean = (*env)->NewObject(env, catBeanClass, defalutConstrocMID);
			return catBean;
		}
	}
	TS_CAT_T stCAT = { 0 };
	int catResult = GetCatTable(pfTsFile, &stCAT);
	if (catResult != 1)
	{
		jobject catBean = (*env)->NewObject(env, catBeanClass, defalutConstrocMID);
		return catBean;
	}
	PrintCAT(&stCAT);
	jobject catBean = (*env)->NewObject(env, catBeanClass, catConstrocMID, stCAT.uiTable_id, stCAT.uiSection_syntax_indicator, stCAT.uiZero, stCAT.uiReserved_first, stCAT.uiSection_length, stCAT.uiReserved_second, stCAT.uiVersion_number,
			stCAT.uiCurrent_next_indicator, stCAT.uiSection_number, stCAT.uiLast_section_number, stCAT.uiCRC_32); //需要增加参数
	fclose(pfTsFile);
	return catBean;
}

//解析PAT
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parsePAT(JNIEnv *env, jclass obj)
{
	jclass patBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_Table");
	jmethodID patConstrocMID = (*env)->GetMethodID(env, patBeanClass, "<init>", "(IIIIIIIIIII[Lcom/alex/ts_parser/bean/psi/PAT_ProgramInfo;II)V");
	jmethodID defalutConstrocMID = (*env)->GetMethodID(env, patBeanClass, "<init>", "()V");
	jclass patInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_ProgramInfo");
	jmethodID patInfoConstrocMID = (*env)->GetMethodID(env, patInfoBeanClass, "<init>", "(III)V");

	FILE *pfTsFile = NULL;
	char cFilePath[] = "D:\\test\\test_ca.ts";
	pfTsFile = fopen(cFilePath, "rb");
	if (NULL == pfTsFile)
	{
		pfTsFile = fopen(cFilePath, "rb");
		if (NULL == pfTsFile)
		{
			DUBUGPRINTF("file does not exist \n");
			jobject patBean = (*env)->NewObject(env, patBeanClass, defalutConstrocMID);
			return patBean;
		}
	}
	int iLoopIndex = 0;
	TS_PAT_T stPat = { 0 };
	int iProgramCount = GetPatTable(pfTsFile, &stPat);
	jobjectArray patInfoArray = (*env)->NewObjectArray(env, iProgramCount, patInfoBeanClass, NULL);
	for (iLoopIndex = 0; iLoopIndex < iProgramCount; iLoopIndex++)
	{
		jobject patinfoBean = (*env)->NewObject(env, patInfoBeanClass, patInfoConstrocMID, stPat.stPAT_Program[iLoopIndex].uiProgram_number, stPat.stPAT_Program[iLoopIndex].uiReserved, stPat.stPAT_Program[iLoopIndex].uiProgram_map_PID);
		(*env)->SetObjectArrayElement(env, patInfoArray, iLoopIndex, patinfoBean);
	}
	jobject patBean = (*env)->NewObject(env, patBeanClass, patConstrocMID, stPat.uiTable_id, stPat.uiSection_syntax_indicator, stPat.uiZero, stPat.uiReserved_first, stPat.uiSection_length, stPat.uiTransport_stream_id, stPat.uiReserved_second,
			stPat.uiVersion_number, stPat.uiCurrent_next_indicator, stPat.uiSection_number, stPat.uiLast_section_number, patInfoArray, stPat.uiNetwork_PID, stPat.uiCRC_32);
	fclose(pfTsFile);
	return patBean;
}

//传入文件路径，进行解析
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

