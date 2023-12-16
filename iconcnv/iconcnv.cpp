// iconcnv.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "iconcnv.h"

vector<BYTE> ConvertImageToARGB(
	const wstring& filePath, COLORREF transparencyColor, bool resize, UINT size, bool force32bit)
{
	auto pBitmap		= std::make_unique<Gdiplus::Bitmap>(filePath.c_str());
	UINT width			= pBitmap->GetWidth();
	UINT height			= pBitmap->GetHeight();

	// サイズが取得できない
	if ( width == 0 || height == 0 )
	{
		throw std::runtime_error("Image size cannot be retrieved.");
	}

	// リサイズオプションが有効で、画像が256pxより大きい場合はリサイズ
	bool resized = false;
	if ( resize && (width > size || height > size))
	{
		// リサイズ後の画像の新しいサイズを計算
		float aspectRatio = (float)width / (float)height;
		UINT newWidth, newHeight;

		if ( aspectRatio > 1.0f)
		{
			newWidth = size;
			newHeight = (UINT)(size / aspectRatio);
		}
		else
		{
			newWidth = (UINT)(size * aspectRatio);
			newHeight = size;
		}

		// 新しいBitmapオブジェクトを作成してリサイズ
		auto pNewBitmap = std::make_unique<Gdiplus::Bitmap>(
			size, size, PixelFormat32bppARGB);  // 256x256サイズで新しいビットマップを作成
		Graphics graphics( pNewBitmap.get());
		graphics.Clear(Color(0, 0, 0, 0));  // 背景を透明に
		graphics.DrawImage( pBitmap.get(),
			(Gdiplus::REAL)((size - newWidth) / 2),
			(Gdiplus::REAL)((size - newHeight) / 2),
			(Gdiplus::REAL)newWidth,
			(Gdiplus::REAL)newHeight);

		pNewBitmap.swap(pBitmap);

		width = size;
		height = size;

		resized = true;
	}

	// 32bit強制変換(上記リサイズ処理が未実施時のみ)
	if ( force32bit && !resized)
	{
		// 32ビットARGB形式の新しいビットマップを作成する
		auto pNewBitmap = std::make_unique<Gdiplus::Bitmap>(
			width, height, PixelFormat32bppARGB);

		Graphics g( pNewBitmap.get());
		g.DrawImage( pBitmap.get(), 0, 0, width, height);

		pNewBitmap.swap( pBitmap);
	}

	// 透明色が指定されている場合
	if ( transparencyColor != CLR_INVALID)
	{
		vector<BYTE> buffer(width * height * 4);
		BitmapData bitmapData;
		Rect rect(0, 0, width, height);
		pBitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
		BYTE* pixels = (BYTE*)bitmapData.Scan0;

		for (size_t i = 0; i < buffer.size(); i += 4)
		{
			COLORREF pixel = RGB(buffer[i + 2], buffer[i + 1], buffer[i]);
			if (pixel == transparencyColor)
			{
				buffer[i + 3] = 0;
			}
		}

		memcpy(&buffer[0], pixels, buffer.size());
		pBitmap->UnlockBits(&bitmapData);
	}

	// メモリストリームにPNG形式で保存
	vector<BYTE> pngData;
	IStream* stream = nullptr;
	if (::CreateStreamOnHGlobal(nullptr, TRUE, &stream) == S_OK)
	{
		// PNG形式で保存
		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		if ( pBitmap->Save(stream, &pngClsid, nullptr) == Ok)
		{
			// ストリームのサイズを取得
			STATSTG stg;
			stream->Stat(&stg, STATFLAG_NONAME);

			ULONG size = stg.cbSize.LowPart;
			HGLOBAL hg = nullptr;
			::GetHGlobalFromStream(stream, &hg);

			// メモリストリームからデータをコピー
			BYTE* pBytes = (BYTE*)GlobalLock(hg);
			pngData.assign(pBytes, pBytes + size);
			::GlobalUnlock(hg);
		}
		stream->Release();
	}

	return pngData;
}

