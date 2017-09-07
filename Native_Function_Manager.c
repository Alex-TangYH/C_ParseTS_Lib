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
#include "C_SourceCode/Parse_DesciptorStream.h"
#include "C_SourceCode/Print_Descriptor.h"
#include "C_SourceCode/DescriptorTag.h"

jbyteArray GetJByteArrayByUChar(JNIEnv *env, unsigned char * pucBuffer, int length);
int ParseDescriptorToJArray(JNIEnv *env, jobjectArray *pDescriptorBeanArray, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
//解析CAT
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseCAT(JNIEnv *env, jclass obj)
{
	jclass catBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/psi/CAT_Table");
	jmethodID defalutConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "()V");
	jmethodID catConstrocMID = (*env)->GetMethodID(env, catBeanClass, "<init>", "(IIIIIIIIII[Lcom/alex/ts_parser/bean/descriptor/Descriptor;I)V");

	jclass descriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/Descriptor");

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
	int iDescriptorCount = GetDescriptorCountInBuffer(stCAT.aucDescriptor, stCAT.uiSection_length - 9);
	jobjectArray descriptorBeanArray = (*env)->NewObjectArray(env, iDescriptorCount, descriptorBeanClass, NULL);
	ParseDescriptorToJArray(env, &descriptorBeanArray, stCAT.aucDescriptor, stCAT.uiSection_length - 9);
	jobject catBean = (*env)->NewObject(env, catBeanClass, catConstrocMID, stCAT.uiTable_id, stCAT.uiSection_syntax_indicator, stCAT.uiZero, stCAT.uiReserved_first, stCAT.uiSection_length, stCAT.uiReserved_second, stCAT.uiVersion_number,
			stCAT.uiCurrent_next_indicator, stCAT.uiSection_number, stCAT.uiLast_section_number, descriptorBeanArray, stCAT.uiCRC_32); //需要增加参数
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

/******************************************
 *
 * 解析描述流
 *
 ******************************************/
int ParseDescriptorToJArray(JNIEnv *env, jobjectArray *pDescriptorBeanArray, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength)
{
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
			switch (iTag)
			{
				case VIDEO_STREAM_DESCRIPTOR_TAG:
					GetVideoStreamDescriptor(&stVideoStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case AUDIO_STREAM_DESCRIPTOR_TAG:
					GetAudioStreamDescriptor(&stAudioStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG:
					GetDataStreamAlignmentDescriptor(&stDataStreamAlignmentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case CA_DESCRIPTOR_TAG:
					GetCA_Descriptor(&stCA_Descriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
					jbyteArray private_data = GetJByteArrayByUChar(env, stCA_Descriptor.aucPrivate_data_byte, stCA_Descriptor.uiDescriptor_length - 4);
					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag, stCA_Descriptor.uiDescriptor_length, stCA_Descriptor.uiCA_system_ID, stCA_Descriptor.uiReserved,
							stCA_Descriptor.uiCA_PID, private_data);
					break;
				case ISO_639_LANGUAGE_DESCRIPTOR_TAG:
					GetISO_639_Language_Descriptor(&stISO_639_LanguageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SYSTEM_CLOCK_DESCRIPTOR_TAG:
					GetSystemClockDescriptor(&stSystemClockDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case MAXIMUM_BITRATE_DESCRIPTOR_TAG:
					GetMaximumBitrateDescriptor(&stMaximumBitrateDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case NETWORK_NAME_DESCRIPTOR_TAG:
					GetNetworkNameDescriptor(&stNetworkNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SERVICE_LIST_DESCRIPTOR_TAG:
					GetServiceListDescriptor(&stServiceListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetSatelliteDeliverySystemDescriptor(&stSatelliteDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetCableDeliverySystemDescriptor(&stCableDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case BOUQUET_NAME_DESCRIPTOR_TAG:
					GetBouquetNameDescriptor(&stBouquetNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SERVICE_DESCRIPTOR_TAG:
					GetServiceDescriptor(&stServiceDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case LINKAGE_DESCRIPTOR_TAG:
					GetLinkageDescriptor(&stLinkageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SHORT_EVENT_DESCRIPTOR_TAG:
					GetShortEventDescriptor(&stShortEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case EXTENDED_EVENT_DESCRIPTOR_TAG:
					GetExtendedEventDescriptor(&stExtendedEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case STREAM_IDENTIFIER_DESCRIPTOR_TAG:
					GetStreamIndentifierDescriptor(&stStreamIndentifierDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case TELETEXT_DESCRIPTOR_TAG:
					GetTeletextDescriptor(&stTeletextDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case LOCAL_TIME_OFFSET_DESCRIPTOR_TAG:
					GetLocalTimeOffsetDescriptor(&stLocalTimeOffsetDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case SUBTITLING_DESCRIPTOR_TAG:
					GetSubtitlingDescriptor(&stSubtitlingDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
					GetTerrestrialDeliverySystemDescriptor(&stTerrestrialDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				case FREQUENCY_LIST_DESCRIPTOR_TAG:
					GetFrequencyListDescriptor(&stFrequencyListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					break;
				default:
					descriptorBean = (*env)->NewObject(env, descriptorBeanClass, descriptorConstrocMID, iTag, iLength);
					LOG("unKnownTag: %d; iDescriptorPosition: %d\n", iTag, iDescriptorPosition);
					break;
			}
			(*env)->SetObjectArrayElement(env, (*pDescriptorBeanArray), decriptorOrderNumber, descriptorBean);
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
	jbyteArray privateDateByteArray = (*env)->NewByteArray(env, length);
	(*env)->SetByteArrayRegion(env, privateDateByteArray, 0, length, (jbyte*) pucBuffer);
	//	TODO 释放内存 (*env)->ReleaseByteArrayElements(env, privateDateByteArray, (jbyte*)b, JNI_ABORT);
	return privateDateByteArray;
}
