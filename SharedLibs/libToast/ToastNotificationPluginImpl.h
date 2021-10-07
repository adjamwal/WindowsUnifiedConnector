/**************************************************************************
*       Copyright (c) 2012, Cisco Systems, All Rights Reserved
***************************************************************************
*
*  File:    ToastNotificationPluginImpl.h
*  Author:  Adam Veres <adveres@cisco.com>
*  Date:    09/2012
*
***************************************************************************
*
*   This file declares the functions that create a toast notification using
*   COM.
*
***************************************************************************/

#ifndef _TOASTNOTIFICATIONPLUGINIMPL_H_
#define _TOASTNOTIFICATIONPLUGINIMPL_H_

#include "pch.h"
#include <string>
#include <vector>

#include "ToastNotificationPlugin.h"

using namespace Microsoft::WRL;
using namespace ABI::Windows::UI::Notifications;

class CToastNotificationPlugin : public IToastNotificationPlugin
{
public:
    CToastNotificationPlugin();
    virtual ~CToastNotificationPlugin();

    virtual PluginResult SendToastNotification(
        _In_ TOASTTYPE templateType,
        _In_ const wchar_t *appId,
        _In_ const wchar_t *iconPath,
        _In_ const wchar_t **textValues, 
		_In_ unsigned int textValuesCount
        );

    virtual PluginResult PluginDirChanged();
    virtual PluginResult SetToastNotificationPluginCB(IToastNotificationPluginCB *cb);
    virtual PluginResult UnSetToastNotificationPluginCB();
    virtual bool IsStartMenuVisible();

private:

    IToastNotificationPluginCB *m_pToastNotificationPluginCB;
    EventRegistrationToken m_toastActivatedToken;
    EventRegistrationToken m_toastDismissedToken;
    EventRegistrationToken m_toastFailedToken;
    ComPtr<IToastNotification> m_comptrToast;

    HRESULT displayToast(
        _In_ ABI::Windows::UI::Notifications::ToastTemplateType templateType,
        _In_ const std::wstring &appId,
        _In_ const std::wstring &iconPath,
        _In_ std::vector<std::wstring> &textLines
        );
    HRESULT createToastXml(
        _In_ ABI::Windows::UI::Notifications::ToastTemplateType templateType,
        _In_ ABI::Windows::UI::Notifications::IToastNotificationManagerStatics *toastManager, 
        _Outptr_ ABI::Windows::Data::Xml::Dom::IXmlDocument **xml,
        _In_ const std::wstring &iconPath,
        _In_ std::vector<std::wstring> &textLines
        );
    HRESULT createToast( 
        _In_ ABI::Windows::UI::Notifications::IToastNotificationManagerStatics *toastManager, 
        _In_ ABI::Windows::Data::Xml::Dom::IXmlDocument *xml,
        _In_ std::wstring appId
        );
    HRESULT setImageSrc(
        _In_ const std::wstring &imageName, 
        _Inout_ ABI::Windows::Data::Xml::Dom::IXmlDocument *toastXml
        );
    HRESULT setTextValues(
        _In_ std::vector<std::wstring> &textLines,
        _Inout_ ABI::Windows::Data::Xml::Dom::IXmlDocument *toastXml
        );
    HRESULT setNodeValueString(
        _In_ HSTRING onputString,
        _Inout_ ABI::Windows::Data::Xml::Dom::IXmlNode *node, 
        _Inout_ ABI::Windows::Data::Xml::Dom::IXmlDocument *xml
        );
    ABI::Windows::UI::Notifications::ToastTemplateType convertToastType( 
        _In_ TOASTTYPE inputType
        );

    inline IToastNotificationPluginCB* GetToastNotificationPluginCB(){ return m_pToastNotificationPluginCB; }

    void resetToast();
};

/**
 * Windows String Reference helper class
 */
class CStringReference
{
public:
    CStringReference(_In_ const std::wstring &stringItem) throw()   
    {   
        HRESULT hr = WindowsCreateStringReference(stringItem.c_str(), (UINT32)stringItem.length(), &m_header, &m_hstring);
        if (FAILED(hr))   
        {   
            RaiseException(static_cast<DWORD>(STATUS_INVALID_PARAMETER), EXCEPTION_NONCONTINUABLE, 0, nullptr);
        }
    }
    ~CStringReference()
    {
        WindowsDeleteString(m_hstring); 
    }
    HSTRING GetStr() const throw()   
    {   
        return m_hstring;   
    }   
private:
    HSTRING             m_hstring;   
    HSTRING_HEADER      m_header;   
};

#endif  //_TOASTNOTIFICATIONPLUGINIMPL_H_
