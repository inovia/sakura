/*!	@file
	@brief HSPタグジャンプリストダイアログボックス

	@author MIK
	@date 2003.4.13
	@date 2005.03.31 MIK キーワード指定TagJump対応のため大幅に変更
*/
/*
	Copyright (C) 2003, MIK
	Copyright (C) 2005, MIK
	Copyright (C) 2006, genta, ryoji
	Copyright (C) 2010, Moca
	Copyright (C) 2018-2022, Sakura Editor Organization

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, 
	including commercial applications, and to alter it and redistribute it 
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such, 
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/

#include "StdAfx.h"
#include "dlg/CDlgHspTagJumpList.h"
#include "func/Funccode.h"
#include "env/DLLSHAREDATA.h"
#include "util/container.h"
#include "util/shell.h"
#include "util/file.h"
#include "util/window.h"
#include "util/tchar_convert.h"
#include "hsp/CHsp3DarkMode.h"
#include "apiwrap/StdControl.h"
#include "CSelectLang.h"
#include "mem/CNativeA.h"
#include "String_define.h"

#include "sakura_rc.h"
#include "sakura.hh"

static const SAnchorList anchorList[] = {
	{IDC_STATIC_BASEDIR,	ANCHOR_BOTTOM},
	{IDC_STATIC_KEYWORD,	ANCHOR_BOTTOM},
	{IDC_KEYWORD,			ANCHOR_BOTTOM},
	{IDC_LIST_TAGJUMP,		ANCHOR_ALL},
	{IDC_BUTTON_PREVTAG,	ANCHOR_BOTTOM},
	{IDC_BUTTON_NEXTTAG,	ANCHOR_BOTTOM},
	{IDC_BUTTON_HELP,		ANCHOR_BOTTOM},
	{IDOK,					ANCHOR_BOTTOM_RIGHT},
	{IDCANCEL,				ANCHOR_BOTTOM_RIGHT},
	{IDC_CHECK_ICASE,		ANCHOR_BOTTOM},
	{IDC_CHECK_ANYWHERE,	ANCHOR_BOTTOM},
};

CDlgHspTagJumpList::CDlgHspTagJumpList()
	: CDialog(true),
	m_tagJumpCallback(nullptr),
	m_nIndex(-1),
	m_tagList()/*,
	m_pszFileName(szFileName) */
{
	/* サイズ変更時に位置を制御するコントロール数 */
	assert(_countof(anchorList) == _countof(m_rcItems));
	m_ptDefaultSize.x = -1;
	m_ptDefaultSize.y = -1;
}

CDlgHspTagJumpList::~CDlgHspTagJumpList()
{
	;
}

/*
	モーダルダイアログの表示
*/
int CDlgHspTagJumpList::DoModal(
	HINSTANCE	hInstance,
	HWND		hwndParent,
	LPARAM		lParam
)
{
	return (int)CDialog::DoModal(hInstance, hwndParent, IDD_TAGJUMPLIST, lParam);
}

/* モードレスダイアログの表示 */
HWND CDlgHspTagJumpList::DoModeless(
	HINSTANCE			hInstance,
	HWND				hwndParent,
	LPARAM				lParam,
	TagJumpCallback		tagJumpCallback,
	CloseCallback		closeCallback
)
{
	m_tagJumpCallback = tagJumpCallback;
	m_closeCallback = closeCallback;
	return CDialog::DoModeless(hInstance, hwndParent, IDD_TAGJUMPLIST, lParam, SW_SHOW);
}

/* ダイアログデータの設定 */
void CDlgHspTagJumpList::SetData( void )
{
	UpdateData();
}

/*! @brief Jump候補の更新

	@date 2005.03.31 MIK 
		ダイアログOpen時以外にも更新が必要なためSetData()より分離
*/
void CDlgHspTagJumpList::UpdateData(  )
{
	HWND	hwndList;
	LV_ITEM	lvi;

	hwndList = GetItemHwnd( IDC_LIST_TAGJUMP );

	// 無効???
	if (!::IsWindow(hwndList))
		return;

	ListView_DeleteAllItems( hwndList );

	WCHAR	tmp[32];

	int nIndex = 0;
	for (const auto& item : m_tagList)
	{
		std::unique_ptr<wchar_t[]> keyword	( new wchar_t[item.keyword.size() + 1]);
		std::unique_ptr<wchar_t[]> type		( new wchar_t[item.type.size() + 1]);
		std::unique_ptr<wchar_t[]> filename	( new wchar_t[item.filename.size() + 1]);
		std::unique_ptr<wchar_t[]> note		( new wchar_t[item.note.size() + 1]);

		wcsncpy_s( keyword.get(),	item.keyword.size() + 1,	item.keyword.c_str(),	_TRUNCATE);
		wcsncpy_s( type.get(),		item.type.size() + 1,		item.type.c_str(),		_TRUNCATE);
		wcsncpy_s( filename.get(),	item.filename.size() + 1,	item.filename.c_str(),	_TRUNCATE);
		wcsncpy_s( note.get(),		item.note.size() + 1,		item.note.c_str(),		_TRUNCATE);

		lvi.mask = LVIF_TEXT;
		lvi.iItem = nIndex;
		lvi.iSubItem = 0;
		lvi.pszText = keyword.get();
		ListView_InsertItem(hwndList, &lvi);

		auto_sprintf(tmp, L"");
		ListView_SetItemText(hwndList, nIndex, 1, type.get());

		auto_sprintf(tmp, L"%d", item.no);
		ListView_SetItemText(hwndList, nIndex, 2, tmp);

		ListView_SetItemText(hwndList, nIndex, 3, note.get());
		ListView_SetItemText(hwndList, nIndex, 4, filename.get());

		auto_sprintf(tmp, L"");
		ListView_SetItemText(hwndList, nIndex, 5, tmp);

		ListView_SetItemState(hwndList, nIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);

		nIndex++;
	}

	ListView_SetColumnWidth(hwndList, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwndList, 1, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwndList, 2, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwndList, 3, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwndList, 4, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hwndList, 5, LVSCW_AUTOSIZE_USEHEADER);

	return;
}

