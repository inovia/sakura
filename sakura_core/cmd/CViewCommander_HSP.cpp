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
#include "doc/CEditDoc.h"

void CViewCommander::Command_HSP_OPEN_SRC_FOLDER(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();

	// 保存時は保存先のフォルダをファイル選択状態で開く
	const auto& pDoc = GetDocument();
	if ( pDoc->m_cDocFile.GetFilePathClass().IsValidPath())
	{
		Hsp3.OpenSrcFolder_File(
			m_pCommanderView->GetHwnd(), pDoc->m_cDocFile.GetFilePath());
		return;
	}

	// 未保存時はカレントディレクトリ
	Hsp3.OpenSrcFolder( m_pCommanderView->GetHwnd());
	return;
}

void CViewCommander::Command_HSP_RUN_ASSIST(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.RunAssist(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_RUN_HSPTV(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.RunHSPTV(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_CREATE_DPM(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.CreateDPM(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_CREATE_PACKOPT(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.CreatePackopt(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_CONVERT_DISH_C(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.ConvertDishC(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_OPEN_HGIMG4_TOOL(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenHGIMG4Tool(m_pCommanderView->GetHwnd());
}

// ペイント起動
void CViewCommander::Command_HSP_OPEN_PAINT_TOOL( void )
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenPaintTool( m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_OPEN_HELP_SOURCE_EDITOR(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenHelpSourceEditor(m_pCommanderView->GetHwnd());
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

void CViewCommander::Command_HSP_OPEN_PG_MANUAL(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenPGManual(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_OPEN_FUNC_REF(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenFuncRef(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_OPEN_MANUAL_INDEX(void)
{
	const auto& langId = CSelectLang::getDefaultLangId();
	auto bEnglish = (langId != 1041);	// 1041 = Japanese

	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenManualIndex( m_pCommanderView->GetHwnd(), bEnglish);
}