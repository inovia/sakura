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

#include "CWriteManager.h"
#include "env/CFormatManager.h"
#include "_main/CProcess.h"
#include "doc/CEditDoc.h"

// バッファサイズ
static const int BUF_SIZE = 1024;

bool CViewCommander::Sub_HSP_GetHSPFileDir( WCHAR* szHSPFileDirPath)
{
	const auto& pDoc = GetDocument();
	if ( pDoc->m_cDocFile.GetFilePathClass().IsValidPath())
	{
		// ファイルパスごとコピー
		::wcsncpy_s(
			szHSPFileDirPath, BUF_SIZE, pDoc->m_cDocFile.GetFilePath(), BUF_SIZE);

		// フォルダパスのみにする
		::PathRemoveFileSpec(szHSPFileDirPath);
	}
	else
	{
		// 現在のカレントディレクトリを取得
		DWORD dwRet = ::GetCurrentDirectory(BUF_SIZE, szHSPFileDirPath);
		if ( dwRet == 0)
		{
			return false;	// まず、失敗しない思う
		}
	}
	return true;
}

bool CViewCommander::Sub_HSP_GetFileName(
	WCHAR* szHSPTmpFilePath, WCHAR* szHSPFilePath, WCHAR* szHSPObjFilePath)
{
	WCHAR szFolderPath[BUF_SIZE] = { 0 };		// 保存フォルダ

	const auto& pDoc = GetDocument();
	if ( pDoc->m_cDocFile.GetFilePathClass().IsValidPath())
	{
		// ファイルパスごとコピー
		::wcsncpy_s(
			szFolderPath, pDoc->m_cDocFile.GetFilePath(), BUF_SIZE);

		// フォルダパスのみにする
		::PathRemoveFileSpec(szFolderPath);

		// ファイルパス
		wcscpy( szHSPFilePath, pDoc->m_cDocFile.GetFilePath());
	}
	else
	{
		// 現在のカレントディレクトリを取得
		DWORD dwRet = ::GetCurrentDirectory(BUF_SIZE, szFolderPath);
		if (dwRet == 0)
		{
			return false;	// まず、失敗しない思う
		}

		// ファイルパス
		wcscpy(szHSPFilePath, L"???");	// 未保存なので確定しない
	}

	// hsptmp/objを付与
	::PathCombine(szHSPTmpFilePath, szFolderPath, L"hsptmp");
	::PathCombine(szHSPObjFilePath, szFolderPath, L"obj");
	return true;
}

