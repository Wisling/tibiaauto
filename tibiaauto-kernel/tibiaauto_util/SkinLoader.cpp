// SkinLoader.cpp: implementation of the CSkinLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "SkinLoader.h"
#include "Util.h"
#include "Skin.h"
#include "InstallPath.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>


XERCES_CPP_NAMESPACE_USE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG


CSkin loadCurrentSkin(CString currentPathBuf)
{
	XMLPlatformUtils::Initialize();
	CSkin loadedSkin;
	XercesDOMParser *parser = new XercesDOMParser();

	try
	{
		parser->parse(currentPathBuf);
		xercesc::DOMDocument *doc = parser->getDocument();
		if (!doc || !doc->getChildNodes())
			throw exception();
		DOMNode *root = doc->getChildNodes()->item(0);
		DOMNode *item = root->getFirstChild();
		item                            = item->getNextSibling();
		loadedSkin.m_ButtonFaceRedValue = CUtil::getNodeIntAttribute(item, L"Red");
		if (loadedSkin.m_ButtonFaceRedValue < 0 || loadedSkin.m_ButtonFaceRedValue > 255)
			throw exception();
		loadedSkin.m_ButtonFaceGreenValue          = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonFaceBlueValue           = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonDSRedValue              = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonDSGreenValue            = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonDSBlueValue             = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonLSRedValue              = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonLSGreenValue            = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonLSBlueValue             = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonHighLightRedValue       = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonHighLightGreenValue     = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonHighLightBlueValue      = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonLightRedValue           = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonLightGreenValue         = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonLightBlueValue          = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_TextRedValue                  = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_TextGreenValue                = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_TextBlueValue                 = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_EditboxRedValue               = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_EditboxGreenValue             = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_EditboxBlueValue              = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_PrimaryBackgroundRedValue     = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_PrimaryBackgroundGreenValue   = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_PrimaryBackgroundBlueValue    = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_SecondaryBackgroundRedValue   = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_SecondaryBackgroundGreenValue = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_SecondaryBackgroundBlueValue  = CUtil::getNodeIntAttribute(item, L"Blue");
	}
	catch (...)
	{
		AfxMessageBox("Unable to load .skin file!");
		delete parser;
		return skin;
	}

	delete parser;

	return loadedSkin;
}

CSkin loadSkin(CString pathBuf)
{
	CSkin loadedSkin;
	XercesDOMParser *parser = new XercesDOMParser();
	try
	{
		parser->parse(pathBuf);
		xercesc::DOMDocument *doc = parser->getDocument();
		if (!doc || !doc->getChildNodes())
			throw exception();
		DOMNode *root = doc->getChildNodes()->item(0);
		DOMNode *item = root->getFirstChild();
		item                            = item->getNextSibling();
		loadedSkin.m_ButtonFaceRedValue = CUtil::getNodeIntAttribute(item, L"Red");
		if (loadedSkin.m_ButtonFaceRedValue < 0 || loadedSkin.m_ButtonFaceRedValue > 255)
			throw exception();
		loadedSkin.m_ButtonFaceGreenValue          = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonFaceBlueValue           = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonDSRedValue              = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonDSGreenValue            = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonDSBlueValue             = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonLSRedValue              = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonLSGreenValue            = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonLSBlueValue             = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonHighLightRedValue       = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonHighLightGreenValue     = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonHighLightBlueValue      = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_ButtonLightRedValue           = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_ButtonLightGreenValue         = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_ButtonLightBlueValue          = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_TextRedValue                  = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_TextGreenValue                = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_TextBlueValue                 = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_EditboxRedValue               = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_EditboxGreenValue             = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_EditboxBlueValue              = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_PrimaryBackgroundRedValue     = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_PrimaryBackgroundGreenValue   = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_PrimaryBackgroundBlueValue    = CUtil::getNodeIntAttribute(item, L"Blue");
		item                                       = item->getNextSibling();
		item                                       = item->getNextSibling();
		loadedSkin.m_SecondaryBackgroundRedValue   = CUtil::getNodeIntAttribute(item, L"Red");
		loadedSkin.m_SecondaryBackgroundGreenValue = CUtil::getNodeIntAttribute(item, L"Green");
		loadedSkin.m_SecondaryBackgroundBlueValue  = CUtil::getNodeIntAttribute(item, L"Blue");
	}
	catch (...)
	{
		AfxMessageBox("Unable to load .skin file!");
	}

	delete parser;

	return loadedSkin;
}

bool saveSkin(CString pathBuf, CSkin saveSkin, bool saveSeperate)
{
	CUtil util;
	XercesDOMParser *parser = new XercesDOMParser();

	try
	{
		FILE *f = NULL;
		f = fopen(pathBuf.GetBuffer(200), "wb");
		char currentPathBuf[2048];
		sprintf(currentPathBuf, "%s\\skins\\CurrentSkin.skin", CInstallPath::getInstallPath().c_str());

		parser->parse(pathBuf);
		xercesc::DOMDocument* doc = NULL;
		DOMImplementation* impl   = DOMImplementationRegistry::getDOMImplementation(L"Core");
		DOMElement* root          = NULL;
		DOMElement* child         = NULL;
		XMLCh tempStr[100];
		char buf[128];
		if (impl)
		{
			doc  = impl->createDocument(NULL, L"Skin", NULL);
			root = doc->getDocumentElement();
			root->setAttribute(L"Name", L"Default");

			child = doc->createElement(L"ButtonFace");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_ButtonFaceRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonFaceGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonFaceBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"ButtonDS");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_ButtonDSRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonDSGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonDSBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"ButtonLS");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_ButtonLSRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonLSGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonLSBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"ButtonHighLight");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_ButtonHighLightRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonHighLightGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonHighLightBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"ButtonLight");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_ButtonLightRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonLightGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_ButtonLightBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"Text");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_TextRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_TextGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_TextBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"Editbox");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_EditboxRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_EditboxGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_EditboxBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"PrimaryBackground");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_PrimaryBackgroundRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_PrimaryBackgroundGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_PrimaryBackgroundBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);

			child = doc->createElement(L"SecondaryBackground");
			root->appendChild(child);
			sprintf(buf, "%d", saveSkin.m_SecondaryBackgroundRedValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Red", tempStr);
			sprintf(buf, "%d", saveSkin.m_SecondaryBackgroundGreenValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Green", tempStr);
			sprintf(buf, "%d", saveSkin.m_SecondaryBackgroundBlueValue);
			XMLString::transcode(buf, tempStr, 99);
			child->setAttribute(L"Blue", tempStr);
		}

		XMLString::transcode("LS", tempStr, 99);
		impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
		DOMConfiguration* dc           = theSerializer->getDomConfig();
		if (dc->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
			dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
		fclose(f);
		xercesc::XMLFormatTarget *outfile = new xercesc::LocalFileFormatTarget(pathBuf);
		DOMLSOutput *lsOut                = ((DOMImplementationLS*)impl)->createLSOutput();
		lsOut->setByteStream(outfile);
		theSerializer->write(doc, lsOut);
		lsOut->release();
		delete outfile;

		outfile = new xercesc::LocalFileFormatTarget(currentPathBuf);
		lsOut   = ((DOMImplementationLS*)impl)->createLSOutput();
		lsOut->setByteStream(outfile);
		theSerializer->write(doc, lsOut);
		lsOut->release();
		doc->release();
		theSerializer->release();
		delete outfile;
	}
	catch (...)
	{
		AfxMessageBox("Unable to save .skin file!");
		return false;
	}
	delete parser;

	return true;
}
