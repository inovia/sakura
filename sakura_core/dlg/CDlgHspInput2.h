/*!	@file
	@brief 1行入力ダイアログボックス + チェックボックス

	@author Norio Nakatani
	@date	1998/05/31 作成
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/
#ifndef SAKURA_CDLGHSPINPUT2_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_
#define SAKURA_CDLGHSPINPUT2_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_
#pragma once

#include "mem/CNativeW.h"

class CDlgHspInput2;

/*-----------------------------------------------------------------------
クラスの宣言
-----------------------------------------------------------------------*/
/*!
	@brief １行入力ダイアログボックス
*/
class CDlgHspInput2
{
public:
	BOOL DoModal( HINSTANCE hInstApp, HWND hwndParent, const WCHAR* pszTitle,
				  const WCHAR* pszMessage, int nMaxTextLen, WCHAR* pszText,
		          const WCHAR* pszCheckText1, bool& pbChecked1,
		          const WCHAR* pszCheckText2, bool& pbChecked2
		);	/* モーダルダイアログの表示 */

	/*
	||  Attributes & Operations
	*/
	INT_PTR DispatchEvent(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);	/* ダイアログのメッセージ処理 */

	HINSTANCE		m_hInstance;		/* アプリケーションインスタンスのハンドル */
	HWND			m_hwndParent;		/* オーナーウィンドウのハンドル */
	HWND			m_hWnd;				/* このダイアログのハンドル */

	const WCHAR*	m_pszTitle;			/* ダイアログタイトル */
	const WCHAR*	m_pszMessage;		/* メッセージ */
	int				m_nMaxTextLen;		/* 入力サイズ上限 */
//	char*			m_pszText;			/* テキスト */
	CNativeW		m_cmemText;			/* テキスト */

	const WCHAR*	m_pszCheckText1;	/* チェックテキスト1 */
	bool			m_bChecked1;		/* チェック状態1 */
	const WCHAR*	m_pszCheckText2;	/* チェックテキスト2 */
	bool			m_bChecked2;		/* チェック状態2 */

protected:
	/*
	||  実装ヘルパ関数
	*/
};
#endif /* SAKURA_CDLGHSPINPUT2_43CB765B_D257_4DBC_85E9_D2587B7E9D8E_H_ */
