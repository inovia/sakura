#include "StdAfx.h"
#include "CHsp3.h"

bool CHsp3::Load(const CNativeW& strLibFileName)
{
	m_pHsp3Dll = new CHsp3Dll(strLibFileName.GetStringPtr());
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

bool CHsp3::OpenPaintTool(HWND hParent) const
{
	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", L"mspaint", nullptr, nullptr, SW_SHOWNORMAL));
}

bool CHsp3::SearchKeyword(HWND hParent, const CNativeW& strKeyword) const
{
	WCHAR	cmdline[1024];
	GetExedir(cmdline, L"hdl.exe");

	return ((HINSTANCE)32 < ::ShellExecute(
		hParent, L"open", cmdline, strKeyword.GetStringPtr(), nullptr, SW_SHOWNORMAL));
}