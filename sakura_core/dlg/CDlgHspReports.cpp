/*!	@file
	@brief HSPレポートダイアログボックス

	@author inovia
	@date	2023/08/26 作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, MIK
	Copyright (C) 2003, KEITA
	Copyright (C) 2006, ryoji
	Copyright (C) 2018-2022, Sakura Editor Organization
	Copyright (C) 2023-2023, inovia

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#include "StdAfx.h"
#include "dlg/CDlgHspReports.h"
#include "CEditApp.h"
#include "Funccode_enum.h"	// EFunctionCode
#include "hsp/CHsp3DarkMode.h"
#include "util/shell.h"
#include "sakura_rc.h"
#include "sakura.hh"
#include "util/window.h"
#include "apiwrap/StdControl.h"
#include "CSelectLang.h"

// 入力 CDlgHspReports.cpp	//@@@ 2002.01.07 add start MIK
static const DWORD p_helpids[] = {	//13000
	IDOK,					HIDOK_DLG1,
	IDC_HSP_EDIT_INPUT,		HIDC_DLG1_EDIT1,	//入力フィールド	IDC_EDIT1->IDC_EDIT_INPUT1	2008/7/3 Uchi
	IDC_STATIC_MSG,			HIDC_DLG1_EDIT1,	//メッセージ
//	IDC_STATIC,				-1,
	0, 0
};	//@@@ 2002.01.07 add end MIK

/* ダイアログプロシージャ */
INT_PTR CALLBACK CDlgHspReportsProc(
	HWND hwndDlg,	// handle to dialog box
	UINT uMsg,		// message
	WPARAM wParam,	// first message parameter
	LPARAM lParam 	// second message parameter
)
{
	CDlgHspReports* pCDlgHspReports;
	switch (uMsg) {
	case WM_INITDIALOG:
		pCDlgHspReports = (CDlgHspReports*)lParam;
		if (NULL != pCDlgHspReports) {
			UpdateDialogFont(hwndDlg);
			return pCDlgHspReports->DispatchEvent(hwndDlg, uMsg, wParam, lParam);
		}
		else {
			return FALSE;
		}
	default:
		// Modified by KEITA for WIN64 2003.9.6
		pCDlgHspReports = (CDlgHspReports*)::GetWindowLongPtr(hwndDlg, DWLP_USER);
		if (NULL != pCDlgHspReports) {
			return pCDlgHspReports->DispatchEvent(hwndDlg, uMsg, wParam, lParam);
		}
		else {
			return FALSE;
		}
	}
}

// サブクラスプロシージャ
LRESULT CALLBACK MyEditProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	// 起動時に勝手に全選択されるのが嫌
	static bool isAllowSelectAll = false;

	switch (uMsg)
	{
		//case WM_COMMAND:
		//{
		//	// すべてを選択 ID_EDIT_SELECT_ALL
		//	if ( LOWORD(wParam) == 1777)
		//	{
		//		isAllowSelectAll = true;
		//		::SendMessage(hwnd, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		//		isAllowSelectAll = false;
		//		return 0;
		//	}
		//}
		case EM_SETSEL:
		{
			// 起動時に勝手に全選択されるのが嫌
			if (!isAllowSelectAll)
				return 0;
			break;
		}
		//case WM_SETTEXT:
		//{
		//	// 全選択を解除する
		//	::SendMessage(hwnd, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
		//	break;
		//}
		case WM_KEYDOWN:
		{
			if ( wParam == 'A' && (::GetKeyState(VK_CONTROL) & 0x8000))
			{
				isAllowSelectAll = true;
				::SendMessage(hwnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
				isAllowSelectAll = false;
				return 0; // メッセージ処理済み
			}
			break;
		}
	}
	return ::DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

/* モーダルダイアログの表示 */
BOOL CDlgHspReports::DoModal(
	HINSTANCE		hInstApp,
	HWND			hwndParent,
	const WCHAR*	pszTitle,
	const WCHAR*	pszMessage,
	const WCHAR*	pszText
)
{
	BOOL bRet;
	m_hInstance = hInstApp;		/* アプリケーションインスタンスのハンドル */
	m_hwndParent = hwndParent;	/* オーナーウィンドウのハンドル */
	m_pszTitle = pszTitle;		/* ダイアログタイトル */
	m_pszMessage = pszMessage;		/* メッセージ */
	m_cmemText.SetString(pszText);
	bRet = (BOOL)::DialogBoxParam(
		CSelectLang::getLangRsrcInstance(),
		MAKEINTRESOURCE(IDD_HSP_REPORTS),
		m_hwndParent,
		CDlgHspReportsProc,
		(LPARAM)this
	);
	return bRet;
}

/* ダイアログのメッセージ処理 */
INT_PTR CDlgHspReports::DispatchEvent(
	HWND hwndDlg,	// handle to dialog box
	UINT uMsg,		// message
	WPARAM wParam,	// first message parameter
	LPARAM lParam 	// second message parameter
)
{
	WORD	wNotifyCode;
	WORD	wID;
	//	int		nRet;

	// ダークモード
	auto& DarkMode = CHsp3DarkMode::GetInstance();
	LPARAM ret;
	if (DarkMode.DarkModeDispatchEvent(hwndDlg, uMsg, wParam, lParam, ret))
	{
		return ret;
	}

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			/* ダイアログデータの設定 */
			// Modified by KEITA for WIN64 2003.9.6
			::SetWindowLongPtr(hwndDlg, DWLP_USER, lParam);

			::SetWindowText(hwndDlg, m_pszTitle);	/* ダイアログタイトル */
			DlgItem_SetText(hwndDlg, IDC_HSP_EDIT_INPUT, m_cmemText.GetStringPtr());	/* テキスト */
			::SetWindowText(::GetDlgItem(hwndDlg, IDC_STATIC_MSG), m_pszMessage);	/* メッセージ */

			// ダークモード
			DarkMode.DarkModeOnInitDialog(hwndDlg, wParam, lParam);

			// Ctrl+A
			::SetWindowSubclass(::GetDlgItem(hwndDlg, IDC_HSP_EDIT_INPUT), MyEditProc, 0, 0);

			return TRUE;
		}
		case WM_COMMAND:
			wNotifyCode = HIWORD(wParam);	/* 通知コード */
			wID = LOWORD(wParam);	/* 項目ID、 コントロールID、 またはアクセラレータID */
			switch (wNotifyCode) {
				/* ボタン／チェックボックスがクリックされた */
			case BN_CLICKED:
				switch (wID) {
				case IDOK:
					::EndDialog(hwndDlg, TRUE);
					return TRUE;
				case IDCANCEL:
					::EndDialog(hwndDlg, FALSE);
					return TRUE;
				}
				break;	//@@@ 2002.01.07 add
			}
			break;	//@@@ 2002.01.07 add
		//@@@ 2002.01.07 add start
		case WM_HELP:
		{
			HELPINFO *p = (HELPINFO *)lParam;
			MyWinHelp((HWND)p->hItemHandle, HELP_WM_HELP, (ULONG_PTR)(LPVOID)p_helpids);	// 2006.10.10 ryoji MyWinHelpに変更に変更
		}
		return TRUE;

		//Context Menu
		case WM_CONTEXTMENU:
			MyWinHelp(hwndDlg, HELP_CONTEXTMENU, (ULONG_PTR)(LPVOID)p_helpids);	// 2006.10.10 ryoji MyWinHelpに変更に変更
			return TRUE;
			//@@@ 2002.01.07 add end
	}
	return FALSE;
}