/*!	ダイアログデータの取得

	@return TRUE: 正常, FALSE: 入力エラー

	@date 2005.04.03 MIK 設定値の保存処理追加
*/
int CDlgHspTagJumpList::GetData( void )
{
	HWND	hwndList;

	hwndList = GetItemHwnd( IDC_LIST_TAGJUMP );
	m_nIndex = ListView_GetNextItem( hwndList, -1, LVIS_SELECTED );

	if( m_nIndex == -1 )
		return FALSE;

	return TRUE;
}

/*!
	@date 2005.03.31 MIK
		階層カラムの追加．キーワード指定欄の追加
*/
BOOL CDlgHspTagJumpList::OnInitDialog( HWND hwndDlg, WPARAM wParam, LPARAM lParam )
{
	HWND		hwndList;
	LV_COLUMN	col;
	RECT		rc;
	long		lngStyle;

	_SetHwnd( hwndDlg );
	::SetWindowLongPtr( GetHwnd(), DWLP_USER, lParam );

	CreateSizeBox();

	// ダークモード
	auto& DarkMode = CHsp3DarkMode::GetInstance();
	DarkMode.DarkModeOnInitDialog(hwndDlg, wParam, lParam);

	CDialog::OnSize();
	
	::GetWindowRect( hwndDlg, &rc );
	m_ptDefaultSize.x = rc.right - rc.left;
	m_ptDefaultSize.y = rc.bottom - rc.top;

	for( int i = 0; i < _countof(anchorList); i++ )
	{
		GetItemClientRect( anchorList[i].id, m_rcItems[i] );
	}

	RECT rcDialog = GetDllShareData().m_Common.m_sOthers.m_rcTagJumpDialog;
	if( rcDialog.left != 0
		|| rcDialog.bottom != 0 )
	{
		m_xPos = rcDialog.left;
		m_yPos = rcDialog.top;
		m_nWidth = rcDialog.right - rcDialog.left;
		m_nHeight = rcDialog.bottom - rcDialog.top;
	}

	// ウィンドウのリサイズ
	SetDialogPosSize();

	//リストビューの表示位置を取得する。
	hwndList = ::GetDlgItem( hwndDlg, IDC_LIST_TAGJUMP );
	//ListView_DeleteAllItems( hwndList );
	rc.left = rc.top = rc.right = rc.bottom = 0;
	::GetWindowRect( hwndList, &rc );
	
	int nWidth = (rc.right - rc.left) - ::GetSystemMetrics( SM_CXHSCROLL ) - CTextWidthCalc::WIDTH_MARGIN_SCROLLBER;

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_LEFT;
	col.cx       = nWidth * 20 / 100;
	col.pszText  = const_cast<WCHAR*>(LS(STR_DLGTAGJMP_LIST1));
	col.iSubItem = 0;
	ListView_InsertColumn( hwndList, 0, &col );

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_CENTER;
	col.cx       = nWidth * 7 / 100;
	col.pszText  = const_cast<WCHAR*>(L"DEF/REF");
	col.iSubItem = 1;
	ListView_InsertColumn( hwndList, 1, &col );

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_RIGHT;
	col.cx       = nWidth * 7 / 100;
	col.pszText  = const_cast<WCHAR*>(LS(STR_DLGTAGJMP_LIST3));
	col.iSubItem = 2;
	ListView_InsertColumn( hwndList, 2, &col );

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_LEFT;
	col.cx       = nWidth * 7 / 100;
	col.pszText  = const_cast<WCHAR*>(LS(STR_DLGTAGJMP_LIST4));
	col.iSubItem = 3;
	ListView_InsertColumn( hwndList, 3, &col );

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_LEFT;
	col.cx       = nWidth * 100 / 100;
	col.pszText  = const_cast<WCHAR*>(LS(STR_DLGTAGJMP_LIST5));
	col.iSubItem = 4;
	ListView_InsertColumn( hwndList, 4, &col );

	col.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt      = LVCFMT_LEFT;
	col.cx       = nWidth * 10 / 100;
	col.pszText  = const_cast<WCHAR*>(LS(STR_DLGTAGJMP_LIST6));
	col.iSubItem = 5;
	ListView_InsertColumn( hwndList, 5, &col );

	/* 行選択 */
	lngStyle = ListView_GetExtendedListViewStyle( hwndList );
	lngStyle |= LVS_EX_FULLROWSELECT;
	ListView_SetExtendedListViewStyle( hwndList, lngStyle );

	// 非表示のコントロール
	::ShowWindow( GetItemHwnd( IDC_STATIC_BASEDIR ),	SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_STATIC_KEYWORD ),	SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_KEYWORD),			SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_CHECK_ICASE ),		SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_CHECK_ANYWHERE ),	SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_BUTTON_HELP ),		SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_BUTTON_NEXTTAG),		SW_HIDE);
	::ShowWindow( GetItemHwnd( IDC_BUTTON_PREVTAG),		SW_HIDE);

	/* 基底クラスメンバ */
	CDialog::OnInitDialog( GetHwnd(), wParam, lParam );
	
	return TRUE;
}

