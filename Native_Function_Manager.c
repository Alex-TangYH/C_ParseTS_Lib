/*
 * Jni.c
 *
 *  Created on: 2017年9月1日
 *      Author: Administrator
 */

#include <jni.h>
#include <jni_md.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "com_alex_ts_parser_native_function_NativeFunctionManager.h"
#include "C_SourceCode/DescriptorTag.h"
#include "C_SourceCode/GetPsiTableInfo.h"
#include "C_SourceCode/Parse_BAT.h"
#include "C_SourceCode/Parse_CAT.h"
#include "C_SourceCode/Parse_DesciptorStream.h"
#include "C_SourceCode/Parse_Descriptor.h"
#include "C_SourceCode/Parse_DIT.h"
#include "C_SourceCode/Parse_EIT.h"
#include "C_SourceCode/Parse_EMM.h"
#include "C_SourceCode/Parse_NIT.h"
#include "C_SourceCode/Parse_PAT.h"
#include "C_SourceCode/Parse_PMT.h"
#include "C_SourceCode/Parse_RST.h"
#include "C_SourceCode/Parse_SDT.h"
#include "C_SourceCode/Parse_SIT.h"
#include "C_SourceCode/Parse_ST.h"
#include "C_SourceCode/Parse_TDT.h"
#include "C_SourceCode/Parse_TOT.h"
#include "C_SourceCode/ParseTS_Length.h"
#include "C_SourceCode/Print_Descriptor.h"
#include "C_SourceCode/TestFuction.h"
#include "C_SourceCode/TsParser.h"
#include "Native_Function_Manager.h"

#define EIT_PF_ACTUAL_TABLE_ID 0x4e
#define EIT_EIT_SCHEDULE_TABLE_ID_ONE 0x50
#define EIT_EIT_SCHEDULE_TABLE_ID_TWO 0x51

/******************************************************
 *
 * 解析PMT
 *
 ******************************************************/
JNIEXPORT jobjectArray JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parsePMT(JNIEnv * env, jclass obj, jstring filePath, jint pmtCount, jobjectArray pmtInfoArray)

{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		int iLoopIndex = 0;
		int iPmtCount = pmtCount;
		TS_PMT_T *pmtArray;
		pmtArray = (TS_PMT_T *) malloc(iPmtCount * sizeof(TS_PMT_T));
		TS_PAT_PROGRAM_T *patInfoArray;
		patInfoArray = (TS_PAT_PROGRAM_T *) malloc(iPmtCount * sizeof(TS_PAT_PROGRAM_T));
		jobject patInfoBean;
		jclass patInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_ProgramInfo");
		jmethodID patGetProgramMapPidMID = (*env)->GetMethodID(env, patInfoBeanClass, "getProgramMapPID", "()I");
		jmethodID patGetReservedMID = (*env)->GetMethodID(env, patInfoBeanClass, "getReserved", "()I");
		jmethodID patGetProgramNumberMID = (*env)->GetMethodID(env, patInfoBeanClass, "getProgramNumber", "()I");
		for (iLoopIndex = 0; iLoopIndex < iPmtCount; iLoopIndex++)
		{
			patInfoBean = (*env)->GetObjectArrayElement(env, pmtInfoArray, iLoopIndex);
			int patProgramNumber = (int) (*env)->CallObjectMethod(env, patInfoBean, patGetProgramNumberMID);
			int patReserved = (int) (*env)->CallObjectMethod(env, patInfoBean, patGetReservedMID);
			int patProgramMapPid = (int) (*env)->CallObjectMethod(env, patInfoBean, patGetProgramMapPidMID);
			patInfoArray[iLoopIndex].uiProgram_number = patProgramNumber;
			patInfoArray[iLoopIndex].uiProgram_map_PID = patProgramMapPid;
			patInfoArray[iLoopIndex].uiReserved = patReserved;
		}
		GetAllPmtTable(pfTsFile, iPmtCount, patInfoArray, pmtArray);
		jclass pmtBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PMT_Table");
		jmethodID pmtConstrocMID = (*env)->GetMethodID(env, pmtBeanClass, "<init>", "(IIIIIIIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;[Lcom/alex/ts_parser/bean/psi/PMT_Stream;I)V");
		jobjectArray pmtBeanArray = (*env)->NewObjectArray(env, pmtCount, pmtBeanClass, NULL);
		
		jclass pmtStreamBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PMT_Stream");
		jmethodID pmtStreamConstrocMID = (*env)->GetMethodID(env, pmtStreamBeanClass, "<init>", "(IIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;)V");
		jobject pmtStreamBean;
		for (iLoopIndex = 0; iLoopIndex < iPmtCount; iLoopIndex++)
		{
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iDescriptorCount = GetDescriptorCountInBuffer(pmtArray[iLoopIndex].aucProgramDescriptor, pmtArray[iLoopIndex].uiProgram_info_length);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, pmtArray[iLoopIndex].aucProgramDescriptor, pmtArray[iLoopIndex].uiProgram_info_length);
			
			int esCount = pmtArray[iLoopIndex].iStreamCount;
			jobjectArray pmtStreamArray = (*env)->NewObjectArray(env, esCount, pmtStreamBeanClass, NULL);
			int pmtStreamIndex = 0;
			for (pmtStreamIndex = 0; pmtStreamIndex < esCount; pmtStreamIndex++)
			{
				TS_PMT_STREAM_T stPmtStream = pmtArray[iLoopIndex].astPMT_Stream[pmtStreamIndex];
				int iDescriptorCount = GetDescriptorCountInBuffer(stPmtStream.aucDescriptor, stPmtStream.uiES_info_length);
				jobjectArray streamDescriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
				ParseDescriptorToJArray(env, &streamDescriptorBeanArray, stPmtStream.aucDescriptor, stPmtStream.uiES_info_length);
				pmtStreamBean = (*env)->NewObject(env, pmtStreamBeanClass, pmtStreamConstrocMID, stPmtStream.uiStream_type, stPmtStream.uiReserved_first, stPmtStream.uiElementary_PID, stPmtStream.uiReserved_second, stPmtStream.uiES_info_length,
						streamDescriptorBeanArray);
				(*env)->SetObjectArrayElement(env, pmtStreamArray, pmtStreamIndex, pmtStreamBean);
				(*env)->DeleteLocalRef(env, pmtStreamBean);
			}
			
			jobject pmtBean = (*env)->NewObject(env, pmtBeanClass, pmtConstrocMID, pmtArray[iLoopIndex].uiTable_id, pmtArray[iLoopIndex].uiSection_syntax_indicator, pmtArray[iLoopIndex].uiZero, pmtArray[iLoopIndex].uiReserved_first,
					pmtArray[iLoopIndex].uiSection_length, pmtArray[iLoopIndex].uiProgram_number, pmtArray[iLoopIndex].uiReserved_second, pmtArray[iLoopIndex].uiVersion_number, pmtArray[iLoopIndex].uiCurrent_next_indicator,
					pmtArray[iLoopIndex].uiSection_number, pmtArray[iLoopIndex].uiLast_section_number, pmtArray[iLoopIndex].uiReserved_third, pmtArray[iLoopIndex].uiPCR_PID, pmtArray[iLoopIndex].uiReserved_fourth,
					pmtArray[iLoopIndex].uiProgram_info_length, descriptorBeanArray, pmtStreamArray, pmtArray[iLoopIndex].uiCRC_32);
			(*env)->SetObjectArrayElement(env, pmtBeanArray, iLoopIndex, pmtBean);
			(*env)->DeleteLocalRef(env, pmtBean);
			(*env)->DeleteLocalRef(env, descriptorBeanArray);
		}
		free(pmtArray);
		free(patInfoArray);
		pmtArray = NULL;
		patInfoArray = NULL;
		fclose(pfTsFile);
		return pmtBeanArray;
	}
}

