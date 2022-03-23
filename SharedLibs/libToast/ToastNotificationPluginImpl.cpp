/**************************************************************************
*       Copyright (c) 2012, Cisco Systems, All Rights Reserved
***************************************************************************
*
*  File:    ToastNotificationImpl.h
*  Author:  Adam Veres <adveres@cisco.com>
*  Date:    09/2012
*
***************************************************************************
*
*   This file defines the functions that create a toast notification using
*   COM.
*
***************************************************************************/

#include "pch.h"
#include <string>
#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#include <wrl\client.h>
#include <wrl\implements.h>

#include "ToastNotificationPluginImpl.h"

using namespace Microsoft::WRL;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace Windows::Foundation;


#define XML_IMAGE_TAG		L"image"
#define XML_SOURCE_TAG		L"src"
#define XML_TEXT_TAG		L"text"
#define WINDOWS_FILE_TAG	L"file:///"


CToastNotificationPlugin::CToastNotificationPlugin()
    : m_pToastNotificationPluginCB(NULL)
    , m_toastActivatedToken( { 0 } )
    , m_toastDismissedToken( { 0 } )
    , m_toastFailedToken( { 0 } )
{
}

CToastNotificationPlugin::~CToastNotificationPlugin()
{
    resetToast();
}

/*
** void CToastNotificationPlugin::resetToast
**  Cleans up the IToastNotification object used to display toast notifications.
*/
void CToastNotificationPlugin::resetToast()
{
    if (m_comptrToast)
    {
        // Unregister event handlers.
        // This is important, otherwise the CToastEventHandler instance
        // passed by createToast upon registering event handlers does not 
        // get cleaned up properly upon plugin disposal.
        //
        if (0 != m_toastActivatedToken.value)
        {
            m_comptrToast->remove_Activated(m_toastActivatedToken);
            m_toastActivatedToken.value = 0;
        }
        if (0 != m_toastDismissedToken.value)
        {
            m_comptrToast->remove_Dismissed(m_toastDismissedToken);
            m_toastDismissedToken.value = 0;
        }
        if (0 != m_toastFailedToken.value)
        {
            m_comptrToast->remove_Failed(m_toastFailedToken);
            m_toastFailedToken.value = 0;
        }
        m_comptrToast.Reset();
    }
    else
    {
        m_toastActivatedToken.value = 0;
        m_toastDismissedToken.value = 0;
        m_toastFailedToken.value = 0;
    }
}

/**
 * This is the main function that will be called by the plugin.
 * It converts the input to data structures easier to manage and pass the information
 * on to DisplayToast().  This function will then interpret the results and return
 * those as a PluginResult.
 */
PluginResult CToastNotificationPlugin::SendToastNotification(
                       _In_ TOASTTYPE templateType,
                       _In_ const wchar_t *appId, 
                       _In_ const wchar_t *iconPath,
                       _In_ const wchar_t **textValues,
                       _In_ unsigned int textValuesCount)
{
	//Check input
	if( (templateType < ToastImageAndText02 || templateType >= INVALIDTYPE) ||
		(nullptr == appId) ||
		(nullptr == textValues) ||
		(nullptr == iconPath) ||
		(textValuesCount <= 0) )
	{
		return Plugin_BadParameter;
	}

    PluginResult ret = Plugin_GenericError;
    std::vector<std::wstring> messageList;

    for(size_t i = 0; i < textValuesCount; i++)
    {
        messageList.push_back(textValues[i]);
    }

    HRESULT hr = displayToast(convertToastType(templateType), appId, iconPath, messageList);
    if( S_OK == hr )
    {
        ret = Plugin_Success;
    }
	else
	{
        if( NULL != m_pToastNotificationPluginCB)
        {
            m_pToastNotificationPluginCB->OnToastLog(L"DisplayToast failed", hr);
        }
	}

    return ret;
}

/*
 * Display the toast using COM.
 * @param templateType the template type as designated by the enumerator
 * @param appId the unique app user model ID associated with the calling application
 * @param iconPath the icon name/directory
 * @param textLines a vector where each index is a line for the toast notification
 * @return hr the HRESULT of this display attempt
 */
HRESULT CToastNotificationPlugin::displayToast(
                       _In_ ToastTemplateType templateType,
                       _In_ const std::wstring &appId, 
                       _In_ const std::wstring &iconPath, 
                       _In_ std::vector<std::wstring> &textLines)
{
    ComPtr<IToastNotificationManagerStatics> toastStatics;
    HRESULT hr = GetActivationFactory(CStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).GetStr(), &toastStatics);

    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlDocument> toastXml;
        hr = this->createToastXml(templateType, toastStatics.Get(), &toastXml , iconPath, textLines);
        if (SUCCEEDED(hr))
        {
            hr = this->createToast(toastStatics.Get(), toastXml.Get(), appId);
        }
        else
        {
            if( NULL != m_pToastNotificationPluginCB)
            {
                m_pToastNotificationPluginCB->OnToastLog(L"Toast XML creation failed", hr);
            }
        }
    }

    return hr;
}

