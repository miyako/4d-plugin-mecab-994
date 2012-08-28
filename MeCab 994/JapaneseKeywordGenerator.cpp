/*
 *  JapaneseKeywordGenerator.cpp
 *
 *  Created by miyako on 2012/08/20.
 *
 */

#include "JapaneseKeywordGenerator.h"

JapaneseKeywordGenerator::JapaneseKeywordGenerator() : mecabDictionaryId(MECAB_DIC_IPA){
		
	if(this->initMecabRcPath()){
		this->mecabModel = MeCab::createModel("--output-format-type=none");
	}
		
}

JapaneseKeywordGenerator::~JapaneseKeywordGenerator(){ 
	
	if(mecabModel)
		delete mecabModel;

}

BOOL JapaneseKeywordGenerator::setSystemDictionary(CUTF8String *dictionaryName){
	
	BOOL success = FALSE;
	
#if VERSIONMAC
	CUTF8String arg = (const uint8_t *)"--output-format-type=none --dicdir=$(rcpath)/../resources/dic/";
#else
	CUTF8String arg = (const uint8_t *)"--output-format-type=none --dicdir=$(rcpath)\\..\\resources\\dic\\";
#endif
	arg += dictionaryName->c_str();
	
	if(mecabModel){
		//swap current model with new model
		MeCab::Model *another_model = MeCab::createModel((const char *)arg.c_str());
		if(another_model){
			this->mecabModel->swap(another_model);
			success = TRUE;
		}
		
	}	
	
	return success;

}

void JapaneseKeywordGenerator::setSystemDictionary(MECAB_DIC dictionaryId){

	CUTF8String dictionaryDirName;
	
	if(this->mecabDictionaryId != dictionaryId){
	
		switch (dictionaryId) {
			case MECAB_DIC_IPA:
				dictionaryDirName = CUTF8String((const uint8_t *)MECAB_DIC_DIR_IPA);
				break;				
			case MECAB_DIC_JUMAN:
				dictionaryDirName = CUTF8String((const uint8_t *)MECAB_DIC_DIR_JUMAN);
				break;
			case MECAB_DIC_NAIST:
				dictionaryDirName = CUTF8String((const uint8_t *)MECAB_DIC_DIR_NAIST);
				break;
			case MECAB_DIC_UNI:
				dictionaryDirName = CUTF8String((const uint8_t *)MECAB_DIC_DIR_UNI);
				break;	
		}
		
		if(dictionaryDirName.length()){
			if(this->setSystemDictionary(&dictionaryDirName))
				this->mecabDictionaryId = dictionaryId;			
		}
	
	}
	
}

void JapaneseKeywordGenerator::getNodes(CUTF8String *sourceText, std::vector<CUTF8String> *surfaces, std::vector<CUTF8String> *features, std::vector<unsigned short> *posIds){

	CUTF8String surface, feature;
	unsigned short posId;
	
	surfaces->clear();
	surfaces->resize(1);	
	
	features->clear();
	features->resize(1);
	
	posIds->clear();
	posIds->resize(1);	
	
	if(this->mecabModel){
		MeCab::Tagger *tagger = this->mecabModel->createTagger();
		if(tagger){
			MeCab::Lattice *lattice = this->mecabModel->createLattice();
			if(lattice){
				lattice->set_sentence((const char *)sourceText->c_str());
				
				if(tagger->parse(lattice)){
					
					const MeCab::Node* node = lattice->bos_node();
					
					while(node){
						
						switch(node->stat)
						{
							case MECAB_BOS_NODE:
								break;
							case MECAB_EOS_NODE:
								break;
							default:
								surface = CUTF8String((const uint8_t *)node->surface, node->length);
								feature = CUTF8String((const uint8_t *)node->feature);
								posId = node->posid;
								surfaces->push_back(surface);
								features->push_back(feature);
								posIds->push_back(posId);
								break;
						}
						node = node->next;
					}				
					
				}
				delete lattice;
			}
			delete tagger;
		}
		
	}	
	
}

