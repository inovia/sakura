#include "StdAfx.h"
#include "CHsp3Interface.h"
#include "_main/CProcess.h"
#include "_main/CControlTray.h"
#include "_main/CCommandLine.h"
#include "doc/CEditDoc.h"
#include "doc/CDocReader.h"
#include "cmd/CViewCommander.h"
#include "window/CEditWnd.h"
#include "charset/CShiftJis.h"
#include "env/CSakuraEnvironment.h"
#include "mem/CMemory.h"
#include "mem/CNativeA.h"
#include "mem/CNativeW.h"
#include "_os/CClipboard.h"
#include "version.h"
#include "charset/charcode.h"
#include "COpeBlk.h"
#include "sakura_rc.h"

std::map<HWND, CHsp3Interface*> CHsp3Interface::s_mapWindowInstance;

inline int CHsp3Interface::ReadPipe(HANDLE hPipe, char **pbuffer)
{
	DWORD dwSize, dwNumberOfBytesRead;
	char *lpBuffer;

	if (!::PeekNamedPipe(hPipe, nullptr, 0, nullptr, &dwSize, nullptr))
	{
		return 1;
	}

	lpBuffer = (char *)malloc(dwSize + 2);	// UTF-16も想定して多め
	*pbuffer = lpBuffer;
	if ( lpBuffer == nullptr) return 1;
	if ( dwSize > 0)
	{
		::ReadFile(hPipe, lpBuffer, dwSize, &dwNumberOfBytesRead, nullptr);
	}

	lpBuffer[dwSize] = '\0';
	lpBuffer[dwSize + 1] = '\0';			// UTF-16も想定して多め
	return 0;
}

// ウィンドウプロシージャー
LRESULT CHsp3Interface::InterfaceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 管理中のウィンドウでない場合
	if ( s_mapWindowInstance.count( hWnd) == 0)
	{
		return ::DefWindowProc( hWnd, uMsg, wParam, lParam);
	}

	// ウィンドウに紐づけされた CHsp3Interface
	auto& thisInstance = s_mapWindowInstance[hWnd];

	// コントロールプロセスへの通信は、
	// メッセージIDをチェックしてサブプロセスへ転送
	LRESULT result;
	if ( thisInstance->m_bControlProcess)
	{
		if ( InterfaceProc_ControlProcess( *thisInstance, result, hWnd, uMsg, wParam, lParam ))
		{
			return result;
		}
	}
	else
	{
		if ( InterfaceProc_SubProcess( *thisInstance, result, hWnd, uMsg, wParam, lParam))
		{
			return result;
		}
	}

	// 処理しなかったメッセージは既定の処理を実行
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam);
}

