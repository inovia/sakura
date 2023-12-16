/*! @file
	@brief 共通設定ダイアログボックス、「HSP」ページ

	@author Norio Nakatani
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2000-2001, genta, MIK, jepro
	Copyright (C) 2002, YAZAKI, MIK, aroka, hor
	Copyright (C) 2004, genta, ryoji
	Copyright (C) 2006, ryoji
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/

#include "StdAfx.h"
#include "prop/CPropCommon.h"
#include "CPropertyManager.h"
#include "util/shell.h"
#include "util/window.h"
#include "util/os.h"
#include "apiwrap/StdControl.h"
#include "CSelectLang.h"
#include "sakura_rc.h"
#include "sakura.hh"
#include "String_define.h"

static const DWORD p_helpids[] = {	//01310
//	IDC_STATIC,								-1,
	0, 0
};

//	From Here Jun. 2, 2001 genta
/*!
	@param hwndDlg ダイアログボックスのWindow Handle
	@param uMsg メッセージ
	@param wParam パラメータ1
	@param lParam パラメータ2
*/
INT_PTR CALLBACK CPropHSP::DlgProc_page(
	HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DlgProc( reinterpret_cast<pDispatchPage>(&CPropHSP::DispatchEvent), hwndDlg, uMsg, wParam, lParam );
}
//	To Here Jun. 2, 2001 genta

/*! ファイルページ メッセージ処理 */
INT_PTR CPropHSP::DispatchEvent(
	HWND	hwndDlg,	//!< handle to dialog box
	UINT	uMsg,	//!< message
	WPARAM	wParam,	//!< first message parameter
	LPARAM	lParam 	//!< second message parameter
)
{
	// WORD		wNotifyCode;
	// WORD		wID;
	// HWND		hwndCtl;
	NMHDR*		pNMHDR;
	int			idCtrl;

	switch( uMsg )
	{
		case WM_INITDIALOG:
		{
			/* ダイアログデータの設定 File */
			SetData( hwndDlg );
			::SetWindowLongPtr( hwndDlg, DWLP_USER, lParam );
			return TRUE;
		}

		case WM_NOTIFY:
		{
			idCtrl = (int)wParam;
			pNMHDR = (NMHDR*)lParam;
			//switch( idCtrl )
			//{
			//	default:
			//	{
					switch( pNMHDR->code )
					{
						/* ダイアログデータの取得 File */
						case PSN_KILLACTIVE:
							GetData( hwndDlg );
							return TRUE;
						case PSN_SETACTIVE:
							m_nPageNum = ID_PROPCOM_PAGENUM_HSP;
							return TRUE;
					}
			//	}
			//	break;
			//}
			break;
		}
	}
	return FALSE;
}

/*! HSPページ: ダイアログデータの設定
	共有メモリからデータを読み出して各コントロールに値を設定する。
	@param hwndDlg プロパティページのWindow Handle
*/
void CPropHSP::SetData( HWND hwndDlg )
{
	/* HSPアシスタントを自動起動する(&A) */
	::CheckDlgButtonBool( hwndDlg, IDC_CHECK_bAutoRunHSPAssistant, m_Common.m_sHSP.m_bHspAssistantAutoStartEnabled);

	/* 従来方式のラベル解析を優先する(HSP 3.6互換)(&L) */
	::CheckDlgButtonBool(hwndDlg, IDC_CHECK_bUseLegacyLabelAnalysis, m_Common.m_sHSP.m_bUseLegacyLabelAnalysis);

	/* コンパイル時、未保存のファイルを自動保存する(&S) */
	::CheckDlgButtonBool(hwndDlg, IDC_CHECK_bAutoSaveBeforeCompile, m_Common.m_sHSP.m_bAutoSaveBeforeCompile);
	return;
}

/*! HSPページ ダイアログデータの取得
	ダイアログボックスに設定されたデータを共有メモリに反映させる
	@param hwndDlg プロパティページのWindow Handle
	@return 常にTRUE
*/
int CPropHSP::GetData( HWND hwndDlg )
{
	/* HSPアシスタントを自動起動する(&A) */
	m_Common.m_sHSP.m_bHspAssistantAutoStartEnabled = ::IsDlgButtonCheckedBool( hwndDlg, IDC_CHECK_bAutoRunHSPAssistant);

	/* 従来方式のラベル解析を優先する(HSP 3.6互換)(&L) */
	m_Common.m_sHSP.m_bUseLegacyLabelAnalysis = ::IsDlgButtonCheckedBool(hwndDlg, IDC_CHECK_bUseLegacyLabelAnalysis);

	/* コンパイル時、未保存のファイルを自動保存する(&S) */
	m_Common.m_sHSP.m_bAutoSaveBeforeCompile = ::IsDlgButtonCheckedBool(hwndDlg, IDC_CHECK_bAutoSaveBeforeCompile);

	return TRUE;
}
