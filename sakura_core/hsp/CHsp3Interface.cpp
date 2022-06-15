#include "StdAfx.h"
#include "CHsp3Interface.h"
#include "_main/CProcess.h"
#include "doc/CEditDoc.h"
#include "window/CEditWnd.h"
#include "charset/CShiftJis.h"
#include "env/CSakuraEnvironment.h"
#include "mem/CMemory.h"
#include "mem/CNativeA.h"
#include "mem/CNativeW.h"
#include "_os/CClipboard.h"
#include "version.h"

std::map<HWND, CHsp3Interface*> CHsp3Interface::s_mapWindowInstance;

LRESULT CHsp3Interface::InterfaceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 管理中のウィンドウでない場合
	if ( s_mapWindowInstance.count( hWnd) == 0)
	{
		return DefWindowProc( hWnd, uMsg, wParam, lParam);
	}

	// ウィンドウに紐づけされた CHsp3Interface
	auto& thisInstance = s_mapWindowInstance[hWnd];



	return DefWindowProc( hWnd, uMsg, wParam, lParam);
}

bool CHsp3Interface::CreateInterfaceWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpszClassName = HSED_INTERFACE_NAME.data();
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = (WNDPROC)InterfaceProc;
	wcex.hCursor = nullptr;
	wcex.hIcon = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.hbrBackground = nullptr;
	wcex.style = 0;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIconSm = nullptr;

	::RegisterClassEx(&wcex);

	HWND hWnd = ::CreateWindow(
		HSED_INTERFACE_NAME.data(), HSED_INTERFACE_NAME.data(),
		0, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

	if ( hWnd == nullptr)
		return false;

	// インスタンス管理へ追加
	s_mapWindowInstance.emplace(hWnd, this);
	m_hWnd = hWnd;

	return true;
}

inline LRESULT CHsp3Interface::GetVersion(WPARAM wParam, LPARAM lParam, bool bUnicode) const
{
	switch (wParam)
	{
	case HGV_PUBLICVER:
		return HSED_PUBLIC_VER;

	case HGV_PRIVATEVER:
		return HSED_PRIVATE_VER;

	case HGV_HSPCMPVER:
		return GetHspCmpVersion((HANDLE)lParam, bUnicode);

	case HGV_FOOTYVER:
		return FOOTY_VER;

	case HGV_FOOTYBETAVER:
		return FOOTY_BETA;

	case HGV_SAKURA_VER_A:
		return VER_A;

	case HGV_SAKURA_VER_B:
		return VER_B;

	case HGV_SAKURA_VER_C:
		return VER_C;

	case HGV_SAKURA_VER_D:
		return VER_D;

	default:
		return -1;
	}
};

// hspcmp.dll のバージョンを取得
inline LRESULT CHsp3Interface::GetHspCmpVersion(HANDLE hPipe, bool bUnicode) const
{
	char szRefstr[4096];
	DWORD dwNumberOfBytesWritten;
	BOOL bRet;

	const auto& Hsp3 = CProcess::getInstance()->GetHsp3();
	if (!Hsp3.IsLoaded())
		return -1;
	Hsp3.Hsp3Dll()->hsc_ver()(0, 0, 0, (int)szRefstr);

	if (bUnicode)
	{
		// Shift_JIS -> UTF-16
		CNativeW szRefstrW;
		CMemory cmemSjis(szRefstr, 4096 - 1);
		CShiftJis::SJISToUnicode(cmemSjis, &szRefstrW);
		cmemSjis.Reset();

		bRet = WriteFile(hPipe, szRefstrW.GetStringPtr(),
			(szRefstrW.GetStringLength() + 1) * 2, &dwNumberOfBytesWritten, nullptr);
	}
	else
	{
		bRet = WriteFile(hPipe, szRefstr,
			strlen(szRefstr) + 1, &dwNumberOfBytesWritten, nullptr);
	}

	return bRet ? dwNumberOfBytesWritten : -1;
}

inline LRESULT CHsp3Interface::GetWindowHandle(WPARAM wParam, LPARAM lParam, bool bUnicode) const
{
	switch (wParam)
	{
	case HGW_MAIN:
		return (LRESULT)CProcess::getInstance()->GetMainWindow();

	case HGW_CLIENT:
		return (LRESULT)CEditWnd::getInstance()->GetHwnd();

	case HGW_TAB:
		return (LRESULT)CEditWnd::getInstance()->m_cTabWnd.GetHwnd();

	case HGW_FOOTY:
	case HGW_SAKURA_EDIT:
		return (LRESULT)GetEditWindowHandle((int)lParam);

	case HGW_TOOLBAR:
		return (LRESULT)CEditWnd::getInstance()->m_cToolbar.GetToolbarHwnd();

	case HGW_STATUSBAR:
		return (LRESULT)CEditWnd::getInstance()->m_cStatusBar.GetStatusHwnd();

	case HGW_SAKURA_TRAY:
		return (LRESULT)m_pShareData->m_sHandles.m_hwndTray;

	default:
		return (LRESULT)nullptr;
	}
}

inline HWND CHsp3Interface::GetEditWindowHandle(int nId) const
{
	EditNode* pEdit = GetEditNode(nId);
	if (pEdit == nullptr)
		return nullptr;
	return pEdit->GetHwnd();
}

