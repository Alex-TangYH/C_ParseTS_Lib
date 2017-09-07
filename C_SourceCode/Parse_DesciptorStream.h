/*
 * Parse_DesciptorStream.h
 *
 *  Created on: 2017Äê8ÔÂ29ÈÕ
 *      Author: AlexTang
 */

#ifndef _PARSE_DESCIPTORSTREAM_H_
#define _PARSE_DESCIPTORSTREAM_H_

void ParseAndPrintDescriptorByTag(int iTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix);
int ParseAndPrintDescriptor(unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix);
int GetDescriptorTag(int *piTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
int GetDescriptorTagPosition(int *piDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, int iTagValue);
int GetDescriptorCountInBuffer(unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
#endif /* PARSE_DESCIPTORSTREAM_H_ */