int GetEncoderClsid(const wstring& format, CLSID* pClsid)
{
	UINT num = 0;  // エンコーダの数
	UINT size = 0; // ImageCodecInfoの配列のサイズ

	if ( format.empty() || pClsid == nullptr) return -1;

	// エンコーダの数と配列のサイズを取得
	Status status = ::GetImageEncodersSize(&num, &size);
	if (status != Ok || size == 0)
	{
		return -2;
	}

	// ImageCodecInfoの配列を確保
	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
	{
		return -3;
	}

	// エンコーダ情報を取得
	status = ::GetImageEncoders(num, size, pImageCodecInfo);
	if (status != Ok)
	{
		free(pImageCodecInfo);
		return -4;
	}

	// 指定されたフォーマットのエンコーダを探す
	for (UINT j = 0; j < num; ++j)
	{
		if ( wcscmp( pImageCodecInfo[j].MimeType, format.c_str()) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	// 見つからなかった場合
	free(pImageCodecInfo);
	return -5;
}

void AnalyzeArgs(
	int argc, wchar_t* argv[],
	vector<wstring>& inputFiles, wstring& outputFile,
	map<int, COLORREF>& transparencyColors,
	bool& resize,
	bool& force32bit)
{
	for (int i = 1; i < argc; ++i)
	{
		// 文字列を大文字に変換
		wstring arg(argv[i]);
		std::transform(arg.begin(), arg.end(), arg.begin(),
			[](wchar_t c) -> wchar_t { return towupper(c); });
		
		if (arg == L"/INPUT")
		{
			while (i + 1 < argc && wstring(argv[i + 1]) != L"/OUTPUT" && argv[i + 1][0] != L'/')
			{
				++i;
				inputFiles.push_back(argv[i]);
			}
		}
		else if (arg == L"/OUTPUT")
		{
			++i;
			if (i < argc)
			{
				outputFile = argv[i];
			}
		}
		else if (arg == L"/RESIZE")
		{
			resize = true;
		}
		else if (arg == L"/FORCE32BIT")
		{
			force32bit = true;
		}
		else if (arg.substr(0, 2) == L"/A")
		{
			int index = stoi(arg.substr(2, arg.find(L':') - 2));
			wstring colorCode = arg.substr(arg.find(L':') + 1);
			int r = stoi(colorCode.substr(0, 2), nullptr, 16);
			int g = stoi(colorCode.substr(2, 2), nullptr, 16);
			int b = stoi(colorCode.substr(4, 2), nullptr, 16);
			transparencyColors[index] = RGB(r, g, b);
		}
	}
}

int ConvertImage2Ico(
	ofstream& icoFile, const vector<wstring>& inputFiles,
	map<int, COLORREF>& transparencyColors,
	bool resize,
	bool force32bit)
{
	// アイコンデータ
	vector<vector<BYTE>> imageData;

	// アイコンヘッダー書き込み
	ICONHEADER iconHeader =
	{
		0, 1, static_cast<uint16_t>(inputFiles.size())
	};
	icoFile.write(
		reinterpret_cast<const char*>(&iconHeader), sizeof(ICONHEADER));

	// アイコンのエントリーデータ書き込み
	uint32_t offset = sizeof(ICONHEADER) + sizeof(ICONDIRENTRY) * inputFiles.size();
	for (const auto& inputFile : inputFiles)
	{
		ICONDIRENTRY iconDirEntry;
		vector<BYTE> data;

		try
		{
			COLORREF transparencyColor = transparencyColors.empty() ?
				CLR_INVALID : transparencyColors[&inputFile - &inputFiles[0]];
			data = ConvertImageToARGB(inputFile, transparencyColor, resize, 256, force32bit);
			imageData.push_back(data);
		}
		catch (const exception& e)
		{
			wcerr << L"Error processing " << inputFile << L": " << e.what() << endl;
			return 4;
		}

		iconDirEntry.ImageOffset = offset;
		iconDirEntry.BytesInRes = static_cast<uint32_t>(data.size());
		iconDirEntry.Width = 0;
		iconDirEntry.Height = 0;
		iconDirEntry.ColorCount = 0;
		iconDirEntry.Reserved = 0;
		iconDirEntry.Planes = 1;
		iconDirEntry.BitCount = 32;

		icoFile.write(
			reinterpret_cast<const char*>(&iconDirEntry), sizeof(ICONDIRENTRY));

		offset += iconDirEntry.BytesInRes;
	}

	// アイコンデータ書き込み
	for ( const auto& data : imageData)
	{
		icoFile.write(
			reinterpret_cast<const char*>(data.data()), data.size());
	}

	return 0;
}

int wmain(int argc, wchar_t* argv[])
{
	vector<wstring>			inputFiles;
	wstring					outputFile;
	map<int, COLORREF>		transparencyColors;
	bool					resize = false;
	bool					force32bit = false;

	// 引数の解析
	AnalyzeArgs(argc, argv,
		inputFiles, outputFile, transparencyColors, resize, force32bit);

	// 必要な引数が足りない
	if ( inputFiles.empty() || outputFile.empty())
	{
		wcerr << L"Usage: /INPUT \"file1.png\" \"file2.png\" ... /OUTPUT \"output.ico\" [/resize] [/force32bit] [/a0:RRGGBB] [/a1:RRGGBB] ... " << endl;
		if (inputFiles.empty())
			return 1;
		return 2;
	}

	// GDI+の初期化
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	// 出力ファイル
	int errCode = 0;
	try
	{
		ofstream icoFile(outputFile, ios::binary);
		icoFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

		errCode = ConvertImage2Ico(
			icoFile, inputFiles, transparencyColors, resize, force32bit);
	}
	catch (const exception& e)
	{
		wcerr << L"Error processing " << outputFile << L": " << e.what() << endl;
		errCode = 3;
	}

	// GDI+の終了処理
	::GdiplusShutdown(gdiplusToken);

	// エラーコードからメッセージ表示
	switch (errCode)
	{
		case 3:
			wcerr << L"[Error 3] Output file write error. " << endl;
		case 4:
			wcerr << L"[Error 4] Input file read error. " << endl;
		default:
			break;
	}

	return errCode;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//   1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