/******************************************************
 *
 * 解析NIT
 *
 ******************************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseNIT(JNIEnv * env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_NIT_T stNIT = { 0 };
		int iTransportStreamDescriptorCount = 0;
		int infoIndex = 0;
		int nitResult = GetNitTable(pfTsFile, &stNIT, &iTransportStreamDescriptorCount);
		if (nitResult != 1)
		{
			LOG("no nit\n");
			return NULL;
		}
		else
		{
			jclass nitBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/NIT_Table");
			jmethodID nitConstrocMID = (*env)->GetMethodID(env, nitBeanClass, "<init>", "(IIIIIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;II[Lcom/alex/ts_parser/bean/psi/TransportStreamDsecriptor;I)V");
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iDescriptorCount = GetDescriptorCountInBuffer(stNIT.aucDescriptor, stNIT.uiNetwork_descriptor_length);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, stNIT.aucDescriptor, stNIT.uiNetwork_descriptor_length);
			//增加INFO类数组
			jclass transportDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/TransportStreamDsecriptor");
			jobjectArray transportDescriptorBeanArray = (*env)->NewObjectArray(env, iTransportStreamDescriptorCount, transportDescriptorBeanClass, NULL);
			jmethodID transportDescriptorConstrocMID = (*env)->GetMethodID(env, transportDescriptorBeanClass, "<init>", "(IIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;)V");
			for (infoIndex = 0; infoIndex < iTransportStreamDescriptorCount; ++infoIndex)
			{
				int iDescriptorCount = GetDescriptorCountInBuffer(stNIT.astNIT_stream[infoIndex].aucDescriptor, stNIT.astNIT_stream[infoIndex].uiTransport_descriport_length);
				jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
				ParseDescriptorToJArray(env, &descriptorBeanArray, stNIT.astNIT_stream[infoIndex].aucDescriptor, stNIT.astNIT_stream[infoIndex].uiTransport_descriport_length);
				jobject transportDescriptorBean = (*env)->NewObject(env, transportDescriptorBeanClass, transportDescriptorConstrocMID, stNIT.astNIT_stream[infoIndex].uiTransport_stream_id, stNIT.astNIT_stream[infoIndex].uiOriginal_network_id,
						stNIT.astNIT_stream[infoIndex].uiReserved_future_use, stNIT.astNIT_stream[infoIndex].uiTransport_descriport_length, descriptorBeanArray);
				(*env)->SetObjectArrayElement(env, transportDescriptorBeanArray, infoIndex, transportDescriptorBean);
			}
			jobject nitBean = (*env)->NewObject(env, nitBeanClass, nitConstrocMID, stNIT.uiTable_id, stNIT.uiSection_syntax_indicator, stNIT.uiReserved_future_use_first, stNIT.uiReserved_first, stNIT.uiSection_length, stNIT.uiNetwork_id,
					stNIT.uiReserved_second, stNIT.uiVersion_number, stNIT.uiCurrent_next_indicator, stNIT.uiSection_number, stNIT.uiLast_section_number, stNIT.uiReserved_future_use_second, stNIT.uiNetwork_descriptor_length, descriptorBeanArray,
					stNIT.uiReserved_future_use_third, stNIT.uiTransport_stream_loop_Length, transportDescriptorBeanArray, stNIT.uiCRC_32); //需要增加参数
			fclose(pfTsFile);
			return nitBean;
		}
	}
}
/******************************************************
 *
 * 解析CAT
 *
 ******************************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseCAT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_CAT_T stCAT = { 0 };
		int catResult = GetCatTable(pfTsFile, &stCAT);
		if (catResult != 1)
		{
			LOG("no cat\n");
			return NULL;
		}
		else
		{
			jclass catBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/CAT_Table");
			jmethodID catConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "(IIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;I)V");
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iDescriptorCount = GetDescriptorCountInBuffer(stCAT.aucDescriptor, stCAT.uiSection_length - 9);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, stCAT.aucDescriptor, stCAT.uiSection_length - 9);
			jobject catBean = (*env)->NewObject(env, catBeanClass, catConstrocMID, stCAT.uiTable_id, stCAT.uiSection_syntax_indicator, stCAT.uiZero, stCAT.uiReserved_first, stCAT.uiSection_length, stCAT.uiReserved_second, stCAT.uiVersion_number,
					stCAT.uiCurrent_next_indicator, stCAT.uiSection_number, stCAT.uiLast_section_number, descriptorBeanArray, stCAT.uiCRC_32);
			fclose(pfTsFile);
			return catBean;
		}
	}
}

/******************************************************
 *
 * 解析PAT
 *
 ******************************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parsePAT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		int iLoopIndex = 0;
		TS_PAT_T stPat = { 0 };
		int iProgramCount = GetPatTable(pfTsFile, &stPat);
		if (-1 == iProgramCount || 0 == iProgramCount)
		{
			LOG("no pat\n");
			return NULL;
		}
		else
		{
			jclass patBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_Table");
			jmethodID patConstrocMID = (*env)->GetMethodID(env, patBeanClass, "<init>", "(IIIIIIIIIII[Lcom/alex/ts_parser/bean/psi/PAT_ProgramInfo;II)V");
			jclass patInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/PAT_ProgramInfo");
			jmethodID patInfoConstrocMID = (*env)->GetMethodID(env, patInfoBeanClass, "<init>", "(III)V");
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
	}
}

/******************************************
 *
 * 解析TOT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseTOT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_TOT_T stTot = { 0 };
		int resultOfGetTable = GetTOTTable(pfTsFile, &stTot);
		if (-1 == resultOfGetTable)
		{
			LOG("no tot\n");
			return NULL;
		}
		else
		{
			jclass totBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/TOT_Table");
			jmethodID totConstrocMID = (*env)->GetMethodID(env, totBeanClass, "<init>", "(IIIII[III[Lcom/alex/ts_parser/bean/descriptor/Descriptor;I)V");
			jintArray jIntArray = GetJintArrayFromIntArray(env, stTot.auiUTC_time, 5);
			
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iDescriptorCount = GetDescriptorCountInBuffer(stTot.aucDescriptor, stTot.uiDescriptors_loop_length);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, stTot.aucDescriptor, stTot.uiDescriptors_loop_length);
			
			jobject totBean = (*env)->NewObject(env, totBeanClass, totConstrocMID, stTot.uiTable_id, stTot.uiSection_syntax_indicator, stTot.uiReserved_future_use, stTot.uiReserved_first, stTot.uiSection_length, jIntArray, stTot.uiReserved_second,
					stTot.uiDescriptors_loop_length, descriptorBeanArray, stTot.uiCRC_32);
			
			(*env)->DeleteLocalRef(env, descriptorBeanClass);
			(*env)->DeleteLocalRef(env, jIntArray);
			(*env)->DeleteLocalRef(env, totBeanClass);
			(*env)->DeleteLocalRef(env, descriptorBeanArray);
			return totBean;
		}
	}
}

/******************************************
 *
 * 解析TDT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseTDT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_TDT_T stTdt = { 0 };
		int resultOfGetTable = GetTDTTable(pfTsFile, &stTdt);
		if (-1 == resultOfGetTable)
		{
			LOG("no tdt\n");
			return NULL;
		}
		else
		{
			jclass tdtBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/TDT_Table");
			jmethodID tdtConstrocMID = (*env)->GetMethodID(env, tdtBeanClass, "<init>", "(IIIII[I)V");
			jintArray jIntArray = GetJintArrayFromIntArray(env, stTdt.auiUTC_time, 5);
			jobject tdtBean = (*env)->NewObject(env, tdtBeanClass, tdtConstrocMID, stTdt.uiTable_id, stTdt.uiSection_syntax_indicator, stTdt.uiReserved_future_use, stTdt.uiReserved, stTdt.uiSection_length, jIntArray);
			(*env)->DeleteLocalRef(env, jIntArray);
			return tdtBean;
		}
	}
}

/******************************************
 *
 * 解析SDT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseSDT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_SDT_T stSdt = { 0 };
		int count = 0;
		int resultOfGetTable = GetSdtTable(pfTsFile, &stSdt, &count);
		if (-1 == resultOfGetTable)
		{
			LOG("no sdt\n");
			return NULL;
		}
		else
		{
			int iLoopIndex = 0;
			jclass sdtInfoClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/SdtInfo");
			jmethodID sdtInfoConstrocMID = (*env)->GetMethodID(env, sdtInfoClass, "<init>", "(IIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;)V");
			jobjectArray sdtInfoArray = (*env)->NewObjectArray(env, count, sdtInfoClass, NULL);
			
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			for (iLoopIndex = 0; iLoopIndex < count; iLoopIndex++)
			{
				SDT_INFO_T stSdtInfo = stSdt.astSDT_info[iLoopIndex];
				int iDescriptorCount = GetDescriptorCountInBuffer(stSdtInfo.aucDescriptor, stSdtInfo.uiDescriptor_loop_length);
				jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
				ParseDescriptorToJArray(env, &descriptorBeanArray, stSdtInfo.aucDescriptor, stSdtInfo.uiDescriptor_loop_length);
				jobject sdtInfoBean = (*env)->NewObject(env, sdtInfoClass, sdtInfoConstrocMID, stSdtInfo.uiService_id, stSdtInfo.uiReserved_future_use, stSdtInfo.uiEIT_schedule_flag, stSdtInfo.uiEIT_present_following_flag, stSdtInfo.uiRunning_status,
						stSdtInfo.uiFree_CA_mode, stSdtInfo.uiDescriptor_loop_length, descriptorBeanArray);
				(*env)->SetObjectArrayElement(env, sdtInfoArray, iLoopIndex, sdtInfoBean);
			}
			(*env)->DeleteLocalRef(env, descriptorBeanClass);
			
			jclass sdtBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/SDT_Table");
			jmethodID sdtConstrocMID = (*env)->GetMethodID(env, sdtBeanClass, "<init>", "(IIIIIIIIIIIII[Lcom/alex/ts_parser/bean/si/SdtInfo;I)V");
			jobject sdtBean = (*env)->NewObject(env, sdtBeanClass, sdtConstrocMID, stSdt.uiTable_id, stSdt.uiSection_syntax_indicator, stSdt.uiReserved_future_use_first, stSdt.uiReserved_first, stSdt.uiSection_length, stSdt.uiTransport_stream_id,
					stSdt.uiReserved_second, stSdt.uiVersion_number, stSdt.uiCurrent_next_indicator, stSdt.uiSection_number, stSdt.uiLast_section_number, stSdt.uiOriginal_network_id, stSdt.uiReserved_future_use_second, sdtInfoArray, stSdt.uiCRC_32);
			return sdtBean;
		}
	}
}

/******************************************
 *
 * 解析ST
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseST(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_ST_T stSt = { 0 };
		int resultOfGetTable = GetStTable(pfTsFile, &stSt);
		if (-1 == resultOfGetTable)
		{
			LOG("no st\n");
			return NULL;
		}
		else
		{
			jclass stBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/ST_Table");
			jmethodID stConstrocMID = (*env)->GetMethodID(env, stBeanClass, "<init>", "(IIIII[B)V");
			jbyteArray byteArrayData = GetJByteArrayByUChar(env, stSt.aucST_data, stSt.uiSection_length);
			jobject stBean = (*env)->NewObject(env, stBeanClass, stConstrocMID, stSt.uiTable_id, stSt.uiSection_syntax_indicator, stSt.uiReserved_future_use, stSt.uiReserved, stSt.uiSection_length, byteArrayData);
			return stBean;
		}
	}
}

/******************************************
 *
 * 解析DIT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseDIT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_DIT_T stDit = { 0 };
		int resultOfGetTable = GetDitTable(pfTsFile, &stDit);
		if (-1 == resultOfGetTable)
		{
			LOG("no dit\n");
			return NULL;
		}
		else
		{
			jclass ditBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/DIT_Table");
			jmethodID ditConstrocMID = (*env)->GetMethodID(env, ditBeanClass, "<init>", "(IIIIIII)V");
			jobject stBean = (*env)->NewObject(env, ditBeanClass, ditConstrocMID, stDit.uiTable_id, stDit.uiSection_syntax_indicator, stDit.uiReserved_future_use_first, stDit.uiReserved, stDit.uiSection_length, stDit.uiTransition_flag,
					stDit.uiReserved_future_use_second);
			return stBean;
		}
	}
}

/******************************************
 *
 * 解析RST
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseRST(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_RST_T stRst = { 0 };
		int resultOfGetTable = GetRstTable(pfTsFile, &stRst);
		if (-1 == resultOfGetTable)
		{
			LOG("no rst\n");
			return NULL;
		}
		else
		{
			int iLoopIndex = 0;
			int iInfoCount = stRst.uiSection_length / 9;
			jclass rstInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/RstInfo");
			jmethodID rstInfoBeanConstrocMID = (*env)->GetMethodID(env, rstInfoBeanClass, "<init>", "(IIIIII)V");
			jobjectArray rstInfoBeanArray = (*env)->NewObjectArray(env, iInfoCount, rstInfoBeanClass, NULL);
			for (iLoopIndex = 0; iLoopIndex < iInfoCount; iInfoCount++)
			{
				jobject rstInfoBean = (*env)->NewObject(env, rstInfoBeanClass, rstInfoBeanConstrocMID, stRst.astRST_info[iLoopIndex].uiTransport_stream_id, stRst.astRST_info[iLoopIndex].uiOriginal_network_id, stRst.astRST_info[iLoopIndex].uiService_id,
						stRst.astRST_info[iLoopIndex].uiEvent_id, stRst.astRST_info[iLoopIndex].uiReserved_future_use, stRst.astRST_info[iLoopIndex].uiRunning_status);
				(*env)->SetObjectArrayElement(env, rstInfoBeanArray, iLoopIndex, rstInfoBean);
			}
			
			jclass rstBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/RST_Table");
			jmethodID rstConstrocMID = (*env)->GetMethodID(env, rstBeanClass, "<init>", "(IIIII[Lcom/alex/ts_parser/bean/si/RstInfo;)V");
			jobject rstBean = (*env)->NewObject(env, rstBeanClass, rstConstrocMID, stRst.uiTable_id, stRst.uiSection_syntax_indicator, stRst.uiReserved_future_use, stRst.uiReserved, stRst.uiSection_length, rstInfoBeanArray);
			return rstBean;
		}
	}
}

/******************************************
 *
 * 解析EIT
 *
 ******************************************/
