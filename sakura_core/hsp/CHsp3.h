#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <cwctype>

#include "mem/CNativeW.h"
#include "charset/CShiftJis.h"
#include "dlg/CDlgHspTagJumpList.h"

#include "CKeyWordSetMgr.h"
#include "CHsp3Dll.h"
#include "CHsp3Font.h"
#include "CHsp3Interface.h"
#include "CHsp3MenuItemParser.h"

class CEditView;
class CViewCommander;

class CHsp3
{
public:
	// 強調キーワード名称
	static constexpr std::wstring_view HSP3_FUNC_NAME		{ L"HSP3(func)" };
	static constexpr std::wstring_view HSP3_PRE_NAME		{ L"HSP3(pre)" };
	static constexpr std::wstring_view HSP3_MACRO_NAME		{ L"HSP3(macro)" };

	// コマンドライン引数バッファ
	static const int COMMANDLINE_BUFFER_LENGTH = 4096;

private:

	// パースしたデータを格納するための構造体
	typedef struct
	{
		wchar_t keyword1_head;			// キーワード1の先頭文字（'d'または'r'）
		std::wstring keyword1_tail;		// キーワード1の残りの部分
		std::wstring keyword2;			// キーワード2
		std::wstring filePath;			// ファイルパス
		int lineNumber;					// 行番号
	} ParsedData;

private:

	// hspcmp用（すべてのプロセスで生成されます）
	CHsp3Dll* m_pHsp3Dll = nullptr;

	// hsedsdk用（コントロールプロセスのみで生成されます）
	CHsp3Interface* m_pHsp3If = nullptr;

	// タグジャンプ（ダイアログが生成された場合に作成されます）
	CDlgHspTagJumpList*	m_pDlgHspTagJumpList = nullptr;

private:

	// Menu.ini のアイテム
	std::vector<MenuItem> m_MenuIniItems;

public:
	// デフォルトコンストラクタ
	CHsp3() {  };

	// デストラクタ
	~CHsp3()
	{
		if ( m_pHsp3Dll != nullptr)
			delete m_pHsp3Dll;
		if ( m_pHsp3If != nullptr)
			 delete m_pHsp3If;
		if ( m_pDlgHspTagJumpList != nullptr)
			delete m_pDlgHspTagJumpList;
	}

public:

	bool Load(const CNativeW& strLibFileName);

	inline bool IsLoaded() const
	{
		return (( m_pHsp3Dll != nullptr ) && m_pHsp3Dll->IsLoaded());
	}

	inline const CHsp3Dll* Hsp3Dll() const { return m_pHsp3Dll; }
	inline const CHsp3Interface* Hsp3If() const { return m_pHsp3If; }

	inline const void GetCommandLineOption(CNativeW& strCommandLineOption) const
	{
		auto& CommandLineOption =
			GetDllShareData().m_Common.m_sHSP.m_szCommandLineOption;
		strCommandLineOption.SetString(CommandLineOption);
	}

	inline void SetCommandLineOption(const CNativeW& strCommandLineOption) const
	{
		auto& CommandLineOption =
			GetDllShareData().m_Common.m_sHSP.m_szCommandLineOption;
		wcsncpy( CommandLineOption,
			strCommandLineOption.GetStringPtr(),
			_countof(CommandLineOption));
		CommandLineOption[_countof(CommandLineOption) - 1] = L'\0';
	}

	inline int GetCommandLineOptionCountOf() const
	{
		auto& CommandLineOption =
			GetDllShareData().m_Common.m_sHSP.m_szCommandLineOption;
		return _countof(CommandLineOption);
	}

	inline const void GetExecuteExternalFile_Name(CNativeW& strFileName) const
	{
		auto& ExecuteExternalFile_Name =
			GetDllShareData().m_Common.m_sHSP.m_szExecuteExternalFile_Name;
		strFileName.SetString(
			GetDllShareData().m_Common.m_sHSP.m_szExecuteExternalFile_Name);
	}

	inline void SetExecuteExternalFile_Name(const CNativeW& strFileName) const
	{
		auto& ExecuteExternalFile_Name =
			GetDllShareData().m_Common.m_sHSP.m_szExecuteExternalFile_Name;
		wcsncpy(ExecuteExternalFile_Name,
			strFileName.GetStringPtr(),
			_countof(ExecuteExternalFile_Name));
		ExecuteExternalFile_Name[_countof(ExecuteExternalFile_Name) - 1] = L'\0';
	}

	inline int GetExecuteExternalFile_NameCountOf() const
	{
		auto& ExecuteExternalFile_Name =
			GetDllShareData().m_Common.m_sHSP.m_szExecuteExternalFile_Name;
		return _countof(ExecuteExternalFile_Name);
	}

