/*
 *  ARRAY_TEXT.cpp
 *  4D Plugin
 *
 *  Created by miyako on 11/01/03.
 *
 */

#include "ARRAY_TEXT.h"

void ARRAY_TEXT::fromParamAtIndex(PackagePtr pParams, uint32_t index)
{	
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);
		
		this->setSize(0);		
		
		if(arr.fType == eVK_ArrayUnicode)
		{	
			uint32_t i;
			
			PA_Unistring str;
			
			for(i = 0; i <= (uint32_t)arr.uValue.fArray.fNbElements; i++)
			{				
				str = (*(PA_Unistring **) (arr.uValue.fArray.fData))[i];
				CUTF16String u = CUTF16String(str.fString, str.fLength);
				this->_CUTF16StringArray->push_back(u);
			}
			
		}

	}
	
}

void ARRAY_TEXT::toParamAtIndex(PackagePtr pParams, uint32_t index)
{
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);
		PA_Variable *param = ((PA_Variable *)pParams[index - 1]);
		
		switch (arr.fType) 
		{
			case eVK_ArrayUnicode:
				break;
			case eVK_Undefined:
				PA_ClearVariable(&arr);				
				arr = PA_CreateVariable(eVK_ArrayUnicode);
				param->fType = arr.fType;
				break;				
			
			default:
				break;
		}
		
		if(arr.fType == eVK_ArrayUnicode)
		{
			
			PA_ResizeArray(&arr, this->_CUTF16StringArray->empty() ? 0 : (uint32_t)(this->_CUTF16StringArray->size() - 1));
			
			uint32_t i;
			
			for(i = 0; i < this->_CUTF16StringArray->size(); i++)
			{
				PA_Unistring str = PA_CreateUnistring((PA_Unichar *)this->_CUTF16StringArray->at(i).c_str());
				PA_SetStringInArray(arr, i, &str);					
			}
			
			param->fFiller = 0;
			param->uValue.fArray.fCurrent = arr.uValue.fArray.fCurrent;
			param->uValue.fArray.fNbElements = arr.uValue.fArray.fNbElements;
			param->uValue.fArray.fData = arr.uValue.fArray.fData;			
			
		}
		
	}
	
}

void ARRAY_TEXT::convertFromUTF8(const CUTF8String* fromString, CUTF16String* toString)	
{
	uint32_t size = ((uint32_t)fromString->length() * sizeof(PA_Unichar)) + sizeof(PA_Unichar);
	std::vector<uint8_t> buf(size);
	
	uint32_t len = PA_ConvertCharsetToCharset(
											  (char *)fromString->c_str(),
											  (uint32_t)fromString->length(),
											  eVTC_UTF_8,
											  (char *)&buf[0],
											  size,
											  eVTC_UTF_16
											  );
	
//	*toString = CUTF16String((const PA_Unichar *)&buf[0], len);	
	*toString = CUTF16String((const PA_Unichar *)&buf[0], len/2);	
}

void ARRAY_TEXT::convertToUTF8(const CUTF16String* fromString, CUTF8String* toString)
{
	uint32_t size = ((uint32_t)fromString->length() * 4) + sizeof(uint8_t);
	std::vector<uint8_t> buf(size);
	
	uint32_t len = PA_ConvertCharsetToCharset(
											  (char *)fromString->c_str(),
											  (uint32_t)fromString->length() * sizeof(PA_Unichar),
											  eVTC_UTF_16,
											  (char *)&buf[0],
											  size,
											  eVTC_UTF_8
											  );
	
	*toString = CUTF8String((const uint8_t *)&buf[0], len);
}

void ARRAY_TEXT::copyUTF8StringAtIndex(CUTF8String* pString, uint32_t index)
{	
	if(index < this->_CUTF16StringArray->size())
	{
		CUTF16String s = CUTF16String(this->_CUTF16StringArray->at(index));
		convertToUTF8(&s, pString);		
	}
}

void ARRAY_TEXT::copyUTF16StringAtIndex(CUTF16String* pString, uint32_t index)
{	
	if(index < this->_CUTF16StringArray->size())
	{
		*pString = CUTF16String(this->_CUTF16StringArray->at(index));
	}
}