void CViewCommander::Command_HSP_COMPILE_RUN(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };		// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };		// .hsp のパス(未保存時は???)
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };		// obj ファイルパス

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (! Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 実行前に自動保存するか？
	if ( Hsp3.IsAutoSaveBeforeCompile())
	{
		if (!Command_FILESAVEALL())
		{
			return;
		}
	}

	// hsptmp を一時保存
	if (! Command_PUTFILE( szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	// 文字コード
	const auto& pDoc = GetDocument();
	ECodeType codeType = pDoc->GetDocumentEncoding();
	
	// コンパイル実行
	if (! Hsp3.CompileRun(
		m_pCommanderView->GetHwnd(),
		szHSPTmpFilePath, szHSPFilePath,
		szHSPObjFilePath,
		false					/* レポート強制表示モードOFF */,
		(codeType == CODE_UTF8)	/* UTF-8 モード? */,
		false					/* コンパイル+実行 */,
		false					/* Debug */
	))
	{
		return;
	}

	return;
}

void CViewCommander::Command_HSP_RUN(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のパス(未保存時は???)
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 実行のみ
	if (!Hsp3.Run(
		m_pCommanderView->GetHwnd(),szHSPObjFilePath))
	{
		return;
	}

	return;
}

void CViewCommander::Command_HSP_COMPILE_ONLY(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のパス(未保存時は???)
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 実行前に自動保存するか？
	if ( Hsp3.IsAutoSaveBeforeCompile())
	{
		if (!Command_FILESAVEALL())
		{
			return;
		}
	}

	// hsptmp を一時保存
	if (!Command_PUTFILE(szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	// 文字コード
	const auto& pDoc = GetDocument();
	ECodeType codeType = pDoc->GetDocumentEncoding();

	// コンパイル実行
	if (!Hsp3.CompileRun(m_pCommanderView->GetHwnd(),
		szHSPTmpFilePath, szHSPFilePath,
		szHSPObjFilePath,
		true					/* レポート強制表示モードON */,
		(codeType == CODE_UTF8)	/* UTF-8 モード? */,
		true					/* コンパイルのみ */,
		false					/* Debug */,
		false					/* パックファイルOFF */
	))
	{
		return;
	}
	return;
}

void CViewCommander::Command_HSP_SHOW_ERROR(void)
{
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (! Hsp3.ShowErrorReport(m_pCommanderView->GetHwnd()))
	{
		return;
	}
	return;
}

void CViewCommander::Command_HSP_CREATE_OBJ(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のフルパス
	WCHAR szHSPFileName[_MAX_PATH]		= { 0 };	// ファイル名のみ
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス -> .ax

	// ファイルパスがある場合のみ実行
	const auto& pDoc = GetDocument();
	if ( !pDoc->m_cDocFile.GetFilePathClass().IsValidPath())
	{
		return;	// 新規作成などのファイルパスが未確定状態
	}

	// 一旦保存する
	Command_FILESAVE(false, false);

	// ファイルパス取得
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if ( !Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 新しいファイル名生成する
	// (test.hsp -> test.ax)
	const auto& lpszFileName =
		::PathFindFileName( szHSPFilePath);
	wcscpy_s( szHSPFileName, lpszFileName);
	::PathRemoveExtension( szHSPFileName);
	::PathAddExtension( szHSPFileName, L".ax");

	// フルパスから obj を除去し、新しいファイ名を付与
	// (C:\test\obj -> C:\test\ -> C:\test\test.ax)
	::PathRemoveFileSpec( szHSPObjFilePath);
	::PathAppend( szHSPObjFilePath, szHSPFileName);

	// 文字コード
	ECodeType codeType = pDoc->GetDocumentEncoding();

	// コンパイル
	if ( !Hsp3.CompileRun( m_pCommanderView->GetHwnd(),
		szHSPTmpFilePath, szHSPFilePath,
		szHSPObjFilePath,
		false					/* レポート強制表示モードOFF */,
		(codeType == CODE_UTF8)	/* UTF-8 モード? */,
		true					/* コンパイルのみ */,
		true					/* Release */,
		false					/* パックファイルOFF */
	))
	{
		return;	// 失敗
	}

	// メッセージ表示
	CNativeW msg;
	msg.AppendStringF(
		L"%s\r\nPath = %s",
		L"オブジェクトファイルを作成しました。",
		szHSPObjFilePath
	);

	::MessageBox(
		m_pCommanderView->GetHwnd(), msg.GetStringPtr(),
		L"Info", MB_OK | MB_ICONINFORMATION);

	return;
}

void CViewCommander::Command_HSP_CREATE_STARTAX(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のフルパス
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス	-> start.ax

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// hsptmp を一時保存
	if (!Command_PUTFILE( szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	// フルパスから obj を除去し、新しいファイ名を付与
	// (C:\test\obj -> C:\test\ -> C:\test\start.ax)
	::PathRemoveFileSpec(szHSPObjFilePath);
	::PathAppend(szHSPObjFilePath, L"start.ax");

	// 文字コード
	const auto& pDoc = GetDocument();
	ECodeType codeType = pDoc->GetDocumentEncoding();

	// コンパイル
	if (!Hsp3.CompileRun( m_pCommanderView->GetHwnd(),
		szHSPTmpFilePath, szHSPFilePath,
		szHSPObjFilePath,
		false					/* レポート強制表示モードOFF */,
		(codeType == CODE_UTF8)	/* UTF-8 モード? */,
		true					/* コンパイルのみ */,
		true					/* Release */,
		false					/* パックファイルOFF */
	))
	{
		return;	// 失敗
	}

	// メッセージ表示
	CNativeW msg;
	msg.AppendStringF(
		L"%s\r\nPath = %s",
		L"START.AXを作成しました。",
		szHSPObjFilePath
		);

	::MessageBox(
		m_pCommanderView->GetHwnd(), msg.GetStringPtr(),
		L"Info", MB_OK | MB_ICONINFORMATION);

	return;
}

void CViewCommander::Command_HSP_CREATE_EXE_AUTO(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のフルパス
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス	-> start.ax
	
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// hsptmp を一時保存
	if (!Command_PUTFILE(szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	// フルパスから obj を除去し、新しいファイ名を付与
	// (C:\test\obj -> C:\test\ -> C:\test\start.ax)
	::PathRemoveFileSpec(szHSPObjFilePath);
	::PathAppend(szHSPObjFilePath, L"start.ax");

	// 文字コード
	const auto& pDoc = GetDocument();
	ECodeType codeType = pDoc->GetDocumentEncoding();

	// コンパイル
	if (!Hsp3.CompileRun( m_pCommanderView->GetHwnd(),
		L"hsptmp" /* フルパス渡すと何故かバグる */, szHSPFilePath,
		szHSPObjFilePath,
		false					/* レポート強制表示モードOFF */,
		(codeType == CODE_UTF8)	/* UTF-8 モード? */,
		true					/* コンパイルのみ */,
		true					/* Release */,
		true					/* パックファイルON */
	))
	{
		return;	// 失敗
	}

	// DPMファイル生成
	if (!Hsp3.Make(m_pCommanderView->GetHwnd()))
	{
		return;	// 失敗
	}

	// メッセージ表示
	CNativeW msg;
	msg.AppendStringF(
		L"%s",
		L"実行ファイルを作成しました。"
	);

	::MessageBox(
		m_pCommanderView->GetHwnd(), msg.GetStringPtr(),
		L"Info", MB_OK | MB_ICONINFORMATION);
	return;
}

void CViewCommander::Command_HSP_RUN_EXTERNAL(void)
{
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.RunExternal( m_pCommanderView->GetHwnd());
	return;
}

void CViewCommander::Command_HSP_RESERVED_KEYWORD_LIST(void)
{
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.ReservedKeywordList( m_pCommanderView->GetHwnd());
	return;
}

void CViewCommander::Command_HSP_COMMANDLINE_OPTION(void)
{
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.CommandLineOption( m_pCommanderView->GetHwnd());
	return;
}

void CViewCommander::Command_HSP_SHOW_DEBUG_WINDOW(void)
{
	auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.SetShowDebugWindow( !Hsp3.IsShowDebugWindow());
	return;
}

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

void CViewCommander::Command_HSP_OPEN_MAP_TOOL(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenMapTool( m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_OPEN_HELP_SOURCE_EDITOR(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenHelpSourceEditor( m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_RUN_ICON_CONVERTER(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.RunIconConverter(m_pCommanderView->GetHwnd());
}

void CViewCommander::Command_HSP_RUN_HSP3_UPDATER(void)
{
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.RunHSP3Updater(m_pCommanderView->GetHwnd());
}

// キーワード検索
void CViewCommander::Command_HSP_SEARCH_KEYWORD( void )
{
	/* 現在カーソル位置単語または選択範囲より検索等のキーを取得 */
	CNativeW		cmemCurText;
	m_pCommanderView->GetCurrentTextForSearchForHSP(cmemCurText);

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
	auto bJapanese = (langId == 1041);	// 1041 = Japanese

	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenManualIndex( m_pCommanderView->GetHwnd(), bJapanese);
}

void CViewCommander::Command_HSP_OPEN_DOCUMENT(void)
{
	// 現在のフォルダパス
	WCHAR szHSPFileDir[BUF_SIZE] = { 0 };	// HSPファイルのフォルダパス
	if (! Sub_HSP_GetHSPFileDir( szHSPFileDir))
	{
		return;
	}

	// Commonフォルダパス
	WCHAR szHSPCommonDir[BUF_SIZE] = { 0 };		// Commonフォルダのパス
	GetExedir( szHSPCommonDir, L"common");

	/* 現在カーソル位置単語または選択範囲よりダブルクオーテーションで囲まれた文字列を取得 */
	CNativeW		cmemCurText;
	if (! m_pCommanderView->GetDoubleQuateCurrentWord(cmemCurText))
	{
		// ダブルクオーテーションなしでもチェックしてみる
		if (!m_pCommanderView->GetCurrentTextForSearchForHSP(cmemCurText, false))
		{
			ErrorBeep();
			return;
		}
	}

	// エスケープ解除
	cmemCurText.Replace(L"\\\n", L"\r\n");
	cmemCurText.Replace(L"\\\t", L"\t");
	cmemCurText.Replace(L"\\\"", L"\"");
	cmemCurText.Replace(L"\\\\", L"\\");

	// コマンド
	WCHAR szCommand[BUF_SIZE] = { 0 };

	// まずはそのまま
	if (!Command_HSP_OPEN_DOCUMENT_SUB(
		cmemCurText, szCommand, szHSPCommonDir, szHSPFileDir))
	{
		//
		// #use対策
		//

		// .as の拡張子付与
		CNativeW cmemFilePathAs(cmemCurText);
		cmemFilePathAs.AppendString(L".as");
		if (!Command_HSP_OPEN_DOCUMENT_SUB(
			cmemFilePathAs, szCommand, szHSPCommonDir, szHSPFileDir))
		{
			// .hsp の拡張子付与
			CNativeW cmemFilePathHsp(cmemCurText);
			cmemFilePathHsp.AppendString(L".hsp");
			if (!Command_HSP_OPEN_DOCUMENT_SUB(
				cmemFilePathAs, szCommand, szHSPCommonDir, szHSPFileDir))
			{
				ErrorBeep();
				return;
			}
		}
	}

	// パスが見つかった場合は開いてみる
	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	Hsp3.OpenDocument( m_pCommanderView->GetHwnd(), *this, szCommand);
}

bool CViewCommander::Command_HSP_OPEN_DOCUMENT_SUB(
	CNativeW &cmemCurText,
	WCHAR  szCommand[1024], const WCHAR  szHSPCommonDir[1024], const WCHAR  szHSPFileDir[1024])
{
	//
	// パスの存在確認チェック処理
	//

	// 相対パスかチェック
	if (::PathIsRelative(cmemCurText.GetStringPtr()))
	{
		// 相対パスは絶対パスに変える
		// Commonから
		::PathCombine(szCommand, szHSPCommonDir, cmemCurText.GetStringPtr());

		// パス存在チェック
		if (!::PathFileExists(szCommand)
			&& !::PathIsDirectory(szCommand))
		{
			// HSPファイルのフォルダから
			::PathCombine(szCommand, szHSPFileDir, cmemCurText.GetStringPtr());

			// パス存在チェック
			if (!::PathFileExists(szCommand)
				&& !::PathIsDirectory(szCommand))
			{
				return false;
			}
		}
	}
	else
	{
		// 絶対パス
		wcscpy(szCommand, cmemCurText.GetStringPtr());

		// パス存在チェック
		if (!::PathFileExists(szCommand)
			&& !::PathIsDirectory(szCommand))
		{
			return false;
		}
	}
	return true;
}

void CViewCommander::Command_HSP_JUMP_DEFINITION(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE]	= { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE]		= { 0 };	// .hsp のパス(未保存時は???)
	WCHAR szHSPObjFilePath[BUF_SIZE]	= { 0 };	// obj ファイルパス
	WCHAR szHSPFileDir[BUF_SIZE]		= { 0 };	// HSPファイルのフォルダパス
	WCHAR szHSPCommonDir[BUF_SIZE]		= { 0 };	// Commonフォルダのパス

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();

	// ファイル名取得
	if (! Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 現在のフォルダパス取得
	if (! Sub_HSP_GetHSPFileDir(szHSPFileDir))
	{
		return;
	}

	// Commonフォルダパス取得
	GetExedir(szHSPCommonDir, L"common");

	// hsptmp を一時保存
	if (! Command_PUTFILE( szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	//// 分割ウィンドウ
	//if ( GetEditWindow()->m_cSplitterWnd.GetAllSplitRows() == 1)
	//{
	//	Command_SPLIT_V();
	//}

	/* 現在カーソル位置単語または選択範囲より検索等のキーを取得 */
	CNativeW		cmemCurText;
	m_pCommanderView->GetCurrentTextForSearchForHSP(cmemCurText);

	Hsp3.JumpDefinition(
		m_pCommanderView->GetHwnd(), *this->m_pCommanderView,
		cmemCurText, szHSPTmpFilePath, szHSPFilePath, szHSPFileDir, szHSPCommonDir);
}

void CViewCommander::Command_HSP_JUMP_ALL_REFERENCES(void)
{
	WCHAR szHSPTmpFilePath[BUF_SIZE] = { 0 };	// hsptmp ファイルパス
	WCHAR szHSPFilePath[BUF_SIZE] = { 0 };	// .hsp のパス(未保存時は???)
	WCHAR szHSPObjFilePath[BUF_SIZE] = { 0 };	// obj ファイルパス
	WCHAR szHSPFileDir[BUF_SIZE] = { 0 };	// HSPファイルのフォルダパス
	WCHAR szHSPCommonDir[BUF_SIZE] = { 0 };	// Commonフォルダのパス

	auto& Hsp3 = CProcess::getInstance()->GetHsp3();

	// ファイル名取得
	if (!Sub_HSP_GetFileName(
		szHSPTmpFilePath, szHSPFilePath, szHSPObjFilePath))
	{
		return;
	}

	// 現在のフォルダパス取得
	if (!Sub_HSP_GetHSPFileDir(szHSPFileDir))
	{
		return;
	}

	// Commonフォルダパス取得
	GetExedir(szHSPCommonDir, L"common");

	// hsptmp を一時保存
	if (!Command_PUTFILE(szHSPTmpFilePath, CODE_AUTODETECT, 0x00))
	{
		return;
	}

	/* 現在カーソル位置単語または選択範囲より検索等のキーを取得 */
	CNativeW		cmemCurText;
	m_pCommanderView->GetCurrentTextForSearchForHSP(cmemCurText);

	Hsp3.JumpAllReferences( m_pCommanderView->GetHwnd(), *this->m_pCommanderView,
		cmemCurText, szHSPTmpFilePath, szHSPFilePath, szHSPFileDir, szHSPCommonDir);
}

void CViewCommander::Command_HSP_HIGHLIGHT_KEYWORDS(void)
{
	Command_SELECTWORD( NULL, false);

	//CNativeW	cmemBuf;
	//if (!m_pCommanderView->GetSelectedData(&cmemBuf, FALSE, NULL, FALSE, false)) {
	//	ErrorBeep();
	//	return;
	//}

	//GetEditWindow()->m_cDlgFind.m_strText = cmemBuf.GetStringPtr();

	Command_SEARCH_CLEARMARK( true, false);

	// 選択解除
	m_pCommanderView->GetSelectionInfo().DisableSelectArea(true);
}