inline bool CHsp3Interface::InterfaceProc_ControlProcess(
	const CHsp3Interface& hspIf, LRESULT& result, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case HSED_GETVER:
		case HSED_GETVERW:
		{
			result = hspIf.GetVersion(wParam, lParam, (uMsg == HSED_GETVERW));
			return true;
		}
		case HSED_GETWND:
		{
			result = hspIf.GetWindowHandle(wParam, lParam);
			return true;
		}
		case HSED_GETPATH:
		case HSED_GETPATHW:
		{
			// パイプハンドル付け替え(孫プロセスに渡せないため)
			result = TransferMessageByIndex(
				hspIf, (int)wParam, uMsg, wParam, lParam, true);
			return true;
		}
		case HSED_SETTEXT:
		case HSED_SETTEXTW:
		case HSED_GETTEXT:
		case HSED_GETTEXTW:
		case HSED_SETSELTEXT:
		case HSED_SETSELTEXTW:
		{
			// パイプハンドル付け替え(孫プロセスに渡せないため)
			result = TransferMessageByHwnd(
				(HWND)wParam, uMsg, wParam, lParam, true);
			return true;
		}
		case HSED_GETTABCOUNT:
		{
			result = hspIf.GetOpenFileCount();
			return true;
		}
		case HSED_GETTABID:
		{
			// HWND -> nIndex
			result = hspIf.GetHsp3InterfaceWindowIndex((HWND)wParam);
			return true;
		}
		case HSED_GETFOOTYID:
		{
			// nIndex -> HWND
			result = (LRESULT)hspIf.GetHsp3InterfaceWindowHandle((int)wParam);
			return true;
		}
		case HSED_GETACTTABID:
		{
			HWND out_hWndHsp3If;
			result = (LRESULT)hspIf.GetLastActiveIndex(out_hWndHsp3If);
			return true;
		}
		case HSED_GETACTFOOTYID:
		{
			HWND out_hWndHsp3If;
			hspIf.GetLastActiveIndex(out_hWndHsp3If);
			result = (LRESULT)out_hWndHsp3If;
			return true;
		}
		case HSED_CANCOPY:
		case HSED_CANUNDO:
		case HSED_CANREDO:
		case HSED_GETMODIFY:
		case HSED_COPY:
		case HSED_CUT:
		case HSED_PASTE:
		case HSED_UNDO:
		case HSED_REDO:
		case HSED_INDENT:
		case HSED_UNINDENT:
		case HSED_SELECTALL:
		case HSED_GETTEXTLENGTH:
		case HSED_GETTEXTLENGTHW:
		case HSED_GETLINES:
		case HSED_GETLINELENGTH:
		case HSED_GETLINELENGTHW:
		case HSED_GETLINECODE:
		case HSED_GETCARETLINE:
		case HSED_GETCARETPOS:
		case HSED_SETCARETLINE:
		case HSED_GETCARETTHROUGH:
		case HSED_SETCARETTHROUGH:
		case HSED_GETCARETVPOS:
		case HSED_SETMARK:
		case HSED_GETMARK:
		{
			result = TransferMessageByHwnd(
				(HWND)wParam, uMsg, wParam, lParam, false);
			return true;
		}
		case HSED_CANPASTE:
		{
			result = (LRESULT)hspIf.CanPaste();
			return true;
		}
		case WM_COPYDATA:	// 旧ＨＳＰスクリプトエディタからの連携用
		{
			hspIf.OpenPage(wParam, lParam);
			result = 0;
			return true;
		}
	}

	return false;
}

inline bool CHsp3Interface::InterfaceProc_SubProcess(
	const CHsp3Interface& hspIf, LRESULT& result, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case HSED_GETPATH:
		case HSED_GETPATHW:
		{
			result = hspIf.GetFilePath((HANDLE)lParam, (uMsg == HSED_GETPATHW));
			return true;
		}
		case HSED_CANCOPY:
		{
			result = hspIf.CanCopy();
			return true;
		}
		case HSED_CANUNDO:
		{
			result = hspIf.CanUndo();
			return true;
		}
		case HSED_CANREDO:
		{
			result = hspIf.CanRedo();
			return true;
		}
		case HSED_GETMODIFY:
		{
			result = hspIf.IsModified();
			return true;
		}
		case HSED_COPY:
		{
			result = hspIf.Copy();
			return true;
		}
		case HSED_CUT:
		{
			result = hspIf.Cut();
			return true;
		}
		case HSED_PASTE:
		{
			result = hspIf.Paste();
			return true;
		}
		case HSED_UNDO:
		{
			result = hspIf.Undo();
			return true;
		}
		case HSED_REDO:
		{
			result = hspIf.Redo();
			return true;
		}
		case HSED_INDENT:
		{
			result = hspIf.Indent();
			return true;
		}
		case HSED_UNINDENT:
		{
			result = hspIf.UnIndent();
			return true;
		}
		case HSED_SELECTALL:
		{
			result = hspIf.SelectAll();
			return true;
		}
		case HSED_SETTEXT:
		case HSED_SETTEXTW:
		{
			result = hspIf.SetAllText((HANDLE)lParam, (uMsg == HSED_SETTEXTW));
			return true;
		}
		case HSED_GETTEXT:
		case HSED_GETTEXTW:
		{
			result = hspIf.GetAllText((HANDLE)lParam, (uMsg == HSED_GETTEXTW));
			return true;
		}
		case HSED_GETTEXTLENGTH:
		case HSED_GETTEXTLENGTHW:
		{
			result = hspIf.GetAllTextLength((uMsg == HSED_GETTEXTLENGTHW));
			return true;
		}
		case HSED_GETLINES:
		{
			result = hspIf.GetLineCount();
			return true;
		}
		case HSED_SETSELTEXT:
		case HSED_SETSELTEXTW:
		{
			result = hspIf.InsertText((HANDLE)lParam, (uMsg == HSED_SETSELTEXTW));
			return true;
		}
		case HSED_GETLINELENGTH:
		case HSED_GETLINELENGTHW:
		{
			result = hspIf.GetLineLength((int)lParam, (uMsg == HSED_GETLINELENGTHW));
			return true;
		}
		case HSED_GETLINECODE:
		{
			result = hspIf.GetNewLineMode();
			return true;
		}
		case HSED_GETCARETLINE:
		{
			result = hspIf.GetCaretLine();
			return true;
		}
		case HSED_GETCARETPOS:
		{
			result = hspIf.GetCaretPos();
			return true;
		}
		case HSED_SETCARETLINE:
		{
			result = hspIf.SetCaretLine((int)lParam);
			return true;
		}
		case HSED_SETCARETPOS:
		{
			result = hspIf.SetCaretPos((int)lParam);
			return true;
		}
		case HSED_GETCARETTHROUGH:
		{
			result = hspIf.GetCaretLineThrough();
			return true;
		}
		case HSED_SETCARETTHROUGH:
		{
			result = hspIf.SetCaretLineThrough((int)lParam);
			return true;
		}
		case HSED_GETCARETVPOS:
		{
			result = hspIf.GetCaretVPos((int)lParam);
			return true;
		}
		case HSED_SETMARK:
		{
			result = hspIf.SetMark((int)lParam);
			return true;
		}
		case HSED_GETMARK:
		{
			result = hspIf.GetMark((int)lParam);
			return true;
		}
		case IS_ACTIVEAPP:
		{
			result = hspIf.IsActive();
			return true;
		}
	}

	return false;
}

