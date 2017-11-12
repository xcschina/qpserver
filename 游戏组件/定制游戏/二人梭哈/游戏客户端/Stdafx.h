#pragma once



#ifndef VC_EXTRALEAN

#define VC_EXTRALEAN    // 从 Windows 标头中排除不常使用的资料

#endif



// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。

// 有关不同平台的相应值的最新信息，请参考 MSDN。

#ifndef WINVER        // 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。

#define WINVER 0x0400   //为 Windows98 和 Windows 2000 及更新版本改变为适当的值。

#endif



#ifndef _WIN32_WINNT    // 允许使用 Windows NT 4 或更高版本的特定功能。

#define _WIN32_WINNT 0x0400   //为 Windows98 和 Windows 2000 及更新版本改变为适当的值。

#endif



#ifndef _WIN32_WINDOWS    // 允许使用 Windows 98 或更高版本的特定功能。

#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。

#endif



#ifndef _WIN32_IE     // 允许使用 IE 4.0 或更高版本的特定功能。

#define _WIN32_IE 0x0400  //为 IE 5.0 及更新版本改变为适当的值。

#endif



#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // 某些 CString 构造函数将是显式的



// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏

#define _AFX_ALL_WARNINGS



#include <afxwin.h>         // MFC 核心和标准组件

#include <afxext.h>         // MFC 扩展

#include <afxdisp.h>        // MFC 自动化类



#include <afxdtctl.h>   // Internet Explorer 4 公共控件的 MFC 支持

#ifndef _AFX_NO_AFXCMN_SUPPORT

#include <afxcmn.h>     // Windows 公共控件的 MFC 支持

#endif // _AFX_NO_AFXCMN_SUPPORT



//////////////////////////////////////////////////////////////////////////

//编译环境
#include "..\..\..\SDKCondition.h"
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境
#include "..\消息定义\CMD_ShowHand.h"
#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

#include "..\消息定义\CMD_ShowHand.h"
#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Ansi/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImageD.lib")
//#pragma comment (lib,"../../../开发库/Lib/Unicode/D3DEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControlD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif


//////////////////////////////////////////////////////////////////////////