/*
 * Create the toast XML from a template
 * @param toastManager toast manager that holds statics we need to acess
 * @param templateType the template type as designated by the enumerator
 * @param iconPath the icon name/directory
 * @param textLines a vector where each index is a line for the toast notification
 * @return hr the HRESULT of this XML creation
 */
HRESULT CToastNotificationPlugin::createToastXml(
                       _In_ ToastTemplateType templateType,
                       _In_ IToastNotificationManagerStatics *toastManager, 
                       _Outptr_ IXmlDocument** inputXml,
                       _In_ const std::wstring &iconPath, 
                       _In_ std::vector<std::wstring> &textLines)
{
    // Retrieve the XML template to fill in later
    HRESULT hr = toastManager->GetTemplateContent(templateType, inputXml);
    if (SUCCEEDED(hr))
    {
        wchar_t *imagePath = _wfullpath(nullptr, iconPath.c_str(), MAX_PATH);
        hr = (imagePath != nullptr) ? S_OK : HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        if(SUCCEEDED(hr))
        {
            hr = this->setImageSrc(imagePath, *inputXml);
            if (SUCCEEDED(hr))
            {
                hr = this->setTextValues(textLines, *inputXml);
            }
        }
    }

    return hr;
}

/**
 * Set the value of the "src" attribute of the "image" node
 *
 * @param imagePath path to the image
 * @param toastXml the toast XML to edit
 * @return hr the HRESULT of this function
 */
HRESULT CToastNotificationPlugin::setImageSrc(
                       _In_ const std::wstring &imagePath, 
                       _Inout_ IXmlDocument *toastXml)
{
    wchar_t imageSrc[MAX_PATH] = WINDOWS_FILE_TAG;
    HRESULT hr = StringCchCat(imageSrc, ARRAYSIZE(imageSrc), imagePath.c_str());
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNodeList> nodeList;
        hr = toastXml->GetElementsByTagName(CStringReference(XML_IMAGE_TAG).GetStr(), &nodeList);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> imageNode;
            hr = nodeList->Item(0, &imageNode);
            if (SUCCEEDED(hr))
            {
                ComPtr<IXmlNamedNodeMap> attributes;
                hr = imageNode->get_Attributes(&attributes);
                if (SUCCEEDED(hr))
                {
                    ComPtr<IXmlNode> srcAttribute;

                    hr = attributes->GetNamedItem(CStringReference(XML_SOURCE_TAG).GetStr(), &srcAttribute);
                    if (SUCCEEDED(hr))
                    {
                        hr = this->setNodeValueString(CStringReference(imageSrc).GetStr(), srcAttribute.Get(), toastXml);
                    }
                }
            }
        }
    }
    return hr;
}

/**
 * Set the values of each of the text nodes corresponding to
 * the vector of strings passed in
 * @param textValues array of wstrings
 * @param toastXML the XML tree we are modifying
 * @return hr HRESULT with success or failure
 */
HRESULT CToastNotificationPlugin::setTextValues(
                       _In_ std::vector<std::wstring> &textValues,
                       _Inout_ IXmlDocument *toastXml)
{
    HRESULT hr = (!textValues.empty()) ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNodeList> nodeList;
        hr = toastXml->GetElementsByTagName(CStringReference(XML_TEXT_TAG).GetStr(), &nodeList);
        if (SUCCEEDED(hr))
        {
            UINT32 nodeListLength;
            hr = nodeList->get_Length(&nodeListLength);
            if (SUCCEEDED(hr))
            {
                hr = (textValues.size() <= nodeListLength) ? S_OK : E_INVALIDARG;
                if (SUCCEEDED(hr))
                {
                    for (UINT32 i = 0; i < textValues.size(); i++)
                    {
                        ComPtr<IXmlNode> textNode;
                        hr = nodeList->Item(i, &textNode);
                        if (SUCCEEDED(hr))
                        {
                            hr = this->setNodeValueString(CStringReference(textValues[i]).GetStr(), textNode.Get(), toastXml);
                        }
                    }
                }
            }
        }
    }
    return hr;
}

/**
 * Sets the value of a node to the specified string
 * @param inputString HSTRING with text to set node to
 * @param node the XML node to set with text
 * @param xmlDoc the XML document to assign the new node to
 */