inline LRESULT CHsp3Interface::TransferMessageByIndex(
	const CHsp3Interface& hspIf, int nIndex, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bPipeSwap)
{
	const auto hWndHsp3If = hspIf.GetHsp3InterfaceWindowHandle( nIndex);
	if ( hWndHsp3If == nullptr)
	{
		return -2;
	}
	return TransferMessageByHwnd( hWndHsp3If, uMsg, wParam, lParam, bPipeSwap);
}

inline LRESULT CHsp3Interface::TransferMessageByHwnd(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bPipeSwap)
{
	// パイプハンドル付け替え(孫プロセスに渡せないため)
	if ( bPipeSwap)
	{
		HANDLE hPipeOrigin = (HANDLE)lParam;
		DWORD dwProcessId = 0;
		::GetWindowThreadProcessId(hWnd, &dwProcessId);

		HANDLE hProcTarget =
			::OpenProcess( PROCESS_ALL_ACCESS, 0, dwProcessId);
		HANDLE hProcCurrent = ::GetCurrentProcess();

		HANDLE hPipeTarget = nullptr;
		::DuplicateHandle(
			hProcCurrent, hPipeOrigin, hProcTarget,
			&hPipeTarget, 0, 0, DUPLICATE_SAME_ACCESS);

		::CloseHandle((HANDLE)hProcTarget);

		lParam = (LPARAM)hPipeTarget;
	}

	LRESULT ret = ::SendMessage(hWnd, uMsg, wParam, lParam);

	if ( bPipeSwap)
	{
		
	}

	return ret;
}