void ARRAY_TEXT::setUTF16StringAtIndex(const PA_Unichar* pString, uint32_t len, uint32_t index)
{
	CUTF16String u;
	u = CUTF16String(pString, (size_t)len);	
	this->setUTF16StringAtIndex(&u, index);
}

void ARRAY_TEXT::setUTF16StringAtIndex(CUTF16String* pString, uint32_t index)
{
	if(index < this->_CUTF16StringArray->size())
	{		
		CUTF16String str;
		
		if(pString->c_str())
		{
			str = CUTF16String(pString->c_str(), pString->length());
		}else{
			str = CUTF16String();		
		}
		
		std::vector<CUTF16String>::iterator it = this->_CUTF16StringArray->begin();
		it += index;
		this->_CUTF16StringArray->insert(this->_CUTF16StringArray->erase(it), str);	
	}
}

void ARRAY_TEXT::appendUTF8String(CUTF8String* pString)
{
	CUTF16String u16;	
	convertFromUTF8(pString, &u16);
	this->_CUTF16StringArray->push_back(u16);		
}

void ARRAY_TEXT::appendUTF8String(const uint8_t* pString, uint32_t len){

	CUTF8String u8 = CUTF8String(pString, len);
	CUTF16String u16;
	convertFromUTF8(&u8, &u16);
	this->_CUTF16StringArray->push_back(u16);		
}

void ARRAY_TEXT::appendUTF16String(const PA_Unichar* pString)
{
	CUTF16String u = CUTF16String(pString);
	this->_CUTF16StringArray->push_back(u);		
}

void ARRAY_TEXT::appendUTF16String(const PA_Unichar* pString, uint32_t len)
{
	CUTF16String u = CUTF16String(pString, len);
	this->_CUTF16StringArray->push_back(u);		
}

void ARRAY_TEXT::setUTF16StringAtIndex(const PA_Unichar* pString, uint32_t index)
{
	CUTF16String u = CUTF16String(pString);
	this->_CUTF16StringArray->push_back(u);		
}

void ARRAY_TEXT::appendUTF16String(CUTF16String* pString)
{
	CUTF16String u = CUTF16String(pString->c_str(), pString->length());	
	this->_CUTF16StringArray->push_back(u);		
}

void ARRAY_TEXT::appendUTF16String(const PA_Unistring* pString)
{
	CUTF16String u = CUTF16String(pString->fString, pString->fLength);
	this->_CUTF16StringArray->push_back(u);	
}

#if VERSIONMAC
#ifdef __OBJC__	
void ARRAY_TEXT::appendUTF16String(const NSString* pString)
{	
	uint32_t len = [pString length];
	uint32_t size = (len * sizeof(PA_Unichar)) + sizeof(PA_Unichar);
	std::vector<uint8_t> buf(size);	
	
	if([pString getCString:(char *)&buf[0] maxLength:size encoding:NSUnicodeStringEncoding])
	{
		CUTF16String u;
		u = CUTF16String((const PA_Unichar *)&buf[0], len);
		this->_CUTF16StringArray->push_back(u);		
	}

}

void ARRAY_TEXT::setUTF16StringAtIndex(NSString* pString, uint32_t index)
{
	uint32_t len = [pString length];
	uint32_t size = (len * sizeof(PA_Unichar)) + sizeof(PA_Unichar);
	std::vector<uint8_t> buf(size);	
	
	if([pString getCString:(char *)&buf[0] maxLength:size encoding:NSUnicodeStringEncoding])
		this->setUTF16StringAtIndex((const PA_Unichar *)&buf[0], len, index);	
}
#endif
#endif

uint32_t ARRAY_TEXT::getSize()
{
	return (uint32_t)this->_CUTF16StringArray->size();		
}

void ARRAY_TEXT::setSize(uint32_t size)
{	
	this->_CUTF16StringArray->resize(size);
}

ARRAY_TEXT::ARRAY_TEXT() : _CUTF16StringArray(new CUTF16StringArray)
{
	
}

ARRAY_TEXT::~ARRAY_TEXT()
{ 
	delete _CUTF16StringArray; 
}