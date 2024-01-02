// CHsp3MenuItemParser.cpp

#include "StdAfx.h"
#include "CHsp3MenuItemParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// 文字列の先頭と末尾のスペースやタブを除去する関数
std::wstring CHsp3MenuItemParser::trim(const std::wstring& str)
{
	size_t first = str.find_first_not_of(L" \t");
	if (first == std::wstring::npos) return L"";
	size_t last = str.find_last_not_of(L" \t");
	return str.substr(first, (last - first + 1));
}

// CSVファイルをパースする関数
ErrorCode CHsp3MenuItemParser::parse(const std::wstring& filePath, std::vector<MenuItem>& items)
{
	// Shift_JISでファイルを開く
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return FILE_OPEN_ERROR; // ファイルが開けなかった場合はエラーコードを返す
	}

	// ファイルの内容をShift_JISで読み取る
	std::string sjisContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Shift_JISからUTF-16に変換
	int requiredSize = MultiByteToWideChar(
		CP_ACP, 0, sjisContent.c_str(), sjisContent.size(), nullptr, 0);
	std::wstring utf16Content(requiredSize, 0);
	MultiByteToWideChar(
		CP_ACP, 0, sjisContent.c_str(), sjisContent.size(), &utf16Content[0], requiredSize);

	// 各行をパースする
	std::wstringstream ss(utf16Content);
	std::wstring line;
	while (std::getline(ss, line, L'\n'))
	{
		// CRLFの場合、'\r'を除去する
		if (!line.empty() && line.back() == L'\r')
		{
			line.pop_back();
		}

		// 各行のカンマの数をチェック
		size_t numCommas = std::count(line.begin(), line.end(), L',');
		if (numCommas != 2)
		{
			return PARSE_ERROR; // カンマの数が想定と異なる場合はエラーを返す
		}

		std::wstringstream lineStream(line);
		std::wstring menuName, runtimeName, axFileName;

		// カンマで区切られた各項目を取得
		if (!std::getline(lineStream, menuName, L',') ||
			!std::getline(lineStream, runtimeName, L',') ||
			!std::getline(lineStream, axFileName, L','))
		{
			return PARSE_ERROR; // 項目の取得に失敗した場合はエラーを返す
		}

		// トリムして結果のベクタに追加
		items.push_back(
		{
			trim(menuName), trim(runtimeName), trim(axFileName)
		});
	}

	return SUCCESS; // 正常に終了
}