bool CHsp3Interface::CreateInterfaceWindow(HINSTANCE hInstance, bool bControlProcess)
{
	// ウィンドウクラス生成
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	if ( bControlProcess)
	{
		wcex.lpszClassName = HSED_INTERFACE_MAIN_NAME.data();
	}
	else
	{
		wcex.lpszClassName = HSED_INTERFACE_SUB_NAME.data();
	}
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

	// ウィンドウ生成
	HWND hWnd = ::CreateWindow(
		(bControlProcess) ? HSED_INTERFACE_MAIN_NAME.data() : HSED_INTERFACE_SUB_NAME.data(),
		(bControlProcess) ? HSED_INTERFACE_MAIN_NAME.data() : HSED_INTERFACE_SUB_NAME.data(),
		0, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

	if ( hWnd == nullptr)
		return false;

	// 旧エディタ連携用
	if ( bControlProcess)
	{
		::CreateMutex( nullptr, TRUE, HSED_INTERFACE_MUTEX_NAME.data());
		::SetProp( hWnd, HSED_INTERFACE_PROP_NAME.data(), (HANDLE)1);
	}

	// インスタンス管理へ追加
	s_mapWindowInstance.emplace(hWnd, this);
	m_hWnd = hWnd;
	m_hInstance = hInstance;
	m_bControlProcess = bControlProcess;
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

inline LRESULT CHsp3Interface::GetWindowHandle(WPARAM wParam, LPARAM lParam) const
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

inline LRESULT CHsp3Interface::OpenPage(WPARAM wParam, LPARAM lParam) const
{
	// 旧ＨＳＰスクリプトエディタ用連携
	COPYDATASTRUCT *pcds = (COPYDATASTRUCT*)lParam;
	if ( pcds->dwData == 0)
	{
		// ファイルパスはSJIS固定の想定
		// Shift_JIS -> UTF-16
		CNativeA strA = (char *)pcds->lpData;
		CNativeW strW;
		CShiftJis::SJISToUnicode(*strA._GetMemory(), &strW);

		// コマンドライン分解
		auto cmdLine = CCommandLine();
		cmdLine.ParseCommandLine( strW.GetStringPtr(), false);

		// ファイルが指定されていれば実行
		if ( 0 < cmdLine.GetFileNum())
		{
			// 指定されたファイルを開く
			SLoadInfo sLoadInfo;
			sLoadInfo.cFilePath = cmdLine.GetFileName(0);
			sLoadInfo.eCharCode = CODE_AUTODETECT;	// 文字コード自動判別
			sLoadInfo.bViewMode = false;
		
			// 新たな編集ウィンドウを起動
			CControlTray::OpenNewEditor(
				m_hInstance, m_pShareData->m_sHandles.m_hwndTray,
				sLoadInfo, nullptr, true, nullptr,
				m_pShareData->m_Common.m_sTabBar.m_bNewWindow ? true : false);
		}
	}

	return 0;			// WM_COPYDATA の戻り値
}

inline HWND CHsp3Interface::GetEditWindowHandle(int nIndex) const
{
	EditNode* pEdit = GetEditNode(nIndex);
	if (pEdit == nullptr)
		return nullptr;
	return pEdit->GetHwnd();
}

inline int CHsp3Interface::GetHsp3InterfaceWindowIndex(HWND hWnd) const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if ( IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			if ( m_pShareData->m_sNodes.m_pEditArr[i].m_hWndHspIf == hWnd)
			{
				return iIndex;
			}
			iIndex++;
		}
	}

	return -1;
}

inline HWND CHsp3Interface::GetHsp3InterfaceWindowHandle(int nIndex) const
{
	EditNode* pEdit = GetEditNode(nIndex);
	if (pEdit == nullptr)
		return nullptr;
	return pEdit->GetHwndHspIf();
}

inline EditNode* CHsp3Interface::GetEditNode(int nIndex) const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if (IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			if (iIndex == nIndex)
				return &m_pShareData->m_sNodes.m_pEditArr[i];
			iIndex++;
		}
	}

	return nullptr;
}

inline LRESULT CHsp3Interface::GetFilePath(HANDLE hPipe, bool bUnicode) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
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

inline LRESULT CHsp3Interface::GetActiveIndex(HWND& out_hWndHsp3If) const
{
	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if ( IsSakuraMainWindow( m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			const auto& hWmdHspIf = m_pShareData->m_sNodes.m_pEditArr[i].m_hWndHspIf;
			auto ret = (int)SendMessage( hWmdHspIf, IS_ACTIVEAPP, 0, 0);
			if ( ret == 1)
			{
				out_hWndHsp3If = hWmdHspIf;
				return iIndex;
			}
			iIndex++;
		}
	}

	out_hWndHsp3If = nullptr;
	return -1;
}

