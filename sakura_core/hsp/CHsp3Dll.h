#pragma once
#include <windows.h>

// 基本的に使わない
typedef BOOL(CALLBACK *HSPDLLFUNC)(int, int, int, int);

// こちらを優先
typedef BOOL(CALLBACK *HSPDLLFUNC_IIII)(int, int, int, int);
typedef BOOL(CALLBACK *HSPDLLFUNC_PIII)(void*, int, int, int);
typedef BOOL(CALLBACK *HSPDLLFUNC_PPII)(void*, void*, int, int);
typedef BOOL(CALLBACK *HSPDLLFUNC_IIIP)(int, int, int, void*);

class CHsp3Dll
{

public:
	static const int ANALYSIS_MODE_LABEL = 0;
	static const int ANALYSIS_MODE_VAR = 1;
	static const int ANALYSIS_MODE_ALL = 2;
	static const int ANALYSIS_MODE_REFERENCE = 16;

private:

	// DLLロード成功済み
	bool m_bLoadedDll					= false;
	bool m_bLoadedDll37					= false;

	// DLLインスタンスハンドル
	HINSTANCE m_hDLL					= nullptr;

	// 関数ポインタ
	HSPDLLFUNC_PPII m_hsc_ini			= nullptr;
	HSPDLLFUNC_PPII m_hsc_refname		= nullptr;
	HSPDLLFUNC_PPII m_hsc_objname		= nullptr;
	HSPDLLFUNC_IIII m_hsc_comp			= nullptr;
	HSPDLLFUNC_PIII m_hsc_getmes		= nullptr;
	HSPDLLFUNC_IIII m_hsc_clrmes		= nullptr;
	HSPDLLFUNC_IIIP m_hsc_ver			= nullptr;
	HSPDLLFUNC_IIII m_hsc_bye			= nullptr;
	HSPDLLFUNC_PPII m_pack_ini			= nullptr;
	HSPDLLFUNC_IIII m_pack_make			= nullptr;
	HSPDLLFUNC_IIII m_pack_exe			= nullptr;
	HSPDLLFUNC_IIII m_pack_opt			= nullptr;
	HSPDLLFUNC_PPII m_pack_rt			= nullptr;
	HSPDLLFUNC_IIII m_hsc3_getsym		= nullptr;
	HSPDLLFUNC_PPII m_hsc3_make			= nullptr;
	HSPDLLFUNC_PIII m_hsc3_messize		= nullptr;
	HSPDLLFUNC_PPII m_hsc3_getruntime	= nullptr;		// 3.0用の追加
	HSPDLLFUNC_PIII m_hsc3_run			= nullptr;		// 3.0用の追加

	HSPDLLFUNC_PIII m_hsc3_kwlbuf		= nullptr;		// 3.7
	HSPDLLFUNC_PIII m_hsc3_kwlsize		= nullptr;		// 3.7
	HSPDLLFUNC_IIII m_hsc3_kwlclose		= nullptr;		// 3.7
	HSPDLLFUNC_PPII m_hsc3_analysis		= nullptr;		// 3.7

private:
	// デフォルトコンストラクタ（呼び出し禁止）
	CHsp3Dll() {  };

public:
	// コンストラクタ
	CHsp3Dll(const wchar_t* lpLibFileName)
	{
		m_hDLL = ::LoadLibrary( lpLibFileName);
	};

	// デストラクタ
	~CHsp3Dll()
	{
		if (m_hsc_bye != nullptr)
			m_hsc_bye(0, 0, 0, 0);

		if (m_hDLL != nullptr)
			::FreeLibrary(m_hDLL);
	}


public:
	bool LoadDll()
	{
		if ( m_hDLL == nullptr)
			return false;

		m_hsc_ini			= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc_ini@16");
		m_hsc_refname		= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc_refname@16");
		m_hsc_objname		= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc_objname@16");
		m_hsc_comp			= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_hsc_comp@16");
		m_hsc_getmes		= (HSPDLLFUNC_PIII)::GetProcAddress(m_hDLL, "_hsc_getmes@16");
		m_hsc_clrmes		= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_hsc_clrmes@16");
		m_hsc_ver			= (HSPDLLFUNC_IIIP)::GetProcAddress(m_hDLL, "_hsc_ver@16");
		m_hsc_bye			= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_hsc_bye@16");
		m_pack_ini			= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_pack_ini@16");
		m_pack_make			= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_pack_make@16");
		m_pack_exe			= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_pack_exe@16");
		m_pack_opt			= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_pack_opt@16");
		m_pack_rt			= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_pack_rt@16");
		m_hsc3_getsym		= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_hsc3_getsym@16");
		m_hsc3_make			= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc3_make@16");
		m_hsc3_messize		= (HSPDLLFUNC_PIII)::GetProcAddress(m_hDLL, "_hsc3_messize@16");
		m_hsc3_getruntime	= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc3_getruntime@16");
		m_hsc3_run			= (HSPDLLFUNC_PIII)::GetProcAddress(m_hDLL, "_hsc3_run@16");

		// 3.7
		m_hsc3_kwlbuf		= (HSPDLLFUNC_PIII)::GetProcAddress(m_hDLL, "_hsc3_kwlbuf@16");
		m_hsc3_kwlsize		= (HSPDLLFUNC_PIII)::GetProcAddress(m_hDLL, "_hsc3_kwlsize@16");
		m_hsc3_kwlclose		= (HSPDLLFUNC_IIII)::GetProcAddress(m_hDLL, "_hsc3_kwlclose@16");
		m_hsc3_analysis		= (HSPDLLFUNC_PPII)::GetProcAddress(m_hDLL, "_hsc3_analysis@16");
		IsLoaded37();

		// 返すのは基本のみ
		return IsLoaded();
	}

public:

