#include "StdAfx.h"
#include "CHsp3.h"
#include "mem/CMemory.h"
#include "mem/CNativeA.h"
#include "mem/CNativeW.h"
#include "dlg/CDlgInput1.h"
#include "dlg/CDlgHspInput2.h"
#include "dlg/CDlgHspReports.h"
#include "_main/CProcess.h"
#include "_main/CControlProcess.h"

bool CHsp3::Load(const CNativeW& strLibFileName)
{
	// コントロールプロセスかどうか取得
	const auto& pCP = dynamic_cast<CControlProcess*>(CProcess::getInstance());
	const auto bControlProcess = (pCP != nullptr);

	// インスタンスハンドルを取得
	const auto& hInstance = CProcess::getInstance()->GetProcessInstance();

	// hsedsdk用管理クラス
	m_pHsp3If = new CHsp3Interface();
	m_pHsp3If->CreateInterfaceWindow( hInstance, bControlProcess);

	// コントロールプロセスでない = エディタプロセス
	if ( bControlProcess)
	{
		//
		// コントロールプロセス (サクラエディタ起動時1度のみ実行)
		//

		// 設定ファイルに保存しない設定値を初期化する
		// --> CShareData_IO.cpp へ移行
		//SetCommandLineOption( L"");
		//SetExecuteExternalFile_Name( L"");
		//SetExecuteExternalFile_CreateObjectOnly( false);
	}
	else
	{
		//
		// エディタプロセス
		//

		// フォント管理クラス
		CHsp3Font().LoadFont(hInstance);
	}

	// コンパイラ管理クラス
	m_pHsp3Dll	= new CHsp3Dll( strLibFileName.GetStringPtr());
	return m_pHsp3Dll->LoadDll();
}

