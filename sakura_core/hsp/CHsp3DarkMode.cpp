#include "StdAfx.h"
#include "CHsp3DarkMode.h"
#include "win32-darkmode/DarkMode.h"

#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Dwmapi.lib")

CHsp3DarkMode CHsp3DarkMode::s_Instance = CHsp3DarkMode();

bool CHsp3DarkMode::Load()
{
	m_hDLL = ::LoadLibrary( UXTHEME_DLL.data());
	if ( m_hDLL == nullptr)
		return false;

	// AllowDarkModeForApp
	m_pAllowDarkModeForApp =
		(F_AllowDarkModeForApp)::GetProcAddress(
			m_hDLL, "AllowDarkModeForApp");
	if (!m_pAllowDarkModeForApp)
	{
		m_pAllowDarkModeForApp =
			(F_AllowDarkModeForApp)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(135));
	}

	// SetPreferredAppMode
	m_pSetPreferredAppMode =
		(F_SetPreferredAppMode)::GetProcAddress(
			m_hDLL, "SetPreferredAppMode");
	if (!m_pSetPreferredAppMode)
	{
		m_pSetPreferredAppMode =
			(F_SetPreferredAppMode)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(135));
	}

	// FlushMenuThemes
	m_pFlushMenuThemes =
		(F_FlushMenuThemes)::GetProcAddress(
			m_hDLL, "FlushMenuThemes");
	if (!m_pFlushMenuThemes)
	{
		m_pFlushMenuThemes =
			(F_FlushMenuThemes)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(136));
	}

	// RefreshImmersiveColorPolicyState
	m_pRefreshImmersiveColorPolicyState =
		(F_RefreshImmersiveColorPolicyState)::GetProcAddress(
			m_hDLL, "RefreshImmersiveColorPolicyState");
	if (!m_pRefreshImmersiveColorPolicyState)
	{
		m_pRefreshImmersiveColorPolicyState =
			(F_RefreshImmersiveColorPolicyState)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(104));
	}

	// ShouldSystemUseDarkMode
	m_pShouldSystemUseDarkMode =
		(F_ShouldSystemUseDarkMode)::GetProcAddress(
			m_hDLL, "ShouldSystemUseDarkMode");
	if (!m_pShouldSystemUseDarkMode)
	{
		m_pShouldSystemUseDarkMode =
			(F_ShouldSystemUseDarkMode)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(138));
	}

	ShouldSystemUseDarkMode();

	// AllowDarkModeForWindow
	m_pAllowDarkModeForWindow =
		(F_AllowDarkModeForWindow)::GetProcAddress(
			m_hDLL, "AllowDarkModeForWindow");
	if (!m_pAllowDarkModeForWindow)
	{
		m_pAllowDarkModeForWindow =
			(F_AllowDarkModeForWindow)::GetProcAddress(
				m_hDLL, MAKEINTRESOURCEA(133));
	}

	// スクロールバーの修正
	InitDarkMode_ScrollBar();

	return true;
}