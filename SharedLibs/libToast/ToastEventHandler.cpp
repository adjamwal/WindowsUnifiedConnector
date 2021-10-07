/**************************************************************************
*       Copyright (c) 2012, Cisco Systems, All Rights Reserved
***************************************************************************
*
*  File:    ToastEventHandler.cpp
*  Author:  Adam Veres <adveres@cisco.com>
*  Date:    09/2012
*
***************************************************************************
*
*   Definition of the event handlers for toast notifications
*
***************************************************************************/

#include "pch.h"
#include "ToastEventHandler.h"
#include "ToastNotificationPlugin.h"
#include "ToastNotificationPluginImpl.h"

using namespace ABI::Windows::UI::Notifications;


CToastEventHandler::CToastEventHandler(IToastNotificationPluginCB *toastCB)
    : m_iRef(1),
	  m_pToastNotificationPluginCB(toastCB)
{
}

CToastEventHandler::~CToastEventHandler()
{
}

/**
 * DesktopToastActivatedEventHandler
 */
IFACEMETHODIMP CToastEventHandler::Invoke(_In_ IToastNotification* /* sender */, _In_ IInspectable* /* args */)
{
    bool succeeded = false;

	if( NULL != m_pToastNotificationPluginCB)
	{
		PluginResult res = m_pToastNotificationPluginCB->OnToastActivated();
        if(Plugin_Success == res)
        {
            succeeded = true;
        }
	}

    return (succeeded ? S_OK : E_FAIL);
}

/**
 * DesktopToastDismissedEventHandler
 */
IFACEMETHODIMP CToastEventHandler::Invoke(_In_ IToastNotification* /* sender */, _In_ IToastDismissedEventArgs* toastEventDismissedArg)
{
	TOASTDISMISSALREASON toastDismissedRetVal = ToastDismissal_Unknown;
    bool succeeded = false;
    HRESULT hr = E_FAIL;

    if( NULL != toastEventDismissedArg )
    {
        ToastDismissalReason tdr;
        hr = toastEventDismissedArg->get_Reason(&tdr);

        if (SUCCEEDED(hr))
        {
            switch (tdr)
            {
            case ToastDismissalReason_ApplicationHidden:
                toastDismissedRetVal = ToastDismissal_ApplicationHidden;
                break;
            case ToastDismissalReason_UserCanceled:
                toastDismissedRetVal = ToastDismissal_UserCanceled;
                break;
            case ToastDismissalReason_TimedOut:
                toastDismissedRetVal = ToastDismissal_TimedOut;
                break;
            }
        }
    }

    if( NULL != m_pToastNotificationPluginCB)
    {
        PluginResult res = m_pToastNotificationPluginCB->OnToastDismissed(toastDismissedRetVal);
        if(Plugin_Success == res)
        {
            succeeded = true;
        }
    }
    
    hr = (succeeded ? S_OK : E_FAIL);

    return hr;
}

/**
 * DesktopToastFailedEventHandler
 */
IFACEMETHODIMP CToastEventHandler::Invoke(_In_ IToastNotification* /* sender */, _In_ IToastFailedEventArgs* toastEventFailArg)
{
	bool succeeded = false;
	
	HRESULT toastError = E_FAIL;
	if( NULL != toastEventFailArg )
	{
		HRESULT hres = toastEventFailArg->get_ErrorCode(&toastError);
	}

	if( NULL != m_pToastNotificationPluginCB )
	{
        PluginResult res = m_pToastNotificationPluginCB->OnToastFailed(toastError);
        if(Plugin_Success == res)
        {
            succeeded = true;
        }
	}

	return succeeded ? S_OK : E_FAIL;
}

/** ---------------------------------------------------------------
 **         IUnknown implementation
 **/
IFACEMETHODIMP_(ULONG) CToastEventHandler::AddRef()
{ 
	return InterlockedIncrement(&m_iRef); 
}

IFACEMETHODIMP_(ULONG) CToastEventHandler::Release()
{
	ULONG l = InterlockedDecrement(&m_iRef);
	if (l == 0) delete this;
	return l;
}

 IFACEMETHODIMP CToastEventHandler::QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv)
 {
	 if (IsEqualIID(riid, IID_IUnknown))
		{
            *ppv = static_cast<IUnknown*>(static_cast<ToastActivatedEventHandler*>(this));
		}
        else if (IsEqualIID(riid, __uuidof(ToastActivatedEventHandler)))
		{
            *ppv = static_cast<ToastActivatedEventHandler*>(this);
		}
        else if (IsEqualIID(riid, __uuidof(ToastDismissedEventHandler)))
		{
            *ppv = static_cast<ToastDismissedEventHandler*>(this);
		}
        else if (IsEqualIID(riid, __uuidof(ToastFailedEventHandler)))
		{
            *ppv = static_cast<ToastFailedEventHandler*>(this);
		}
        else
		{
				*ppv = nullptr;
		}

        if (*ppv)
		{
            reinterpret_cast<IUnknown*>(*ppv)->AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
 }
