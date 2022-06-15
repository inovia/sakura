#include "StdAfx.h"
#include "CHsp3.h"
#include "_main/CProcess.h"
#include "_main/CControlProcess.h"

bool CHsp3::Load(const CNativeW& strLibFileName)
{
	// すべてのプロセスで生成を許可する
	m_pHsp3Dll	= new CHsp3Dll(strLibFileName.GetStringPtr());

	// コントロールプロセスのみ生成を許可する
	const auto& pCP = dynamic_cast<CControlProcess*>(CProcess::getInstance());
	if ( pCP != nullptr)
	{
		m_pHsp3If = new CHsp3Interface();
		m_pHsp3If->CreateInterfaceWindow( pCP->GetProcessInstance());
	}

	return m_pHsp3Dll->LoadDll();
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
	GetExedir(cmdline, L"ahtman.exe");

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

bool CHsp3::OpenManualIndex(HWND hParent, bool bEnglish = false) const
{
	WCHAR	cmdline[1024];
	if ( bEnglish)
	{
		GetExedir( cmdline, L"index_en.htm");
	}
	else
	{
		GetExedir( cmdline, L"index.htm");
	}

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, nullptr, nullptr, SW_SHOWNORMAL));
}