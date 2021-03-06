/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.c
 #	source generated by 4D Plugin Wizard
 #	Project : MeCab 994
 #	author : miyako
 #	2012/08/21
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

#include "JapaneseKeywordGenerator.h"

JapaneseKeywordGenerator JKG; 

void PluginMain(int32_t selector, PA_PluginParameters params)
{
	try
	{
		int32_t pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (int32_t pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
// --- MeCab

		case 1 :
			MeCab_SET_SYSTEM_DICTIONARY(pResult, pParams);
			break;

		case 2 :
			MeCab_Get_system_dictionary(pResult, pParams);
			break;

		case 3 :
			MeCab_GET_KEYWORDS(pResult, pParams);
			break;

		case 4 :
			MeCab_GET_NODES(pResult, pParams);
			break;

// --- MeCab Developer

		case 5 :
			MeCab_CREATE_USER_DICTIONARY(pResult, pParams);
			break;

	}
}

// ------------------------------------- MeCab ------------------------------------


void MeCab_SET_SYSTEM_DICTIONARY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param1;

	Param1.fromParamAtIndex(pParams, 1);

	JKG.setSystemDictionary((JapaneseKeywordGenerator::dictionaryId)Param1.getIntValue());

}

void MeCab_Get_system_dictionary(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT returnValue;

	returnValue.setIntValue(JKG.getSystemDictionary());

	returnValue.setReturn(pResult);
}

void MeCab_GET_KEYWORDS(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;

	Param1.fromParamAtIndex(pParams, 1);

	CUTF8String sourceText;
	Param1.copyUTF8String(&sourceText);
	
	std::vector<CUTF8String> keywords;
	
	JKG.getKeywords(&sourceText, &keywords);
	
	for(unsigned int i = 0; i < keywords.size(); ++i){
		Param2.appendUTF8String(&keywords.at(i));
	}

	Param2.toParamAtIndex(pParams, 2);
}

void MeCab_GET_NODES(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	ARRAY_TEXT Param3;
	ARRAY_LONGINT Param4;

	Param1.fromParamAtIndex(pParams, 1);

	CUTF8String sourceText;
	Param1.copyUTF8String(&sourceText);
	
	std::vector<CUTF8String> surfaces;
	std::vector<CUTF8String> features;
	std::vector<unsigned short> posIds;
	
	JKG.getNodes(&sourceText, &surfaces, &features, &posIds);

	for(unsigned int i = 0; i < surfaces.size(); ++i){
		Param2.appendUTF8String(&surfaces.at(i));
		Param3.appendUTF8String(&features.at(i));
		Param4.appendIntValue(posIds.at(i));
	}
	
	Param2.toParamAtIndex(pParams, 2);
	Param3.toParamAtIndex(pParams, 3);
	Param4.toParamAtIndex(pParams, 4);
}

// -------------------------------- MeCab Developer -------------------------------


void MeCab_CREATE_USER_DICTIONARY(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param3;
	C_TEXT Param1;
	C_TEXT Param2;

	Param3.fromParamAtIndex(pParams, 1);
	Param1.fromParamAtIndex(pParams, 2);
	Param2.fromParamAtIndex(pParams, 3);

	// --- write the code of MeCab_CREATE_USER_DICTIONARY here...

}