JNIEXPORT jobjectArray JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseEIT(JNIEnv *env, jclass obj, jstring filePath, jint eitType)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);

	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		int iEitTableCount = 0;
		//TODO 提前获取总数或者动态分配
		TS_EIT_T *astEitArray;
		int resultOfGetTable = -1;
		if (1 == eitType)
		{
			astEitArray = (TS_EIT_T *) malloc(500 * sizeof(TS_EIT_T));
			resultOfGetTable = GetAllEitTable(pfTsFile, astEitArray, &iEitTableCount, EIT_PF_ACTUAL_TABLE_ID);
		}
		else if (2 == eitType)
		{
			astEitArray = (TS_EIT_T *) malloc(1000 * sizeof(TS_EIT_T));
			resultOfGetTable = GetAllEitTable(pfTsFile, astEitArray, &iEitTableCount, EIT_EIT_SCHEDULE_TABLE_ID_ONE);
		}
		else if (3 == eitType)
		{
			astEitArray = (TS_EIT_T *) malloc(1000 * sizeof(TS_EIT_T));
			resultOfGetTable = GetAllEitTable(pfTsFile, astEitArray, &iEitTableCount, EIT_EIT_SCHEDULE_TABLE_ID_TWO);
		}
		else
		{
			return NULL;
		}
		if (-1 == resultOfGetTable || 0 == iEitTableCount)
		{
			LOG("no eit\n");
			return NULL;
		}
		else
		{
			int iLoopIndex = 0;
			jclass eitBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/EIT_Table");
			jmethodID eitConstrocMID = (*env)->GetMethodID(env, eitBeanClass, "<init>", "(IIIIIIIIIIIIIII[Lcom/alex/ts_parser/bean/si/EitInfo;I)V");
			jobjectArray eitBeanArray = (*env)->NewObjectArray(env, iEitTableCount, eitBeanClass, NULL);
			jclass eitInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/EitInfo");
			jmethodID eitInfoBeanConstrocMID = (*env)->GetMethodID(env, eitInfoBeanClass, "<init>", "(I[I[IIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;)V");
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iTableIndex = 0;
			for (iTableIndex = 0; iTableIndex < iEitTableCount; iTableIndex++)
			{

				TS_EIT_T stEit = astEitArray[iTableIndex];
				jobjectArray eitInfoBeanArray = (*env)->NewObjectArray(env, stEit.eitInfoCount, eitInfoBeanClass, NULL);
				if (stEit.eitInfoCount > 0)
				{
					for (iLoopIndex = 0; iLoopIndex < stEit.eitInfoCount; iLoopIndex++)
					{
						//TODO test.ts的这一行会报错
//						printf("stEit.astEIT_info[iLoopIndex].uiDescriptors_loop_length: %d\n",stEit.astEIT_info[iLoopIndex].uiDescriptors_loop_length);
						int iDescriptorCount = 0;
						iDescriptorCount = GetDescriptorCountInBuffer(stEit.astEIT_info[iLoopIndex].aucDescriptor, stEit.astEIT_info[iLoopIndex].uiDescriptors_loop_length);
						jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
						ParseDescriptorToJArray(env, &descriptorBeanArray, stEit.astEIT_info[iLoopIndex].aucDescriptor, stEit.astEIT_info[iLoopIndex].uiDescriptors_loop_length);
						jintArray startTimeArray = GetJintArrayFromIntArray(env, stEit.astEIT_info[iLoopIndex].auiStart_time, 5);
						jintArray durationArray = GetJintArrayFromIntArray(env, stEit.astEIT_info[iLoopIndex].uiDuration, 3);

						jobject eitInfoBean = (*env)->NewObject(env, eitInfoBeanClass, eitInfoBeanConstrocMID, stEit.astEIT_info[iLoopIndex].uiEvent_id, startTimeArray, durationArray, stEit.astEIT_info[iLoopIndex].uiRunning_status,
								stEit.astEIT_info[iLoopIndex].uiFree_CA_mode, stEit.astEIT_info[iLoopIndex].uiDescriptors_loop_length, descriptorBeanArray);

						(*env)->SetObjectArrayElement(env, eitInfoBeanArray, iLoopIndex, eitInfoBean);
						(*env)->DeleteLocalRef(env, descriptorBeanArray);
						(*env)->DeleteLocalRef(env, startTimeArray);
						(*env)->DeleteLocalRef(env, durationArray);
						(*env)->DeleteLocalRef(env, eitInfoBean);
					}
				}

				jobject eitBean = (*env)->NewObject(env, eitBeanClass, eitConstrocMID, stEit.uiTable_id, stEit.uiSection_syntax_indicator, stEit.uiReserved_future_use_first, stEit.uiReserved_first, stEit.uiSection_length, stEit.uiService_id,
						stEit.uiReserved_second, stEit.uiVersion_number, stEit.uiCurrent_next_indicator, stEit.uiSection_number, stEit.uiLast_section_number, stEit.uiTransport_stream_id, stEit.uiOriginal_network_id, stEit.uiSegment_last_section_number,
						stEit.uiLast_table_id, eitInfoBeanArray, stEit.uiCRC_32);
				(*env)->SetObjectArrayElement(env, eitBeanArray, iTableIndex, eitBean);
				(*env)->DeleteLocalRef(env, eitBean);
			}
			free(astEitArray);
			astEitArray = NULL;
			return eitBeanArray;
		}
	}
}

