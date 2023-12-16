#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <cwctype>
#include <map>

using namespace Gdiplus;
using namespace std;

#pragma comment(lib,"gdiplus.lib")

struct ICONHEADER
{
	uint16_t Reserved;
	uint16_t Type;
	uint16_t Count;
};

struct ICONDIRENTRY
{
	uint8_t Width;
	uint8_t Height;
	uint8_t ColorCount;
	uint8_t Reserved;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t BytesInRes;
	uint32_t ImageOffset;
};

vector<BYTE> ConvertImageToARGB(
	const wstring& filePath,
	COLORREF transparencyColor = CLR_INVALID,
	bool resize = false, UINT size = 256);

int GetEncoderClsid(
	const wstring& format, CLSID* pClsid);

void AnalyzeArgs(
	int argc, wchar_t* argv[],
	vector<wstring>& inputFiles, wstring& outputFile,
	map<int, COLORREF>& transparencyColors,
	bool& resize);

int ConvertImage2Ico(
	ofstream& icoFile, const vector<wstring>& inputFiles,
	map<int, COLORREF>& transparencyColors,
	bool resize);