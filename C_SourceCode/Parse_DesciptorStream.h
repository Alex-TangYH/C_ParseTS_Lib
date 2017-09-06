/*
 * Parse_DesciptorStream.h
 *
 *  Created on: 2017Äê8ÔÂ29ÈÕ
 *      Author: AlexTang
 */

#ifndef _PARSE_DESCIPTORSTREAM_H_
#define _PARSE_DESCIPTORSTREAM_H_

void ParseAndPrintDescriptorByTag(int iTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix);
void* ParseDescriptorByTag(int iTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
int ParseAndPrintDescriptor(unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, char *pacOutputPrefix);
int GetDescriptorTag(int *piTag, int iDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
int GetDescriptorTagPosition(int *piDescriptorPosition, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength, int iTagValue);

#endif /* PARSE_DESCIPTORSTREAM_H_ */
