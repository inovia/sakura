#include "StdAfx.h"
#include "CHsp3.h"
#include "mem/CMemory.h"
#include "mem/CNativeA.h"
#include "mem/CNativeW.h"
#include "dlg/CDlgInput1.h"
#include "dlg/CDlgHspInput2.h"
#include "dlg/CDlgHspReports.h"
#include "dlg/CDlgHspTagJumpList.h"
#include "view/CEditView.h"
#include "window/CEditWnd.h"
#include "cmd/CViewCommander.h"
#include "_main/CProcess.h"
#include "_main/CControlProcess.h"

static DWORD ExecuteAndGetExitCode(
	const std::wstring& programPath, const std::wstring& cmdArgs)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;

	// コマンドライン引数を組み立てる（programPathとcmdArgsをスペースでつなぐ）
	std::wstring fullCmdLine = programPath + L" " + cmdArgs;

	if (::CreateProcess(
		nullptr,             // プログラム名（NULLでコマンドラインに含める）
		&fullCmdLine[0],     // コマンドライン
		nullptr,             // プロセスセキュリティ属性
		nullptr,             // スレッドセキュリティ属性
		FALSE,               // ハンドルの継承オプション
		0,                   // 作成フラグ
		nullptr,             // 新しい環境ブロック
		nullptr,             // カレントディレクトリの名前
		&si,                 // スタートアップ情報
		&pi))                // プロセス情報
	{
		// プロセスが終了するまで待つ
		MSG msg;
		DWORD result;

		while (true)
		{
			result = ::MsgWaitForMultipleObjects(
				1, &pi.hProcess, FALSE, INFINITE, QS_ALLEVENTS);

			// プロセスが終了
			if ( result == WAIT_OBJECT_0)
			{
				break;
			}
			else if (result == WAIT_OBJECT_0 + 1)
			{
				// メッセージキューにイベントがある
				while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			else
			{
				// エラー
				::CloseHandle(pi.hProcess);
				::CloseHandle(pi.hThread);
				return (DWORD)-3;
			}
		}

		// 終了コードを取得
		DWORD exitCode;
		if (::GetExitCodeProcess(pi.hProcess, &exitCode))
		{
			// ハンドルを閉じる
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
			return exitCode;
		}
		else
		{
			// ハンドルを閉じる
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
			return (DWORD)-2;
		}
	}
	else
	{
		return (DWORD)-1;
	}
}


static bool ShowOpenFileDialog(HWND hParent, std::vector<std::wstring> &filePaths)
{
	OPENFILENAMEW ofn = {0};
	WCHAR szFile[4096] = { 0 }; // バッファー。ここに選択されたファイル名が格納される

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hParent;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Image Files (*.png;*.gif;*.jpg;*.bmp)\0*.png;*.gif;*.jpg;*.bmp\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.lpstrTitle = L"入力画像を選択してください（複数選択可能）";  // タイトルバーのテキスト
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR;

	if ( GetOpenFileNameW(&ofn) == TRUE)
	{
		WCHAR* p = ofn.lpstrFile;
		std::wstring dir = p;
		p += dir.size() + 1;  // 最初のディレクトリ名をスキップ

		while (*p)
		{
			std::wstring file = p;
			filePaths.push_back(dir + L"\\" + file);
			p += file.size() + 1;  // 次のファイル名へ移動
		}

		// 単一のファイルが選択された場合
		if ( filePaths.empty())
		{  
			filePaths.push_back(dir);
		}
	}
	else {
		return false;
	}

	return true;
}

static bool ShowSaveFileDialog(HWND hwnd, std::wstring& outFilePath)
{
	OPENFILENAME ofn = {0};
	WCHAR szFileName[1024] = L"";

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.lpstrTitle = L"ICOファイルの保存先を選択してください";
	ofn.nMaxFile = sizeof(szFileName);
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = L"ico";

	if (::GetSaveFileName(&ofn))
	{
		outFilePath = szFileName;
		return true;
	}
	else
	{
		return false;
	}
}


