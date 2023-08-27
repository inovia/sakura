/*!	@file
	@brief HSPレポートダイアログボックス

	@author inovia
	@date	2023/08/26 作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2018-2022, Sakura Editor Organization
	Copyright (C) 2023-2023, inovia

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#ifndef SAKURA_CDLGHSPREPORTS_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_
#define SAKURA_CDLGHSPREPORTS_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_
#pragma once

#include "mem/CNativeW.h"

class CDlgHspReports;

/*-----------------------------------------------------------------------
クラスの宣言
-----------------------------------------------------------------------*/
/*!
	@brief HSPレポートダイアログボックス
*/
class CDlgHspReports
{
public:
	BOOL DoModal(
		HINSTANCE hInstApp, HWND hwndParent, const WCHAR* pszTitle,
		const WCHAR* pszMessage, const WCHAR* pszText);	/* モーダルダイアログの表示 */
	INT_PTR DispatchEvent(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);	/* ダイアログのメッセージ処理 */

/*
||  Attributes & Operations
*/
	HINSTANCE		m_hInstance;	/* アプリケーションインスタンスのハンドル */
	HWND			m_hwndParent;	/* オーナーウィンドウのハンドル */
	HWND			m_hWnd;			/* このダイアログのハンドル */

	const WCHAR*	m_pszTitle;		/* ダイアログタイトル */
	const WCHAR*	m_pszMessage;	/* メッセージ */
	CNativeW		m_cmemText;		/* テキスト */

protected:
	/*
	||  実装ヘルパ関数
	*/

};
#endif /* SAKURA_CDLGHSPREPORTS_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_ */
