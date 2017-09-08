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

/******************************************************
 *
 * 解析CAT
 *
 ******************************************************/
JNIEXPORT jobject JNICALL Java_com_alex_ts_1parser_native_1function_NativeFunctionManager_parseCAT(JNIEnv *env, jclass obj, jstring filePath)
{
	char cFilePath[1024];
	String2UnsignedCharArray(env, filePath, cFilePath);
	FILE *pfTsFile = GetFilePointer(cFilePath);
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
					stCAT.uiCurrent_next_indicator, stCAT.uiSection_number, stCAT.uiLast_section_number, descriptorBeanArray, stCAT.uiCRC_32); //需要增加参数
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
	char cFilePath[1024];
	String2UnsignedCharArray(env, filePath, cFilePath);
	FILE *pfTsFile = GetFilePointer(cFilePath);
	if (pfTsFile == NULL)
	{
		return NULL;
	}
	else
	{
		int iLoopIndex = 0;
		TS_PAT_T stPat = { 0 };
		int iProgramCount = GetPatTable(pfTsFile, &stPat);
		if (-1 == iProgramCount)
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
					jclass videoStreamDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/VideoStreamDescriptor");
					jmethodID videoStreamDescriptorConstrocMID = (*env)->GetMethodID(env, videoStreamDescriptorBeanClass, "<init>", "(IIIIIIIIIII)V");
					descriptorBean = (*env)->NewObject(env, videoStreamDescriptorBeanClass, videoStreamDescriptorConstrocMID, stVideoStreamDescriptor.uiDescriptor_tag, stVideoStreamDescriptor.uiDescriptor_length,
							stVideoStreamDescriptor.uiMultiple_frame_rate_flag, stVideoStreamDescriptor.uiFrame_rate_code, stVideoStreamDescriptor.uiMPEG_2_flag, stVideoStreamDescriptor.uiConstrained_parameter_flag,
							stVideoStreamDescriptor.uiStill_picture_flag, stVideoStreamDescriptor.uiProfile_and_level_indication, stVideoStreamDescriptor.uiChroma_format, stVideoStreamDescriptor.uiFrame_rate_extension_flag,
							stVideoStreamDescriptor.uiReserved);
					break;
				case AUDIO_STREAM_DESCRIPTOR_TAG:
					GetAudioStreamDescriptor(&stAudioStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass audioStreamDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/AudioStreamDescriptor");
					jmethodID audioStreamDescriptorConstrocMID = (*env)->GetMethodID(env, audioStreamDescriptorBeanClass, "<init>", "(IIIIII)V");
					descriptorBean = (*env)->NewObject(env, audioStreamDescriptorBeanClass, audioStreamDescriptorConstrocMID, stAudioStreamDescriptor.uiDescriptor_tag, stAudioStreamDescriptor.uiDescriptor_length,
							stAudioStreamDescriptor.uiFree_format_flag, stAudioStreamDescriptor.uiID, stAudioStreamDescriptor.uiLayer, stAudioStreamDescriptor.uiReserved);
					break;
				case DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG:
					GetDataStreamAlignmentDescriptor(&stDataStreamAlignmentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
					jclass dataStreamAlignmentDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/DataStreamAlignmentDescriptor");
					jmethodID dataStreamAlignmentDescriptorConstrocMID = (*env)->GetMethodID(env, dataStreamAlignmentDescriptorBeanClass, "<init>", "(III)V");
					descriptorBean = (*env)->NewObject(env, dataStreamAlignmentDescriptorBeanClass, dataStreamAlignmentDescriptorConstrocMID, stDataStreamAlignmentDescriptor.uiDescriptor_tag, stDataStreamAlignmentDescriptor.uiDescriptor_length,
							stDataStreamAlignmentDescriptor.uiAlignment_type);
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
					jclass iso_639_LanguageDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/ISO_639_Language_Descriptor");
					jmethodID iso_639_LanguageDescriptorConstrocMID = (*env)->GetMethodID(env, iso_639_LanguageDescriptorBeanClass, "<init>", "(II[Lcom/alex/ts_parser/bean/descriptor/ISO_639_LanguageCode;I)V");
					int iso639LanguageCodeCount = (stISO_639_LanguageDescriptor.uiDescriptor_length - 1) / sizeof(ISO_639_LANGUAGE_CODE_T);
					jobjectArray iso639LanguageCodeArray = GetISO639LanguageCodeBeanArray(env, stISO_639_LanguageDescriptor.astISO_639_Language_code, iso639LanguageCodeCount);
					descriptorBean = (*env)->NewObject(env, iso_639_LanguageDescriptorBeanClass, iso_639_LanguageDescriptorConstrocMID, stISO_639_LanguageDescriptor.uiDescriptor_tag, stISO_639_LanguageDescriptor.uiDescriptor_length,
							iso639LanguageCodeArray, stISO_639_LanguageDescriptor.uiAudio_type);
					break;
//				case SYSTEM_CLOCK_DESCRIPTOR_TAG:
//					GetSystemClockDescriptor(&stSystemClockDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case MAXIMUM_BITRATE_DESCRIPTOR_TAG:
//					GetMaximumBitrateDescriptor(&stMaximumBitrateDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case NETWORK_NAME_DESCRIPTOR_TAG:
//					GetNetworkNameDescriptor(&stNetworkNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case SERVICE_LIST_DESCRIPTOR_TAG:
//					GetServiceListDescriptor(&stServiceListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
//					GetSatelliteDeliverySystemDescriptor(&stSatelliteDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
//					GetCableDeliverySystemDescriptor(&stCableDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case BOUQUET_NAME_DESCRIPTOR_TAG:
//					GetBouquetNameDescriptor(&stBouquetNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case SERVICE_DESCRIPTOR_TAG:
//					GetServiceDescriptor(&stServiceDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case LINKAGE_DESCRIPTOR_TAG:
//					GetLinkageDescriptor(&stLinkageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case SHORT_EVENT_DESCRIPTOR_TAG:
//					GetShortEventDescriptor(&stShortEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case EXTENDED_EVENT_DESCRIPTOR_TAG:
//					GetExtendedEventDescriptor(&stExtendedEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case STREAM_IDENTIFIER_DESCRIPTOR_TAG:
//					GetStreamIndentifierDescriptor(&stStreamIndentifierDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case TELETEXT_DESCRIPTOR_TAG:
//					GetTeletextDescriptor(&stTeletextDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case LOCAL_TIME_OFFSET_DESCRIPTOR_TAG:
//					GetLocalTimeOffsetDescriptor(&stLocalTimeOffsetDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case SUBTITLING_DESCRIPTOR_TAG:
//					GetSubtitlingDescriptor(&stSubtitlingDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
//					GetTerrestrialDeliverySystemDescriptor(&stTerrestrialDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
//				case FREQUENCY_LIST_DESCRIPTOR_TAG:
//					GetFrequencyListDescriptor(&stFrequencyListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
//					jclass caDescriptorBeanClass = (*env)->FindClass(env, "com/alex/ts_parser/bean/descriptor/CA_Descriptor");
//					jmethodID caDescriptorConstrocMID = (*env)->GetMethodID(env, caDescriptorBeanClass, "<init>", "(IIIII[B)V");
//					descriptorBean = (*env)->NewObject(env, caDescriptorBeanClass, caDescriptorConstrocMID, stCA_Descriptor.uiDescriptor_tag);
//					break;
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
	jobjectArray iso639LanguageCodeArray;
	jobject iso639LanguageCodeBean;
	int index = 0;
	for (index = 0; index < iso639LanguageCodeCount; index++)
	{
		iso639LanguageCodeBean = GetISO639LanguageCodeBean(env, &pastISO_639_Language_CodeArray[index]);
		iso639LanguageCodeArray = (*env)->NewObjectArray(env, iso639LanguageCodeCount, iso639LanguageCodeBean, NULL);
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
 * String to char array
 *
 ******************************************************/
int String2UnsignedCharArray(JNIEnv *env, jstring filePath, char pcFilePath[])
{
	const char *str;
	str = (*env)->GetStringUTFChars(env, filePath, NULL);
	strcpy(pcFilePath, str);
	(*env)->ReleaseStringUTFChars(env, filePath, str);
	return 1;
}

