#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <dwmapi.h>

#include "win32-custom-menubar-aero-theme/UAHMenuBar.h"

class CHsp3DarkMode
{
public:
	enum PreferredAppMode : int
	{
		APPMODE_DEFAULT = 0,
		APPMODE_ALLOWDARK = 1,
		APPMODE_FORCEDARK = 2,
		APPMODE_FORCELIGHT = 3,
		APPMODE_MAX = 4
	};

private:
	typedef BOOL(CALLBACK *F_AllowDarkModeForApp)(BOOL);
	typedef PreferredAppMode (CALLBACK *F_SetPreferredAppMode)(PreferredAppMode);
	typedef void (CALLBACK *F_RefreshImmersiveColorPolicyState)();
	typedef void (CALLBACK *F_FlushMenuThemes)();
	typedef BOOL (CALLBACK *F_ShouldSystemUseDarkMode)();
	typedef BOOL (CALLBACK *F_AllowDarkModeForWindow)(HWND, BOOL);

	static constexpr std::wstring_view UXTHEME_DLL = { L"uxtheme.dll" };
	static const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
	static const int DWMWA_USE_IMMERSIVE_DARK_MODE_19 = 19;

private:
	bool								m_bSysDarkMode						= false;
	HINSTANCE							m_hDLL								= nullptr;
	HFONT								m_hSysFont							= nullptr;

	F_AllowDarkModeForApp				m_pAllowDarkModeForApp				= nullptr;
	F_SetPreferredAppMode				m_pSetPreferredAppMode				= nullptr;

	F_RefreshImmersiveColorPolicyState	m_pRefreshImmersiveColorPolicyState = nullptr;
	F_FlushMenuThemes					m_pFlushMenuThemes					= nullptr;

	F_ShouldSystemUseDarkMode			m_pShouldSystemUseDarkMode			= nullptr;
	F_AllowDarkModeForWindow 			m_pAllowDarkModeForWindow			= nullptr;

	std::map<int, HBRUSH>		m_mapBrush					= std::map<int, HBRUSH>();

protected:
	// デフォルトコンストラクタ
	CHsp3DarkMode()
	{
		ShouldSystemUseDarkMode();

		//// 現在のDPIを取得
		//HDC hdcScreen = ::GetDC(NULL);
		//int currentDPI = ::GetDeviceCaps(hdcScreen, LOGPIXELSY);
		//::ReleaseDC(nullptr, hdcScreen);

		//// デフォルトのDPI (通常は96 DPI) に対する現在のDPIの比率を計算
		//float scalingFactor = (float)currentDPI / 96.0f;

		// システムフォント取得
		LOGFONT logfont;
		if ( ::SystemParametersInfo(
			SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &logfont, 0))
		{
			//// DPI値でスケーリング
			//logfont.lfHeight = (LONG)(logfont.lfHeight * scalingFactor);
			m_hSysFont = ::CreateFontIndirect(&logfont);
		}
	};

	// デストラクタ
	~CHsp3DarkMode()
	{
		for (auto& p : m_mapBrush)
		{
			if ( p.second)
				::DeleteObject( p.second);
		}

		if ( m_hSysFont)
			::DeleteObject( m_hSysFont);
	};

private:
	void operator=(const CHsp3DarkMode& obj) {}		// 代入演算子禁止
	CHsp3DarkMode(const CHsp3DarkMode &obj) {}		// コピーコンストラクタ禁止

public:

	static CHsp3DarkMode s_Instance;
	static CHsp3DarkMode& GetInstance()
	{
		return s_Instance;
	}

public:

	inline bool IsSystemUseDarkMode() const
	{
		return m_bSysDarkMode;
	}

	inline HFONT GetSystemFont() const
	{
		return m_hSysFont;
	}

	bool Load();

	inline bool AllowDarkModeForApp(bool bAllow) const
	{
		if (!m_pAllowDarkModeForApp)
			return false;
		return m_pAllowDarkModeForApp(bAllow);
	}

	inline PreferredAppMode SetPreferredAppMode(PreferredAppMode mode) const
	{
		if (!m_pSetPreferredAppMode)
			return PreferredAppMode::APPMODE_DEFAULT;
		return m_pSetPreferredAppMode(mode);
	}

	inline bool RefreshImmersiveColorPolicyState() const
	{
		if (!m_pSetPreferredAppMode)
			return false;
		m_pRefreshImmersiveColorPolicyState();
		return true;
	}

	inline bool ShouldSystemUseDarkMode()
	{
		if (!m_pShouldSystemUseDarkMode)
			return false;
		m_bSysDarkMode = m_pShouldSystemUseDarkMode();
		return m_bSysDarkMode;
	}

	inline bool AllowDarkModeForWindow(HWND hWnd, bool bAllow) const
	{
		if (!m_pAllowDarkModeForWindow)
			return false;
		return m_pAllowDarkModeForWindow(hWnd, bAllow);
	}

