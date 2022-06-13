#pragma once
#include <windows.h>
#include "env/DLLSHAREDATA.h"

typedef BOOL(CALLBACK *HSPDLLFUNC)(int, int, int, int);

class CHsp3Interface
{

public:

	// hsed public version
	const static int HSED_PUBLIC_VER = 0x3702;

	// hsed private version
	const static int HSED_PRIVATE_VER = 0x20000;

	// Footyのバージョン×1000（※互換用）
	const static int FOOTY_VER = 2017;
	const static int FOOTY_BETA = 0;

	// ウィンドウ名、クラス名
	const static constexpr std::wstring_view HSED_INTERFACE_NAME{ L"HspEditorInterface" };

	// Messages
	const static int UNICODE_VER				= 0x1000;
	const static int HSED_GETVER				= (WM_APP + 0x000);
	const static int HSED_GETWND				= (WM_APP + 0x100);
	const static int HSED_GETPATH				= (WM_APP + 0x101);

	const static int HSED_GETTABCOUNT			= (WM_APP + 0x200);
	const static int HSED_GETTABID				= (WM_APP + 0x201);
	const static int HSED_GETFOOTYID			= (WM_APP + 0x202);
	const static int HSED_GETACTTABID			= (WM_APP + 0x203);
	const static int HSED_GETACTFOOTYID			= (WM_APP + 0x204);

	const static int HSED_CANCOPY				= (WM_APP + 0x300);
	const static int HSED_CANPASTE				= (WM_APP + 0x301);
	const static int HSED_CANUNDO				= (WM_APP + 0x302);
	const static int HSED_CANREDO				= (WM_APP + 0x303);
	const static int HSED_GETMODIFY				= (WM_APP + 0x304);

	const static int HSED_COPY					= (WM_APP + 0x310);
	const static int HSED_CUT					= (WM_APP + 0x311);
	const static int HSED_PASTE					= (WM_APP + 0x312);
	const static int HSED_UNDO					= (WM_APP + 0x313);
	const static int HSED_REDO					= (WM_APP + 0x314);
	const static int HSED_INDENT				= (WM_APP + 0x315);
	const static int HSED_UNINDENT				= (WM_APP + 0x316);
	const static int HSED_SELECTALL				= (WM_APP + 0x317);

	const static int HSED_SETTEXT				= (WM_APP + 0x320);
	const static int HSED_GETTEXT				= (WM_APP + 0x321);
	const static int HSED_GETTEXTLENGTH			= (WM_APP + 0x322);
	const static int HSED_GETLINES				= (WM_APP + 0x323);
	const static int HSED_SETSELTEXT			= (WM_APP + 0x324);
	const static int HSED_GETSELTEXT			= (WM_APP + 0x325);
	const static int HSED_GETLINETEXT			= (WM_APP + 0x326);
	const static int HSED_GETLINELENGTH			= (WM_APP + 0x327);
	const static int HSED_GETLINECODE			= (WM_APP + 0x328);

	const static int HSED_SETSELA				= (WM_APP + 0x330);
	const static int HSED_SETSELB				= (WM_APP + 0x331);
	const static int HSED_GETSELA				= (WM_APP + 0x332);
	const static int HSED_GETSELB				= (WM_APP + 0x333);

	const static int HSED_GETCARETLINE			= (WM_APP + 0x340);
	const static int HSED_GETCARETPOS			= (WM_APP + 0x341);
	const static int HSED_GETCARETTHROUGH		= (WM_APP + 0x342);
	const static int HSED_GETCARETVPOS			= (WM_APP + 0x343);
	const static int HSED_SETCARETLINE			= (WM_APP + 0x344);
	const static int HSED_SETCARETPOS			= (WM_APP + 0x345);
	const static int HSED_SETCARETTHROUGH		= (WM_APP + 0x346);

	const static int HSED_SETMARK				= (WM_APP + 0x350);
	const static int HSED_GETMARK				= (WM_APP + 0x351);
	const static int HSED_SETHIGHLIGHT			= (WM_APP + 0x352);

	// Constants for HSED_GETVER
	const static int HGV_PUBLICVER		= 0;
	const static int HGV_PRIVATEVER		= 1;
	const static int HGV_HSPCMPVER		= 2;
	const static int HGV_FOOTYVER		= 3;
	const static int HGV_FOOTYBETAVER	= 4;
	const static int HGV_SAKURA_VER_A	= 10;
	const static int HGV_SAKURA_VER_B	= 11;
	const static int HGV_SAKURA_VER_C	= 12;
	const static int HGV_SAKURA_VER_D	= 13;

	// Constants for HSED_GETWND
	const static int HGW_MAIN			= 0;
	const static int HGW_CLIENT			= 1;
	const static int HGW_TAB			= 2;
	const static int HGW_FOOTY			= 3;
	const static int HGW_TOOLBAR		= 4;
	const static int HGW_STATUSBAR		= 5;
	const static int HGW_SAKURA_TRAY	= 10;
	const static int HGW_SAKURA_EDIT	= 11;

private:

	// hsedsdk用ウィンドウハンドル
	HWND m_hWnd						= nullptr;

	// サクラエディタ共有データ
	DLLSHAREDATA* m_pShareData		= nullptr;

public:
	// デフォルトコンストラクタ
	CHsp3Interface()
	{
		/* 共有データ構造体のアドレスを返す */
		m_pShareData = &GetDllShareData();
	};

	// デストラクタ
	~CHsp3Interface() { };

public:

	inline LRESULT GetVersion(WPARAM wParam, LPARAM lParam, bool bUnicode) const;
	inline LRESULT GetHspCmpVersion(HANDLE hPipe, bool bUnicode) const;
	inline LRESULT GetWindowHandle(WPARAM wParam, LPARAM lParam, bool bUnicode) const;
	inline HWND GetEditWindowHandle(int nId) const;
	inline EditNode* GetEditNode(int nId) const;

	//inline CEditView* GetEditView(EditNode* pEditNode)
	//{
	//	// 編集ウィンドウ（外枠）
	//	// CEditWnd

	//	// CEditWnd -> CEditDoc	

	//	// 文書ウィンドウの管理
	//	// CEditView (スプリッターとかある内部)

	//	// pEditNode->m_hWnd;
	//	
	//	// CEditWnd::getInstance()->GetHwnd();

	//	
	//}

};