/******************************************
 *
 * 解析BAT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseBAT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);

	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_BAT_T stBat = { 0 };
		int iBatInfoCount = 0;
		int resultOfGetTable = GetBatTable(pfTsFile, &stBat, &iBatInfoCount);
		if (-1 == resultOfGetTable)
		{
			LOG("no bat\n");
			return NULL;
		}
		else
		{
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
			int iDescriptorCount = GetDescriptorCountInBuffer(stBat.aucDescriptor, stBat.uiBouquet_descriptor_length);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, stBat.aucDescriptor, stBat.uiBouquet_descriptor_length);

			jclass batInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/BatInfo");
			jmethodID batInfoConstrocMID = (*env)->GetMethodID(env, batInfoBeanClass, "<init>", "(IIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;)V");
			jobjectArray batInfoBeanArray = (*env)->NewObjectArray(env, iBatInfoCount, batInfoBeanClass, NULL);
			int iLoopIndex = 0;
			for (iLoopIndex = 0; iLoopIndex < iBatInfoCount; iLoopIndex++)
			{
				BAT_INFO_T stBatInfo = stBat.astBAT_info[iLoopIndex];
				int iDescriptorCount = GetDescriptorCountInBuffer(stBatInfo.aucDescriptor, stBatInfo.uiTransport_descriptor_length);
				jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
				ParseDescriptorToJArray(env, &descriptorBeanArray, stBatInfo.aucDescriptor, stBatInfo.uiTransport_descriptor_length);

				jobject batInfoBean = (*env)->NewObject(env, batInfoBeanClass, batInfoConstrocMID, stBatInfo.uiTransport_stream_id, stBatInfo.uiOriginal_network_id, stBatInfo.uiReserved_future_use, stBatInfo.uiTransport_descriptor_length,
						descriptorBeanArray);
				(*env)->SetObjectArrayElement(env, batInfoBeanArray, iLoopIndex, batInfoBean);
				(*env)->DeleteLocalRef(env, descriptorBeanArray);
			}

			jclass batBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/BAT_Table");
			jmethodID batConstrocMID = (*env)->GetMethodID(env, batBeanClass, "<init>", "(IIIIIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;II[Lcom/alex/ts_parser/bean/si/BatInfo;J)V");
			jobject batBean = (*env)->NewObject(env, batBeanClass, batConstrocMID, stBat.uiTable_id, stBat.uiSection_syntax_indicator, stBat.uiReserved_future_use_first, stBat.uiReserved_first, stBat.uiSection_length, stBat.uiBoquet_id,
					stBat.uiReserved_second, stBat.uiVersion_number, stBat.uiCurrent_next_indicator, stBat.uiSection_number, stBat.uiLast_section_number, stBat.uiReserved_future_use_second, stBat.uiBouquet_descriptor_length, descriptorBeanArray,
					stBat.uiReserved_third, stBat.uiTransport_stream_loop_length, batInfoBeanArray, stBat.uiCRC_32);
			return batBean;
		}
	}
}

/******************************************
 *
 * 解析SIT
 *
 ******************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseSIT(JNIEnv *env, jclass obj, jstring filePath)
{
	char *pcFilePath = Jstring2CharPointer(env, filePath);
	FILE *pfTsFile = GetFilePointer(pcFilePath);
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		TS_SIT_T stSit = { 0 };
		int iInfoCount = 0;
		int resultOfGetTable = GetSitTable(pfTsFile, &stSit, &iInfoCount);
		if (-1 == resultOfGetTable)
		{
			LOG("no sit\n");
			return NULL;
		}
		else
		{
			int iLoopIndex = 0;
			jclass sitInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/SitInfo");
			jmethodID sitInfoBeanConstrocMID = (*env)->GetMethodID(env, sitInfoBeanClass, "<init>", "(IIII[Lcom/alex/ts_parser/bean/si/Descriptor;)V");

			jobjectArray sitInfoBeanArray = (*env)->NewObjectArray(env, iInfoCount, sitInfoBeanClass, NULL);
			jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");

			for (iLoopIndex = 0; iLoopIndex < iInfoCount; iInfoCount++)
			{
				int iDescriptorCount = GetDescriptorCountInBuffer(stSit.astSIT_info[iLoopIndex].aucDescriptor, stSit.astSIT_info[iLoopIndex].uiService_loop_length);
				jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
				ParseDescriptorToJArray(env, &descriptorBeanArray, stSit.astSIT_info[iLoopIndex].aucDescriptor, stSit.astSIT_info[iLoopIndex].uiService_loop_length);

				jobject rstInfoBean = (*env)->NewObject(env, sitInfoBeanClass, sitInfoBeanConstrocMID, stSit.astSIT_info[iLoopIndex].uiService_id, stSit.astSIT_info[iLoopIndex].uiDVB_reserved_future_use, stSit.astSIT_info[iLoopIndex].uiRunning_status,
						stSit.astSIT_info[iLoopIndex].uiService_loop_length, descriptorBeanArray);
				(*env)->SetObjectArrayElement(env, sitInfoBeanArray, iLoopIndex, rstInfoBean);
			}

			int iDescriptorCount = GetDescriptorCountInBuffer(stSit.aucSIT_info_descriptor, stSit.uiTransmission_info_loop_length);
			jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
			ParseDescriptorToJArray(env, &descriptorBeanArray, stSit.aucSIT_info_descriptor, stSit.uiTransmission_info_loop_length);

			jclass sitBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/si/SIT_Table");
			jmethodID sitConstrocMID = (*env)->GetMethodID(env, sitBeanClass, "<init>", "(IIIIIIIIIIIII[Lcom/alex/ts_parser/bean/si/Descriptor;[Lcom/alex/ts_parser/bean/si/SitInfo;I)V");
			jobject sitBean = (*env)->NewObject(env, sitBeanClass, sitConstrocMID, stSit.uiTable_id, stSit.uiSection_syntax_indicator, stSit.uiDVB_Reserved_future_use_first, stSit.uiISO_Reserved_first, stSit.uiSection_length,
					stSit.uiDVB_Reserved_future_use_second, stSit.uiISO_Reserved_second, stSit.uiVersion_number, stSit.uiCurrent_next_indicator, stSit.uiSection_number, stSit.uiLast_section_number, stSit.uiDVB_Reserved_future_use_third,
					stSit.uiTransmission_info_loop_length, descriptorBeanArray, sitInfoBeanArray, stSit.uiCRC32);
			return sitBean;
		}
	}
}

/******************************************
 *
 * 解析描述流
 *
 ******************************************/
