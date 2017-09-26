#include <stdio.h>
#include <string.h>
#include "Parse_Descriptor.h"
#include "Print_Descriptor.h"
#include "Parse_DesciptorStream.h"
#include "TsParser.h"
#include "DescriptorTag.h"

#define SOURCEFILENAME "Parse_DesciptorStream"

/******************************************
 *
 * 解析并打印与TAG对应的描述子
 *
 ******************************************/
void ParseAndPrintDescriptorByTag(int iTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix)
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

	switch (iTag)
	{
		case VIDEO_STREAM_DESCRIPTOR_TAG:
			GetVideoStreamDescriptor(&stVideoStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_VideoStreamDescriptor(&stVideoStreamDescriptor, pacOutputPrefix);
			break;
		case AUDIO_STREAM_DESCRIPTOR_TAG:
			GetAudioStreamDescriptor(&stAudioStreamDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_AudioStreamDescriptor(&stAudioStreamDescriptor, pacOutputPrefix);
			break;
		case DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG:
			GetDataStreamAlignmentDescriptor(&stDataStreamAlignmentDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_DataStreamAlignmentDescriptor(&stDataStreamAlignmentDescriptor, pacOutputPrefix);
			break;
		case CA_DESCRIPTOR_TAG:
			GetCA_Descriptor(&stCA_Descriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_CA_Descriptor(&stCA_Descriptor, pacOutputPrefix);
			break;
		case ISO_639_LANGUAGE_DESCRIPTOR_TAG:
			GetISO_639_Language_Descriptor(&stISO_639_LanguageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_ISO_639_LANGUAGE_DESCRIPTOR(&stISO_639_LanguageDescriptor, pacOutputPrefix);
			break;
		case SYSTEM_CLOCK_DESCRIPTOR_TAG:
			GetSystemClockDescriptor(&stSystemClockDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_SystemClockDescriptor(&stSystemClockDescriptor, pacOutputPrefix);
			break;
		case MAXIMUM_BITRATE_DESCRIPTOR_TAG:
			GetMaximumBitrateDescriptor(&stMaximumBitrateDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_MaximumBitrateDescriptor(&stMaximumBitrateDescriptor, pacOutputPrefix);
			break;
		case NETWORK_NAME_DESCRIPTOR_TAG:
			GetNetworkNameDescriptor(&stNetworkNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_NetworkNameDescriptor(&stNetworkNameDescriptor, pacOutputPrefix);
			break;
		case SERVICE_LIST_DESCRIPTOR_TAG:
			GetServiceListDescriptor(&stServiceListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_ServiceListDescriptor(&stServiceListDescriptor, pacOutputPrefix);
			break;
		case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
			GetSatelliteDeliverySystemDescriptor(&stSatelliteDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_SatelliteDeliverySystemDescriptor(&stSatelliteDeliverySystemDescriptor, pacOutputPrefix);
			break;
		case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
			GetCableDeliverySystemDescriptor(&stCableDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_CableDeliverySystemDescriptor(&stCableDeliverySystemDescriptor, pacOutputPrefix);
			break;
		case BOUQUET_NAME_DESCRIPTOR_TAG:
			GetBouquetNameDescriptor(&stBouquetNameDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_BouquetNameDescriptor(&stBouquetNameDescriptor, pacOutputPrefix);
			break;
		case SERVICE_DESCRIPTOR_TAG:
			GetServiceDescriptor(&stServiceDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_ServiceDescriptor(&stServiceDescriptor, pacOutputPrefix);
			break;
		case LINKAGE_DESCRIPTOR_TAG:
			GetLinkageDescriptor(&stLinkageDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_LinkageDescriptor(&stLinkageDescriptor, pacOutputPrefix);
			break;
		case SHORT_EVENT_DESCRIPTOR_TAG:
			GetShortEventDescriptor(&stShortEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_ShortEventDescriptor(&stShortEventDescriptor, pacOutputPrefix);
			break;
		case EXTENDED_EVENT_DESCRIPTOR_TAG:
			GetExtendedEventDescriptor(&stExtendedEventDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_ExtendedEventDescriptor(&stExtendedEventDescriptor, pacOutputPrefix);
			break;
		case STREAM_IDENTIFIER_DESCRIPTOR_TAG:
			GetStreamIndentifierDescriptor(&stStreamIndentifierDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_StreamIndentifierDescriptor(&stStreamIndentifierDescriptor, pacOutputPrefix);
			break;
		case TELETEXT_DESCRIPTOR_TAG:
			GetTeletextDescriptor(&stTeletextDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_TeletextDescriptor(&stTeletextDescriptor, pacOutputPrefix);
			break;
		case LOCAL_TIME_OFFSET_DESCRIPTOR_TAG:
			GetLocalTimeOffsetDescriptor(&stLocalTimeOffsetDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_LocalTimeOffsetDescriptor(&stLocalTimeOffsetDescriptor, pacOutputPrefix);
			break;
		case SUBTITLING_DESCRIPTOR_TAG:
			GetSubtitlingDescriptor(&stSubtitlingDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_SubtitlingDescriptor(&stSubtitlingDescriptor, pacOutputPrefix);
			break;
		case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
			GetTerrestrialDeliverySystemDescriptor(&stTerrestrialDeliverySystemDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_TerrestrialDeliverySystemDescriptor(&stTerrestrialDeliverySystemDescriptor, pacOutputPrefix);
			break;
		case FREQUENCY_LIST_DESCRIPTOR_TAG:
			GetFrequencyListDescriptor(&stFrequencyListDescriptor, pucDescriptorBuffer, iDescriptorBufferLength, iDescriptorPosition);
			Print_FrequencyListDescriptor(&stFrequencyListDescriptor, pacOutputPrefix);
			break;
		default:
			LOG("unKnownTag: %d; iDescriptorPosition: %d\n", iTag, iDescriptorPosition);
			break;
	}
}
/******************************************
 *
 * 解析标准描述流中描述符的个数
 * 返回值：
 * 		-1：最后一段数据超过了描述流的长度
 * 		其它：描述符的个数
 *
 ******************************************/
int GetDescriptorCountInBuffer(unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength)
{
	if(iDescriptorBufferLength <= 0){
		return 0;
	}
	int descriptorCount = 0;
	int iTag = 0;
	int iDescriptorPosition = 0;
	do
	{
		iTag = GetDescriptorTag(&iTag, iDescriptorPosition, pucDescriptorBuffer, iDescriptorBufferLength);
		if (-1 == iTag)
		{
			LOG("GetDescriptorCountInBuffer iTag == -1 \n");
			break;
		}
		else
		{
			iDescriptorPosition += pucDescriptorBuffer[iDescriptorPosition + 1] + 2;
			descriptorCount++;
		}
	}
	while (iDescriptorPosition < iDescriptorBufferLength);

	return descriptorCount;
}

/******************************************
 *
 * 解析标准描述流中描述符的tag值
 *
 ******************************************/
int GetDescriptorTag(int *piTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength)
{
	if (iDescriptorPosition < iDescriptorBufferLength)
	{
		*piTag = pucDescriptorBuffer[iDescriptorPosition];
		return *piTag;
	}
	else
	{
		return -1;
	}
}

/******************************************
 *
 * 解析指定描述符的tag位置
 *
 ******************************************/
int GetDescriptorTagPosition(int *piDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, int iTagValue)
{
	int iDescriptorLength = 0;
	while ((*piDescriptorPosition) < iDescriptorBufferLength)
	{
		if (pucDescriptorBuffer[(*piDescriptorPosition)] == iTagValue)
		{
			return (*piDescriptorPosition);
		}
		else
		{
			iDescriptorLength = pucDescriptorBuffer[1 + (*piDescriptorPosition)];
			(*piDescriptorPosition) += 2 + iDescriptorLength;
		}
	}
	return -1;
}

/******************************************
 *
 * 解析并打印描述流
 *
 ******************************************/
int ParseAndPrintDescriptor(unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix)
{

	int iTag = 0;
	int iDescriptorPosition = 0;
	do
	{
		iTag = GetDescriptorTag(&iTag, iDescriptorPosition, pucDescriptorBuffer, iDescriptorBufferLength);
		if (-1 == iTag)
		{
			LOG("%s ParseDescriptor iTag == -1 \n", SOURCEFILENAME);
			break;
		}
		else
		{
			ParseAndPrintDescriptorByTag(iTag, iDescriptorPosition, pucDescriptorBuffer, iDescriptorBufferLength, pacOutputPrefix);
			iDescriptorPosition += pucDescriptorBuffer[iDescriptorPosition + 1] + 2;
		}
	}
	while (iDescriptorPosition < iDescriptorBufferLength);
	return 1;
}
