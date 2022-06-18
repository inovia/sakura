#include "StdAfx.h"
#include "CHsp3Font.h"

bool CHsp3Font::LoadFont(HINSTANCE hInstance)
{
	// 
	// 本当はリソース内にフォントを組み込んで、
	// AddFontMemResourceEx() でロードしたかった。
	// ChooseFont() でフォントが列挙できない問題があり、見送り。
	// 

	WCHAR	filePath[1024];
	GetExedir(filePath, L"UDEVGothic-Regular.ttf");

	if (::PathFileExists( filePath))
	{
		int nCount = ::AddFontResourceEx(filePath, FR_PRIVATE, 0);
		if (0 < nCount)
		{
			return true;
		}
	}

	return false;
}