int ParseDescriptorToJArray(JNIEnv *env, jobjectArray *pDescriptorBeanArray, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength)
{
	if(iDescriptorBufferLength <= 0){
		return 0;
	}

	VIDEO_STREAM_DESCRIPTOR_T stVideoStreamDescriptor = { 0 };
	NETWORK_NAME_DESCRIPTOR_T stNetworkNameDescriptor = { 0 };
	AUDIO_STREAM_DESCRIPTOR_T stAudioStreamDescriptor = { 0 };
	DATA_STREAM_ALIGNMENT_DESCRIPTOR_T stDataStreamAlignmentDescriptor = { 0 };
	CA_DESCRIPTOR_T stCA_Descriptor = { 0 };
	ISO_639_LANGUAGE_DESCRIPTOR_T stISO_639_LanguageDescriptor = { 0 };
	SYSTEM_CLOCK_DESCRIPTOR_T stSystemClockDescriptor = { 0 };
	MAXIMUM_BITRATE_DESCRIPTOR_T stMaximumBitrateDescriptor = { 0 };
	SERVICE_LIST_DESCRIPTOR_T stServiceListDescriptor = { 0 };
	SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_T stSatelliteDeliverySystemDescriptor = { 0 };
	CABLE_DELIVERY_SYSTEM_DESCRIPTOR_T stCableDeliverySystemDescriptor = { 0 };
	BOUQUET_NAME_DESCRIPTOR_T stBouquetNameDescriptor = { 0 };
	SERVICE_DESCRIPTOR_T stServiceDescriptor = { 0 };
	LINKAGE_DESCRIPTOR_T stLinkageDescriptor = { 0 };
	SHORT_EVENT_DESCRIPTOR_T stShortEventDescriptor = { 0 };
	EXTENDED_EVENT_DESCRIPTOR_T stExtendedEventDescriptor = { 0 };
	STREAM_IDENTIFIER_DESCRIPTOR_T stStreamIndentifierDescriptor = { 0 };
	TELETEXT_DESCRIPTOR_T stTeletextDescriptor = { 0 };
	SUBTITLING_DESCRIPTOR_T stSubtitlingDescriptor = { 0 };
	LOCAL_TIME_OFFSET_DESCRIPTOR_T stLocalTimeOffsetDescriptor = { 0 };
	TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_T stTerrestrialDeliverySystemDescriptor = { 0 };
	FREQUENCY_LIST_DESCRIPTOR_T stFrequencyListDescriptor = { 0 };

	COMPONENT_DESCRIPTOR_T stComponentDescriptor = { 0 };
	CONTENT_DESCRIPTOR_T stContentDescriptor = { 0 };
	PARENTAL_RATING_DESCRIPTOR_T stParentalRatingDescriptor = { 0 };

	int decriptorOrderNumber = 0;
	int iTag = 0;
	int iDescriptorPosition = 0;

	jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");
	jmethodID descriptorConstrocMID = (*env)->GetMethodID(env, descriptorBeanClass, "<init>", "(II)V");
	do
	{
		iTag = GetDescriptorTag(&iTag, iDescriptorPosition, pucDescriptorBuffer, iDescriptorBufferLength);
		int iLength = pucDescriptorBuffer[1 + iDescriptorPosition];
		if (-1 == iTag)
		{
			LOG("ParseDescriptorToJArray iTag == -1 \n");
			break;
		}
		else
		{
			jobject descriptorBean;
			jobject tempObject;
			jobjectArray tempObjectArray;
			jbyteArray byteArrayData;
			int iLoopIndex = 0;
			int iLoopCount = 0;
			switch (iTag)
			{
				case VIDEO_STREAM_DESCRIPTOR_TAG:
					GetVideoStreamDescriptor(&stVideoStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass videoStreamDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/VideoStreamDescriptor");
					jmethodID videoStreamDescriptorConstrocMID = (*env)->GetMethodID(env, videoStreamDescriptorBeanClass, "<init>", "(IIIIIIIIIII)V");
					descriptorBean = (*env)->NewObject(env, videoStreamDescriptorBeanClass, videoStreamDescriptorConstrocMID, stVideoStreamDescriptor.uiDescriptor_tag, stVideoStreamDescriptor.uiDescriptor_length,
							stVideoStreamDescriptor.uiMultiple_frame_rate_flag, stVideoStreamDescriptor.uiFrame_rate_code, stVideoStreamDescriptor.uiMPEG_2_flag, stVideoStreamDescriptor.uiConstrained_parameter_flag,
							stVideoStreamDescriptor.uiStill_picture_flag, stVideoStreamDescriptor.uiProfile_and_level_indication, stVideoStreamDescriptor.uiChroma_format, stVideoStreamDescriptor.uiFrame_rate_extension_flag,
							stVideoStreamDescriptor.uiReserved);
					(*env)->DeleteLocalRef(env, videoStreamDescriptorBeanClass);
					break;
				case AUDIO_STREAM_DESCRIPTOR_TAG:
					GetAudioStreamDescriptor(&stAudioStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass audioStreamDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/AudioStreamDescriptor");
					jmethodID audioStreamDescriptorConstrocMID = (*env)->GetMethodID(env, audioStreamDescriptorBeanClass, "<init>", "(IIIIII)V");
					descriptorBean = (*env)->NewObject(env, audioStreamDescriptorBeanClass, audioStreamDescriptorConstrocMID, stAudioStreamDescriptor.uiDescriptor_tag, stAudioStreamDescriptor.uiDescriptor_length,
							stAudioStreamDescriptor.uiFree_format_flag, stAudioStreamDescriptor.uiID, stAudioStreamDescriptor.uiLayer, stAudioStreamDescriptor.uiReserved);
					(*env)->DeleteLocalRef(env, audioStreamDescriptorBeanClass);
					break;
				case DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG:
					GetDataStreamAlignmentDescriptor(&stDataStreamAlignmentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass dataStreamAlignmentDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/DataStreamAlignmentDescriptor");
					jmethodID dataStreamAlignmentDescriptorConstrocMID = (*env)->GetMethodID(env, dataStreamAlignmentDescriptorBeanClass, "<init>", "(III)V");
					descriptorBean = (*env)->NewObject(env, dataStreamAlignmentDescriptorBeanClass, dataStreamAlignmentDescriptorConstrocMID, stDataStreamAlignmentDescriptor.uiDescriptor_tag, stDataStreamAlignmentDescriptor.uiDescriptor_length,
							stDataStreamAlignmentDescriptor.uiAlignment_type);
					(*env)->DeleteLocalRef(env, dataStreamAlignmentDescriptorBeanClass);
					break;
				case CA_DESCRIPTOR_TAG:
					GetCA_Descriptor(&stCA_Descriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
					jbyteArray byteArrayData = GetJByteArrayByUChar(env, stCA_Descriptor.aucPrivate_data_byte, stCA_Descriptor.uiDescriptor_length - 4);
					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag, stCA_Descriptor.uiDescriptor_length, stCA_Descriptor.uiCA_system_ID, stCA_Descriptor.uiReserved,
							stCA_Descriptor.uiCA_PID, byteArrayData);
					(*env)->DeleteLocalRef(env, caDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, byteArrayData);
					break;
				case ISO_639_LANGUAGE_DESCRIPTOR_TAG:
					GetISO_639_Language_Descriptor(&stISO_639_LanguageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass iso_639_LanguageDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ISO_639_Language_Descriptor");
					jmethodID iso_639_LanguageDescriptorConstrocMID = (*env)->GetMethodID(env, iso_639_LanguageDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;I)V");
					int iso639LanguageCodeCount = (stISO_639_LanguageDescriptor.uiDescriptor_length - 1) / 3;
					jobjectArray iso639LanguageCodeArray = GetISO639LanguageCodeBeanArray(env, stISO_639_LanguageDescriptor.astISO_639_Language_code, iso639LanguageCodeCount);
					descriptorBean = (*env)->NewObject(env, iso_639_LanguageDescriptorBeanClass, iso_639_LanguageDescriptorConstrocMID, stISO_639_LanguageDescriptor.uiDescriptor_tag, stISO_639_LanguageDescriptor.uiDescriptor_length,
							iso639LanguageCodeArray, stISO_639_LanguageDescriptor.uiAudio_type);
					(*env)->DeleteLocalRef(env, iso_639_LanguageDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, iso639LanguageCodeArray);
					break;
				case SYSTEM_CLOCK_DESCRIPTOR_TAG:
					GetSystemClockDescriptor(&stSystemClockDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass systemClockDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/SystemClockDescriptor");
					jmethodID systemClockDescriptorConstrocMID = (*env)->GetMethodID(env, systemClockDescriptorBeanClass, "<init>", "(IIIIIII)V");
					descriptorBean = (*env)->NewObject(env, systemClockDescriptorBeanClass, systemClockDescriptorConstrocMID, stSystemClockDescriptor.uiDescriptor_tag, stSystemClockDescriptor.uiDescriptor_length,
							stSystemClockDescriptor.uiExternal_clock_reference_indicator, stSystemClockDescriptor.uiReserved_first, stSystemClockDescriptor.uiClock_accuracy_integer, stSystemClockDescriptor.uiClock_accuracy_exponent,
							stSystemClockDescriptor.uiReserved_second);
					(*env)->DeleteLocalRef(env, systemClockDescriptorBeanClass);
					break;
				case MAXIMUM_BITRATE_DESCRIPTOR_TAG:
					GetMaximumBitrateDescriptor(&stMaximumBitrateDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass maximumBitrateDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/MaximumBitrateDescriptor");
					jmethodID maximumBitrateDescriptorConstrocMID = (*env)->GetMethodID(env, maximumBitrateDescriptorBeanClass, "<init>", "(IIII)V");
					descriptorBean = (*env)->NewObject(env, maximumBitrateDescriptorBeanClass, maximumBitrateDescriptorConstrocMID, stMaximumBitrateDescriptor.uiDescriptor_tag, stMaximumBitrateDescriptor.uiDescriptor_length,
							stMaximumBitrateDescriptor.uiReserved, stMaximumBitrateDescriptor.uiMaximum_bitrate);
					(*env)->DeleteLocalRef(env, maximumBitrateDescriptorBeanClass);
					break;
				case NETWORK_NAME_DESCRIPTOR_TAG:
					GetNetworkNameDescriptor(&stNetworkNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass networkNameDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/NetworkNameDescriptor");
					jmethodID networkNameDescriptorConstrocMID = (*env)->GetMethodID(env, networkNameDescriptorBeanClass, "<init>", "(II[B)V");
					byteArrayData = GetJByteArrayByUChar(env, stNetworkNameDescriptor.aucInfo, stNetworkNameDescriptor.uiDescriptor_length);
					descriptorBean = (*env)->NewObject(env, networkNameDescriptorBeanClass, networkNameDescriptorConstrocMID, stNetworkNameDescriptor.uiDescriptor_tag, stNetworkNameDescriptor.uiDescriptor_length, byteArrayData);
					(*env)->DeleteLocalRef(env, networkNameDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, byteArrayData);
					break;
				case SERVICE_LIST_DESCRIPTOR_TAG:
					GetServiceListDescriptor(&stServiceListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);

					jclass serviceInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ServiceInfo");
					jmethodID serviceInfoConstrocMID = (*env)->GetMethodID(env, serviceInfoBeanClass, "<init>", "(II)V");
					iLoopCount = stServiceListDescriptor.uiDescriptor_length / 3;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, serviceInfoBeanClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						tempObject = (*env)->NewObject(env, serviceInfoBeanClass, serviceInfoConstrocMID, stServiceListDescriptor.astService_info[iLoopIndex].uiServiec_id, stServiceListDescriptor.astService_info[iLoopIndex].uiService_type);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, tempObject);
					}
					(*env)->DeleteLocalRef(env, serviceInfoBeanClass);

					jclass serviceListDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ServiceListDescriptor");
					jmethodID serviceListDescriptorConstrocMID = (*env)->GetMethodID(env, serviceListDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/ServiceInfo;)V");
					descriptorBean = (*env)->NewObject(env, serviceListDescriptorBeanClass, serviceListDescriptorConstrocMID, stServiceListDescriptor.uiDescriptor_tag, stServiceListDescriptor.uiDescriptor_length, tempObjectArray);
					(*env)->DeleteLocalRef(env, serviceListDescriptorBeanClass);
					break;
				case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetSatelliteDeliverySystemDescriptor(&stSatelliteDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass satelliteDeliverySystemDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/SatelliteDeliverySystemDescriptor");
					jmethodID satelliteDeliverySystemDescriptorConstrocMID = (*env)->GetMethodID(env, satelliteDeliverySystemDescriptorBeanClass, "<init>", "(IIIIIIIIIII)V");
					descriptorBean = (*env)->NewObject(env, satelliteDeliverySystemDescriptorBeanClass, satelliteDeliverySystemDescriptorConstrocMID, stSatelliteDeliverySystemDescriptor.uiDescriptor_tag,
							stSatelliteDeliverySystemDescriptor.uiDescriptor_length, stSatelliteDeliverySystemDescriptor.uiFrequency, stSatelliteDeliverySystemDescriptor.uiOrbital_position, stSatelliteDeliverySystemDescriptor.uiWest_east_flag,
							stSatelliteDeliverySystemDescriptor.uiPolarization, stSatelliteDeliverySystemDescriptor.uiRoll_off, stSatelliteDeliverySystemDescriptor.uiModulation_system, stSatelliteDeliverySystemDescriptor.uiModulation_type,
							stSatelliteDeliverySystemDescriptor.uiSymbol_rate, stSatelliteDeliverySystemDescriptor.uiFEC_inner);
					(*env)->DeleteLocalRef(env, satelliteDeliverySystemDescriptorBeanClass);
					break;
				case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetCableDeliverySystemDescriptor(&stCableDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass cableDeliverySystemDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CableDeliverySystemDescriptor");
					jmethodID cableDeliverySystemDescriptorConstrocMID = (*env)->GetMethodID(env, cableDeliverySystemDescriptorBeanClass, "<init>", "(IIIIIIII)V");
					descriptorBean = (*env)->NewObject(env, cableDeliverySystemDescriptorBeanClass, cableDeliverySystemDescriptorConstrocMID, stCableDeliverySystemDescriptor.uiDescriptor_tag, stCableDeliverySystemDescriptor.uiDescriptor_length,
							stCableDeliverySystemDescriptor.uiFrequency, stCableDeliverySystemDescriptor.uiReserved_future_use, stCableDeliverySystemDescriptor.uiFec_outer, stCableDeliverySystemDescriptor.uiModulation,
							stCableDeliverySystemDescriptor.uiSymbol_rate, stCableDeliverySystemDescriptor.uiFEC_inner);
					(*env)->DeleteLocalRef(env, cableDeliverySystemDescriptorBeanClass);
					break;
				case SERVICE_DESCRIPTOR_TAG:
					GetServiceDescriptor(&stServiceDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass serviceDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ServiceDescriptor");
					jbyteArray providerNameData = GetJByteArrayByUChar(env, stServiceDescriptor.aucService_provider_name, stServiceDescriptor.uiService_provider_name_length);
					jbyteArray serviceNameData = GetJByteArrayByUChar(env, stServiceDescriptor.aucService_name, stServiceDescriptor.uiService_name_length);
					jmethodID serviceDescriptorConstrocMID = (*env)->GetMethodID(env, serviceDescriptorBeanClass, "<init>", "(IIII[BI[B)V");
					descriptorBean = (*env)->NewObject(env, serviceDescriptorBeanClass, serviceDescriptorConstrocMID, stServiceDescriptor.uiDescriptor_tag, stServiceDescriptor.uiDescriptor_length, stServiceDescriptor.uiService_type,
							stServiceDescriptor.uiService_provider_name_length, providerNameData, stServiceDescriptor.uiService_name_length, serviceNameData);
					(*env)->DeleteLocalRef(env, serviceDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, providerNameData);
					(*env)->DeleteLocalRef(env, serviceNameData);
					break;
				case LINKAGE_DESCRIPTOR_TAG:
					GetLinkageDescriptor(&stLinkageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass linkageDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/LinkageDescriptor");
					if (0x08 == stLinkageDescriptor.uiLinage_type)
					{
						jmethodID linkageDescriptor_08_ConstrocMID = (*env)->GetMethodID(env, linkageDescriptorBeanClass, "<init>", "(IIIIIIIIIII[B)V");
						LINKAGE_UNION_08_T stUnion08 = stLinkageDescriptor.stLinkage_info_union.stLinkage_union_08;
						byteArrayData = GetJByteArrayByUChar(env, stLinkageDescriptor.aucPrivate_data, stLinkageDescriptor.uiDescriptor_length - 7 - 5);
						descriptorBean = (*env)->NewObject(env, linkageDescriptorBeanClass, linkageDescriptor_08_ConstrocMID, stLinkageDescriptor.uiDescriptor_tag, stLinkageDescriptor.uiDescriptor_length, stLinkageDescriptor.uiTransport_stream_id,
								stLinkageDescriptor.uiOriginal_network_id, stLinkageDescriptor.uiService_id, stLinkageDescriptor.uiLinage_type, stUnion08.uiHand_over_type, stUnion08.uiReserved_future_use, stUnion08.uiOrigin_type, stUnion08.uiNetwork_id,
								stUnion08.uiInitial_service_id, byteArrayData);
						(*env)->DeleteLocalRef(env, byteArrayData);
					}
					else if (0x0D == stLinkageDescriptor.uiLinage_type)
					{
						jmethodID linkageDescriptor_0d_ConstrocMID = (*env)->GetMethodID(env, linkageDescriptorBeanClass, "<init>", "(IIIIIIIIII[B)V");
						LINKAGE_UNION_0D_T stUnion0d = stLinkageDescriptor.stLinkage_info_union.stLinkage_union_0d;
						byteArrayData = GetJByteArrayByUChar(env, stLinkageDescriptor.aucPrivate_data, stLinkageDescriptor.uiDescriptor_length - 7 - 3);
						descriptorBean = (*env)->NewObject(env, linkageDescriptorBeanClass, linkageDescriptor_0d_ConstrocMID, stLinkageDescriptor.uiDescriptor_tag, stLinkageDescriptor.uiDescriptor_length, stLinkageDescriptor.uiTransport_stream_id,
								stLinkageDescriptor.uiOriginal_network_id, stLinkageDescriptor.uiService_id, stLinkageDescriptor.uiLinage_type, stUnion0d.uiTarget_event_id, stUnion0d.uiTarget_listed, stUnion0d.uiEvent_simulcast, stUnion0d.uiReserved,
								byteArrayData);
						(*env)->DeleteLocalRef(env, byteArrayData);
					}
					else
					{
						jmethodID linkageDescriptor_Defualt_ConstrocMID = (*env)->GetMethodID(env, linkageDescriptorBeanClass, "<init>", "(IIIIII[B)V");
						byteArrayData = GetJByteArrayByUChar(env, stLinkageDescriptor.aucPrivate_data, stLinkageDescriptor.uiDescriptor_length - 7);
						descriptorBean = (*env)->NewObject(env, linkageDescriptorBeanClass, linkageDescriptor_Defualt_ConstrocMID, stLinkageDescriptor.uiDescriptor_tag, stLinkageDescriptor.uiDescriptor_length, stLinkageDescriptor.uiTransport_stream_id,
								stLinkageDescriptor.uiOriginal_network_id, stLinkageDescriptor.uiService_id, stLinkageDescriptor.uiLinage_type, byteArrayData);
						(*env)->DeleteLocalRef(env, byteArrayData);
					}
					break;
				case SHORT_EVENT_DESCRIPTOR_TAG:
					GetShortEventDescriptor(&stShortEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass shortEventDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ShortEventDescriptor");
					jmethodID shortEventDescriptorConstrocMID = (*env)->GetMethodID(env, shortEventDescriptorBeanClass, "<init>", "(IILcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;I[BI[B)V");

					jobject iso639LanguageCodeBean = GetISO639LanguageCodeBean(env, &stShortEventDescriptor.stISO_639_Language_code);
					byteArrayData = GetJByteArrayByUChar(env, stShortEventDescriptor.aucEvent_name_char, stShortEventDescriptor.uiEvent_name_length);
					jbyteArray textCharArray = GetJByteArrayByUChar(env, stShortEventDescriptor.aucText_char, stShortEventDescriptor.uiText_length);

					descriptorBean = (*env)->NewObject(env, shortEventDescriptorBeanClass, shortEventDescriptorConstrocMID, stShortEventDescriptor.uiDescriptor_tag, stShortEventDescriptor.uiDescriptor_length, iso639LanguageCodeBean,
							stShortEventDescriptor.uiEvent_name_length, byteArrayData, stShortEventDescriptor.uiText_length, textCharArray);
					(*env)->DeleteLocalRef(env, shortEventDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, iso639LanguageCodeBean);
					(*env)->DeleteLocalRef(env, byteArrayData);
					(*env)->DeleteLocalRef(env, textCharArray);
					break;
				case EXTENDED_EVENT_DESCRIPTOR_TAG:
					GetExtendedEventDescriptor(&stExtendedEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);

					jobject iso639LanguageCodeBeanObject = GetISO639LanguageCodeBean(env, &stExtendedEventDescriptor.stISO_639_language_code);
					jclass extendedEventInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ExtendedEventInfo");
					jmethodID extendedEventInfoConstrocMID = (*env)->GetMethodID(env, extendedEventInfoBeanClass, "<init>", "(I[BI[B)V");
					//TODO 去除iInfoCount 成员
					iLoopCount = stExtendedEventDescriptor.iInfoCount;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, extendedEventInfoBeanClass, NULL);
					// todo 这段代码有错误
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						EXTENDED_EVENT_INFO_T stExtendedEventInfo = stExtendedEventDescriptor.astExtended_event_info[iLoopIndex];
						tempObject = (*env)->NewObject(env, extendedEventInfoBeanClass, extendedEventInfoConstrocMID, stExtendedEventInfo.uiItem_descriptor_length, NULL, stExtendedEventInfo.uiItem_length, NULL);
						jbyteArray itemDescriptorCharData = GetJByteArrayByUChar(env, stExtendedEventInfo.aucItem_descriptor_char, stExtendedEventInfo.uiItem_descriptor_length);
						jbyteArray itemCharData = GetJByteArrayByUChar(env, stExtendedEventInfo.aucItem_char, stExtendedEventInfo.uiItem_length);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, itemDescriptorCharData);
						(*env)->DeleteLocalRef(env, itemCharData);
					}
					(*env)->DeleteLocalRef(env, extendedEventInfoBeanClass);

					byteArrayData = GetJByteArrayByUChar(env, stExtendedEventDescriptor.aucText_char, stExtendedEventDescriptor.uiText_length);

					jclass extendedEventDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ExtendedEventDescriptor");
					jmethodID extendedEventDescriptorConstrocMID = (*env)->GetMethodID(env, extendedEventDescriptorBeanClass, "<init>",
							"(IIIILcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;I[Lcom/alex/ts_parser/bean/descriptor/ExtendedEventInfo;I[B)V");
					descriptorBean = (*env)->NewObject(env, extendedEventDescriptorBeanClass, extendedEventDescriptorConstrocMID, stExtendedEventDescriptor.uiDescriptor_tag, stExtendedEventDescriptor.uiDescriptor_length,
							stExtendedEventDescriptor.uiDescriptor_number, stExtendedEventDescriptor.uiLast_descriptor_number, iso639LanguageCodeBeanObject, stExtendedEventDescriptor.uiLength_of_items, tempObjectArray,
							stExtendedEventDescriptor.uiText_length, byteArrayData);
					(*env)->DeleteLocalRef(env, iso639LanguageCodeBeanObject);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, extendedEventDescriptorBeanClass);
					(*env)->DeleteLocalRef(env, byteArrayData);
					break;
				case STREAM_IDENTIFIER_DESCRIPTOR_TAG:
					GetStreamIndentifierDescriptor(&stStreamIndentifierDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass streamIndentifierDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/StreamIndentifierDescriptor");
					jmethodID streamIndentifierDescriptorConstrocMID = (*env)->GetMethodID(env, streamIndentifierDescriptorBeanClass, "<init>", "(III)V");
					descriptorBean = (*env)->NewObject(env, streamIndentifierDescriptorBeanClass, streamIndentifierDescriptorConstrocMID, stStreamIndentifierDescriptor.uiDescriptor_tag, stStreamIndentifierDescriptor.uiDescriptor_length,
							stStreamIndentifierDescriptor.uiComponent_tag);
					(*env)->DeleteLocalRef(env, streamIndentifierDescriptorBeanClass);
					break;
				case TELETEXT_DESCRIPTOR_TAG:
					GetTeletextDescriptor(&stTeletextDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);

					jclass teletextInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/TeletextInfo");
					jmethodID teletextInfoConstrocMID = (*env)->GetMethodID(env, teletextInfoBeanClass, "<init>", "(Lcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;III)V");
					iLoopCount = stTeletextDescriptor.uiDescriptor_length / 5;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, teletextInfoBeanClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						jobject iso639LanguageCodeBean = GetISO639LanguageCodeBean(env, &stTeletextDescriptor.astTeletext_Info[iLoopIndex].stISO_639_Language_code);
						tempObject = (*env)->NewObject(env, teletextInfoBeanClass, teletextInfoConstrocMID, iso639LanguageCodeBean, stTeletextDescriptor.astTeletext_Info[iLoopIndex].uiTeletext_type,
								stTeletextDescriptor.astTeletext_Info[iLoopIndex].uiTeletext_magazine_number, stTeletextDescriptor.astTeletext_Info[iLoopIndex].uiTeletext_page_number);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, tempObject);
					}

					jclass teletextDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/TeletextDescriptor");
					jmethodID teletextDescriptorConstrocMID = (*env)->GetMethodID(env, teletextDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/TeletextInfo;)V");
					descriptorBean = (*env)->NewObject(env, teletextDescriptorBeanClass, teletextDescriptorConstrocMID, stTeletextDescriptor.uiDescriptor_tag, stTeletextDescriptor.uiDescriptor_length, tempObjectArray);
					(*env)->DeleteLocalRef(env, teletextInfoBeanClass);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, teletextDescriptorBeanClass);
					break;
				case LOCAL_TIME_OFFSET_DESCRIPTOR_TAG:
					GetLocalTimeOffsetDescriptor(&stLocalTimeOffsetDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);

					jclass localTimeOffsetInfoClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/LocalTimeOffsetDescriptorInfo");
					jmethodID localTimeOffsetInfoConstrocMID = (*env)->GetMethodID(env, localTimeOffsetInfoClass, "<init>", "([BIIII[II)V");
					iLoopCount = stLocalTimeOffsetDescriptor.uiDescriptor_length / 13;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, localTimeOffsetInfoClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						LOCAL_TIME_OFFSET_INFO_T stLocalTimeOffsetInfo = stLocalTimeOffsetDescriptor.astLocalTimeOffset_Info[iLoopIndex];
						byteArrayData = GetJByteArrayByUChar(env, stLocalTimeOffsetInfo.uiCountry_code, 3);
						jintArray jintArray = GetJintArrayFromIntArray(env, stLocalTimeOffsetInfo.auiTime_of_change, 5);
						tempObject = (*env)->NewObject(env, localTimeOffsetInfoClass, localTimeOffsetInfoConstrocMID, byteArrayData, stLocalTimeOffsetInfo.uiCountry_region_id, stLocalTimeOffsetInfo.uiReserved,
								stLocalTimeOffsetInfo.uiLocal_time_offset_polarity, stLocalTimeOffsetInfo.uiLocal_time_offset, jintArray, stLocalTimeOffsetInfo.uiNext_time_offset);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, byteArrayData);
						(*env)->DeleteLocalRef(env, jintArray);
						(*env)->DeleteLocalRef(env, tempObject);
					}

					jclass localTimeOffsetDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/LocalTimeOffsetDescriptor");
					jmethodID localTimeOffsetDescriptorConstrocMID = (*env)->GetMethodID(env, localTimeOffsetDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/LocalTimeOffsetDescriptorInfo;)V");
					descriptorBean = (*env)->NewObject(env, localTimeOffsetDescriptorBeanClass, localTimeOffsetDescriptorConstrocMID, stLocalTimeOffsetDescriptor.uiDescriptor_tag, stLocalTimeOffsetDescriptor.uiDescriptor_length, tempObjectArray);
					(*env)->DeleteLocalRef(env, localTimeOffsetInfoClass);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, localTimeOffsetDescriptorBeanClass);
					break;
				case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetTerrestrialDeliverySystemDescriptor(&stTerrestrialDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass terrestrialDeliverySystemDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/TerrestrialDeliverySystemDescriptor");
					jmethodID terrestrialDeliverySystemDescriptorConstrocMID = (*env)->GetMethodID(env, terrestrialDeliverySystemDescriptorBeanClass, "<init>", "(IIIIIIIIIIIIIIII)V");
					descriptorBean = (*env)->NewObject(env, terrestrialDeliverySystemDescriptorBeanClass, terrestrialDeliverySystemDescriptorConstrocMID, stTerrestrialDeliverySystemDescriptor.uiDescriptor_tag,
							stTerrestrialDeliverySystemDescriptor.uiDescriptor_length, stTerrestrialDeliverySystemDescriptor.uiCentre_frequency, stTerrestrialDeliverySystemDescriptor.uiBandwidth, stTerrestrialDeliverySystemDescriptor.uiPriority,
							stTerrestrialDeliverySystemDescriptor.uiTime_Slicing_indicator, stTerrestrialDeliverySystemDescriptor.uiMPE_FEC_indicator, stTerrestrialDeliverySystemDescriptor.uiReserved_future_use_first,
							stTerrestrialDeliverySystemDescriptor.uiConstellation, stTerrestrialDeliverySystemDescriptor.uiHierarchy_information, stTerrestrialDeliverySystemDescriptor.uiCode_rate_HP_stream,
							stTerrestrialDeliverySystemDescriptor.uiCode_rate_LP_stream, stTerrestrialDeliverySystemDescriptor.uiGuard_interval, stTerrestrialDeliverySystemDescriptor.uiTransmission_mode,
							stTerrestrialDeliverySystemDescriptor.uiOther_frequency_flag, stTerrestrialDeliverySystemDescriptor.uiReserved_future_use_second);
					(*env)->DeleteLocalRef(env, terrestrialDeliverySystemDescriptorBeanClass);
					break;
				case FREQUENCY_LIST_DESCRIPTOR_TAG:
					GetFrequencyListDescriptor(&stFrequencyListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);

					jclass frequencyListInfoBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/FrequencyListInfo");
					jmethodID frequencyListInfoConstrocMID = (*env)->GetMethodID(env, frequencyListInfoBeanClass, "<init>", "(I)V");
					iLoopCount = (stFrequencyListDescriptor.uiDescriptor_length - 1) / 4;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, frequencyListInfoBeanClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						tempObject = (*env)->NewObject(env, frequencyListInfoBeanClass, frequencyListInfoConstrocMID, stFrequencyListDescriptor.astCentre_frequency[iLoopIndex].uiCentre_frequency);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
					}
					(*env)->DeleteLocalRef(env, frequencyListInfoBeanClass);

					jclass frequencyListDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/FrequencyListDescriptor");
					jmethodID frequencyListDescriptorConstrocMID = (*env)->GetMethodID(env, frequencyListDescriptorBeanClass, "<init>", "(IIII[Lcom/alex/ts_parser/bean/descriptor/FrequencyListInfo;)V");
					descriptorBean = (*env)->NewObject(env, frequencyListDescriptorBeanClass, frequencyListDescriptorConstrocMID, stFrequencyListDescriptor.uiDescriptor_tag, stFrequencyListDescriptor.uiDescriptor_length,
							stFrequencyListDescriptor.uiReserved_future_use, stFrequencyListDescriptor.uiCoding_type, tempObjectArray);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, frequencyListDescriptorBeanClass);
					break;
//				case SUBTITLING_DESCRIPTOR_TAG:
//					GetSubtitlingDescriptor(&stSubtitlingDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					(*env)->DeleteLocalRef(env, caDescriptorBeanClass);
//					break;
//				case BOUQUET_NAME_DESCRIPTOR_TAG:
//					GetBouquetNameDescriptor(&stBouquetNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
				case COMPONENT_DESCRIPTOR_TAG:
					GetComponentDescriptor(&stComponentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					tempObject = GetISO639LanguageCodeBean(env, &stComponentDescriptor.stISO_639_language_code);
					byteArrayData = GetJByteArrayByUChar(env, stComponentDescriptor.aucText_char, stComponentDescriptor.uiDescriptor_length - 6);

					jclass componentDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ComponentDescriptor");
					jmethodID componentDescriptorConstrocMID = (*env)->GetMethodID(env, componentDescriptorBeanClass, "<init>", "(IIIIIILcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;[B)V");
					descriptorBean = (*env)->NewObject(env, componentDescriptorBeanClass, componentDescriptorConstrocMID, stComponentDescriptor.uiDescriptor_tag, stComponentDescriptor.uiDescriptor_length, stComponentDescriptor.uiReserved_future_use,
							stComponentDescriptor.uiStream_content, stComponentDescriptor.uiComponent_type, stComponentDescriptor.uiComponent_tag, tempObject, byteArrayData);
					(*env)->DeleteLocalRef(env, tempObject);
					(*env)->DeleteLocalRef(env, byteArrayData);
					(*env)->DeleteLocalRef(env, componentDescriptorBeanClass);
					break;
				case CONTENT_DESCRIPTOR_TAG:
					GetContentDescriptor(&stContentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass contentInfoClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ContentInfo");
					jmethodID contentInfoConstrocMID = (*env)->GetMethodID(env, contentInfoClass, "<init>", "(III)V");
					iLoopCount = stContentDescriptor.uiDescriptor_length / 2;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, contentInfoClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						CONTENT_INFO_T stContentInfo = stContentDescriptor.astContent_info[iLoopIndex];
						tempObject = (*env)->NewObject(env, contentInfoClass, contentInfoConstrocMID, stContentInfo.uiContent_nibble_level_1, stContentInfo.uiContent_nibble_level_2, stContentInfo.uiUser_byte);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, tempObject);
					}
					(*env)->DeleteLocalRef(env, contentInfoClass);

					jclass contentDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ContentDescriptor");
					jmethodID contentDescriptorConstrocMID = (*env)->GetMethodID(env, contentDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/ContentInfo;)V");
					descriptorBean = (*env)->NewObject(env, contentDescriptorBeanClass, contentDescriptorConstrocMID, stContentDescriptor.uiDescriptor_tag, stContentDescriptor.uiDescriptor_length, tempObjectArray);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, contentDescriptorBeanClass);
					break;
				case PARENTAL_RATING_DESCRIPTOR_TAG:
					GetParentalRatingDescriptor(&stParentalRatingDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass parentalRatingInfoClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ParentalRatingInfo");
					jmethodID parentalRatingInfoConstrocMID = (*env)->GetMethodID(env, parentalRatingInfoClass, "<init>", "([BI)V");
					iLoopCount = stParentalRatingDescriptor.uiDescriptor_length / 2;
					tempObjectArray = (*env)->NewObjectArray(env, iLoopCount, parentalRatingInfoClass, NULL);
					for (iLoopIndex = 0; iLoopIndex < iLoopCount; iLoopIndex++)
					{
						PARENTAL_RATING_INFO_T stParentalRatingInfo = stParentalRatingDescriptor.astParental_rating_info[iLoopIndex];
						byteArrayData = GetJByteArrayByUChar(env, stParentalRatingInfo.uiCountry_code, 3);
						tempObject = (*env)->NewObject(env, parentalRatingInfoClass, parentalRatingInfoConstrocMID, byteArrayData, stParentalRatingInfo.uiRating);
						(*env)->SetObjectArrayElement(env, tempObjectArray, iLoopIndex, tempObject);
						(*env)->DeleteLocalRef(env, tempObject);
					}
					(*env)->DeleteLocalRef(env, parentalRatingInfoClass);

					jclass parentalRatingDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ParentalRatingDescriptor");
					jmethodID parentalRatingDescriptorConstrocMID = (*env)->GetMethodID(env, parentalRatingDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/ParentalRatingInfo;)V");
					descriptorBean = (*env)->NewObject(env, parentalRatingDescriptorBeanClass, parentalRatingDescriptorConstrocMID, stParentalRatingDescriptor.uiDescriptor_tag, stParentalRatingDescriptor.uiDescriptor_length, tempObjectArray);
					(*env)->DeleteLocalRef(env, tempObjectArray);
					(*env)->DeleteLocalRef(env, parentalRatingDescriptorBeanClass);
					break;
				default:
					descriptorBean = (*env)->NewObject(env, descriptorBeanClass, descriptorConstrocMID, iTag, iLength);
//					LOG("unKnownTag: %d; iDescriptorPosition: %d\n", iTag, iDescriptorPosition);
					break;
			}
			(*env)->SetObjectArrayElement(env, (*pDescriptorBeanArray), decriptorOrderNumber, descriptorBean);
			(*env)->DeleteLocalRef(env, descriptorBean);
			iDescriptorPosition += pucDescriptorBuffer[iDescriptorPosition + 1] + 2;
			decriptorOrderNumber++;
		}
	}
	while (iDescriptorPosition < iDescriptorBufferLength);
	return 1;
}

