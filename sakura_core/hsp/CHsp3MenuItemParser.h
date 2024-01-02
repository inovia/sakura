// CHsp3MenuItemParser.h

#pragma once

#include <windows.h>
#include <vector>
#include <string>

// メニューアイテムを表す構造体
struct MenuItem
{
	// もし、メンバを入れ替える際は注意してください。
	// C++20 以前の構造体の初期化を使っているところがあります。

	std::wstring menuName;      // メニュー名
	std::wstring runtimeName;   // ランタイム名
	std::wstring axFileName;    // AXファイル名
};

// エラーコードの列挙型
enum ErrorCode
{
	SUCCESS = 0,           // 成功
	FILE_OPEN_ERROR,       // ファイルオープンのエラー
	FILE_READ_ERROR,       // ファイルの読み取りエラー
	PARSE_ERROR            // パースのエラー
};

class CHsp3MenuItemParser
{
public:
	ErrorCode parse(const std::wstring& filePath, std::vector<MenuItem>& items);

private:
	std::wstring trim(const std::wstring& str);
};