	inline bool SetDarkModeHwnd(HWND hWnd)const
	{
		if (! m_bSysDarkMode)
			return false;

		bool bRet = ( ::SetWindowTheme(
			hWnd, L"DarkMode_Explorer", nullptr) == S_OK);

		if (!bRet)
		{
			bRet = ( ::SetWindowTheme(
				hWnd, L"Explorer", nullptr) == S_OK);
		}
		return bRet;
	}

	inline bool SetDarkModeHwnd_ReBar(HWND hWnd)const
	{
		if (!m_bSysDarkMode)
			return false;

		return (::SetWindowTheme(
			hWnd, L"DarkModeNavBar", nullptr) == S_OK);
	}

	inline bool SetDarkModeTitleBar(HWND hWnd) const
	{
		if (!m_bSysDarkMode)
			return false;

		BOOL value = TRUE;
		return (::DwmSetWindowAttribute(
			hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value)) == S_OK);
	}

	inline bool SetDarkModeTitleBar19(HWND hWnd) const
	{
		if (!m_bSysDarkMode)
			return false;

		BOOL value = TRUE;
		return (::DwmSetWindowAttribute(
			hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_19, &value, sizeof(value)) == S_OK);
	}

	inline bool FlushMenuThemes() const
	{
		if (!m_pFlushMenuThemes)
			return false;
		m_pFlushMenuThemes();
		return true;
	}

	inline DWORD GetSysColor(int nIndex) const
	{
		if (!m_bSysDarkMode)
		{
			return ::GetSysColor(nIndex);
		}

		switch (nIndex)
		{
			case COLOR_SCROLLBAR:
				return 0;
			case COLOR_BACKGROUND:
				return 0;
			case COLOR_ACTIVECAPTION:
				return 0;
			case COLOR_INACTIVECAPTION:
				return RGB(48,48,48);
			case COLOR_MENU:
				return RGB(48,48,48);
			case COLOR_WINDOW:
				return 0;
			case COLOR_WINDOWFRAME:
				return 0;
			case COLOR_MENUTEXT:
				return RGB(255, 255, 255);
			case COLOR_WINDOWTEXT:
				return RGB(255, 255, 255);
			case COLOR_CAPTIONTEXT:
				return RGB(255, 255, 255);
			case COLOR_ACTIVEBORDER:
				return 0;
			case COLOR_INACTIVEBORDER:
				return 0;
			case COLOR_APPWORKSPACE:
				return 0;
			case COLOR_HIGHLIGHT:
				return RGB(9, 71, 113);
			case COLOR_HIGHLIGHTTEXT:
				return RGB(255, 255, 255);
			case COLOR_BTNFACE:
				return RGB(51, 51, 51);
			case COLOR_BTNSHADOW:
				return 0;
			case COLOR_GRAYTEXT:
				return RGB(255, 255, 255);
			case COLOR_BTNTEXT:
				return RGB(255, 255, 255);
			case COLOR_INACTIVECAPTIONTEXT:
				return RGB(255, 255, 255);
			case COLOR_BTNHIGHLIGHT:
				return RGB(9, 71, 113);
			case COLOR_3DDKSHADOW:
				return 0;
			case COLOR_3DLIGHT:
				return 0;
			case COLOR_INFOTEXT:
				return 0;
			case COLOR_INFOBK:
				return 0;
			case COLOR_HOTLIGHT:
				return 0;
			case COLOR_GRADIENTACTIVECAPTION:
				return 0;
			case COLOR_GRADIENTINACTIVECAPTION:
				return 0;
			case COLOR_MENUHILIGHT:
				return RGB(9, 71, 113);
			case COLOR_MENUBAR:
				return RGB(48, 48, 48);
			default:
				break;
		}
		return 0;
	}

	inline HBRUSH GetSysColorBrush(int nIndex)
	{
		if (!m_bSysDarkMode)
		{
			return ::GetSysColorBrush( nIndex);
		}

		HBRUSH hBrush;
		if ( m_mapBrush.count( nIndex) == 0)
		{
			hBrush = ::CreateSolidBrush( GetSysColor(nIndex));
			m_mapBrush.emplace( nIndex, hBrush);
		}
		else
		{
			hBrush = m_mapBrush.at( nIndex);
		}

		return hBrush;
	}

	inline bool WndProcDarkMode(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& ret)
	{
		if (!m_bSysDarkMode)
		{
			return false;
		}

		return UAHWndProc(hWnd, message, wParam, lParam, ret);
	}

	inline bool DarkModeStatusBar(HWND hWndStatusBar) const
	{
		if (!m_bSysDarkMode)
		{
			return false;
		}

		::SetWindowSubclass(hWndStatusBar, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR dwRefData) -> LRESULT
		{
			CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);
			switch (uMsg)
			{
				case WM_ERASEBKGND:
				{
					HDC hdc = (HDC)wParam;
					HBRUSH hbrBkgnd = ::CreateSolidBrush(RGB(30, 30, 30));
					HBRUSH hbrOld = (HBRUSH) ::SelectObject(hdc, hbrBkgnd);

					RECT rc;
					::GetClientRect(hWnd, &rc);

					::Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
					::SelectObject(hdc, hbrOld);
					::DeleteObject(hbrBkgnd);
					return TRUE;
				}
				break;
			}
			return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
		}, 0, reinterpret_cast<DWORD_PTR>(this));

		return true;
	}

	static LRESULT CALLBACK DarkModeOwnerDrawRegistSysTabControl32_SubclassProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);
		
		switch (uMsg)
		{
			case WM_ERASEBKGND:
				return 1;
			case WM_MOUSEMOVE:
			{
				TCHITTESTINFO hitTestInfo;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

				hitTestInfo.pt = pt;
				int tabIndex = TabCtrl_HitTest(hWnd, &hitTestInfo);

				int hoverTabIndex = (int)GetProp(hWnd, _T("HoverTabIndexProp"));
				if (tabIndex != hoverTabIndex)
				{
					RECT tabRect;
					TabCtrl_GetItemRect(hWnd, tabIndex, &tabRect);

					SetProp(hWnd, _T("HoverTabIndexProp"), (HANDLE)tabIndex);
					InvalidateRect(hWnd, &tabRect, TRUE); // タブの再描画を要求

					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hWnd;
					::TrackMouseEvent(&tme);
				}
				break;
			}
			case WM_MOUSELEAVE:
			{
				int hoverTabIndex = (int)GetProp(hWnd, _T("HoverTabIndexProp"));
				if (hoverTabIndex != -1)
				{
					RECT tabRect;
					TabCtrl_GetItemRect(hWnd, hoverTabIndex, &tabRect);
					SetProp(hWnd, _T("HoverTabIndexProp"), (HANDLE)-1);
					InvalidateRect(hWnd, &tabRect, TRUE); // タブの再描画を要求
				}
				break;
			}
			case WM_PAINT:
			{
				HDC hdc;
				PAINTSTRUCT ps;
				hdc = BeginPaint(hWnd, &ps);

				// フォントを取得
				HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
				HFONT hOldFont = nullptr;
				if (hFont)
				{
					hOldFont = (HFONT)SelectObject(hdc, hFont);
				}

				HTHEME hTheme = OpenThemeData(hWnd, L"TAB");
				HTHEME hThemeBtn = OpenThemeData(hWnd, L"BUTTON");
				if (hTheme && hThemeBtn)
				{
					int hoverTabIndex = (int)GetProp(hWnd, _T("HoverTabIndexProp"));

					// タブコントロールのフレームを描画(ボタンで代替)
					RECT rcClient;
					GetClientRect(hWnd, &rcClient);
					DrawThemeBackground(hThemeBtn, hdc, BP_PUSHBUTTON, 0, &rcClient, nullptr);

					int tabCount = TabCtrl_GetItemCount(hWnd);
					for (int i = 0; i < tabCount; ++i)
					{
						RECT tabRect;
						TabCtrl_GetItemRect(hWnd, i, &tabRect);

						int iStateId;
						if (i == TabCtrl_GetCurSel(hWnd))
							iStateId = PBS_PRESSED;
						else if (i == hoverTabIndex)
							iStateId = PBS_HOT;
						else
							iStateId = PBS_NORMAL;

						//tabRect.right += 4;
						//tabRect.bottom += 4;
						DrawThemeBackground(hThemeBtn, hdc, BP_PUSHBUTTON, iStateId, &tabRect, nullptr);

						// タブのテキストを描画
						TCITEM tci = { 0 };
						WCHAR szText[100] = { 0 };
						tci.mask = TCIF_TEXT;
						tci.pszText = szText;
						tci.cchTextMax = ARRAYSIZE(szText);
						TabCtrl_GetItem(hWnd, i, &tci);

						DrawThemeText(hThemeBtn, hdc, BP_PUSHBUTTON, iStateId, szText, -1, DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0, &tabRect);
					}

					CloseThemeData(hTheme);
					CloseThemeData(hThemeBtn);
				}

				// フォントのリセット
				if (hFont)
				{
					SelectObject(hdc, hOldFont);
				}

				EndPaint(hWnd, &ps);
				return 0;
			}

		}

		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	inline void DarkModeOwnerDrawRegistSysTabControl32(HWND hWnd)
	{
		if (!m_bSysDarkMode)
		{
			return;
		}
		::SetWindowSubclass(hWnd,
			DarkModeOwnerDrawRegistSysTabControl32_SubclassProc, 0,
			reinterpret_cast<DWORD_PTR>(this));
	}

	static LRESULT CALLBACK DarkModeOwnerDrawRegistScrollBar_Grip_SubclassProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);
		switch (uMsg)
		{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				::BeginPaint(hWnd, &ps);
				
				// 背景色を設定
				::FillRect(ps.hdc, &ps.rcPaint, pThis->GetSysColorBrush(COLOR_BTNFACE));

				// サイズ変更グリップを描画
				HTHEME hTheme = ::OpenThemeData(hWnd, L"SCROLLBAR");
				if ( hTheme)
				{
					// グリップを右下に描画
					SIZE sz;
					::GetThemePartSize(hTheme, ps.hdc, SBP_SIZEBOX, SZB_RIGHTALIGN, NULL, TS_TRUE, &sz);
					ps.rcPaint.left = ps.rcPaint.right - sz.cx;
					ps.rcPaint.top = ps.rcPaint.bottom - sz.cy;

					::DrawThemeBackground(hTheme, ps.hdc, SBP_SIZEBOX, SZB_RIGHTALIGN, &ps.rcPaint, NULL);
					::CloseThemeData(hTheme);
				}
				else
				{
					::SetBkColor(ps.hdc, pThis->GetSysColor(COLOR_BTNFACE));
					::SetBkMode(ps.hdc, OPAQUE);
					::DrawFrameControl(ps.hdc, &ps.rcPaint, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
				}

				::EndPaint(hWnd, &ps);
				return 0;
			}
		}
		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	inline void DarkModeOwnerDrawRegistScrollBar_Grip(HWND hWnd)
	{
		if (!m_bSysDarkMode)
		{
			return;
		}
		::SetWindowSubclass(hWnd,
			DarkModeOwnerDrawRegistScrollBar_Grip_SubclassProc, 0,
			reinterpret_cast<DWORD_PTR>(this));
	}

	inline void DarkModeOwnerDrawRegistCheckAndRadioBox(HWND hWnd, int ButtonType = BP_CHECKBOX)
	{
		if (!m_bSysDarkMode)
		{
			return;
		}

		// サブクラス化する前の現在のチェック状態を取得しておく
		int checkState = ::SendMessage(hWnd, BM_GETCHECK, 0, 0);
		::SetProp(hWnd, _T("CheckedState"), (HANDLE)checkState);

		LONG style = ::GetWindowLong(hWnd, GWL_STYLE);
		::SetWindowLong(hWnd, GWL_STYLE, style | BS_OWNERDRAW);
		::SetProp(hWnd, _T("ButtonType"), (HANDLE)ButtonType);	// チェックボックス/ラジオボタン...

		::SetWindowSubclass(hWnd,
			DarkModeOwnerDrawRegistCheckAndRadioBox_SubclassProc, 0,
			reinterpret_cast<DWORD_PTR>(this));

	}

	static LRESULT CALLBACK DarkModeOwnerDrawRegistCheckAndRadioBox_SubclassProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);
		switch (uMsg)
		{
			case WM_ERASEBKGND:
			{
				HDC hdc = (HDC)wParam;
				RECT rc;
				::GetClientRect(hWnd, &rc);
				::FillRect(hdc, &rc, pThis->GetSysColorBrush(COLOR_BTNFACE));
				return TRUE;
			}

			case BM_GETCHECK:
			{
				int CheckedState = (int)::GetProp(hWnd, _T("CheckedState"));

				// DEBUG:
				//TCHAR buffer[256];
				//wsprintf(buffer, _T("Get! Window Handle: 0x%X, CheckedState: %d\n"), (UINT_PTR)hWnd, CheckedState);
				//::OutputDebugString(buffer);

				return (LRESULT)CheckedState;
			}
			case BM_SETCHECK:
			{
				int CheckedState = (int)wParam;

				// DEBUG:
				//TCHAR buffer[256];
				//wsprintf(buffer, _T("Set! Window Handle: 0x%X, CheckedState: %d\n"), (UINT_PTR)hWnd, CheckedState);
				//::OutputDebugString(buffer);

				::SetProp(hWnd, _T("CheckedState"), (HANDLE)CheckedState);
				::InvalidateRect(hWnd, NULL, TRUE);  // 必要に応じて再描画
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				if (!::GetProp(hWnd, _T("IsHovering")))
				{
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hWnd;
					::TrackMouseEvent(&tme);

					::SetProp(hWnd, _T("IsHovering"), (HANDLE)TRUE);
					::InvalidateRect(hWnd, NULL, TRUE);  // 再描画を要求
				}
				break;
			}

			case WM_MOUSELEAVE:
			{
				::SetProp(hWnd, _T("IsHovering"), (HANDLE)FALSE);
				::InvalidateRect(hWnd, NULL, TRUE);  // 再描画を要求
				break;
			}

			case WM_LBUTTONDOWN:
			{
				::SetProp(hWnd, _T("IsPressed"), (HANDLE)TRUE);
				::InvalidateRect(hWnd, NULL, TRUE);  // 再描画を要求
				break;
			}

			case WM_LBUTTONUP:
			{
				::SetProp(hWnd, _T("IsPressed"), (HANDLE)FALSE);
				::InvalidateRect(hWnd, NULL, TRUE);  // 再描画を要求
				break;
			}

			case WM_SETFOCUS:
			{
				// チェックボックスがフォーカスを取得したときの処理
				::SetProp(hWnd, _T("HasFocus"), (HANDLE)TRUE);
				::InvalidateRect(hWnd, NULL, TRUE);  // 必要に応じて再描画
				break;
			}
				

			case WM_KILLFOCUS:
			{
				// チェックボックスがフォーカスを失ったときの処理
				::SetProp(hWnd, _T("HasFocus"), (HANDLE)FALSE);
				::InvalidateRect(hWnd, NULL, TRUE);  // 必要に応じて再描画
				break;
			}
		}
		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	inline bool DarkModeOwnerDrawWndProcCheckAndRadioBox(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!m_bSysDarkMode)
		{
			return true;
		}

		switch (uMsg)
		{
			case WM_COMMAND:
			{
				if ((HIWORD(wParam) == BN_CLICKED)
					/*|| (HIWORD(wParam) == BN_DOUBLECLICKED)*/ )
				{
					HWND hControl = (HWND)lParam;

					// コントロールのクラス名を取得
					TCHAR szClassName[256];
					::GetClassName(hControl, szClassName, sizeof(szClassName) / sizeof(TCHAR));

					// コントロールが "Button" クラスであるかどうかを確認
					if (lstrcmpi(szClassName, _T("Button")) == 0)
					{
						// コントロールのスタイルを取得
						LONG_PTR dwStyle = ::GetWindowLongPtr(hControl, GWL_STYLE);

						// コントロールが BS_OWNERDRAW スタイルを持っているかどうかを確認
						if ((dwStyle & BS_OWNERDRAW) == BS_OWNERDRAW)
						{
							// チェック状態を取得
							BOOL isChecked = (BOOL)(::SendMessage(hControl, BM_GETCHECK, 0, 0) == BST_CHECKED);

							// ボタンの種別を取得
							int iPart = (int)::GetProp(hControl, _T("ButtonType"));

							if ( BP_CHECKBOX == iPart)
							{
								// チェック状態をトグル
								::SendMessage(hControl, BM_SETCHECK, !isChecked ? BST_CHECKED : BST_UNCHECKED, 0);
							}
							else if ( BP_RADIOBUTTON == iPart)
							{
								// ラジオボタン

								// 同じグループ内の前方のラジオボタンの選択をクリア
								HWND hPrev = hControl;
								while (hPrev = ::GetWindow(hPrev, GW_HWNDPREV))
								{
									// コントロールが "Button" クラスであるかどうかを確認
									::GetClassName(hPrev, szClassName, sizeof(szClassName) / sizeof(TCHAR));
									if ( lstrcmpi(szClassName, _T("Button")) == 0)
									{
										// コントロールのスタイルを取得
										LONG_PTR dwStyle = ::GetWindowLongPtr(hPrev, GWL_STYLE);

										// コントロールが BS_OWNERDRAW スタイルを持っているかどうかを確認
										if ((dwStyle & BS_OWNERDRAW) == BS_OWNERDRAW)
										{
											int iPart_Prev = (int)::GetProp(hPrev, _T("ButtonType"));
											if ( BP_RADIOBUTTON == iPart_Prev)
											{
												// ラジオボタン状態を変更
												::SendMessage(hPrev, BM_SETCHECK, BST_UNCHECKED, 0);
											}
										}
									}

									// グループの先頭だったので終了
									if ( ::GetWindowLongPtr(hPrev, GWL_STYLE) & WS_GROUP)
									{
										break;
									}
								}

								// 同じグループ内の後方のラジオボタンの選択をクリア
								HWND hNext = hControl;
								while (hNext = ::GetWindow(hNext, GW_HWNDNEXT))
								{
									// グループの先頭だったので終了
									if (::GetWindowLongPtr(hNext, GWL_STYLE) & WS_GROUP)
									{
										break;
									}

									// コントロールが "Button" クラスであるかどうかを確認
									::GetClassName(hNext, szClassName, sizeof(szClassName) / sizeof(TCHAR));
									if (lstrcmpi(szClassName, _T("Button")) == 0)
									{
										// コントロールのスタイルを取得
										LONG_PTR dwStyle = ::GetWindowLongPtr(hNext, GWL_STYLE);

										// コントロールが BS_OWNERDRAW スタイルを持っているかどうかを確認
										if ((dwStyle & BS_OWNERDRAW) == BS_OWNERDRAW)
										{
											int iPart_Prev = (int)::GetProp(hNext, _T("ButtonType"));
											if (BP_RADIOBUTTON == iPart_Prev)
											{
												// ラジオボタン状態を変更
												::SendMessage(hNext, BM_SETCHECK, BST_UNCHECKED, 0);
											}
										}
									}
								}

								// ラジオボタン状態を変更
								::SendMessage(hControl, BM_SETCHECK, BST_CHECKED, 0);
							}
						}
					}
				}
				break;
			}

			case WM_DRAWITEM:
			{
				DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
				if ( pDIS->CtlType == ODT_BUTTON)
				{
					// テーマAPIを使用してチェックボックスを描画
					HTHEME hTheme = ::OpenThemeData(pDIS->hwndItem, L"Button");
					if (hTheme)
					{
						HWND hControl = pDIS->hwndItem;
						BOOL isChecked			= (::SendMessage(hControl, BM_GETCHECK, 0, 0) == BST_CHECKED);
						BOOL isHovering			= (BOOL)::GetProp(hControl, _T("IsHovering"));
						BOOL isPressed			= (BOOL)::GetProp(hControl, _T("IsPressed"));
						BOOL isFocus			= (BOOL)::GetProp(hControl, _T("HasFocus"));
						BOOL isEnabled			= ::IsWindowEnabled(hControl);

						int iPart = (int)::GetProp(hControl, _T("ButtonType"));
						int iState;

						if (isChecked)
						{
							if (isPressed)
								iState = CBS_CHECKEDPRESSED;
							else if (isHovering)
								iState = CBS_CHECKEDHOT;
							else if (!isEnabled)
								iState = CBS_CHECKEDDISABLED;
							else
								iState = CBS_CHECKEDNORMAL;
						}
						else
						{
							if (isPressed)
								iState = CBS_UNCHECKEDPRESSED;
							else if (isHovering)
								iState = CBS_UNCHECKEDHOT;
							else if (!isEnabled)
								iState = CBS_UNCHECKEDDISABLED;
							else
								iState = CBS_UNCHECKEDNORMAL;
						}

						// チェック/ラジオボックスのサイズを取得
						SIZE size;
						::GetThemePartSize(hTheme, pDIS->hDC, iPart, iState, nullptr, TS_TRUE, &size);
						RECT rcCheckbox = pDIS->rcItem;
						rcCheckbox.left += 2;
						rcCheckbox.right = rcCheckbox.left + size.cx;
						rcCheckbox.top = 2;
						rcCheckbox.bottom = rcCheckbox.top + size.cy;

						// チェック/ラジオボックスを描画
						::DrawThemeBackground(hTheme, pDIS->hDC, iPart, iState, &rcCheckbox, nullptr);
						::CloseThemeData(hTheme);

						// テキストの色を変更して描画
						::SetTextColor(pDIS->hDC, isEnabled ? GetSysColor(COLOR_BTNTEXT) : RGB(128,128,128));
						::SetBkColor(pDIS->hDC, GetSysColor(COLOR_BTNFACE));
						::SetBkMode(pDIS->hDC, OPAQUE);
						// ::SetBkMode(pDIS->hDC, TRANSPARENT);	// 透明だと微妙なケースあり

						// テキスト文字列を取得
						TCHAR szText[512];
						::GetWindowText(hControl, szText, sizeof(szText) / sizeof(TCHAR));

						// ボタンのスタイル取得
						UINT format = DT_WORDBREAK;
						LONG_PTR lStyle = ::GetWindowLongPtr(hControl, GWL_STYLE);
						if ( lStyle & BS_LEFT)
						{
							format |= DT_LEFT;
						}
						else if ( lStyle & BS_RIGHT)
						{
							format |= DT_RIGHT;
						}
						else if (lStyle & BS_CENTER)
						{
							format |= DT_CENTER;
						}
						else if (lStyle & BS_TOP)
						{
							format |= DT_TOP;
						}
						else if (lStyle & BS_BOTTOM)
						{
							format |= DT_BOTTOM;
						}
						else if (lStyle & BS_VCENTER)
						{
							format |= DT_VCENTER;
						}

						// アクセラレータキーを描画しない
						// 通常はこのフラグが立っているため描画しないが、Altキーを押下した時はアクセラレータキーを描画する
						if ( pDIS->itemState & ODS_NOACCEL)
						{
							format |= DT_HIDEPREFIX;
						}

						// テキストの描画位置を調整
						RECT rcText = pDIS->rcItem;
						rcText.top -= 2;
						rcText.left += rcCheckbox.right + 2; // チェックボックスの大体のサイズ + 余白

						// テキストの描画
						::DrawText(pDIS->hDC, szText, -1, &rcText, format);

						// フォーカスを持っており、かつ、ODS_NOFOCUSRECTフラグが設定されていない場合
						if ((pDIS->itemState & ODS_FOCUS)
							&& !(pDIS->itemState & ODS_NOFOCUSRECT))
						{
							//// 少し小さく描く
							//rcText.left++;
							//rcText.top++;
							//rcText.right--;
							//rcText.bottom--;

							// フォーカスの枠は自前で描く(テキストを囲むようにする)
							::DrawFocusRect(pDIS->hDC, &rcText);
						}

						return true;	// 処理した場合は true
					}
				}
				break;
			}
		}
		return false;
	}

	inline void DarkModeOwnerDrawRegistGroupBox(HWND hWnd)
	{
		if (!m_bSysDarkMode)
		{
			return;
		}

		::SetWindowSubclass(hWnd, GroupBoxSubclassProc, 0, reinterpret_cast<DWORD_PTR>(this));
	}

	static LRESULT CALLBACK GroupBoxSubclassProc(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);

		switch (uMsg)
		{
			case WM_PAINT:
			{
				// テーマデータを取得
				HTHEME hTheme = OpenThemeData(hWnd, L"Button");
				if (hTheme)
				{
					PAINTSTRUCT ps;
					HDC hdc = ::BeginPaint(hWnd, &ps);

					RECT rc;
					::GetClientRect(hWnd, &rc);

					// デフォルトのフォントを取得
					HFONT hSysFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);
					if (!hSysFont)
					{
						hSysFont = pThis->GetSystemFont();
					}

					if ( hSysFont)
						::SelectObject(hdc, hSysFont);

					// テキストを取得
					TCHAR text[256];
					::GetWindowText(hWnd, text, sizeof(text) / sizeof(TCHAR));

					// フォントの高さを取得
					SIZE size;

					// テキストが空の場合、代替の文字列を使用
					if ( lstrlen(text) == 0)
					{
						TCHAR DUMMY[] = L" ";
						::GetTextExtentPoint32(hdc, DUMMY, lstrlen(DUMMY), &size);
					}
					else {
						::GetTextExtentPoint32(hdc, text, lstrlen(text), &size);
					}
					
					// テーマデータを使用してグループボックスを描画
					rc.top += size.cy/2;
					::DrawThemeBackground(hTheme, hdc, BP_GROUPBOX, GBS_NORMAL, &rc, NULL);

					::SetTextColor(hdc, pThis->GetSysColor(COLOR_BTNTEXT));
					::SetBkColor(hdc, pThis->GetSysColor(COLOR_BTNFACE));
					::SetBkMode(hdc, OPAQUE);

					rc.left += size.cy / 2;
					rc.top = 0;

					::DrawText(hdc, text, -1, &rc, DT_LEFT | DT_TOP | DT_SINGLELINE);

					::CloseThemeData(hTheme);

					::EndPaint(hWnd, &ps);

				}
				else {
					return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
				}
				return 0;
			}
			case WM_NCDESTROY:
			{
				::RemoveWindowSubclass(hWnd, GroupBoxSubclassProc, uIdSubclass);
				break;
			}
			
		}

		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}


	bool DarkModeDispatchEvent(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam, LRESULT& result)
	{
		if ( !IsSystemUseDarkMode())
			return false;

		if ( DarkModeOwnerDrawWndProcCheckAndRadioBox(hWnd, wMsg, wParam, lParam))
		{
			result = TRUE;
			return true;		// 処理した
		}

		// 親ウィンドウのメッセージ処理
		switch (wMsg)
		{
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORSTATIC:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLOREDIT:
			{
				// ダークモード
				::SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
				::SetTextColor((HDC)wParam, GetSysColor(COLOR_BTNTEXT));
				result = (LRESULT)GetSysColorBrush(COLOR_BTNFACE);
				return true;	// 処理した
			}

			//case WM_NOTIFY:
			//{
			//	LPNMHDR pnmh = (LPNMHDR)lParam;
			//	if (pnmh->code == NM_CUSTOMDRAW)
			//	{
			//		LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;

			//		switch (lpnmcd->dwDrawStage)
			//		{
			//			case CDDS_PREPAINT:
			//				result = CDRF_NOTIFYITEMDRAW;
			//				return true;

			//			case CDDS_ITEMPREPAINT:
			//			{
			//				if (lpnmcd->dwItemSpec == TabCtrl_GetCurSel(pnmh->hwndFrom))
			//				{
			//					// 選択されたタブの背景色を設定します。
			//					SetBkColor(lpnmcd->hdc, RGB(200, 0, 0));  // 例として赤に設定
			//				}
			//				else
			//				{
			//					// 選択されていないタブの背景色を設定します。
			//					SetBkColor(lpnmcd->hdc, RGB(200, 200, 200));  // 例としてグレーに設定
			//				}

			//				result = CDRF_DODEFAULT;
			//				return true;
			//			}
			//		}
			//	}
			//}
			break;
		}
		return false;
	}

	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
	{
		std::vector<HWND>* childWindows = reinterpret_cast<std::vector<HWND>*>(lParam);
		childWindows->push_back(hwnd);
		return TRUE;
	}

	bool DarkModeOnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
	{
		if ( IsSystemUseDarkMode())
		{
			// ウィンドウ関連
			AllowDarkModeForWindow(hwndDlg, true);
			SetDarkModeTitleBar19(hwndDlg);
			SetDarkModeTitleBar(hwndDlg);

			// コントロールの検索
			std::vector<HWND> childWindows;
			::EnumChildWindows(hwndDlg, EnumChildProc, reinterpret_cast<LPARAM>(&childWindows));

			for ( HWND child : childWindows)
			{
				// ウィンドウクラス名
				TCHAR className[256];
				::GetClassNameW(child, className, sizeof(className) / sizeof(wchar_t));

				// コントロールの種類別に処理する
				if ( wcscmp( className, _T("Button")) == 0)
				{
					LONG style = ::GetWindowLong(child, GWL_STYLE);

					switch (style & 0x0F)
					{
						case BS_CHECKBOX:
						case BS_AUTOCHECKBOX:
							DarkModeOwnerDrawRegistCheckAndRadioBox(child, BP_CHECKBOX);
							break;
						//case BS_3STATE:
						//case BS_AUTO3STATE:
						//	DarkModeOwnerDrawRegistCheckAndRadioBox(child, BP_CHECKBOX);
						//	break;
						case BS_RADIOBUTTON:
						case BS_AUTORADIOBUTTON:
							DarkModeOwnerDrawRegistCheckAndRadioBox(child, BP_RADIOBUTTON);
							break;
						case BS_GROUPBOX:
							DarkModeOwnerDrawRegistGroupBox(child);
							break;
						default:
							AllowDarkModeForWindow(child, true);
							::SetWindowTheme(child, L"DarkMode_Explorer", nullptr);
							break;
					}
				}
				else if (wcscmp(className, _T("ComboBox")) == 0)
				{
					// コンボボックス
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode_CFD", nullptr);
				}
				else if (wcscmp(className, _T("Static")) == 0)
				{
					// スタティックコントロール
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode_Explorer", nullptr);
				}
				else if (wcscmp(className, _T("Edit")) == 0)
				{
					// エディットボックス
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode_CFD", nullptr);
				}
				else if (wcscmp(className, _T("ScrollBar")) == 0)
				{
					LONG style = ::GetWindowLong(child, GWL_STYLE);
					if ((style & SBS_SIZEBOX) || (style & SBS_SIZEGRIP))
					{
						// スクロールバー(サイズ変更グリップ)
						DarkModeOwnerDrawRegistScrollBar_Grip(child);
					}
				}
				else if (wcscmp(className, _T("SysTabControl32")) == 0)
				{
					// 本来適用可能なダークテーマがないが、ボタンで描画を一部代替している
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode_Explorer", nullptr);
					DarkModeOwnerDrawRegistSysTabControl32(child);
				}
				else if (wcscmp(className, _T("SysTreeView32")) == 0)
				{
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode_Explorer", nullptr);
				}
				else if (wcscmp(className, _T("SysListView32")) == 0)
				{
					AllowDarkModeForWindow(child, true);
					::SetWindowTheme(child, L"DarkMode", nullptr);
				}
			}
			return true;
		}

		return false;
	}

	//inline bool DarkModeCheckbox(HWND hWndCheckbox) const
	//{
	//	if (!m_bSysDarkMode)
	//	{
	//		return false;
	//	}

	//	// チェックボックスのスタイルをオーナードローに設定
	//	LONG style = ::GetWindowLong(hWndCheckbox, GWL_STYLE);
	//	::SetWindowLong(hWndCheckbox, GWL_STYLE, style | BS_OWNERDRAW);
	//	::SetWindowSubclass(hWndCheckbox, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR /*uIdSubclass*/, DWORD_PTR dwRefData) -> LRESULT
	//	{
	//		CHsp3DarkMode* pThis = reinterpret_cast<CHsp3DarkMode*>(dwRefData);
	//		switch (uMsg)
	//		{
	//			case WM_DRAWITEM:
	//			{
	//				DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
	//				if (pDIS->CtlType == ODT_BUTTON)
	//				{
	//					// テーマAPIを使用してチェックボックスを描画
	//					HTHEME hTheme = ::OpenThemeData(hWnd, L"BUTTON");
	//					if (hTheme)
	//					{
	//						int iState;
	//						if (pDIS->itemState & ODS_CHECKED)
	//							iState = CBS_CHECKEDNORMAL;
	//						else
	//							iState = CBS_UNCHECKEDNORMAL;

	//						::DrawThemeBackground(hTheme, pDIS->hDC, BP_CHECKBOX, iState, &pDIS->rcItem, NULL);

	//						::CloseThemeData(hTheme);
	//					}

	//					// テキスト色を設定してテキストを描画
	//					::SetTextColor(pDIS->hDC, pThis->GetSysColor(COLOR_BTNTEXT));
	//					WCHAR szText[256];
	//					::GetWindowText(hWnd, szText, sizeof(szText) / sizeof(WCHAR));
	//					::DrawText(pDIS->hDC, szText, -1, &pDIS->rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	//					return TRUE;
	//				}
	//				break;
	//			}
	//		}
	//		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	//	}, 0, reinterpret_cast<DWORD_PTR>(this));

	//	return true;
	//}

	HDC CreateEmptyBitmapAndHDC(int width, int height, HBITMAP& hBitmapOut)
	{
		HDC hScreenDC = ::GetDC(nullptr);
		HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hScreenDC, width, height);
		::SelectObject(hMemDC, hBitmap);

		::ReleaseDC(nullptr, hScreenDC);

		hBitmapOut = hBitmap;
		return hMemDC;
	}

};