inline LRESULT CHsp3Interface::GetLastActiveIndex(HWND& out_hWndHsp3If) const
{
	// 最近
	EditNode*	pEditNode = CAppNodeGroupHandle(0).GetEditNodeAt(0);
	if ( pEditNode == nullptr)
		return -2;

	out_hWndHsp3If = pEditNode->GetHwndHspIf();

	int	i;
	int iIndex;

	iIndex = 0;
	for (i = 0; i < m_pShareData->m_sNodes.m_nEditArrNum; i++)
	{
		if (IsSakuraMainWindow(m_pShareData->m_sNodes.m_pEditArr[i].m_hWnd))
		{
			const auto& hWmdHspIf = m_pShareData->m_sNodes.m_pEditArr[i].m_hWndHspIf;
			if ( out_hWndHsp3If == hWmdHspIf)
			{
				return iIndex;
			}
			iIndex++;
		}
	}

	out_hWndHsp3If = nullptr;
	return -1;
}

inline LRESULT CHsp3Interface::IsActive() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if ( pEditWnd == nullptr)
		return -2;
	return pEditWnd->IsActiveApp() ? 1 : 0;
}

inline LRESULT CHsp3Interface::CanCopy() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
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

inline LRESULT CHsp3Interface::CanUndo() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsEnableUndo() ? 1 : 0;
}

inline LRESULT CHsp3Interface::CanRedo() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsEnableRedo() ? 1 : 0;
}

inline LRESULT CHsp3Interface::IsModified() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocEditor.IsModified() ? 1 : 0;
}

inline LRESULT CHsp3Interface::Copy() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
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

inline LRESULT CHsp3Interface::Cut() const
{
	// 処理はコピーとほぼ同じ
	auto ret = Copy();
	if (ret != 0)
		return ret;

	const auto& pEditWnd = CEditWnd::getInstance();
	if ( pEditWnd == nullptr)
		return -2;
	auto& pActiveView = pEditWnd->GetActiveView();

	/* カーソル位置または選択エリアを削除 */
	pActiveView.DeleteData(true);

	return 0;
}

inline LRESULT CHsp3Interface::Paste() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if ( pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(
		F_PASTE, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_PASTE(0);

	return 0;
}

inline LRESULT CHsp3Interface::Undo() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_UNDO, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	//pCommander.Command_UNDO();

	return 0;
}

inline LRESULT CHsp3Interface::Redo() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_REDO, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_REDO();

	return 0;
}

inline LRESULT CHsp3Interface::Indent() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_INDENT_TAB, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_INDENT( WCODE::TAB, CViewCommander::INDENT_TAB);

	return 0;
}

inline LRESULT CHsp3Interface::UnIndent() const
{ 
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_UNINDENT_TAB, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_UNINDENT( WCODE::TAB);

	return 0;
}

inline LRESULT CHsp3Interface::SelectAll() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_SELECTALL, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_SELECTALL();

	return 0;
}

inline LRESULT CHsp3Interface::SetAllText(HANDLE hPipe, bool bUnicode) const
{
	// セットするテキスト
	CNativeW bufW;

	// パイプから文字列を取得
	{
		char *lpBuffer;
		if ( ReadPipe( hPipe, &lpBuffer))
		{
			return -3;
		}

		// 文字コード変換が必要であれば行う
	
		if ( !bUnicode)
		{
			// Shift_JIS -> UTF-16
			CNativeA bufA = lpBuffer;
			CShiftJis::SJISToUnicode(*(bufA._GetMemory()), &bufW);
		}
		else
		{
			// UTF-16 なのでそのままキャストして良い
			bufW = (wchar_t*)lpBuffer;
		}

		free(lpBuffer);
	}

	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_SELECTALL, true, 0, 0, 0, 0);
	pCommander.HandleCommand(F_DELETE, true, 0, 0, 0, 0);
	pCommander.HandleCommand(F_ADDTAIL_W, true, (LPARAM)bufW.GetStringPtr(), -1, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_SELECTALL();
	// pCommander.Command_DELETE();
	// pCommander.Command_ADDTAIL( bufW.GetStringPtr(), -1);

	return 0;
}

