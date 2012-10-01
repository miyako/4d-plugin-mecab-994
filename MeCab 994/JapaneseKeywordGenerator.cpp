/*
 *  JapaneseKeywordGenerator.cpp
 *
 *  Created by miyako on 2012/08/20.
 *
 */

#include "JapaneseKeywordGenerator.h"

JapaneseKeywordGenerator::JapaneseKeywordGenerator(JapaneseKeywordGenerator::dictionaryId dictionaryId){
		
	mecabDictionaryId = dictionaryId;
	
	std::string dicdir = "--dicdir=$(rcpath)/../Resources/dic/";

	switch (mecabDictionaryId) {
		case MECAB_DIC_IPA:
			dicdir += MECAB_DIC_DIR_IPA;
			break;				
		case MECAB_DIC_JUMAN:
			dicdir += MECAB_DIC_DIR_JUMAN;
			break;
		case MECAB_DIC_NAIST:
			dicdir += MECAB_DIC_DIR_NAIST;
			break;
		case MECAB_DIC_UNI:
			dicdir += MECAB_DIC_DIR_UNI;
			break;	
	}	
	
	//construct the command-line-style argument to initialise mecab model
	rcfile = "--output-format-type=none\n--rcfile=";
	
#ifdef _WIN32
	
	wchar_t	libmecabPath[ _MAX_PATH ] = {0};
	wchar_t	mecabrcPath[ _MAX_PATH ] = {0};	
	wchar_t	fDrive[_MAX_DRIVE], fDir[_MAX_DIR], fName[_MAX_FNAME], fExt[_MAX_EXT];
	
	HMODULE libmecab = GetModuleHandleW(THIS_DLL_NAME);
	
	if(libmecab){
		GetModuleFileNameW(libmecab, libmecabPath, _MAX_PATH);
		_wsplitpath_s(libmecabPath, fDrive, fDir, fName, fExt);
		_wmakepath_s(mecabrcPath, fDrive, fDir, L"mecabrc", NULL );
		
		//convert wide characters to utf-8		
		int len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, mecabrcPath, -1, NULL, 0, NULL, NULL);
		
		if(len){
			std::vector<uint8_t> buf(len);
			if(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, mecabrcPath, -1, (LPSTR)&buf[0], len, NULL, NULL)){
				rcfile += (const char *)&buf[0];
			}
		}
	}
	
#else	
	
	NSBundle *libmecab = [NSBundle bundleWithIdentifier:THIS_BUNDLE_ID];
	
	if(libmecab){
		NSString *mecabrcPath = [[[libmecab executablePath]stringByDeletingLastPathComponent]stringByAppendingPathComponent:@"mecabrc"];
		rcfile += (const char *)[mecabrcPath UTF8String];
	}

#endif	
	
	//we have modified file param.cpp (192) to use 0x0A as delimeter instead of isspace() 
	rcfile += "\n";
	
	meCabModel = MeCab::createModel((rcfile + dicdir).c_str());
	
	if(meCabModel){
		
		meCabTagger = meCabModel->createTagger();
		
	}
	
}

JapaneseKeywordGenerator::~JapaneseKeywordGenerator(){ 
	
	if(meCabModel){
		
		if(meCabTagger){
			
			delete meCabTagger;
			
		}
		
		delete meCabModel;
	}

}

BOOL JapaneseKeywordGenerator::setSystemDictionary(CUTF8String *dictionaryName){
	
	BOOL success = FALSE;
	
	std::string dicdir = "--dicdir=$(rcpath)/../Resources/dic/";

	dicdir += (const char *)dictionaryName->c_str();
	
	if(meCabModel){
		//swap current model with new model
		
		MeCab::Model *another_model = MeCab::createModel((rcfile + dicdir).c_str());
		if(another_model){
			this->meCabModel->swap(another_model);
			success = TRUE;
		}
		
	}	
	
	return success;

}

