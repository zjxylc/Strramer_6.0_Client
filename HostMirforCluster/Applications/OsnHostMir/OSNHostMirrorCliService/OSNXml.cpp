﻿#include <stdlib.h>
#include "string"
#include "OSNXml.h"

DWORD  COSNxml::GetXMLText(char *Msg)
{
	return UnicodeToUTF_8(m_pDoc->xml,Msg);
}

DWORD COSNxml::UnicodeToUTF_8(wchar_t *pUnicode ,char *pUtf8)
{
	DWORD iLen = 0;

	iLen = WideCharToMultiByte(CP_UTF8,0,pUnicode,-1,NULL,0,NULL,NULL);

	WideCharToMultiByte(CP_UTF8,0,pUnicode,-1,pUtf8,iLen,NULL,NULL);

	return iLen;
}

DWORD COSNxml::UTF_8ToUnicode(char *pUtf8 ,wchar_t *pUnicode)
{
	DWORD iLen = 0;

	iLen = MultiByteToWideChar(CP_UTF8,0,pUtf8,-1,NULL,0);

	MultiByteToWideChar(CP_UTF8,0,pUtf8,-1,pUnicode,iLen);

	return iLen;
}

DWORD  COSNxml::AddXMLElement(char *pElementName)
{
	if(!m_pRootElement)
		return EXIT_FAILURE;

	m_pNodeSub = m_pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)pElementName, (_bstr_t)(char*)"");
	m_pNode = (MSXML2::IXMLDOMNodePtr) m_pRootElement;
    m_pRootElement->appendChild(m_pNodeSub); 

	return EXIT_SUCCESS;
}

DWORD  COSNxml::AddXMLAttribute(char *pNodeName,char *pAttributeName,char *pAttributeValue)
{
	m_pElement = (MSXML2::IXMLDOMElementPtr)m_pNodeSub;
    m_pElement->setAttribute((_bstr_t)(char*)pAttributeName, (_variant_t)(char*)pAttributeValue);
    m_pNode->appendChild((MSXML2::IXMLDOMNodePtr)m_pElement);

	return EXIT_SUCCESS;
}

DWORD  COSNxml::AddXMLElement(char *pFatName,char *pSubName)
{
	m_pNodeList = m_pRootElement->getElementsByTagName((_bstr_t)(char*)pFatName);
	if(m_pNodeList->Getlength() == 0)
	{
		return EXIT_FAILURE;
	}
	
	for(int i=0;i<m_pNodeList->Getlength();i++)
	{
		m_pNode = m_pNodeList->Getitem(i);
		break;
	}

	m_pNodeSub = m_pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)pSubName, (_bstr_t)(char*)"");
    m_pNode->appendChild(m_pNodeSub);

	return EXIT_SUCCESS;
}

DWORD  COSNxml::AddXMLChannelElement(char *pFatName,char *pSubName)
{
	_variant_t  variantValue;

	m_pNodeList = m_pRootElement->getElementsByTagName((_bstr_t)(char*)pFatName);
	for(int i=0;i<m_pNodeList->Getlength();i++)
	{
		m_pNode = m_pNodeList->Getitem(i);
		break;
	}

	m_pNodeSub = m_pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)pSubName, (_bstr_t)(char*)"");
	m_pNode->appendChild(m_pNodeSub);

	variantValue.Clear();
	return EXIT_SUCCESS;
}

DWORD  COSNxml::AddXMLVolElement(char *pFatName,char *pSubName,char *pDiskGuid)
{
	_variant_t  variantValue;

	m_pNodeList = m_pRootElement->getElementsByTagName((_bstr_t)(char*)pFatName);
	if(m_pNodeList->Getlength() == 0)
	{
		return EXIT_FAILURE;
	}
	
	for(int i=0;i<m_pNodeList->Getlength();i++)
	{
		m_pNode = m_pNodeList->Getitem(i);
		m_pElement = m_pNode;
		if(m_pElement == NULL)
		{
			continue;
		}
		variantValue = m_pElement->getAttribute("Guid");  

		if(strcmp(pDiskGuid,(char *)(_bstr_t)variantValue) == 0)
		{
			m_pNodeSub = m_pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)pSubName, (_bstr_t)(char*)"");
			m_pNode->appendChild(m_pNodeSub);

			variantValue.Clear();
			return EXIT_SUCCESS;
		}
	}

	variantValue.Clear();
	return EXIT_FAILURE;
}

DWORD COSNxml::CreateXMLFile(char *pRootName)
{
	m_pProInstruction = m_pDoc->createProcessingInstruction((_bstr_t)(char*)"xml", (_bstr_t)(char*)"version=\"1.0\" encoding=\"utf-8\"");
    m_pDoc->appendChild((MSXML2::IXMLDOMNode*)m_pProInstruction);

    m_pRootElement =  m_pDoc->createElement((_bstr_t)pRootName);   
    m_pDoc->PutRefdocumentElement(m_pRootElement);

	return EXIT_SUCCESS;
}

DWORD COSNxml::SaveFile(char *pPathName)
{
	if(!m_pDoc)
		return EXIT_FAILURE;

	m_pDoc->save((_variant_t)pPathName);
	return EXIT_SUCCESS;
}

DWORD COSNxml::LoadFile(wchar_t *pXML)
{
	if(!m_pDoc || !pXML)
		return EXIT_FAILURE;

	m_pDoc->loadXML(pXML);
	return EXIT_SUCCESS;
}

DWORD COSNxml::GetXMLNodeAttribute(char *pAbsolutePath,char *pAttributeName,char *pOutBuffer,int Length)
{
	m_pElement = (MSXML2::IXMLDOMElementPtr)m_pDoc->selectSingleNode((_bstr_t)(char*)pAbsolutePath);
	if(m_pElement == NULL)
	{
		return EXIT_FAILURE;
	}

	_variant_t varId;  
    varId = m_pElement->getAttribute(pAttributeName);
	strcpy_s(pOutBuffer,Length,(_bstr_t)varId.bstrVal);
	varId.Clear(); 

	return EXIT_SUCCESS;
}

DWORD COSNxml::GetXMLNodeText(char *pRootName,char *pNodeName,char *pOutBuffer)
{
	m_pNode = m_pDoc->selectSingleNode((_bstr_t)(char*)pRootName);
	if(m_pNode == NULL)
	{
		return EXIT_FAILURE;
	}

	m_pNodeSub = m_pNode->selectSingleNode((_bstr_t)(char*)pNodeName);
	if(m_pNodeSub == NULL)
	{
		return EXIT_FAILURE;
	}

	strcpy(pOutBuffer,(char*)m_pNodeSub->Gettext());
	return EXIT_SUCCESS;
}