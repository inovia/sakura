#pragma once
#include <windows.h>
#include <shlwapi.h>
#include "mem/CNativeW.h"
#include "charset/CShiftJis.h"

#include "CKeyWordSetMgr.h"
#include "CHsp3Dll.h"
#include "CHsp3Font.h"
#include "CHsp3Interface.h"

class CHsp3
{
public:
	// 強調キーワード名称
	static constexpr std::wstring_view HSP3_FUNC_NAME		{ L"HSP3(func)" };
	static constexpr std::wstring_view HSP3_PRE_NAME		{ L"HSP3(pre)" };
	static constexpr std::wstring_view HSP3_MACRO_NAME		{ L"HSP3(macro)" };

private:

	// hspcmp用（すべてのプロセスで生成されます）
	CHsp3Dll* m_pHsp3Dll = nullptr;

	// hsedsdk用（コントロールプロセスのみで生成されます）
	CHsp3Interface* m_pHsp3If = nullptr;

public:
	// デフォルトコンストラクタ
	CHsp3() {  };

	// デストラクタ
	~CHsp3() { delete m_pHsp3Dll; delete m_pHsp3If; }

public:

	bool Load(const CNativeW& strLibFileName);

	inline bool IsLoaded() const
	{
		return (( m_pHsp3Dll != nullptr ) && m_pHsp3Dll->IsLoaded());
	}

	inline const CHsp3Dll* Hsp3Dll() const { return m_pHsp3Dll; }
	inline const CHsp3Interface* Hsp3If() const { return m_pHsp3If; }

	bool InitKeyword(CKeyWordSetMgr& keywordMgr) const;

public:
	bool OpenSrcFolder(HWND hParent) const;
	bool OpenSrcFolder_File(HWND hParent, const CNativeW& strFilePath) const;
	bool RunAssist(HWND hParent) const;
	bool RunHSPTV(HWND hParent) const;
	bool CreateDPM(HWND hParent) const;
	bool CreatePackopt(HWND hParent) const;
	bool ConvertDishC(HWND hParent) const;
	bool OpenHGIMG4Tool(HWND hParent) const;
	bool OpenPaintTool(HWND hParent) const;
	bool OpenHelpSourceEditor(HWND hParent) const;
	bool SearchKeyword(HWND hParent, const CNativeW& strKeyword) const;
	bool OpenPGManual(HWND hParent) const;
	bool OpenFuncRef(HWND hParent) const;
	bool OpenManualIndex(HWND hParent, bool bJapanese) const;
};