HRESULT CToastNotificationPlugin::setNodeValueString(
                       _In_ HSTRING inputString, 
                       _Inout_ IXmlNode *node, 
                       _Inout_ IXmlDocument *xmlDoc)
{
    ComPtr<IXmlText> inputText;
    HRESULT hr = xmlDoc->CreateTextNode(inputString, &inputText);
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNode> inputTextNode;
        hr = inputText.As(&inputTextNode);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> pAppendedChild;
            hr = node->AppendChild(inputTextNode.Get(), &pAppendedChild);
        }
    }

    return hr;
}

/**
 * Create and display the toast notification.
 * @param toastManager pointer to the toast manager
 * @param xml the XML document describing the toast notification
 * @return hr the result of this function
 */
HRESULT CToastNotificationPlugin::createToast( 
                       _In_ IToastNotificationManagerStatics *toastManager, 
                       _In_ IXmlDocument *xml,
                       _In_ std::wstring appId)
{
    ComPtr<IToastNotifier> notifier;

    HRESULT hr = toastManager->CreateToastNotifierWithId(CStringReference(appId).GetStr(), &notifier);

    if (SUCCEEDED(hr))
    {
        ComPtr<IToastNotificationFactory> factory;
        hr = GetActivationFactory(CStringReference(RuntimeClass_Windows_UI_Notifications_ToastNotification).GetStr(), &factory);
        if (SUCCEEDED(hr))
        {
            // Clean up previous IToastNotification instance, if needed.
            resetToast();
            hr = factory->CreateToastNotification(xml, &m_comptrToast);
            if (SUCCEEDED(hr))
            {
                // Register the event handlers before showing
                ComPtr<CToastEventHandler> eventHandler(new CToastEventHandler(m_pToastNotificationPluginCB));
                hr = m_comptrToast->add_Activated(eventHandler.Get(), &m_toastActivatedToken);
                if (SUCCEEDED(hr))
                {
                    hr = m_comptrToast->add_Dismissed(eventHandler.Get(), &m_toastDismissedToken);
                    if (SUCCEEDED(hr))
                    {
                        hr = m_comptrToast->add_Failed(eventHandler.Get(), &m_toastFailedToken);
                        if (SUCCEEDED(hr))
                        {
                            hr = notifier->Show(m_comptrToast.Get());
                        }
                    }
                }
            }
        }
    }
    return hr;
}

/**
 * Convert the TOASTYPE enum to the Windows ToastTemplateType
 * @param inputType the TOASTTYPE to convert
 * @return returnType the corresponding ToastTemplateType
 */
ToastTemplateType CToastNotificationPlugin::convertToastType( TOASTTYPE inputType )
{
    ToastTemplateType returnType = ToastTemplateType_ToastImageAndText02;

    switch(inputType)
    {
    case ToastImageAndText02:
        returnType = ToastTemplateType_ToastImageAndText02;
        break;

    case ToastImageAndText03:
        returnType = ToastTemplateType_ToastImageAndText03;
        break;

    case ToastImageAndText04:
        returnType = ToastTemplateType_ToastImageAndText04;
        break;

    case INVALIDTYPE:
    default:
        //Intentionally blank
        break;
    }

    return returnType;
}

/**
 * Checks if the Windows 8 Start Menu is currently visible
 * @return bRet true for yes, false for no
 */
bool CToastNotificationPlugin::IsStartMenuVisible()
{
	bool bRet = false;
	Microsoft::WRL::ComPtr<IAppVisibility> spAppVisibility;
	HRESULT hr = CoCreateInstance(CLSID_AppVisibility, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spAppVisibility));
	if (SUCCEEDED(hr))
	{
		// Display the current launcher visibility
		BOOL bIsStartMenuVisible = FALSE;
		if (SUCCEEDED(spAppVisibility->IsLauncherVisible(&bIsStartMenuVisible)))
		{
			if(TRUE == bIsStartMenuVisible)
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

/***************************************************************************
*
*   Plugin implementation
*
***************************************************************************/

/*
 * Whenever there is any change in the plugin directory this function will be called.
 * A change in the plug-in directory indicates that new plugins are added or existing ones
 * are removed. So take necessary action.
 */
PluginResult CToastNotificationPlugin::PluginDirChanged()
{
	return Plugin_GenericError;
}

PluginResult CToastNotificationPlugin::SetToastNotificationPluginCB(IToastNotificationPluginCB *cb)
{
    if (NULL == cb)
    {
      return Plugin_NullPointer;
    }

    m_pToastNotificationPluginCB = cb;
    return Plugin_Success;
}

PluginResult CToastNotificationPlugin::UnSetToastNotificationPluginCB()
{
    m_pToastNotificationPluginCB = NULL;
    return Plugin_Success;
}
