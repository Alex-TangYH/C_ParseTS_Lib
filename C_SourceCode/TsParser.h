/*
 * TsParser.h
 *
 *  Created on: 2017��8��30��
 *      Author: AlexTang
 */

#ifndef _TSPARSER_H_
#define _TSPARSER_H_


//#define DEBUG
#define PRINTFPAT_INFO 0
#define PRINTFCAT_INFO 0
#define PRINTFTS_HEAD 0
#define PRINTFNIT_INFO 0
#define PRINTFPMT_INFO 0
#define PRINTFTDT_INFO 0
#define PRINTFTOT_INFO 0
#define PRINTFSDT_INFO 0
#define PRINTFST_INFO 1
#define PRINTFDIT_INFO 1
#define LOGPRINTF

#ifdef DEBUG
    #define DUBUGPRINTF printf
#else
    #define DUBUGPRINTF
#endif

#ifdef LOGPRINTF
    #define LOG printf
#else
    #define LOG
#endif


int parseStream();
#endif /* _TSPARSER_H_ */