inline EditNode* CHsp3Interface::GetEditNode(int nId) const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if (IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			if (iIndex == nId)
				return &m_pShareData->m_sNodes.m_pEditArr[i];
			iIndex++;
		}
	}

	return nullptr;
}

inline CEditWnd* CHsp3Interface::GetEditWindowByHWnd(HWND hWnd) const
{
	return (CEditWnd*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
}

inline CEditWnd* CHsp3Interface::GetEditWindowById(int nId) const
{
	auto hWnd = GetEditWindowHandle(nId);
	if ( hWnd == nullptr)
		return nullptr;
	return GetEditWindowByHWnd(hWnd);
}

inline LRESULT CHsp3Interface::GetFilePath(int nId, HANDLE hPipe, bool bUnicode) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if ( pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	const auto& szFilePath = pDoc->m_cDocFile.GetFilePath();

	LPCVOID buffer = szFilePath;
	DWORD nNumberOfBytesToWrite = sizeof(wchar_t) * wcslen(szFilePath);

	CNativeA destSjis;

	if (!bUnicode)
	{
		// UTF-16 -> Shift_JIS
		CShiftJis::UnicodeToSJIS( szFilePath, destSjis._GetMemory());
		buffer = destSjis.GetStringPtr();
		nNumberOfBytesToWrite = destSjis.GetStringLength();
	}

	DWORD dwNumberOfBytesWritten;
	BOOL bRet;
	bRet = WriteFile(hPipe, buffer, nNumberOfBytesToWrite, &dwNumberOfBytesWritten, nullptr);
	return bRet ? 0 : -1;
}

inline LRESULT CHsp3Interface::GetOpenFileCount() const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if (IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			iIndex++;
		}
	}

	return iIndex;
}

inline LRESULT CHsp3Interface::GetActiveId() const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if ( IsSakuraMainWindow( m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			const auto& pEditWnd = GetEditWindowById(iIndex);
			if ( pEditWnd != nullptr)
			{
				if ( pEditWnd->IsActiveApp())
				{
					return iIndex;
				}
			}

			iIndex++;
		}
	}

	return -1;
}

inline LRESULT CHsp3Interface::CanCopy(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	// テキストが選択されているかで判断
	const auto& pActiveView = pEditWnd->GetActiveView();
	const auto& pSelect = pActiveView.GetSelectionInfo();
	return pSelect.IsTextSelected() ? 1 : 0;
}

inline LRESULT CHsp3Interface::CanPaste() const
{
	return CClipboard::HasValidData() ? 1 : 0;
}

inline LRESULT CHsp3Interface::CanUndo(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsEnableUndo() ? 1 : 0;
}

inline LRESULT CHsp3Interface::CanRedo(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsEnableRedo() ? 1 : 0;
}

inline LRESULT CHsp3Interface::IsModified(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsModified() ? 1 : 0;
}

inline LRESULT CHsp3Interface::Copy(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if ( pEditWnd == nullptr)
		return -2;

	auto& pActiveView = pEditWnd->GetActiveView();

	/* テキストが選択されているときは、選択範囲のデータを取得 */
	CNativeW	cmemBuf;
	bool		bBeginBoxSelect = false;

	// 矩形選択？
	if ( !pActiveView.GetSelectionInfo().IsBoxSelecting())
	{
		bBeginBoxSelect = true;
	}

	/* 選択範囲のデータを取得 */
	/* 正常時はTRUE,範囲未選択の場合はFALSEを返す */
	const auto& bAddCRLFWhenCopy = GetDllShareData().m_Common.m_sEdit.m_bAddCRLFWhenCopy;
	if ( !pActiveView.GetSelectedData(
		&cmemBuf, FALSE, NULL, FALSE, bAddCRLFWhenCopy, EEolType::none))
	{	
		return -5;
	}

	/* クリップボードにデータcmemBufの内容を設定 */
	if ( !pActiveView.MySetClipboardData(
		cmemBuf.GetStringPtr(), cmemBuf.GetStringLength(), bBeginBoxSelect, FALSE))
	{
		return -3;
	}

	return 0;
}

inline LRESULT CHsp3Interface::Cut(int nId) const
{
	// 処理はコピーとほぼ同じ
	auto ret = Copy( nId);
	if (ret != 0)
		return ret;

	const auto& pEditWnd = GetEditWindowById(nId);
	if ( pEditWnd == nullptr)
		return -2;
	auto& pActiveView = pEditWnd->GetActiveView();

	/* カーソル位置または選択エリアを削除 */
	pActiveView.DeleteData(true);

	return 0;
}

inline LRESULT CHsp3Interface::Paste(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if ( pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	// （結果が取れないという難点）
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.Command_PASTE(0);

	return 0;
}

inline LRESULT CHsp3Interface::Undo(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	// （結果が取れないという難点）
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.Command_UNDO();

	return 0;
}

inline LRESULT CHsp3Interface::Redo(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	// （結果が取れないという難点）
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.Command_REDO();

	return 0;
}

inline LRESULT CHsp3Interface::Indent(int nId) const
{
	return -1;
}

inline LRESULT CHsp3Interface::UnIndent(int nId) const
{
	return -1;
}

inline LRESULT CHsp3Interface::SelectAll(int nId) const
{
	const auto& pEditWnd = GetEditWindowById(nId);
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	// （結果が取れないという難点）
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.Command_SELECTALL();

	return 0;
}