static HRESULT SelectFolder(HWND hwnd, std::wstring &folderPath)
{
	IFileOpenDialog *pFileOpenDialog;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
		nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpenDialog));
	if (FAILED(hr))
	{
		return hr;
	}

	DWORD dwOptions;
	pFileOpenDialog->GetOptions(&dwOptions);
	pFileOpenDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);

	hr = pFileOpenDialog->Show(hwnd);
	if (FAILED(hr))
	{
		pFileOpenDialog->Release();
		return hr;
	}

	IShellItem *pItem;
	hr = pFileOpenDialog->GetResult(&pItem);
	if (FAILED(hr))
	{
		pFileOpenDialog->Release();
		return hr;
	}

	PWSTR pszFilePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (FAILED(hr))
	{
		pItem->Release();
		pFileOpenDialog->Release();
		return hr;
	}

	folderPath = pszFilePath;

	CoTaskMemFree(pszFilePath);
	pItem->Release();
	pFileOpenDialog->Release();

	return S_OK;
}


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
		// なお、ここでは設定値が未ロードのため注意
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
	return m_pHsp3Dll->hsc_ini()(0, (void*)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscRefNameW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS(name, nameA._GetMemory());
	return m_pHsp3Dll->hsc_refname()(0, (void*)nameA.GetStringPtr(), 0, 0);
}

BOOL CHsp3::HscObjNameW(const CNativeW& name) const
{
	// UTF-16 -> Shift_JIS
	CNativeA nameA;
	CShiftJis::UnicodeToSJIS(name, nameA._GetMemory());
	return m_pHsp3Dll->hsc_objname()(0, (void*)nameA.GetStringPtr(), 0, 0);
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
	return m_pHsp3Dll->hsc3_make()(0,
		(void*)nameA.GetStringPtr(), 1/* アイコン書き換え機能有効 */, 0);
}

BOOL CHsp3::HscGetMesW(CNativeW& msg) const
{
	// バッファサイズ取得
	int bufsize = 0;
	m_pHsp3Dll->hsc3_messize()((void*)&bufsize, 0, 0, 0);

	if ( bufsize == 0)
		return FALSE;

	std::unique_ptr<char[]> buf(new char[bufsize + 1]);
	m_pHsp3Dll->hsc_getmes()((void*)buf.get(), 0, 0, 0);

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
		(void*)buf.get(), (void*)objA.GetStringPtr(), 0, 0);

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
		(void*)strExeCmdsA.GetStringPtr(), IsShowDebugWindow() ? 1 : 0, 0, 0);
}

