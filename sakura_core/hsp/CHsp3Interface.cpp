#include "StdAfx.h"
#include "CHsp3Interface.h"
#include "_main/CProcess.h"
#include "doc/CEditDoc.h"
#include "window/CEditWnd.h"
#include "charset/CShiftJis.h"
#include "env/CSakuraEnvironment.h"
#include "version.h"

inline LRESULT CHsp3Interface::GetVersion(WPARAM wParam, LPARAM lParam, bool bUnicode) const
{
	switch (wParam)
	{
	case HGV_PUBLICVER:
		return HSED_PUBLIC_VER;

	case HGV_PRIVATEVER:
		return HSED_PRIVATE_VER;

	case HGV_HSPCMPVER:
		return GetHspCmpVersion((HANDLE)lParam, bUnicode);

	case HGV_FOOTYVER:
		return FOOTY_VER;

	case HGV_FOOTYBETAVER:
		return FOOTY_BETA;

	case HGV_SAKURA_VER_A:
		return VER_A;

	case HGV_SAKURA_VER_B:
		return VER_B;

	case HGV_SAKURA_VER_C:
		return VER_C;

	case HGV_SAKURA_VER_D:
		return VER_D;

	default:
		return -1;
	}
};

// hspcmp.dll のバージョンを取得
inline LRESULT CHsp3Interface::GetHspCmpVersion(HANDLE hPipe, bool bUnicode) const
{
	char szRefstr[4096];
	DWORD dwNumberOfBytesWritten;
	BOOL bRet;

	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Hsp3.IsLoaded())
		return -1;
	Hsp3.Hsp3Dll()->hsc_ver()(0, 0, 0, (int)szRefstr);

	if (bUnicode)
	{
		// Shift_JIS -> UTF-16
		CNativeW szRefstrW;
		CMemory cmemSjis(szRefstr, 4096 - 1);
		CShiftJis::SJISToUnicode(cmemSjis, &szRefstrW);
		cmemSjis.Reset();

		bRet = WriteFile(hPipe, szRefstrW.GetStringPtr(),
			(szRefstrW.GetStringLength() + 1) * 2, &dwNumberOfBytesWritten, nullptr);
	}
	else
	{
		bRet = WriteFile(hPipe, szRefstr,
			strlen(szRefstr) + 1, &dwNumberOfBytesWritten, nullptr);
	}

	return bRet ? dwNumberOfBytesWritten : -1;
}

inline LRESULT CHsp3Interface::GetWindowHandle(WPARAM wParam, LPARAM lParam, bool bUnicode) const
{
	switch (wParam)
	{
	case HGW_MAIN:
		return (LRESULT)CProcess::getInstance()->GetMainWindow();

	case HGW_CLIENT:
		return (LRESULT)CEditWnd::getInstance()->GetHwnd();

	case HGW_TAB:
		return (LRESULT)CEditWnd::getInstance()->m_cTabWnd.GetHwnd();

	case HGW_FOOTY:
	case HGW_SAKURA_EDIT:
		return (LRESULT)GetEditWindowHandle((int)lParam);

	case HGW_TOOLBAR:
		return (LRESULT)CEditWnd::getInstance()->m_cToolbar.GetToolbarHwnd();

	case HGW_STATUSBAR:
		return (LRESULT)CEditWnd::getInstance()->m_cStatusBar.GetStatusHwnd();

	case HGW_SAKURA_TRAY:
		return (LRESULT)m_pShareData->m_sHandles.m_hwndTray;

	default:
		return (LRESULT)nullptr;
	}
}

inline HWND CHsp3Interface::GetEditWindowHandle(int nId) const
{
	EditNode* pEdit = GetEditNode(nId);
	if (pEdit == nullptr)
		return nullptr;
	return pEdit->GetHwnd();
}

inline EditNode* CHsp3Interface::GetEditNode(int nId) const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if (IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			if (iIndex == nId)
				return &m_pShareData->m_sNodes.m_pEditArr[i];
			iIndex++;
		}
	}

	return nullptr;
}