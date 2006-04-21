// ConfigCreatorUtil.cpp: implementation of the CConfigCreatorUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "ConfigCreatorUtil.h"
#include "ModuleUtil.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
 


XERCES_CPP_NAMESPACE_USE


class XStr
{
public :
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }
private:
    XMLCh*   fUnicodeForm;
};


#define X(str) XStr(str).unicodeForm()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigCreatorUtil::CConfigCreatorUtil()
{
	multiParams = new CConfigCreatorUtilMultiParams();
}

CConfigCreatorUtil::~CConfigCreatorUtil()
{
	delete multiParams;
}

DOMNode * CConfigCreatorUtil::getEmptyConfigForModule(char *moduleName)
{
	try
	{
		XMLPlatformUtils::Initialize();		
			 
		DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
		if (impl)
		{
			 DOMDocument* doc = impl->createDocument(
                           NULL,                    // root element namespace URI.
                           X("module"),         // root element name
                           NULL);                   // document type object (DTD).
			 DOMElement* root = doc->getDocumentElement();
			 root->setAttribute(X("moduleName"),X(moduleName));

			 return doc;
		}
	} catch (...)
	{
		AfxMessageBox("ERROR: Problem when manipulating XML config!");
	}
	return NULL;
}

void CConfigCreatorUtil::serializeConfigForModule(DOMNode *modConfig,FILE *f)
{
	if (!modConfig)
		return;

	DOMElement* root = ((DOMDocument *)modConfig)->getDocumentElement();

	DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(X("Core"));
    DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

	MemBufFormatTarget *myFormTarget = new MemBufFormatTarget();

	theSerializer->writeNode(myFormTarget, *root);

	const unsigned char *buffer = myFormTarget->getRawBuffer();
	fprintf(f,"%s\n",buffer);

	delete myFormTarget;
	delete theSerializer;

}

void CConfigCreatorUtil::addParamToConfig(DOMNode *modConfig, char *paramName, char *paramValue)
{
	if (!modConfig)
		return;

	DOMElement* root = ((DOMDocument *)modConfig)->getDocumentElement();

	addParamFromNode((DOMDocument *)modConfig,root,paramName,paramValue);
}

void CConfigCreatorUtil::releaseConfig(DOMNode *modConfig)
{
	if (!modConfig)
		return;
	modConfig->release();
}

void CConfigCreatorUtil::addParamFromNode(DOMDocument *doc,DOMNode *node, char *paramName, char *paramValue)
{
	if (!strstr(paramName,"/"))
	{
		// no more '/' so the node should be added here
		DOMElement *newNode=doc->createElement(X(paramName));
		node->appendChild(newNode);
		newNode->setAttribute(X("value"),X(paramValue));
	} else {
		// we should create more child nodes
		int i,len;
		char newNodeName[1024];
		char remainingNodeName[1024];
		sprintf(newNodeName,paramName);
		newNodeName[strstr(paramName,"/")-paramName]=0;
		sprintf(remainingNodeName,strstr(paramName,"/")+1);	

		DOMNode *newNode=NULL;
		DOMNodeList *childNodes=node->getChildNodes();
		for (i=0,len=childNodes->getLength();i<len;i++)
		{
			if (!wcscmp(X(newNodeName),childNodes->item(i)->getNodeName()))
			{
				newNode=childNodes->item(i);
				break;
			}
		}
		if (!newNode)
		{
			newNode=doc->createElement(X(newNodeName));
		}
		node->appendChild(newNode);
		addParamFromNode(doc,newNode,remainingNodeName,paramValue);
	}
}

DOMNode * CConfigCreatorUtil::getConfigForModule(DOMElement *root, char *moduleName)
{
	DOMNodeList *childNodes = root->getElementsByTagName(X("module"));
	int i;
	int len;
	for (i=0,len=childNodes->getLength();i<len;i++)
	{
		DOMNode *moduleConfig = childNodes->item(i);
		char *moduleNameTmp = CUtil::getNodeAttribute(moduleConfig,"moduleName");
		if (!strcmp(moduleNameTmp,moduleName))
		{
			free(moduleNameTmp);
			return moduleConfig;
		}
		free(moduleNameTmp);
	}
	return NULL;
}

void CConfigCreatorUtil::parseConfigFromNode(CModuleProxy *module,DOMNode *node, char *paramName)
{
	int isLeaf=1;		
	//DOMNodeList *childNodes = node->getChildNodes();
	DOMNode *subNode = node->getFirstChild();
	if (subNode)
	{
		do {	
			if (subNode->getNodeType()==DOMNode::ELEMENT_NODE)
			{				
				char newParamName[512];
				char subNodeName[512];
				wcstombs(subNodeName,subNode->getNodeName(),200);
				sprintf(newParamName,"%s/%s",paramName,subNodeName);
				isLeaf=0;

				parseConfigFromNode(module,subNode,newParamName);
			}
		} while ((subNode=subNode->getNextSibling())!=NULL);
	};
	if (isLeaf)
	{	
		DOMNode *attrNode=node->getAttributes()->getNamedItem(_L("value"));
			
		if (strlen(paramName+1)&&attrNode)
		{
			char nodeValue[512];
			wcstombs(nodeValue,attrNode->getNodeValue(),200);
			// +8 to skip /config/
			if (module->isMultiParam(paramName+8)&&!multiParams->isKnownMultiparam(paramName+8))
			{
				multiParams->registerMultiparam(paramName+8);
				module->resetMultiParamAccess(paramName+8);
			}
			module->loadConfigParam(paramName+8,nodeValue);			
		}		
	}
}
