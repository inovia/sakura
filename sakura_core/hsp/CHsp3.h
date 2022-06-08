#pragma once
#include <windows.h>
#include "mem/CNativeW.h"
#include "charset/CShiftJis.h"

#include "CKeyWordSetMgr.h"
#include "CHsp3dll.h"

class CHsp3
{
public:
	// 強調キーワード名称
	static constexpr std::wstring_view HSP3_FUNC_NAME		{ L"HSP3(func)" };
	static constexpr std::wstring_view HSP3_PRE_NAME		{ L"HSP3(pre)" };
	static constexpr std::wstring_view HSP3_MACRO_NAME		{ L"HSP3(macro)" };

private:
	CHsp3Dll* m_pHsp3Dll = nullptr;

public:
	// デフォルトコンストラクタ
	CHsp3() {  };

	// デストラクタ
	~CHsp3() { delete m_pHsp3Dll; }

public:

	bool Load(const CNativeW& strLibFileName);

	inline bool IsLoaded() const
	{
		return (( m_pHsp3Dll != nullptr ) && m_pHsp3Dll->IsLoaded());
	}

	inline const CHsp3Dll* Hsp3Dll() const
	{
		return m_pHsp3Dll;
	}

	bool InitKeyword(CKeyWordSetMgr& keywordMgr) const;

public:

	bool OpenPaintTool(HWND hParent) const;
	bool SearchKeyword(HWND hParent, const CNativeW& strKeyword) const;

};