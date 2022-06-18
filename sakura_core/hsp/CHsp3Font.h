#pragma once
#include <windows.h>
#include <shlwapi.h>

class CHsp3Font
{
public:
	// デフォルトコンストラクタ
	CHsp3Font() {  };

	// デストラクタ
	~CHsp3Font() {  };

public:
	bool LoadFont(HINSTANCE hInstance);
};

