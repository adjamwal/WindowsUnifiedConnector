/**************************************************************************
*       Copyright (c) 2012, Cisco Systems, All Rights Reserved
***************************************************************************
*
*  File:    ToastEventHandler.h
*  Author:  Adam Veres <adveres@cisco.com>
*  Date:    09/2012
*
***************************************************************************
*
*   Declaration of the event handlers for toast notifications
*
***************************************************************************/

#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

class CToastNotificationPlugin;
class IToastNotificationPluginCB;

typedef ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification *, ::IInspectable *> ToastActivatedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification *, ABI::Windows::UI::Notifications::ToastDismissedEventArgs *> ToastDismissedEventHandler;
typedef ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Notifications::ToastNotification *, ABI::Windows::UI::Notifications::ToastFailedEventArgs *> ToastFailedEventHandler;

/**
 * Class ToastEventHandler handles callbacks from
 * toast notifications.  Implements toast handlers.
 */
class CToastEventHandler :
    public Microsoft::WRL::Implements<ToastActivatedEventHandler, ToastDismissedEventHandler, ToastFailedEventHandler>
{
public:
    CToastEventHandler(IToastNotificationPluginCB *toastCB);
    ~CToastEventHandler();

	//Invoke functions for Activated/Dismissed/Failed
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender, _In_ IInspectable* args);
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender, _In_ ABI::Windows::UI::Notifications::IToastDismissedEventArgs *e);
    IFACEMETHODIMP Invoke(_In_ ABI::Windows::UI::Notifications::IToastNotification *sender, _In_ ABI::Windows::UI::Notifications::IToastFailedEventArgs *toastEventFailArg);

    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _COM_Outptr_ void **ppv);
    
private:
    unsigned long m_iRef;
	IToastNotificationPluginCB *m_pToastNotificationPluginCB;
};

#endif	//_EVENTHANDLER_H_