/******************************************
 *
 * 字符串转换为jbyteArray
 *
 ******************************************/
jbyteArray GetJByteArrayByUChar(JNIEnv *env, unsigned char * pucBuffer, int length)
{
	if (length <= 0)
	{
		return NULL;
	}
	jbyteArray privateDateByteArray = (*env)->NewByteArray(env, length);
	(*env)->SetByteArrayRegion(env, privateDateByteArray, 0, length, (jbyte*) pucBuffer);
	(*env)->ReleaseByteArrayElements(env, privateDateByteArray, (jbyte*) pucBuffer, JNI_COMMIT);
	return privateDateByteArray;
}

/******************************************************
 *
 * ISO639LanguageCode字符串转换为ISO639LanguageCode java类
 *
 ******************************************************/
jobject GetISO639LanguageCodeBean(JNIEnv *env, ISO_639_LANGUAGE_CODE_T *pstISO_639_Language_code)
{
	jclass iso639LanguageCodeBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode");
	jmethodID iso639LanguageCodeConstrocMID = (*env)->GetMethodID(env, iso639LanguageCodeBeanClass, "<init>", "([B)V");
	jbyteArray privateDateByteArray = GetJByteArrayByUChar(env, pstISO_639_Language_code->aucPrivate_data_byte, 3);
	jobject iso639LanguageCodeBean = (*env)->NewObject(env, iso639LanguageCodeBeanClass, iso639LanguageCodeConstrocMID, privateDateByteArray);
	return iso639LanguageCodeBean;
}

