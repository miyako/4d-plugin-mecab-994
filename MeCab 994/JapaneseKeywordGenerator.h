/*
 *  JapaneseKeywordGenerator.h
 *
 *  Created by miyako on 2012/08/20.
 *
 */

#ifndef __JAPANESE_KEYWORD_GENERATOR_H__
#define __JAPANESE_KEYWORD_GENERATOR_H__ 1

#include "4DPluginAPI.h"
#include "mecab.h"

//used in initMecabRcPath to locate the mecabrc, which should be next to the executable

#if VERSIONMAC
#define THIS_BUNDLE_ID @"com.4D.4DPlugin.miyako.MeCab994"
#else
#define THIS_DLL_NAME L"MeCab 994.4dx"
#endif

#define MECAB_DIC_DIR_IPA	"ipadic-2.7.0"
#define MECAB_DIC_DIR_JUMAN	"jumandic-5.1"
#define MECAB_DIC_DIR_NAIST	"naist-jdic-0.4.3"
#define MECAB_DIC_DIR_UNI	"unidic-1.3.12"

typedef enum MECAB_DIC{
	
	MECAB_DIC_IPA	= 0,
	MECAB_DIC_NAIST	= 1,
	MECAB_DIC_JUMAN	= 2,	
	MECAB_DIC_UNI	= 3
	
}MECAB_DIC;

#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

	class JapaneseKeywordGenerator
	{
		
	private:
		
		MeCab::Model *mecabModel;
		MECAB_DIC mecabDictionaryId;
		
		BOOL initMecabRcPath();

		BOOL setSystemDictionary(CUTF8String *dictionaryName);
		
	public:	
		
		void setSystemDictionary(MECAB_DIC dictionaryId);
		MECAB_DIC getSystemDictionary(){return this->mecabDictionaryId;};
		
		void getKeywords(CUTF8String *sourceText, std::vector<CUTF8String> *keywords);
		void getNodes(CUTF8String *sourceText, std::vector<CUTF8String> *surfaces, std::vector<CUTF8String> *features, std::vector<unsigned short> *posIds);
		
		JapaneseKeywordGenerator();
		~JapaneseKeywordGenerator();

	};		
	
#ifdef __cplusplus
}
#endif

#endif