void JapaneseKeywordGenerator::getKeywords(CUTF8String *sourceText, std::vector<CUTF8String> *keywords){
	
	CUTF8String keyword, leftKeyword;
	unsigned short leftPosId = 0;
	
	keywords->clear();
	keywords->resize(1);
	
	if(this->mecabModel){
		MeCab::Tagger *tagger = this->mecabModel->createTagger();
		if(tagger){
			MeCab::Lattice *lattice = this->mecabModel->createLattice();
			if(lattice){
				lattice->set_sentence((const char *)sourceText->c_str());
				
				if(tagger->parse(lattice)){
				
					const MeCab::Node* node = lattice->bos_node();
					
					while(node){
						
						switch(node->stat){
							case MECAB_BOS_NODE:
								break;
							case MECAB_EOS_NODE:
								break;
							default:	
								
								keyword = CUTF8String((const uint8_t *)node->surface, node->length);
								
								BOOL shouldSkip = FALSE;
								BOOL shouldReplace = FALSE;
								
								switch (this->mecabDictionaryId){
									case MECAB_DIC_IPA:
									case MECAB_DIC_NAIST:	
										
										//http://www.unixuser.org/~euske/doc/postag/index.html#chasen
										
										//#1 ignore punctuation (kigou)
										if((node->posid >= 3) && (node->posid <= 9)){
											shouldSkip = TRUE;
											break;
										}
										
										//#2 append particle to preceeding verb/adjective
										if(((leftPosId == 18) || (leftPosId == 25) || (leftPosId == 31) || (leftPosId == 32) || (leftPosId == 33) || (leftPosId == 54) || (leftPosId == 62) || (leftPosId == 65))
										   && ((node->posid == 18) || (node->posid == 22) || (node->posid == 25) || (node->posid == 32) || (node->posid == 54) || (node->posid == 62) || (node->posid == 65))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}
										
										//#3 append particle to preceeding noun
										if((((leftPosId >= 36) && (leftPosId <= 39)) || (leftPosId == 41) || ((leftPosId >= 46) && (leftPosId <= 48)) || ((leftPosId >= 50) && (leftPosId <= 53)) || (leftPosId == 58))
										&& (((node->posid >= 50) && (node->posid <= 53)) || (node->posid == 56) || (node->posid == 58))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}											

										//#4 append verb/noun/adjective to prefix
										if(((leftPosId >= 27) && (leftPosId <= 30))
										   && ((node->posid == 10) || (node->posid == 31) || (node->posid == 36) || (node->posid == 38) || (node->posid == 48))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}
										
										//#5 concatenate common nouns
										if(((leftPosId == 38) || (leftPosId == 41))
										   && ((node->posid == 38) || (node->posid == 41))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}											
										
										//#6 ignore certain particles
										if(((node->posid >= 13) && (node->posid <= 17)) || ((node->posid >= 20) && (node->posid <= 25)) || (node->posid == 55) || (node->posid == 57) || (node->posid == 63)){
											shouldSkip = TRUE;
											break;
										}										
										
										break;
									case MECAB_DIC_JUMAN:
										
										//http://www.unixuser.org/~euske/doc/postag/index.html#juman
										
										//#1 ignore punctuation (tokushu)
										if((node->posid >= 23) && (node->posid <= 28)){
											shouldSkip = TRUE;
											break;
										}
										
										//#2 append particle to preceeding verb/adjective
										if(((leftPosId == 1) || ((leftPosId >= 15) && (leftPosId <= 18)) || (leftPosId == 22))
										   && (((node->posid >= 15) && (node->posid <= 18)) || (node->posid == 6) || (node->posid == 9) || (node->posid == 29))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}										
										
										//#3 append particle to preceeding noun
										if(((leftPosId == 31) || (leftPosId == 33) || (leftPosId == 34) || ((leftPosId >= 36) && (leftPosId <= 39)) || ((leftPosId >= 19) && (leftPosId <= 21)))
										   && ((node->posid == 15) ||(node->posid == 16) || (node->posid == 19) || (node->posid == 20))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}
										
										//#4 append verb/noun/adjective to prefix
										if(((leftPosId >= 11) && (leftPosId <= 14))
										   && ((node->posid == 1) || (node->posid == 22) || (node->posid == 31) || ((node->posid >= 33) && (node->posid <= 39)))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}

										//#5 concatenate common nouns
										if(((leftPosId == 33) || (leftPosId == 39))
										   && ((node->posid == 33) || (node->posid == 39))){
											keyword = leftKeyword + keyword;
											shouldReplace = TRUE;
											break;
										}
										
										//#6 ignore certain particles (kakujyoshi, setsuzoku-jyoshi, fukujyoshi, keishiki-meishi)
										if((node->posid == 5) || (node->posid == 7) || (node->posid == 8) || (node->posid == 21) || (node->posid == 29) || (node->posid == 32) || (node->posid == 40)){
											shouldSkip = TRUE;
											break;
										}
										break;
									case MECAB_DIC_UNI:
										//this dictionary has no pos-id.def
										break;											
								}
								
								if(shouldReplace){
									std::vector<CUTF8String>::iterator f;
									f = find(keywords->begin(),keywords->end(), leftKeyword);
									if(f != keywords->end()){
										keywords->erase(f);
									}								
								}
								
								if(!shouldSkip){
									//#check if the word is already in list
									if(find(keywords->begin(),keywords->end(), keyword) == keywords->end()){	
										keywords->push_back(keyword);
									}								
								}
								break;
						}
						leftPosId = node->posid;
						leftKeyword = keyword;
						node = node->next;
					}					
				
				}
				delete lattice;
			}
			delete tagger;
		}

	}
	
}

BOOL JapaneseKeywordGenerator::initMecabRcPath(){
	
	BOOL success = FALSE;
	
#if VERSIONMAC	
	NSBundle *libmecab = [NSBundle bundleWithIdentifier:THIS_BUNDLE_ID];
	if(libmecab){
		NSString *mecabrcPath = [[[libmecab executablePath]stringByDeletingLastPathComponent]stringByAppendingPathComponent:@"mecabrc"];
		setenv("MECABRC",(const char *)[mecabrcPath UTF8String], 1);
		success = TRUE;
	}
#else
	wchar_t	libmecabPath[ _MAX_PATH ] = {0};
	wchar_t	mecabrcPath[ _MAX_PATH ] = {0};
	wchar_t	fDrive[ _MAX_DRIVE ], fDir[ _MAX_DIR ], fName[ _MAX_FNAME ], fExt[ _MAX_EXT ];
	
	HMODULE libmecab = GetModuleHandleW(THIS_DLL_NAME);
	
	if(libmecab){
		GetModuleFileNameW(libmecab, libmecabPath, _MAX_PATH);
		_wsplitpath_s( libmecabPath, fDrive, fDir, fName, fExt );
		_wmakepath_s( mecabrcPath, fDrive, fDir, L"mecabrc", NULL );
		_wputenv_s(L"MECABRC", mecabrcPath);
		success = TRUE;
	}
#endif
	return success;
}