int CHsp3::HscAnalysisW(
	const CNativeW& keyword,
	const CNativeW& filePath,
	const CNativeW& refName,
	std::vector<CHsp3::ParsedData>& outData) const
{
	// HSP 3.7 以上
	if (! m_pHsp3Dll->IsLoaded37())
	{
		return -1;
	}

	// キーワードを小文字化
	CNativeW keywordW = keyword.GetStringPtr();
	ToLowerCase( keywordW.GetStringPtr());

	// UTF-16 -> Shift_JIS
	CNativeA keywordA;
	CNativeA filePathA;
	CNativeA refNameA;
	CShiftJis::UnicodeToSJIS( keywordW.GetStringPtr(), keywordA._GetMemory());
	CShiftJis::UnicodeToSJIS( filePath, filePathA._GetMemory());
	CShiftJis::UnicodeToSJIS( refName, refNameA._GetMemory());

	// コンパイラー側へ一覧取得依頼
	CNativeW keywordList;
	{
		int bufsize = 0;

		m_pHsp3Dll->hsc_ini()(0, (void*)filePathA.GetStringPtr(), 0, 0);
		m_pHsp3Dll->hsc_refname()(0, (void*)refNameA.GetStringPtr() /* "???" */, 0, 0);
		m_pHsp3Dll->hsc_objname()(0, (void*)"obj", 0, 0);		// 参照されてないっぽい?
		m_pHsp3Dll->hsc3_analysis()(0, (void*)keywordA.GetStringPtr(),
			m_pHsp3Dll->ANALYSIS_MODE_ALL + m_pHsp3Dll->ANALYSIS_MODE_REFERENCE, 0);

		if ( m_pHsp3Dll->hsc_comp()(16, 0, 0, 0))
		{
			return -2;
		}

		m_pHsp3Dll->hsc3_kwlsize()((void*)&bufsize, 0, 0, 0);
		if ( bufsize <= 0)
		{
			return -3;
		}

		std::unique_ptr<char[]> buf( new char[bufsize + 1]);
		m_pHsp3Dll->hsc3_kwlbuf()((void*)buf.get(), 0, 0, 0);
		m_pHsp3Dll->hsc3_kwlclose()(0, 0, 0, 0);

		// Shift_JIS -> UTF-16
		CMemory cmemSjis( buf.get(), bufsize);
		CShiftJis::SJISToUnicode(cmemSjis, &keywordList);
		cmemSjis.Reset();
	}

	// パース
	outData = ParseLines( keywordList);
	return 0;
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
		m_pHsp3Dll->hsc_ini()(0, (void*)"hsptmp", 0, 0);
		m_pHsp3Dll->hsc_refname()(0, (void*)"???", 0, 0);
		m_pHsp3Dll->hsc_objname()(0, (void*)"obj", 0, 0);
		m_pHsp3Dll->hsc3_getsym()(0, 0, 0, 0);
		m_pHsp3Dll->hsc3_messize()((void*)&bufsize, 0, 0, 0);

		std::unique_ptr<char[]> buf( new char[bufsize + 1]);
		m_pHsp3Dll->hsc_getmes()((void*)buf.get(), 0, 0, 0);

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
		m_pHsp3Dll->hsc_ini()(0, (void*)"hsptmp", 0, 0);
		m_pHsp3Dll->hsc_refname()(0, (void*)"???", 0, 0);
		m_pHsp3Dll->hsc_objname()(0, (void*)"obj", 0, 0);
		if ( m_pHsp3Dll->hsc3_getsym()(0, 0, 0, 0))
		{
			return false;
		}
		m_pHsp3Dll->hsc3_messize()((void*)&bufsize, 0, 0, 0);

		std::unique_ptr<char[]> buf(new char[bufsize + 1]);
		m_pHsp3Dll->hsc_getmes()((void*)buf.get(), 0, 0, 0);

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

bool CHsp3::RunAssistAtOnce(HWND hParent) const
{
	// 自動起動用
	auto& bAutoStartFlag = GetDllShareData().m_Common.m_sHSP.m_bHspAssistantAutoStartFlag;
	if (!bAutoStartFlag)
	{
		GetDllShareData().m_Common.m_sHSP.m_bHspAssistantAutoStartFlag = true;
		return RunAssist(hParent);
	}
	return false;
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

	// HSP 3.7 beta 5 以降
	WCHAR	axPath[1024];
	GetExedir( axPath, L"support\\packdir_make_list.ax");
	if (!::PathFileExists( axPath))	// 見つからない場合
	{
		// HSP 3.7 beta 2～4頃
		GetExedir( axPath, L"support\\pack_make_list.ax");
		if (!::PathFileExists( axPath))	// 見つからない場合
		{
			// HSP 3.6 以前
			GetExedir( axPath, L"mkpack.ax");
			if (!::PathFileExists( axPath))	// 見つからない場合
			{
				return false;
			}
		}
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
	if (!::PathFileExists(axPath))	// 見つからない場合
	{
		return false;
	}

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", exePath, axPath, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::OpenMapTool(HWND hParent) const
{
	WCHAR	exePath[1024];
	GetExedir(exePath, L"hsp3dish.exe");

	// HSP 3.7 以降
	WCHAR	axPath[1024];
	GetExedir(axPath, L"support\\tamamap.ax");
	if (!::PathFileExists(axPath))	// 見つからない場合
	{
		return false;
	}

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

bool CHsp3::RunIconConverter(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"iconcnv.exe");

	// 入力ファイル選択ダイアログ(複数選択可)
	std::vector<std::wstring> filePaths;
	if (! ShowOpenFileDialog( hParent, filePaths) || filePaths.empty())
	{
		return false;	// 未選択
	}

	// 出力ファイル選択ダイアログ
	std::wstring fileSavePath;
	if (! ShowSaveFileDialog( hParent, fileSavePath))
	{
		return false;	// 未選択
	}

	// 引数
	std::wstring args;
	{
		// /INPUT フラグとその後のファイルパスを追加
		args = L"/INPUT ";
		for (const auto& filePath : filePaths)
		{
			args += L" \"" + filePath + L"\" ";
		}

		// /OUTPUT フラグとその後のファイルパスを追加
		args += L"/OUTPUT \"" + fileSavePath + L"\" ";

		// /RESIZE フラグを追加
		args += L"/RESIZE ";

		// /FORCE32BIT フラグを追加
		args += L"/FORCE32BIT ";
	}

	// 実行
	std::wstring msg;
	auto exitCode = ExecuteAndGetExitCode(cmdline, args);
	switch (exitCode)
	{
		case -1:
			msg = L"CreateProcessに失敗しました";
			break;
		case -2:
			msg = L"終了コードが取得できません";
			break;
		case -3:
			msg = L"終了待機中にエラーが発生しました";
			break;
		case 0:
			msg = L"正常終了しました";
			break;
		case 1:
			msg = L"入力ファイルのパスが指定されていません";
			break;
		case 2:
			msg = L"出力ファイルのパスが指定されていません";
			break;
		case 3:
			msg = L"出力ファイルの書き込み異常です";
			break;
		case 4:
			msg = L"入力ファイルの読み取り異常です";
			break;
		default:
			break;
	}

	// メッセージ
	bool bError = (exitCode != 0);
	::MessageBox(
		hParent, msg.c_str(),
		bError ? L"Error" : L"Info",
		MB_OK | bError ? MB_ICONERROR : MB_ICONINFORMATION);
	return !bError;
}

bool CHsp3::RunHSP3Updater(HWND hParent) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hsp3upd.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
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

bool CHsp3::OpenDocument(HWND hParent, CViewCommander& Commander, const CNativeW& strCommand) const
{
	// 拡張子取得
	const auto& lpCommand = strCommand.GetStringPtr();
	const auto& lpExtention = ::PathFindExtension( lpCommand);

	// パス存在チェック
	if (!::PathFileExists(lpCommand)
		&& !::PathIsDirectory(lpCommand))
	{
		return false;
	}

	// 拡張子で内部起動するか判断する
	if ( _wcsicmp( lpExtention, L".hsp") == 0
		|| _wcsicmp( lpExtention, L".as") == 0)
	{
		// タグジャンプ情報の保存
		TagJump	tagJump;
		{
			// 参照元ウィンドウ保存
			tagJump.hwndReferer = CEditWnd::getInstance()->GetHwnd();

			// 現在のカーソル位置を取得
			Commander.GetDocument()->m_cLayoutMgr.LayoutToLogic(
				Commander.GetCaret().GetCaretLayoutPos(),
				&tagJump.point
			);

			// 戻れるように登録する
			CTagJumpManager().PushTagJump(&tagJump);
		}

		// 内部起動
		Commander.Command_FILEOPEN(lpCommand);
		return true;
	}
	else
	{
		// 外部起動
		return ((HINSTANCE)32 < ::ShellExecute(
			hParent, L"open", lpCommand, nullptr, nullptr, SW_SHOWNORMAL));
	}
}

bool CHsp3::JumpDefinition(
	HWND hParent, CEditView& Commander,
	const CNativeW& strKeyword, const CNativeW& strFileName, const CNativeW& strRefName,
	const CNativeW& strFileDir, const CNativeW& strCommonDir
	) const
{
	// 3.7 のDLLがロードされていない
	if (! m_pHsp3Dll->IsLoaded37())
	{
		std::wstring msg = L"旧バージョンの hspcmp.dll がロードされているため、実行できません";
		::MessageBox(
			hParent, msg.c_str(), L"Error",
			MB_OK | MB_ICONERROR);
		return false;
	}

	// 解析実行
	std::vector<CHsp3::ParsedData> outKeywords;
	int ret = HscAnalysisW( strKeyword, strFileName, strRefName, outKeywords);
	if ( ret < 0)
	{
		if (ret == -2)
			ShowErrorReport(hParent);
		else
			ErrorBeep();
		return false;
	}

	// 現在キャレット行
	CLayout* pCLayout = Commander.GetDocument()->m_cLayoutMgr.SearchLineByLayoutY(
		Commander.GetCaret().GetCaretLayoutPos().GetY2());
	int lineNo = pCLayout->GetLogicLineNo() + 1;

	// 参照元が一覧にあるかをチェックする
	// 正確な名前を検出する目的
	CNativeW strRealKeyword;

	for ( const auto& keyword : outKeywords)
	{
		// 自分自身のファイルかどうか？
		if ( ( _wcsicmp(
			keyword.filePath.c_str(), strRefName.GetStringPtr()) == 0)
			|| _wcsicmp(
				keyword.filePath.c_str(), strFileName.GetStringPtr()) == 0)
		{
			if ( keyword.lineNumber == lineNo)
			{
				strRealKeyword = keyword.keyword2.c_str();
				break;
			}
		}
	}

	// 参照元が見つからない
	if ( strRealKeyword.GetStringLength() <= 0)
	{
		ErrorBeep();
		return false;
	}

	// 定義元の検索
	CNativeW strJumpFileName;
	int nJumpLineNo = 0;
	for ( const auto& keyword : outKeywords)
	{
		// 定義のみ
		if ( keyword.keyword1_head == L'd')
		{
			// 完全一致するキーワードか?
			//（ただし、大文字小文字は区別しない）
			if ( _wcsicmp(
				keyword.keyword2.c_str(), strRealKeyword.GetStringPtr()) == 0)
			{
				strJumpFileName = keyword.filePath.c_str();
				nJumpLineNo = keyword.lineNumber;
				break;
			}
		}
	}

	// それでも見つからない
	if ( strJumpFileName.GetStringLength() <= 0)
	{
		ErrorBeep();
		return false;
	}

	// 自分自身かどうか？
	if ((_wcsicmp(
		strJumpFileName.GetStringPtr(), strRefName.GetStringPtr()) == 0)
		|| _wcsicmp(
			strJumpFileName.GetStringPtr(), strFileName.GetStringPtr()) == 0)
	{
		// タグジャンプ情報の保存
		TagJump	tagJump;
		{
			// 参照元ウィンドウ保存
			tagJump.hwndReferer = CEditWnd::getInstance()->GetHwnd();

			// 現在のカーソル位置を取得
			Commander.GetDocument()->m_cLayoutMgr.LayoutToLogic(
				Commander.GetCaret().GetCaretLayoutPos(),
				&tagJump.point
			);

			// 戻れるように登録する
			CTagJumpManager().PushTagJump(&tagJump);
		}

		// キャレットの移動
		CLayoutPoint ptCaretPos;
		Commander.GetDocument()->m_cLayoutMgr.LogicToLayout(
			CLogicPoint(0, nJumpLineNo - 1),
			&ptCaretPos
		);
		Commander.GetCaret().MoveCursor(ptCaretPos, true);
		return true;
	}

	// 外部ファイル
	{
		WCHAR szCommand[1024] = { 0 };

		// 相対パスかチェック
		if (::PathIsRelative( strJumpFileName.GetStringPtr()))
		{
			// 相対パスは絶対パスに変える
			// Commonから
			::PathCombine(szCommand, strCommonDir.GetStringPtr(), strJumpFileName.GetStringPtr());

			// パス存在チェック
			if (!::PathFileExists(szCommand)
				&& !::PathIsDirectory(szCommand))
			{
				// HSPファイルのフォルダから
				::PathCombine(szCommand, strFileDir.GetStringPtr(), strJumpFileName.GetStringPtr());

				// パス存在チェック
				if (!::PathFileExists(szCommand)
					&& !::PathIsDirectory(szCommand))
				{
					ErrorBeep();
					return false;
				}
			}
		}
		else
		{
			// 絶対パス
			wcscpy( szCommand, strJumpFileName.GetStringPtr());

			// パス存在チェック
			if (!::PathFileExists(szCommand)
				&& !::PathIsDirectory(szCommand))
			{
				ErrorBeep();
				return false;
			}
		}

		Commander.TagJumpSub( szCommand, CMyPoint(1, nJumpLineNo /*LineNo*/));
	}
	return true;
}

bool CHsp3::JumpAllReferences(
	HWND hParent, CEditView& Commander,
	const CNativeW& strKeyword, const CNativeW& strFileName, const CNativeW& strRefName,
	const CNativeW& strFileDir, const CNativeW& strCommonDir)
{
	WCHAR szCommand[1024] = { 0 };

	// 3.7 のDLLがロードされていない
	if (! m_pHsp3Dll->IsLoaded37())
	{
		std::wstring msg = L"旧バージョンの hspcmp.dll がロードされているため、実行できません";
		::MessageBox(
			hParent, msg.c_str(), L"Error",
			MB_OK | MB_ICONERROR);
		return false;
	}

	// 解析実行
	std::vector<CHsp3::ParsedData> outKeywords;
	int ret = HscAnalysisW(strKeyword, strFileName, strRefName, outKeywords);
	if (ret < 0)
	{
		if (ret == -2)
			ShowErrorReport(hParent);
		else
			ErrorBeep();
		return false;
	}

	// 完全一致のみ格納していく
	std::vector<HspTagJumpInfo> tagList;
	for (const auto& keyword : outKeywords)
	{
		// 完全一致しない場合はスキップ（大文字小文字は区別しない）
		if ( _wcsicmp(
			keyword.keyword2.c_str(), strKeyword.GetStringPtr()) != 0)
		{
			continue;
		}

		// 積んでいく
		HspTagJumpInfo tag;
		tag.keyword = keyword.keyword2.c_str();
		tag.no = keyword.lineNumber;
		tag.type = (keyword.keyword1_head == L'd') ? L"DEF" : L"REF";
		tag.note = keyword.keyword1_tail.c_str();

		// 自分自身かどうか？
		if (( _wcsicmp(
			keyword.filePath.c_str(), strRefName.GetStringPtr()) == 0)
			|| _wcsicmp(
				keyword.filePath.c_str(), strFileName.GetStringPtr()) == 0)
		{
			// ファイルパスが未確定
			if (!Commander.GetDocument()->m_cDocFile.GetFilePathClass().IsValidPath())
			{
				tag.filename = L"???";
			}
			else
			{
				tag.filename = Commander.GetDocument()->m_cDocFile.GetFilePath();
			}
		}
		else
		{
			// 自分以外のファイル
			szCommand[0] = '\0';

			// 相対パスかチェック
			if (::PathIsRelative( keyword.filePath.c_str()))
			{
				// 相対パスは絶対パスに変える
				// Commonから
				::PathCombine(szCommand, strCommonDir.GetStringPtr(), keyword.filePath.c_str());

				// パス存在チェック
				if (!::PathFileExists(szCommand)
					&& !::PathIsDirectory(szCommand))
				{
					// HSPファイルのフォルダから
					::PathCombine(szCommand, strFileDir.GetStringPtr(), keyword.filePath.c_str());

					// パス存在チェック
					if (!::PathFileExists(szCommand)
						&& !::PathIsDirectory(szCommand))
					{
						// どこにも見つからない。仕方がない。
						wcscpy(szCommand, keyword.filePath.c_str());
					}
				}
			}
			else
			{
				// 絶対パス
				wcscpy(szCommand, keyword.filePath.c_str());
			}

			tag.filename = szCommand;
		}

		tagList.push_back(tag);
	}

	// 一つも見つからない
	if ( tagList.size() == 0)
	{
		ErrorBeep();
		return false;
	}

	// 初回起動?
	if ( m_pDlgHspTagJumpList == nullptr)
	{
		m_pDlgHspTagJumpList = new CDlgHspTagJumpList();
	}

	// モーダル
#if 0
	if (! ::IsWindow( m_pDlgHspTagJumpList->GetHwnd()))
	{
		// ダイアログ起動
		m_pDlgHspTagJumpList->SetItem(tagList);
		if (! m_pDlgHspTagJumpList->DoModal(G_AppInstance(), hParent, 0))
		{
			return true;	//キャンセル
		}

		// 選択項目取得
		const auto pSelectedItem = m_pDlgHspTagJumpList->GetSelectedItem();
		if (pSelectedItem == nullptr)
		{
			ErrorBeep();
			return false;
		}

		return TagJumpSub(Commander, *pSelectedItem);
	}
#endif

#if 1
	// モードレス
	if (! ::IsWindow( m_pDlgHspTagJumpList->GetHwnd()))
	{
		// ダイアログ起動
		m_pDlgHspTagJumpList->SetItem(tagList);
		const auto ret = m_pDlgHspTagJumpList->DoModeless(
			G_AppInstance(), hParent, 0,
			[&Commander, this](const HspTagJumpInfo* pSelectedItem)
		{
			// ジャンプ時の処理
			if ( pSelectedItem == nullptr)
			{
				ErrorBeep();
				return;
			}
			TagJumpSub(Commander, *pSelectedItem);
		}
		, []()
		{
			
		});

		// モードレスなので即座に返ってくる点注意
		if (!ret)
		{
			ErrorBeep();
			return false;	//失敗
		}
	}
	else
	{
		/* アクティブにする */
		m_pDlgHspTagJumpList->SetItem(tagList);
		m_pDlgHspTagJumpList->UpdateItem();
		ActivateFrameWindow( m_pDlgHspTagJumpList->GetHwnd());
	}
#endif

	return true;
}

bool CHsp3::TagJumpSub(CEditView& Commander, const HspTagJumpInfo& SelectedItem) const
{
	// 現在キャレット行
	CLayout* pCLayout = Commander.GetDocument()->m_cLayoutMgr.SearchLineByLayoutY(
		Commander.GetCaret().GetCaretLayoutPos().GetY2());
	int lineNo = pCLayout->GetLogicLineNo() + 1;

	// ファイルパス未確定な自分自身?
	if ( _wcsicmp(L"???", SelectedItem.filename.c_str()) == 0 )
	{
		// タグジャンプ情報の保存
		TagJump	tagJump;
		{
			// 参照元ウィンドウ保存
			tagJump.hwndReferer = CEditWnd::getInstance()->GetHwnd();

			// 現在のカーソル位置を取得
			Commander.GetDocument()->m_cLayoutMgr.LayoutToLogic(
				Commander.GetCaret().GetCaretLayoutPos(),
				&tagJump.point
			);

			// 戻れるように登録する
			CTagJumpManager().PushTagJump(&tagJump);
		}

		// キャレットの移動
		CLayoutPoint ptCaretPos;
		Commander.GetDocument()->m_cLayoutMgr.LogicToLayout(
			CLogicPoint(0, SelectedItem.no - 1),
			&ptCaretPos
		);
		Commander.GetCaret().MoveCursor(ptCaretPos, true);
		return true;
	}

	Commander.TagJumpSub(
		SelectedItem.filename.c_str(), CMyPoint(1, SelectedItem.no /*LineNo*/));
	return true;
}

// 改行（CRLF）で文字列を分割する関数
std::vector<std::wstring> CHsp3::SplitLines(const CNativeW& str) const
{
	std::vector<std::wstring> lines;
	const wchar_t* start = str.GetStringPtr();
	const wchar_t* end;

	while ((end = std::wcsstr(start, L"\r\n")) != nullptr)
	{
		lines.emplace_back(start, end);
		start = end + 2;  // CRLFの長さ（\r\n）は2文字
	}

	// 最後の行（CRLFで終わっていない場合）
	if (*start)
	{
		lines.emplace_back(start);
	}

	return lines;
}

// 各行をパースする関数
std::vector<CHsp3::ParsedData> CHsp3::ParseLines(const CNativeW& str) const
{
	std::vector<CHsp3::ParsedData> parsedDataList;

	// 各行に対して処理を行う
	const auto& lines = SplitLines(str);
	for (const auto& line : lines)
	{
		// キーワード1が4文字でない場合、スキップ
		if ( line.size() < 4 || line[4] != L' ')
		{
			continue;
		}

		ParsedData data;

		size_t firstSpace = line.find(L' ');  // 最初のスペースの位置
		size_t secondSpace = line.find(L' ', firstSpace + 1);  // 二番目のスペースの位置

		// キーワード1の先頭文字と残りを分割
		data.keyword1_head = line[0];
		data.keyword1_tail = line.substr(1, 3);

		// キーワード2を取得する
		data.keyword2 = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);

		// キーワード3を取得し、行番号とファイルパスに分割する
		std::wstring keyword3 = line.substr(secondSpace + 1);
		size_t colonPos = keyword3.find(L':');
		if (colonPos != std::wstring::npos)
		{
			data.lineNumber = std::stoi(keyword3.substr(0, colonPos));
			data.filePath = keyword3.substr(colonPos + 1);
		}

		// -----------------------------------------------------
		// 旧型式
		// -----------------------------------------------------
		//// キーワード2と3を抽出
		//size_t bracketStart = line.find(L"[");
		//size_t bracketEnd = line.find(L"]");

		//// []でくくられていない場合、スキップ
		//if ( bracketStart == std::wstring::npos
		//	|| bracketEnd == std::wstring::npos)
		//{
		//	continue;
		//}

		//// 末尾のスペースを削除
		//std::wstring keyword2 = line.substr(5, bracketStart - 5);
		//keyword2.erase(keyword2.find_last_not_of(L" ") + 1);
		//data.keyword2 = keyword2;

		//std::wstring keyword3 = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);

		//// 最後のコロンを探す（Windowsフルパス対応）
		//size_t lastColonPos = keyword3.rfind(L":");

		//// コロンが見つからない場合、スキップ
		//if (lastColonPos == std::wstring::npos)
		//{
		//	continue;
		//}

		//data.filePath = keyword3.substr(0, lastColonPos);
		//data.lineNumber = std::stoi(keyword3.substr(lastColonPos + 1));
		// -----------------------------------------------------

		// 結果を格納
		parsedDataList.push_back(data);
	}

	return parsedDataList;
}

// 大文字を小文字に変換する関数(自分自身を書き換えます)
void CHsp3::ToLowerCase(wchar_t* str) const
{
	for (size_t i = 0; str[i] != L'\0'; ++i)
	{
		str[i] = std::towlower(str[i]);
	}
}

bool CHsp3::LoadMenuIniItems()
{
	// Menu.txt ロード
	const auto& langId = CSelectLang::getDefaultLangId();
	auto bJapanese = (langId == 1041);	// 1041 = Japanese

	WCHAR	iniPath[1024];
	if ( bJapanese)
		GetExedir(iniPath, L"support\\menu.txt");		// 日本語
	else
		GetExedir(iniPath, L"support\\menu_en.txt");	// 日本語以外

	if (!::PathFileExists(iniPath))
		return false;

	m_MenuIniItems.clear();

	CHsp3MenuItemParser parser;
	auto result = parser.parse(iniPath, m_MenuIniItems);
	return (result == SUCCESS);
}

bool CHsp3::ExecMenuIniItem(HWND hParent, const MenuItem& item) const
{
	WCHAR	exePath[1024];
	WCHAR	axPath[1024];

	// ランタイム名
	auto runtimeName = item.runtimeName + L".exe";
	GetExedir(exePath, runtimeName.c_str());
	if (!::PathFileExists(exePath))		// 見つからない場合
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

		return false;
	}

	// AXファイルパス名
	auto axFilePath = L"support\\" + item.axFileName;
	GetExedir( axPath, axFilePath.c_str());
	if (!::PathFileExists(axPath))		// 見つからない場合
	{
		CNativeW errMsg;
		errMsg.AppendStringF(
			L"%s\r\nPath = %s",
			L"AXファイルが見つかりません。",
			axPath
		);

		::MessageBox(
			hParent, errMsg.GetStringPtr(),
			L"Error", MB_OK | MB_ICONERROR);

		return false;
	}

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", exePath, axPath, nullptr, SW_SHOWNORMAL));
}