void JapaneseKeywordGenerator::setSystemDictionary(JapaneseKeywordGenerator::dictionaryId dictionaryId){

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
	
	if(sourceText->size()){
		
		if(meCabModel){
			if(meCabTagger){
				MeCab::Lattice *lattice = meCabModel->createLattice();
				if(lattice){
					lattice->set_sentence((const char *)sourceText->c_str());
					
					if(meCabTagger->parse(lattice)){
						
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

			}
			
		}	
		
	}
	
}

void JapaneseKeywordGenerator::getKeywords(CUTF8String *sourceText, std::vector<CUTF8String> *keywords){
	
	CUTF8String keyword, oldKeyword;
	
	keywords->clear();
	keywords->resize(1);
	
	if(sourceText->size()){
	
		if(meCabModel){
			if(meCabTagger){
				MeCab::Lattice *lattice = meCabModel->createLattice();
				if(lattice){
					lattice->set_sentence((const char *)sourceText->c_str());
					
					if(meCabTagger->parse(lattice)){
					
						size_t newPos = 0, oldLen = 0;
						size_t newLen = 0, oldPos = 0;
						
						unsigned short oldPosId = 0;
						
						const MeCab::Node* node = lattice->bos_node();
						
						while(node){
							
							BOOL shouldAdd = FALSE;
							BOOL shouldReplace = FALSE;
							
							switch(node->stat){
								case MECAB_BOS_NODE:
									break;
								case MECAB_EOS_NODE:
									break;
								default:	
									
									newPos = (const uint8_t *)node->surface - sourceText->c_str();
									newLen = node->length;
									
									keyword = CUTF8String((const uint8_t *)node->surface, node->length);
									
									JapaneseKeywordGenerator::keywordActionType keywordActionType = KEYWORD_ADD;
									
									if((oldPos + oldLen) != newPos)
									{
										oldPosId = 0;
									}
									
									keywordActionType = this->keywordActionTypeForPosIdPair(oldPosId, node->posid, this->mecabDictionaryId);
									
									switch (keywordActionType) {
											
										case KEYWORD_NO_ACTION:
											break;
											
										case KEYWORD_ADD:
											shouldAdd = TRUE;
											break;
											
										case KEYWORD_REPLACE:
											
											newPos = oldPos;
											newLen += oldLen;

											shouldReplace = TRUE;
											shouldAdd = TRUE;
											break;												
									}
									
									if(shouldReplace){
										std::vector<CUTF8String>::iterator f;
										f = find(keywords->begin(),keywords->end(), oldKeyword);
										if(f != keywords->end()){
											keywords->erase(f);
										}								
									}
									
									if(shouldAdd){
										if(find(keywords->begin(),keywords->end(), keyword) == keywords->end()){	
											keywords->push_back(keyword);
										}								
									}
									break;
							}
							
							oldPosId = node->posid;
							oldPos = newPos;
							oldLen = newLen;
							oldKeyword = keyword;
							
							node = node->next;
						}					
					
					}
					delete lattice;
				}

			}

		}
		
	}

}

//this is where we implement custom rules to create useful keywords 
JapaneseKeywordGenerator::keywordActionType JapaneseKeywordGenerator::keywordActionTypeForPosIdPair(unsigned short previousPosId, unsigned short currentPosId, JapaneseKeywordGenerator::dictionaryId dictionaryId){
	
	//custom keyword rule begins here;
	//mecab will break sentence to part-of-text, but some elements don't mean much as keywords.
	//we reject some non-word part-of-text,
	//concatenate words that makes more sense as a single phrase.
	//we use the posId returned by mecab to determine the nature of the word.
	
	//for description of posId used in naist-jdic see
	//http://www.unixuser.org/~euske/doc/postag/index.html#chasen
	//or the pos-id.def file in source (euc-jp encoding text)
	
	switch (dictionaryId) {
		case MECAB_DIC_NAIST:
			
			//# concatenate alphabet (posId 3)
			if((previousPosId == 3) && (currentPosId == 3))
			{
				return KEYWORD_REPLACE;
			}	
			
			//# jyoshi particles (posId 13->24)
			if(   (currentPosId == 13)	//jyoshi, kaku-jyoshi, ippan
			   || (currentPosId == 14)	//jyoshi, kaku-jyoshi, in-yo
			   || (currentPosId == 15)	//jyoshi, kaku-jyoshi, ren-go
			   || (currentPosId == 16)	//jyoshi, kei-jyoshi
			   || (currentPosId == 17)	//jyoshi, shu-jyoshi
			   || (currentPosId == 18)	//jyoshi, setsuzoku-jyoshi
			   || (currentPosId == 19)	//jyoshi, tokushu
			   || (currentPosId == 20)	//jyoshi, fukushi-ka
			   || (currentPosId == 21)	//jyoshi, fuku-jyoshi
			   || (currentPosId == 22)	//jyoshi, fuku-jyoshi, heiritsu-jyoshi, shu-jyoshi
			   || (currentPosId == 23)	//jyoshi, heiritsu-jyoshi
			   || (currentPosId == 24)	//jyoshi, rentai-ka	   
			   )	
			{
				return KEYWORD_NO_ACTION;
			}
			
			//# kanto particles (posId 0->2)
			if(   (currentPosId == 0)	//sonota, kanto
			   || (currentPosId == 1)	//filler
			   || (currentPosId == 2)	//kando-shi	   
			   )	
			{
				return KEYWORD_NO_ACTION;
			}
			
			//# kigou particles (posId 3->9)
			if(//   (currentPosId == 3)	//kigou, alphabet
			   (currentPosId == 4)	//kigou, ippan
			   || (currentPosId == 5)	//kigou, kakko
			   || (currentPosId == 6)	//kigou, kakko-toji
			   || (currentPosId == 7)	//kigou, ku-ten
			   || (currentPosId == 8)	//kigou, ku-haku
			   || (currentPosId == 9)	//kigou, tou-ten  
			   )	
			{
				return KEYWORD_NO_ACTION;
			}			
			
			break;
		case MECAB_DIC_IPA:
			break;
		case MECAB_DIC_JUMAN:
			break;	
		case MECAB_DIC_UNI:
			break;			
	}
	
	/*
	 //# ignore punctuation, but not alphabet (posId 4 to 9)
	 if((currentPosId >= 4) && (currentPosId <= 9)){
	 return KEYWORD_NO_ACTION;
	 }
	 
	 //# concatenate numbers (posId 48 can be digits or kanji)
	 //note that this will NOT recognize commas or decimal points
	 if((previousPosId == 48) && (currentPosId == 48)){
	 return KEYWORD_REPLACE;
	 }
	 
	 //# append particle to preceeding verb or adjective
	 //we do this to register verbs and adjectives as keywords in their complete form  
	 if(((previousPosId == 18) || (previousPosId == 25) || (previousPosId == 31) || (previousPosId == 32) || (previousPosId == 33) || (previousPosId == 54) || (previousPosId == 62) || (previousPosId == 65))
	 && ((currentPosId == 18) || (currentPosId == 22) || (currentPosId == 25) || (currentPosId == 32) || (currentPosId == 54) || (currentPosId == 62) || (currentPosId == 65))){
	 return KEYWORD_REPLACE;
	 }
	 
	 //# append particle to preceeding noun
	 //same as above; particles have no meaning by themselves
	 //we sould either drop them or attach them to the previous phrase
	 if((((previousPosId >= 36) && (previousPosId <= 39)) || (previousPosId == 41) || ((previousPosId >= 46) && (previousPosId <= 48)) || ((previousPosId >= 50) && (previousPosId <= 53)) || (previousPosId == 58))
	 && (((currentPosId >= 50) && (currentPosId <= 53)) || (currentPosId == 56) || (currentPosId == 58))){
	 return KEYWORD_REPLACE;
	 }
	 
	 //# append verb/noun/adjective to prefix
	 //reverse of what we did above
	 //prefixes have no meaning by themselves
	 if(((previousPosId >= 27) && (previousPosId <= 30))
	 && ((currentPosId == 10) || (currentPosId == 31) || (currentPosId == 36) || (currentPosId == 38) || (currentPosId == 48))){
	 return KEYWORD_REPLACE;
	 }
	 
	 //# concatenate consecutive common nouns (as opposes to names which should not be merged)
	 if(((previousPosId == 38) || (previousPosId == 41))
	 && ((currentPosId == 38) || (currentPosId == 41))){
	 return KEYWORD_REPLACE;
	 }
	 */

	
	return KEYWORD_ADD;
	
}