inline LRESULT CHsp3Interface::GetAllText(HANDLE hPipe, bool bUnicode) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	wchar_t*	pDataAll;
	int		nDataAllLen;

	const auto& pDoc = pEditWnd->GetDocument();
	pDataAll = CDocReader(pDoc->m_cDocLineMgr).GetAllData(&nDataAllLen);

	LPCVOID buffer = pDataAll;
	DWORD nNumberOfBytesToWrite = sizeof(wchar_t) * wcslen(pDataAll);

	CNativeA destSjis;

	if (!bUnicode)
	{
		// UTF-16 -> Shift_JIS
		CShiftJis::UnicodeToSJIS(pDataAll, destSjis._GetMemory());
		buffer = destSjis.GetStringPtr();
		nNumberOfBytesToWrite = destSjis.GetStringLength();
	}

	DWORD dwNumberOfBytesWritten;
	BOOL bRet;
	bRet = WriteFile(hPipe, buffer, nNumberOfBytesToWrite, &dwNumberOfBytesWritten, nullptr);

	free(pDataAll);

	return bRet ? 0 : -3;
}

inline LRESULT CHsp3Interface::GetAllTextLength(bool bUnicode) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	wchar_t*	pDataAll;
	int		nDataAllLen;

	const auto& pDoc = pEditWnd->GetDocument();
	pDataAll = CDocReader(pDoc->m_cDocLineMgr).GetAllData(&nDataAllLen);

	LPCVOID buffer = pDataAll;
	DWORD nStringLength = wcslen(pDataAll);

	CNativeA destSjis;

	if (!bUnicode)
	{
		// UTF-16 -> Shift_JIS
		CShiftJis::UnicodeToSJIS(pDataAll, destSjis._GetMemory());
		buffer = destSjis.GetStringPtr();
		nStringLength = destSjis.GetStringLength();
	}

	free(pDataAll);
	return nStringLength;
}

inline LRESULT CHsp3Interface::GetLineCount() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if ( pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	return pDoc->m_cDocLineMgr.GetLineCount();
}

inline LRESULT CHsp3Interface::InsertText(HANDLE hPipe, bool bUnicode) const
{
	// セットするテキスト
	CNativeW bufW;

	// パイプから文字列を取得
	{
		char *lpBuffer;
		if (ReadPipe(hPipe, &lpBuffer))
		{
			return -3;
		}

		// 文字コード変換が必要であれば行う
		if (!bUnicode)
		{
			// Shift_JIS -> UTF-16
			CNativeA bufA = lpBuffer;
			CShiftJis::SJISToUnicode(*(bufA._GetMemory()), &bufW);
		}
		else
		{
			// UTF-16 なのでそのままキャストして良い
			bufW = (wchar_t*)lpBuffer;
		}

		free(lpBuffer);
	}

	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();

	pCommander.HandleCommand(
		F_INSTEXT_W, true,
		(LPARAM)bufW.GetStringPtr(), (LPARAM)bufW.GetStringLength(), TRUE, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_INSTEXT(true, bufW.GetStringPtr(), bufW.GetStringLength(), TRUE);
	return 0;
}

inline LRESULT CHsp3Interface::GetLineLength(int nLineNo, bool bUnicode) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const wchar_t*	pDataLine;
	CLogicInt	nDataLineLen;

	const auto& pDoc = pEditWnd->GetDocument();
	pDataLine = CDocReader(pDoc->m_cDocLineMgr).GetLineStr(CLogicInt(nLineNo - 1), &nDataLineLen);

	LPCVOID buffer = pDataLine;
	DWORD nStringLength = wcslen(pDataLine);

	CNativeA destSjis;

	if (!bUnicode)
	{
		// UTF-16 -> Shift_JIS
		CShiftJis::UnicodeToSJIS(pDataLine, destSjis._GetMemory());
		buffer = destSjis.GetStringPtr();
		nStringLength = destSjis.GetStringLength();
	}

	return nStringLength;
}

