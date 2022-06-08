/*!	@file
@brief CViewCommanderクラスのコマンド(HSP系)関数群

*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, MIK
	Copyright (C) 2018-2022, Sakura Editor Organization

	This source code is designed for sakura editor.
	Please contact the copyright holders to use this code for other purpose.
*/

#include "StdAfx.h"
#include "CViewCommander.h"
#include "CViewCommander_inline.h"

#include "env/CFormatManager.h"
#include "_main/CProcess.h"

// ペイント起動
void CViewCommander::Command_HSP_OPEN_PAINT_TOOL( void )
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenPaintTool( m_pCommanderView->GetHwnd());
}

// キーワード検索
void CViewCommander::Command_HSP_SEARCH_KEYWORD( void )
{
	/* 現在カーソル位置単語または選択範囲より検索等のキーを取得 */
	CNativeW		cmemCurText;
	m_pCommanderView->GetCurrentTextForSearchDlg(cmemCurText);

	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.SearchKeyword( m_pCommanderView->GetHwnd(), cmemCurText);
}