/******************************************************
 *
 * ISO639LanguageCode数组转换为ISO639LanguageCode java类数组
 *
 ******************************************************/
jobjectArray GetISO639LanguageCodeBeanArray(JNIEnv *env, ISO_639_LANGUAGE_CODE_T pastISO_639_Language_CodeArray[], int iso639LanguageCodeCount)
{
	jclass iso639LanguageCodeBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode");
	jobjectArray iso639LanguageCodeArray = (*env)->NewObjectArray(env, iso639LanguageCodeCount, iso639LanguageCodeBeanClass, NULL);
	int index = 0;
	for (index = 0; index < iso639LanguageCodeCount; index++)
	{
		jobject iso639LanguageCodeBean = GetISO639LanguageCodeBean(env, &pastISO_639_Language_CodeArray[index]);
		(*env)->SetObjectArrayElement(env, iso639LanguageCodeArray, index, iso639LanguageCodeBean);
	}
	return iso639LanguageCodeArray;
}

/******************************************************
 *
 * 传入文件路径，返回文件指针
 *
 ******************************************************/
FILE* GetFilePointer(char acFilePath[])
{
	FILE *pfTsFile = NULL;
	pfTsFile = fopen(acFilePath, "rb");
	if (NULL == pfTsFile)
	{
		LOG("file does not exist \n");
	}
	return pfTsFile;
}