inline LRESULT CHsp3Interface::GetNewLineMode() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	const auto& eol = pDoc->m_cDocEditor.GetNewLineCode();
	switch (eol.GetType())
	{
		case EEolType::cr_and_lf:
			return 1;
		case EEolType::carriage_return:
			return 2;
		case EEolType::line_feed:
			return 3;
	}
	return 4;
}

inline LRESULT CHsp3Interface::GetCaretLine() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	return pCommander.GetCaret().GetCaretLogicPos().GetY() + 1;
}

inline LRESULT CHsp3Interface::GetCaretPos() const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	return pCommander.GetCaret().GetCaretLogicPos().GetX() + 1;
}

inline LRESULT CHsp3Interface::SetCaretLine(int nLineNo) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 折り返し考慮しない
	auto bkLineMode = m_pShareData->m_bLineNumIsCRLF_ForJump;
	m_pShareData->m_bLineNumIsCRLF_ForJump = true;

	// 行番号設定
	auto bkLineNo = pEditWnd->m_cDlgJump.m_nLineNum;
	pEditWnd->m_cDlgJump.m_nLineNum = nLineNo;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_JUMP, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_JUMP();

	m_pShareData->m_bLineNumIsCRLF_ForJump = bkLineMode;	// 復帰
	pEditWnd->m_cDlgJump.m_nLineNum = bkLineNo;				// 復帰
	return 0;
}

inline LRESULT CHsp3Interface::SetCaretPos(int nPosNo) const
{
	return 0;
}

inline LRESULT CHsp3Interface::GetCaretLineThrough() const
{
	// 折り返し対応
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();

#ifdef _DEBUG
	return pCommander.GetCaret().GetCaretLayoutPos().GetX().GetValue() + 1;
#else
	return pCommander.GetCaret().GetCaretLayoutPos().GetX() + 1;
#endif
	
}

inline LRESULT CHsp3Interface::SetCaretLineThrough(int nLineNo) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	// 折り返し考慮する
	auto bkLineMode = m_pShareData->m_bLineNumIsCRLF_ForJump;
	m_pShareData->m_bLineNumIsCRLF_ForJump = false;

	// 行番号設定
	auto bkLineNo = pEditWnd->m_cDlgJump.m_nLineNum;
	pEditWnd->m_cDlgJump.m_nLineNum = nLineNo;

	// 処理が複雑すぎるので、コマンダーにそのまま投げる
	
	auto& pActiveView = pEditWnd->GetActiveView();
	auto& pCommander = pActiveView.GetCommander();
	pCommander.HandleCommand(F_JUMP, true, 0, 0, 0, 0);

	// ↓直接呼んではいけない
	// pCommander.Command_JUMP();

	m_pShareData->m_bLineNumIsCRLF_ForJump = bkLineMode;	// 復帰
	pEditWnd->m_cDlgJump.m_nLineNum = bkLineNo;				// 復帰
	return 0;
}

inline LRESULT CHsp3Interface::GetCaretVPos(int nPosNo) const
{
	return 0;
}

inline LRESULT CHsp3Interface::SetMark(int nPosNo) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	const auto& pCDocLine = pDoc->m_cDocLineMgr.GetLine(CLogicInt(nPosNo - 1));
	if (pCDocLine == nullptr)
		return -1;

	CBookmarkSetter cBookmark(pCDocLine);
	cBookmark.SetBookmark(!cBookmark.IsBookmarked());		// トグル動作

	// 分割したビューも更新
	pEditWnd->Views_Redraw();
	return 0;
}

inline LRESULT CHsp3Interface::GetMark(int nPosNo) const
{
	const auto& pEditWnd = CEditWnd::getInstance();
	if (pEditWnd == nullptr)
		return -2;

	const auto& pDoc = pEditWnd->GetDocument();
	const auto& pCDocLine = pDoc->m_cDocLineMgr.GetLine(CLogicInt(nPosNo - 1));
	if (pCDocLine == nullptr)
		return -1;

	CBookmarkSetter cBookmark(pCDocLine);
	return cBookmark.IsBookmarked() ? 1 : 0;
}