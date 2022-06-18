#pragma once
#include <windows.h>
#include <shlwapi.h>

class CHsp3Font
{
public:
	// フォント名
	static constexpr std::wstring_view FONT_NAME = { L"UDEV Gothic" };

public:
	// デフォルトコンストラクタ
	CHsp3Font() {  };

	// デストラクタ
	~CHsp3Font() {  };

public:
	bool LoadFont(HINSTANCE hInstance);
};

