/*
 * Native_Function_Manager.h
 *
 *  Created on: 2017Äê9ÔÂ8ÈÕ
 *      Author: Administrator
 */

#ifndef _NATIVE_FUNCTION_MANAGER_H_
#define _NATIVE_FUNCTION_MANAGER_H_

FILE* GetFilePointer(char cFilePath[]);
jbyteArray GetJByteArrayByUChar(JNIEnv *env, unsigned char * pucBuffer, int length);
int ParseDescriptorToJArray(JNIEnv *env, jobjectArray *pDescriptorBeanArray, unsigned char *pucDescriptorBuffer, int iDescriptorBufferLength);
jobject GetISO639LanguageCodeBean(JNIEnv *env, ISO_639_LANGUAGE_CODE_T *pstISO_639_Language_code);
jobjectArray GetISO639LanguageCodeBeanArray(JNIEnv *env, ISO_639_LANGUAGE_CODE_T astISO_639_Language_CodeArray[], int iso639LanguageCodeCount);
int String2UnsignedCharArray(JNIEnv *env, jstring filePath, char *pcFilePath);
#endif /* NATIVE_FUNCTION_MANAGER_H_ */