	inline const bool IsExecuteExternalFile_CreateObjectOnly() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bExecuteExternalFile_CreateObjectOnly;
	}

	inline void SetExecuteExternalFile_CreateObjectOnly(bool bCreateObjectOnly) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bExecuteExternalFile_CreateObjectOnly = bCreateObjectOnly;
	}

	inline const bool IsShowDebugWindow() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bShowDebugWindow;
	}

	inline const bool IsExecuteExternalFile_Utf8Mode() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bExecuteExternalFile_UTF8Mode;
	}

	inline void SetExecuteExternalFile_Utf8Mode(bool bUtf8Mode) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bExecuteExternalFile_UTF8Mode = bUtf8Mode;
	}

	inline void SetShowDebugWindow( bool bShowDebugWindow) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bShowDebugWindow = bShowDebugWindow;
	}

	inline const bool IsHspAssistantAutoStartEnabled() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bHspAssistantAutoStartEnabled;
	}

	inline void SetHspAssistantAutoStartEnabled(bool bAutoStartEnabled) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bHspAssistantAutoStartEnabled = bAutoStartEnabled;
	}

	inline const bool IsUseLegacyLabelAnalysis() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bUseLegacyLabelAnalysis;
	}

	inline void SetUseLegacyLabelAnalysis(bool bUseLegacyLabelAnalysis) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bUseLegacyLabelAnalysis = bUseLegacyLabelAnalysis;
	}

	inline const bool IsAutoSaveBeforeCompile() const
	{
		return GetDllShareData().m_Common.m_sHSP.m_bAutoSaveBeforeCompile;
	}

	inline void SetAutoSaveBeforeCompile(bool bAutoSaveBeforeCompile) const
	{
		GetDllShareData().m_Common.m_sHSP.m_bAutoSaveBeforeCompile = bAutoSaveBeforeCompile;
	}

	bool InitKeyword(CKeyWordSetMgr& keywordMgr) const;

private:
	BOOL HscIniW(const CNativeW& name) const;
	BOOL HscRefNameW(const CNativeW& name) const;
	BOOL HscObjNameW(const CNativeW& name) const;
	BOOL HscCmp(int p1, int p2, int p3, int p4) const;
	BOOL HscMakeW(const CNativeW& name) const;
	BOOL HscGetMesW(CNativeW& msg) const;
	BOOL HscGetRuntimeW(CNativeW& name, const CNativeW& strObj) const;
	BOOL HscRunW(const CNativeW& strExeCmds) const;
	int HscAnalysisW(
		const CNativeW& keyword,
		const CNativeW& filePath,
		const CNativeW& refName,
		std::vector<CHsp3::ParsedData>& data ) const;

	bool TagJumpSub(CEditView& Commander, const HspTagJumpInfo& SelectedItem) const;
	std::vector<std::wstring> SplitLines(const CNativeW& str) const;
	std::vector<CHsp3::ParsedData> ParseLines(const CNativeW& str) const;
	void ToLowerCase(wchar_t* str) const;

public:

	inline void ClearMenuIniItems()
	{
		m_MenuIniItems.clear();
	}

	inline std::vector<MenuItem> GetMenuIniItems() const
	{
		return m_MenuIniItems;
	}

	bool LoadMenuIniItems();

	bool ExecMenuIniItem(HWND hParent, const MenuItem& item) const;

public:
	bool ReservedKeywordList(HWND hParent) const;
	bool CompileRun(
		HWND hParent,
		const CNativeW& strHSPTmpFilePath,
		const CNativeW& strHSPFilePath,
		const CNativeW& strHSPObjFilePath,
		bool bForceErrorReportMode = false,
		bool bInputUtf8Mode = false,
		bool bSkipExecution = false,
		bool bReleaseMode = false,
		bool bMakePack = false
	) const;
	bool Run(
		HWND hParent,
		const CNativeW& strHSPObjFilePath) const;
	bool ShowErrorReport(HWND hParent) const;
	bool Make(HWND hParent) const;
	bool RunExternal(HWND hParent) const;

	bool CommandLineOption(HWND hParent) const;
	bool OpenSrcFolder(HWND hParent) const;
	bool OpenSrcFolder_File(HWND hParent, const CNativeW& strFilePath) const;
	bool RunAssist(HWND hParent) const;
	bool RunAssistAtOnce(HWND hParent) const;
	bool RunHSPTV(HWND hParent) const;
	bool CreateDPM(HWND hParent) const;
	bool CreatePackopt(HWND hParent) const;
	bool ConvertDishC(HWND hParent) const;
	bool OpenHGIMG4Tool(HWND hParent) const;
	bool OpenPaintTool(HWND hParent) const;
	bool OpenMapTool(HWND hParent) const;
	bool OpenHelpSourceEditor(HWND hParent) const;
	bool RunIconConverter(HWND hParent) const;
	bool RunHSP3Updater(HWND hParent) const;
	bool SearchKeyword(HWND hParent, const CNativeW& strKeyword) const;
	bool OpenPGManual(HWND hParent) const;
	bool OpenFuncRef(HWND hParent) const;
	bool OpenManualIndex(HWND hParent, bool bJapanese) const;
	bool OpenDocument(HWND hParent, CViewCommander& Commander, const CNativeW& strCommand) const;
	bool JumpDefinition(HWND hParent, CEditView& Commander,
		const CNativeW& strKeyword, const CNativeW& strFileName, const CNativeW& strRefName,
		const CNativeW& strFileDir, const CNativeW& strCommonDir) const;
	bool JumpAllReferences(HWND hParent, CEditView& Commander,
		const CNativeW& strKeyword, const CNativeW& strFileName, const CNativeW& strRefName,
		const CNativeW& strFileDir, const CNativeW& strCommonDir);

};