BOOL CHsp3::HscIniW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS(name, nameA._GetMemory());
	return m_pHsp3Dll->hsc_ini()(0, (int)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscRefNameW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS(name, nameA._GetMemory());
	return m_pHsp3Dll->hsc_refname()(0, (int)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscObjNameW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS(name, nameA._GetMemory());
	return m_pHsp3Dll->hsc_objname()(0, (int)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscCmp(int p1, int p2, int p3, int p4) const
{
	return m_pHsp3Dll->hsc_comp()(p1, p2, p3, p4);
}

BOOL CHsp3::HscMakeW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS( name, nameA._GetMemory());
	return m_pHsp3Dll->hsc3_make()(0, (int)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscGetMesW(CNativeW& msg) const
{
	// バッファサイズ取得
	int bufsize = 0;
	m_pHsp3Dll->hsc3_messize()((int)&bufsize, 0, 0, 0);

	if ( bufsize == 0)
		return FALSE;

	std::unique_ptr<char[]> buf(new char[bufsize + 1]);
	m_pHsp3Dll->hsc_getmes()((int)buf.get(), 0, 0, 0);

	// Shift_JIS -> UTF-16
	CMemory cmemSjis(buf.get(), bufsize);
	CShiftJis::SJISToUnicode(cmemSjis, &msg);
	cmemSjis.Reset();

	return TRUE;
}


BOOL CHsp3::HscGetRuntimeW(CNativeW& name, const CNativeW& strObj) const
{
	// UTF-16 -> Shift_JIS
	CNativeA objA;
	CShiftJis::UnicodeToSJIS(strObj, objA._GetMemory());

	// ランタイム取得
	std::unique_ptr<char[]> buf(new char[1024 + 1]);
	m_pHsp3Dll->hsc3_getruntime()(
		(int)buf.get(), (int)objA.GetStringPtr(), 0, 0);

	// Shift_JIS -> UTF-16
	CMemory cmemSjis(buf.get(), 1024);
	CShiftJis::SJISToUnicode(cmemSjis, &name);
	cmemSjis.Reset();
	return TRUE;
}

BOOL CHsp3::HscRunW( const CNativeW& strExeCmds) const
{
	// UTF-16 -> Shift_JIS
	CNativeA strExeCmdsA;
	CShiftJis::UnicodeToSJIS(strExeCmds, strExeCmdsA._GetMemory());

	// 実行
	return m_pHsp3Dll->hsc3_run()(
		(int)strExeCmdsA.GetStringPtr(), IsShowDebugWindow() ? 1 : 0, 0, 0);
}


bool CHsp3::InitKeyword(CKeyWordSetMgr& keywordMgr) const
{
	if (!IsLoaded())
		return false;

	static constexpr std::wstring_view SYS_MACRO	= L"sys|macro";
	static constexpr std::wstring_view SYS_FUNC		= L"sys|func";
	static constexpr std::wstring_view SYS_FUNC1	= L"sys|func|1";
	static constexpr std::wstring_view SYS_FUNC2	= L"sys|func|2";
	static constexpr std::wstring_view PRE_FUNC		= L"pre|func";

	// 名前からインデックスを取得する
	int idxFunc = -1, idxPre = -1, idxMacro = -1;
	for (int i = 0; i < keywordMgr.m_nKeyWordSetNum; i++)
	{
		const auto& typeName = keywordMgr.GetTypeName(i);
		if ( HSP3_FUNC_NAME == typeName)
			idxFunc = i;
		else if( HSP3_PRE_NAME == typeName)
			idxPre = i;
		else if( HSP3_MACRO_NAME == typeName)
			idxMacro = i;
	}

	// 指定した名前が見つからないので作成する
	if ( idxFunc == -1)
	{
		if ( keywordMgr.AddKeyWordSet(HSP3_FUNC_NAME.data(), false))
			idxFunc = keywordMgr.m_nKeyWordSetNum - 1;
	}

	if ( idxPre == -1)
	{
		if ( keywordMgr.AddKeyWordSet(HSP3_PRE_NAME.data(), false))
			idxPre = keywordMgr.m_nKeyWordSetNum - 1;
	}

	if ( idxMacro == -1)
	{
		if ( keywordMgr.AddKeyWordSet(HSP3_MACRO_NAME.data(), false))
			idxMacro = keywordMgr.m_nKeyWordSetNum - 1;
	}

	// コンパイラー側へ一覧取得依頼
	CNativeW keywordList;
	{
		int bufsize = 0;
		m_pHsp3Dll->hsc_ini()(0, (int)"hsptmp", 0, 0);
		m_pHsp3Dll->hsc_refname()(0, (int)"???", 0, 0);
		m_pHsp3Dll->hsc_objname()(0, (int)"obj", 0, 0);
		m_pHsp3Dll->hsc3_getsym()(0, 0, 0, 0);
		m_pHsp3Dll->hsc3_messize()((int)&bufsize, 0, 0, 0);

		std::unique_ptr<char[]> buf( new char[bufsize + 1]);
		m_pHsp3Dll->hsc_getmes()((int)buf.get(), 0, 0, 0);

		// Shift_JIS -> UTF-16
		CMemory cmemSjis(buf.get(), bufsize);
		CShiftJis::SJISToUnicode(cmemSjis, &keywordList);
		cmemSjis.Reset();
	}

	// キーワードとタイプを分解
	wchar_t name[512], type[256];
	auto line = keywordList.GetStringPtr();
	while (true)
	{
		if ( swscanf( line, L"%s\t,%s", name, type) == 2)
		{
			if ( SYS_MACRO == type)
			{
				keywordMgr.AddKeyWord( idxMacro, name);
			}
			else if ((SYS_FUNC == type) || (SYS_FUNC1 == type) || (SYS_FUNC2 == type))
			{
				keywordMgr.AddKeyWord( idxFunc, name);
			}
			else if ( PRE_FUNC == type)
			{
				keywordMgr.AddKeyWord( idxPre, name);
			}
		}

		while (*line != '\0' && *line != '\n') line++;
		if (*line == '\0') break;
		line++;
	}

	return true;
}

bool CHsp3::ReservedKeywordList(HWND hParent) const
{
	// コンパイラー側へ一覧取得依頼
	CNativeW keywordList;
	{
		int bufsize = 0;
		m_pHsp3Dll->hsc_ini()(0, (int)"hsptmp", 0, 0);
		m_pHsp3Dll->hsc_refname()(0, (int)"???", 0, 0);
		m_pHsp3Dll->hsc_objname()(0, (int)"obj", 0, 0);
		if ( m_pHsp3Dll->hsc3_getsym()(0, 0, 0, 0))
		{
			return false;
		}
		m_pHsp3Dll->hsc3_messize()((int)&bufsize, 0, 0, 0);

		std::unique_ptr<char[]> buf(new char[bufsize + 1]);
		m_pHsp3Dll->hsc_getmes()((int)buf.get(), 0, 0, 0);

		// Shift_JIS -> UTF-16
		CMemory cmemSjis(buf.get(), bufsize);
		CShiftJis::SJISToUnicode(cmemSjis, &keywordList);
		cmemSjis.Reset();
	}

	// ダイアログ表示
	CDlgHspReports cDlgHspReports;
	std::wstring strTitle = L"結果レポート";	// LS(STR_DLGPROFILE_NEW_PROF_TITLE);
	std::wstring strMessage = L"予約キーワード一覧";	// LS(STR_DLGPROFILE_NEW_PROF_MSG);
	if (!cDlgHspReports.DoModal(
		::GetModuleHandle(nullptr), hParent,
		strTitle.c_str(), strMessage.c_str(), keywordList.GetStringPtr()))
	{
		return false;
	}
	return true;
}

bool CHsp3::CompileRun(
	HWND hParent,
	const CNativeW& strHSPTmpFilePath,
	const CNativeW& strHSPFilePath,
	const CNativeW& strHSPObjFilePath,
	bool bForceErrorReportMode,
	bool bInputUtf8Mode,
	bool bSkipExecution,
	bool bReleaseMode,
	bool bMakePack
	) const
{
	HscIniW(strHSPTmpFilePath);
	HscRefNameW(strHSPFilePath);
	HscObjNameW(strHSPObjFilePath);

	// コンパイルオプション
	int opt = 0;
	if ( bMakePack)
		opt |= 4;	/* HSC3_OPT_MAKEPACK */
	if ( bInputUtf8Mode)
		opt |= 32;	/* HSC3_OPT_UTF8IN */

	// コンパイル実行
	auto ret = HscCmp(
		bReleaseMode ? 0 : 1 /* HSC3_OPT_DEBUGMODE */,
		opt, IsShowDebugWindow() ? 1 : 0, 0);

	// コンパイルに失敗したか、
	// 強制的にダイアログを表示するモードの場合
	if ( ret || bForceErrorReportMode)
	{
		ShowErrorReport( hParent);
		return false;
	}

	// 実行スキップ
	if ( bSkipExecution)
	{
		return true;
	}

	// 実行
	return Run(hParent, strHSPObjFilePath);
}

bool CHsp3::Run(
	HWND hParent,
	const CNativeW& strHSPObjFilePath) const
{
	// ランタイム名
	CNativeW strRuntimeName;		
	HscGetRuntimeW(strRuntimeName, strHSPObjFilePath);

	// 実行パス
	WCHAR	exePath[1024];
	if ( L'\0' == strRuntimeName.GetStringPtr()[0])
	{
		GetExedir( exePath, L"hsp3.exe");	// 標準ランタイム
	}
	else
	{
		GetExedir( exePath, strRuntimeName.GetStringPtr());
	}

	// ランタイム起動のためのコマンドライン引数を組み立てる
	CNativeW strExeCmd;
	strExeCmd.AppendStringF(L"\"%s\" \"%s\"", exePath, strHSPObjFilePath);
	{
		// エディタ側で指定したコマンドライン引数がある場合は追記する
		CNativeW strOriginalCmd;
		GetCommandLineOption( strOriginalCmd);
		if ( L'\0' != strOriginalCmd.GetStringPtr()[0])
		{
			strExeCmd.AppendString(L" ");
			strExeCmd.AppendString(strOriginalCmd.GetStringPtr());
		}
	}

	// ランタイム起動
	auto ret = HscRunW( strExeCmd);
	if (ret)
	{
		CNativeW errMsg;
		errMsg.AppendStringF(
			L"%s\r\nPath = %s",
			L"実行用ランタイムファイルが見つかりません。",
			exePath
		);

		::MessageBox(
			hParent, errMsg.GetStringPtr(),
			L"Error", MB_OK | MB_ICONERROR);
	}
	return !ret;
}

bool CHsp3::ShowErrorReport(HWND hParent) const
{
	// エラー内容取得
	CNativeW errMsg;
	HscGetMesW(errMsg);

	// ダイアログ表示
	CDlgHspReports cDlgHspReports;
	std::wstring strTitle = L"結果レポート";	// LS(STR_DLGPROFILE_NEW_PROF_TITLE);
	std::wstring strMessage = L"コンパイル結果";	// LS(STR_DLGPROFILE_NEW_PROF_MSG);
	if (!cDlgHspReports.DoModal(
		::GetModuleHandle(nullptr), hParent,
		strTitle.c_str(), strMessage.c_str(), errMsg.GetStringPtr()))
	{
		return false;
	}
	return true;
}

bool CHsp3::Make(HWND hParent) const
{
	WCHAR szHSPDPMFilePath[1024] = { 0 };		// .dpm のフルパス
	GetExedir( szHSPDPMFilePath, L"hsptmp.dpm");

	if ( HscMakeW(szHSPDPMFilePath))
	{
		ShowErrorReport(hParent);
		return false;	// 失敗
	}
	return true;
}

bool CHsp3::RunExternal(HWND hParent) const
{
	CDlgHspInput2 cDlgInput2;

	bool bUtf8Mode = IsExecuteExternalFile_Utf8Mode();
	bool bCreateObjectOnly = IsExecuteExternalFile_CreateObjectOnly();

	CNativeW ExecuteExternalFile_Name;
	GetExecuteExternalFile_Name(ExecuteExternalFile_Name);

	std::unique_ptr<wchar_t[]> buf(
		new wchar_t[GetExecuteExternalFile_NameCountOf() + 1]);
	::wcscpy(buf.get(), ExecuteExternalFile_Name.GetStringPtr());

	std::wstring strTitle = L"外部ファイル実行";	// LS(STR_DLGPROFILE_NEW_PROF_TITLE);
	std::wstring strMessage = L"コンパイル+実行するファイル名を入力してください";	// LS(STR_DLGPROFILE_NEW_PROF_MSG);
	std::wstring strCheck1Text = L"オブジェクトファイルのみ作成";
	std::wstring strCheck2Text = L"UTF-8 モード";

	if (!cDlgInput2.DoModal(
		::GetModuleHandle(nullptr), hParent,
		strTitle.c_str(), strMessage.c_str(),
		GetExecuteExternalFile_NameCountOf() - 5, buf.get(),
		strCheck1Text.c_str(), bCreateObjectOnly,
		strCheck2Text.c_str(), bUtf8Mode
	))
	{
		return false;
	}

	// 拡張子がついていない場合は、.hsp を付与
	::PathAddExtension( buf.get(), L".hsp");

	// 値を保存
	SetExecuteExternalFile_Name(buf.get());
	SetExecuteExternalFile_CreateObjectOnly(bCreateObjectOnly);
	SetExecuteExternalFile_Utf8Mode(bUtf8Mode);

	// C:\test\obj or C:\test\xxx.ax のフルパス
	std::unique_ptr<wchar_t[]> strAxObjPath(new wchar_t[1024 + 1]);

	// C:\test\xxx.hsp のフルパス
	std::unique_ptr<wchar_t[]> strHspPath(new wchar_t[1024 + 1]);
	::GetCurrentDirectory( 1024, strHspPath.get());
	::PathAppend( strHspPath.get(), buf.get());

	// オブジェクトのみ生成?
	if ( bCreateObjectOnly)
	{
		// C:\test\xxx.ax
		wcscpy_s( strAxObjPath.get(), 1024, strHspPath.get());
		::PathRenameExtension( strAxObjPath.get(), L".ax");

		// コンパイル
		if (! CompileRun( hParent,
			strHspPath.get(), strHspPath.get(),
			strAxObjPath.get(),
			false					/* レポート強制表示モードOFF */,
			bUtf8Mode				/* UTF-8 モード? */,
			true					/* コンパイルのみ */,
			true					/* Release */,
			false					/* パックファイルOFF */
		))
		{
			return false;	// 失敗
		}

		// メッセージ表示
		CNativeW msg;
		msg.AppendStringF(
			L"%s\r\nPath = %s",
			L"オブジェクトファイルを作成しました。",
			strAxObjPath.get()
		);

		::MessageBox( hParent, msg.GetStringPtr(),
			L"Info", MB_OK | MB_ICONINFORMATION);
		return true;
	}

	// C:\test\obj
	wcscpy_s(strAxObjPath.get(), 1024, strHspPath.get());
	::PathRemoveFileSpec( strAxObjPath.get());
	::PathAppend( strAxObjPath.get(), L"obj");

	// コンパイル+実行
	if (!CompileRun(hParent,
		strHspPath.get(), strHspPath.get(),
		strAxObjPath.get(),
		false					/* レポート強制表示モードOFF */,
		bUtf8Mode				/* UTF-8 モード? */,
		false					/* コンパイル+実行 */,
		false					/* Debug */,
		false					/* パックファイルOFF */
	))
	{
		return false;	// 失敗
	}

	return true;
}

bool CHsp3::CommandLineOption(HWND hParent) const
{
	CDlgInput1 cDlgInput1;

	CNativeW CommandLineOption;
	GetCommandLineOption( CommandLineOption);

	std::unique_ptr<wchar_t[]> buf( new wchar_t[GetCommandLineOptionCountOf() + 1]);
	::wcscpy( buf.get(), CommandLineOption.GetStringPtr());

	std::wstring strTitle = L"HSP起動オプション指定";	// LS(STR_DLGPROFILE_NEW_PROF_TITLE);
	std::wstring strMessage = L"コマンドラインに追加される文字列を入力してください";	// LS(STR_DLGPROFILE_NEW_PROF_MSG);
	if (!cDlgInput1.DoModal(
		::GetModuleHandle(nullptr), hParent,
		strTitle.c_str(), strMessage.c_str(),
		GetCommandLineOptionCountOf(), buf.get()))
	{
		return false;
	}

	SetCommandLineOption( buf.get());
	return true;
}

bool CHsp3::OpenSrcFolder(HWND hParent) const
{
	// カレントディレクトリ版
	WCHAR	cmdline[1024];
	::GetCurrentDirectory(1024 - 1, cmdline);

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"explore", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenSrcFolder_File(HWND hParent, const CNativeW& strFilePath) const
{
	// 指定されたファイルが選択された状態でフォルダを開く版
	CNativeW cmdline;
	cmdline.AppendStringF(L"/select,\"%s\"", strFilePath);

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", L"explorer.exe", cmdline.GetStringPtr(), nullptr, SW_SHOWNORMAL));
}

bool CHsp3::RunAssist(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hspat.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::RunHSPTV(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hsptv.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::CreateDPM(HWND hParent) const
{
	WCHAR	exePath[1024];
	GetExedir(exePath, L"hsp3.exe");

	// HSP 3.7 以降
	WCHAR	axPath[1024];
	GetExedir( axPath, L"support\\pack_make_list.ax");
	if (! ::PathFileExists( axPath))	// 見つからない場合
	{
		// HSP 3.6 以前
		GetExedir( axPath, L"mkpack.ax");
	}

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", exePath, axPath, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::CreatePackopt(HWND hParent) const
{
	WCHAR	exePath[1024];
	GetExedir(exePath, L"hsp3.exe");

	// HSP 3.7 以降
	WCHAR	axPath[1024];
	GetExedir(axPath, L"support\\packopt_make_list.ax");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", exePath, axPath, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::ConvertDishC(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hsp3dh.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenHGIMG4Tool(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"gpbconv.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenPaintTool(HWND hParent) const
{
	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", L"mspaint", nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenHelpSourceEditor(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hdl.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, L"/hsedit:", nullptr, SW_SHOWNORMAL));
}

bool CHsp3::SearchKeyword(HWND hParent, const CNativeW& strKeyword) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hdl.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, strKeyword.GetStringPtr(), nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenPGManual(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"doclib\\hspprog.htm");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenFuncRef(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hdl.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenManualIndex(HWND hParent, bool bJapanese = true) const
{
	WCHAR	cmdline[1024];
	GetExedir( cmdline, bJapanese ? L"index.htm" : L"index_en.htm");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}