	// HSP3.0以降のDLLがロードされているか？
	inline bool IsLoaded()
	{
		if ( m_bLoadedDll)
			return true;

		// 各関数の読み込みチェック
		if ((m_hsc_ini				== nullptr)
			|| (m_hsc_refname		== nullptr)
			|| (m_hsc_objname		== nullptr)
			|| (m_hsc_comp			== nullptr)
			|| (m_hsc_getmes		== nullptr)
			|| (m_hsc_clrmes		== nullptr)
			|| (m_hsc_ver			== nullptr)
			|| (m_hsc_bye			== nullptr)
			|| (m_pack_ini			== nullptr)
			|| (m_pack_make			== nullptr)
			|| (m_pack_exe			== nullptr)
			|| (m_pack_opt			== nullptr)
			|| (m_pack_rt			== nullptr)
			|| (m_hsc3_getsym		== nullptr)
			|| (m_hsc3_make			== nullptr)
			|| (m_hsc3_messize		== nullptr)
			|| (m_hsc3_getruntime	== nullptr)
			|| (m_hsc3_run			== nullptr)
			)
		{
			return false;
		}

		m_bLoadedDll = true;
		return true;
	}

	// HSP3.7以降のDLLがロードされているか？
	inline bool IsLoaded37()
	{
		if (m_bLoadedDll37)
			return true;

		// 各関数の読み込みチェック
		if ((m_hsc3_kwlbuf == nullptr)
			|| (m_hsc3_kwlsize == nullptr)
			|| (m_hsc3_kwlclose == nullptr)
			|| (m_hsc3_analysis == nullptr)
			)
		{
			return false;
		}

		m_bLoadedDll37 = true;
		return true;
	}

public:

	inline HSPDLLFUNC_PPII hsc_ini()			const { return m_hsc_ini; };
	inline HSPDLLFUNC_PPII hsc_refname()		const { return m_hsc_refname; };
	inline HSPDLLFUNC_PPII hsc_objname()		const { return m_hsc_objname; };
	inline HSPDLLFUNC_IIII hsc_comp()			const { return m_hsc_comp; };
	inline HSPDLLFUNC_PIII hsc_getmes()			const { return m_hsc_getmes;	};
	inline HSPDLLFUNC_IIII hsc_clrmes()			const { return m_hsc_clrmes; };
	inline HSPDLLFUNC_IIIP hsc_ver()			const { return m_hsc_ver; };
	inline HSPDLLFUNC_IIII hsc_bye()			const { return m_hsc_bye; };
	inline HSPDLLFUNC_PPII pack_ini()			const { return m_pack_ini; };
	inline HSPDLLFUNC_IIII pack_make()			const { return m_pack_make; };
	inline HSPDLLFUNC_IIII pack_exe()			const { return m_pack_exe; };
	inline HSPDLLFUNC_IIII pack_opt()			const { return m_pack_opt; };
	inline HSPDLLFUNC_PPII pack_rt()			const { return m_pack_rt; };
	inline HSPDLLFUNC_IIII hsc3_getsym()		const { return m_hsc3_getsym; };
	inline HSPDLLFUNC_PPII hsc3_make()			const { return m_hsc3_make; };
	inline HSPDLLFUNC_PIII hsc3_messize()		const { return m_hsc3_messize; };
	inline HSPDLLFUNC_PPII hsc3_getruntime()	const { return m_hsc3_getruntime; };
	inline HSPDLLFUNC_PIII hsc3_run()			const { return m_hsc3_run; };

	inline HSPDLLFUNC_PIII hsc3_kwlbuf()		const { return m_hsc3_kwlbuf; };
	inline HSPDLLFUNC_PIII hsc3_kwlsize()		const { return m_hsc3_kwlsize; };
	inline HSPDLLFUNC_IIII hsc3_kwlclose()		const { return m_hsc3_kwlclose; };
	inline HSPDLLFUNC_PPII hsc3_analysis()		const { return m_hsc3_analysis; };
};
