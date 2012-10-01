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

#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

	class JapaneseKeywordGenerator
	{
		
	public:			
		typedef enum dictionaryId{
			
			MECAB_DIC_IPA	= 0,
			MECAB_DIC_NAIST	= 1,
			MECAB_DIC_JUMAN	= 2,	
			MECAB_DIC_UNI	= 3
			
		}dictionaryId;

		//used internally to process keywords (ignore, concatenate...)
		typedef enum keywordActionType
		{
			
			KEYWORD_NO_ACTION	= 0,
			KEYWORD_ADD			= 1,
			KEYWORD_REPLACE		= 2	
			
		}keywordActionType;
		
		void setSystemDictionary(JapaneseKeywordGenerator::dictionaryId dictionaryId);
		JapaneseKeywordGenerator::dictionaryId getSystemDictionary(){return this->mecabDictionaryId;};
		
		void getKeywords(CUTF8String *sourceText, std::vector<CUTF8String> *keywords);
		void getNodes(CUTF8String *sourceText, std::vector<CUTF8String> *surfaces, std::vector<CUTF8String> *features, std::vector<unsigned short> *posIds);
		
		JapaneseKeywordGenerator(JapaneseKeywordGenerator::dictionaryId dictionaryId = MECAB_DIC_NAIST);
		~JapaneseKeywordGenerator();

		keywordActionType keywordActionTypeForPosIdPair(unsigned short previousPosId, unsigned short currentPosId, JapaneseKeywordGenerator::dictionaryId dictionaryId);	

	private:
		
		MeCab::Model*   meCabModel;
		MeCab::Tagger*  meCabTagger;
		
		JapaneseKeywordGenerator::dictionaryId mecabDictionaryId;
		
		std::string rcfile;
		
		BOOL setSystemDictionary(CUTF8String *dictionaryName);		
	};		
	
#ifdef __cplusplus
}
#endif

#endif