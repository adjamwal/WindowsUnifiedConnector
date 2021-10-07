/**************************************************************************
*       Copyright (c) 2012, Cisco Systems, All Rights Reserved
***************************************************************************
*
*  File:    ToastNotificationPlugin.h
*  Author:  Adam Veres <adveres@cisco.com>
*  Date:    09/2012
*
***************************************************************************
*
*   Declaration of the interface for controlling the Toast Notification
*   plugin DLL.  Toast notifications are part of Windows 8 and are used
*   to get a user's attention.
*
***************************************************************************/

#ifndef _TOASTNOTIFICATIONPLUGIN_H_
#define _TOASTNOTIFICATIONPLUGIN_H_

enum PluginResult
{
    Plugin_Success = 0,
    Plugin_GenericError = -1,
    Plugin_NotHandled = -2,
    Plugin_BufferInvalid = -3,
    Plugin_BadParameter = -4,
    Plugin_BadHandle = -5,
    Plugin_NullPointer = -6,
    Plugin_AllocationError = -7,
    Plugin_NotInitialized = -8,
    Plugin_AlreadyInitialized = -9,
    Plugin_Unexpected = -10,
    Plugin_OperationPending = -11,
    Plugin_Timeout = -12,
    Plugin_MissingDependency = -13,
    Plugin_Blocked = -14,
    Plugin_Permissions = -15,

    Plugin_Enum_End = -16 // must be lowest number
};

#define TOASTNOTIFICATION               "com.cisco.anyconnect.gui.win8.toast.notification."	// Toast notification module plug-in
#define TOASTNOTIFICATION_VERSION       1

// Enumerator for how the toast notification was closed
enum TOASTDISMISSALREASON
{
    ToastDismissal_ApplicationHidden = 0,
    ToastDismissal_UserCanceled,
    ToastDismissal_TimedOut,
    ToastDismissal_Unknown
};

// Enumerator for toast notification type
// Refers to templates at http://msdn.microsoft.com/en-us/library/windows/apps/hh761494.aspx
enum TOASTTYPE
{
    ToastImageAndText01 = 0,	//An image and a single string wrapped across a maximum of three lines of text.
    ToastImageAndText02 ,	    //An image, one string of bold text on the first line, one string of regular text wrapped across the second and third lines.
    ToastImageAndText03,        //An image, one string of bold text wrapped across the first two lines, one string of regular text on the third line.
    ToastImageAndText04,        //An image, one string of bold text on the first line, one string of regular text on the second line, one string of regular text on the third line.
    INVALIDTYPE                 //Invalid - leave this last.
};


// Forward Declarations
class IToastNotificationPluginCB;

class IToastNotificationPlugin 
{
public:
    
    virtual PluginResult SendToastNotification(
        TOASTTYPE templateType,
        const wchar_t *appUserModelId,
        const wchar_t *toastIconPath,
        const wchar_t **textValues, 
		unsigned int textValuesCount) = 0;
    virtual PluginResult PluginDirChanged() = 0;
    virtual PluginResult SetToastNotificationPluginCB(IToastNotificationPluginCB *cb) = 0;
    virtual PluginResult UnSetToastNotificationPluginCB() = 0;
    virtual bool IsStartMenuVisible() = 0;

    virtual ~IToastNotificationPlugin() {}
};

class IToastNotificationPluginCB
{
public:
	virtual PluginResult OnToastDismissed(const TOASTDISMISSALREASON responseType) = 0;
    virtual PluginResult OnToastActivated() = 0;
    virtual PluginResult OnToastFailed(HRESULT errorCode) = 0;
    virtual PluginResult OnToastLog(const wchar_t* logMessage, HRESULT errorCode) = 0;

    virtual ~IToastNotificationPluginCB(){}
};

#endif // _TOASTNOTIFICATIONPLUGIN_H_