/******************************************************
 *
 * String to char Pointer
 *
 ******************************************************/
char* Jstring2CharPointer(JNIEnv *env, jstring filePath)
{
	char* pcFilePath = NULL;
	jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jstring strencode = (*env)->NewStringUTF(env, "GB2312");
	jbyteArray barr = (jbyteArray) (*env)->CallObjectMethod(env, filePath, mid, strencode);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	jsize alen = (*env)->GetArrayLength(env, barr);
	if (alen > 0)
	{
		pcFilePath = (char*) malloc(alen + 1);
		memcpy(pcFilePath, ba, alen);
		pcFilePath[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);
	return pcFilePath;
}
/******************************************************
 *
 * 转换int数组到jint数组
 *
 ******************************************************/
jintArray GetJintArrayFromIntArray(JNIEnv *env, unsigned int intArray[], int iArrayLength)
{
	jint intTempArray[iArrayLength];
	jintArray rtnArray = (*env)->NewIntArray(env, iArrayLength);
	int iLoopIndex = 0;
	for (iLoopIndex = 0; iLoopIndex < iArrayLength; iLoopIndex++)
	{
		intTempArray[iLoopIndex] = intArray[iLoopIndex];
	}
	(*env)->SetIntArrayRegion(env, rtnArray, 0, iArrayLength, intTempArray);
	return rtnArray;
}