BOOL CDlgHspTagJumpList::OnDestroy( void )
{
	CDialog::OnDestroy();
	RECT& rect = GetDllShareData().m_Common.m_sOthers.m_rcTagJumpDialog;
	rect.left = m_xPos;
	rect.top = m_yPos;
	rect.right = rect.left + m_nWidth;
	rect.bottom = rect.top + m_nHeight;
	return TRUE;
}

BOOL CDlgHspTagJumpList::OnBnClicked( int wID )
{
	switch( wID )
	{
		case IDOK:
		{
			const auto ret = (BOOL)GetData();

			if ( m_bModal)
			{
				::EndDialog( GetHwnd(), ret);
			}
			else
			{
				if ( m_tagJumpCallback)
					m_tagJumpCallback(GetSelectedItem());
			}
			return TRUE;
		}

		case IDCANCEL:
		{
			if (m_bModal)
				::EndDialog( GetHwnd(), FALSE );
			else
				::DestroyWindow( GetHwnd());
			return TRUE;
		}
	}

	/* 基底クラスメンバ */
	return CDialog::OnBnClicked( wID );
}

INT_PTR CDlgHspTagJumpList::DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
	LPARAM ret;
	INT_PTR result;

	// ダークモード
	auto& DarkMode = CHsp3DarkMode::GetInstance();
	if ( DarkMode.DarkModeDispatchEvent( hWnd, wMsg, wParam, lParam, ret))
	{
		return ret;
	}

	result = CDialog::DispatchEvent( hWnd, wMsg, wParam, lParam );
	if( wMsg == WM_GETMINMAXINFO )
	{
		return OnMinMaxInfo( lParam );
	}
	return result;
}

BOOL CDlgHspTagJumpList::OnSize( WPARAM wParam, LPARAM lParam )
{
	/* 基底クラスメンバ */
	CDialog::OnSize( wParam, lParam );

	RECT  rc;
	POINT ptNew;
	::GetWindowRect( GetHwnd(), &rc );
	ptNew.x = rc.right - rc.left;
	ptNew.y = rc.bottom - rc.top;

	for( int i = 0 ; i < _countof(anchorList); i++ )
	{
		ResizeItem( GetItemHwnd(anchorList[i].id),
			m_ptDefaultSize, ptNew, m_rcItems[i], anchorList[i].anchor );
	}
	::InvalidateRect( GetHwnd(), NULL, TRUE );
	return TRUE;
}

BOOL CDlgHspTagJumpList::OnMinMaxInfo( LPARAM lParam )
{
	LPMINMAXINFO lpmmi = (LPMINMAXINFO) lParam;
	if( m_ptDefaultSize.x < 0 )
	{
		return 0;
	}
	lpmmi->ptMinTrackSize.x = m_ptDefaultSize.x;
	lpmmi->ptMinTrackSize.y = m_ptDefaultSize.y;
	lpmmi->ptMaxTrackSize.x = m_ptDefaultSize.x*2;
	lpmmi->ptMaxTrackSize.y = m_ptDefaultSize.y*3;
	return 0;
}

BOOL CDlgHspTagJumpList::OnNotify(NMHDR* pNMHDR)
{
	HWND	hwndList;
	hwndList = GetItemHwnd( IDC_LIST_TAGJUMP );

	//	候補一覧リストボックス
	if( hwndList == pNMHDR->hwndFrom )
	{
		switch( pNMHDR->code )
		{
			case NM_DBLCLK:
			{
				const auto ret = (BOOL)GetData();
				if ( m_bModal)
				{
					::EndDialog(GetHwnd(), ret);
				}
				else
				{
					if ( m_tagJumpCallback)
						m_tagJumpCallback(GetSelectedItem());
				}
				return TRUE;
			}
		}
	}

	/* 基底クラスメンバ */
	return CDialog::OnNotify(pNMHDR);
}
