/*
 * Jni.c
 *
 *  Created on: 2017年9月1日
 *      Author: Administrator
 */

#include <jni.h>
#include <string.h>
#include "C_SourceCode/TsParser.h"
#include "com_alex_ts_parser_native_function_NativeFunctionManager.h"

//解析PAT
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parsePAT(JNIEnv *env, jclass obj)
{
	jclass patBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_Table");
	jmethodID constrocMID = (*env)->GetMethodID(env, patBeanClass, "<init>", "(IIIIIIIIIII[Ljava/lang/Object;IJ)V");

	//TODO 解析本地PAT表，将PAT表的各个字段赋值，传入patBean构造函数中

	jobject patBean = (*env)->NewObject(env, patBeanClass, constrocMID);//需要增加参数
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

