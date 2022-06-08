#pragma once
#include <windows.h>

typedef BOOL(CALLBACK *HSPDLLFUNC)(int, int, int, int);

class CHsp3Dll
{

private:

	// DLLロード成功済み
	bool m_bLoadedDll				= false;

	// DLLインスタンスハンドル
	HINSTANCE m_hDLL				= nullptr;

	// 関数ポインタ
	HSPDLLFUNC m_hsc_ini			= nullptr;
	HSPDLLFUNC m_hsc_refname		= nullptr;
	HSPDLLFUNC m_hsc_objname		= nullptr;
	HSPDLLFUNC m_hsc_comp			= nullptr;
	HSPDLLFUNC m_hsc_getmes			= nullptr;
	HSPDLLFUNC m_hsc_clrmes			= nullptr;
	HSPDLLFUNC m_hsc_ver			= nullptr;
	HSPDLLFUNC m_hsc_bye			= nullptr;
	HSPDLLFUNC m_pack_ini			= nullptr;
	HSPDLLFUNC m_pack_make			= nullptr;
	HSPDLLFUNC m_pack_exe			= nullptr;
	HSPDLLFUNC m_pack_opt			= nullptr;
	HSPDLLFUNC m_pack_rt			= nullptr;
	HSPDLLFUNC m_hsc3_getsym		= nullptr;
	HSPDLLFUNC m_hsc3_make			= nullptr;
	HSPDLLFUNC m_hsc3_messize		= nullptr;
	HSPDLLFUNC m_hsc3_getruntime	= nullptr;		// 3.0用の追加
	HSPDLLFUNC m_hsc3_run			= nullptr;		// 3.0用の追加

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

		m_hsc_ini			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_ini@16");
		m_hsc_refname		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_refname@16");
		m_hsc_objname		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_objname@16");
		m_hsc_comp			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_comp@16");
		m_hsc_getmes		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_getmes@16");
		m_hsc_clrmes		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_clrmes@16");
		m_hsc_ver			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_ver@16");
		m_hsc_bye			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc_bye@16");
		m_pack_ini			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_pack_ini@16");
		m_pack_make			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_pack_make@16");
		m_pack_exe			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_pack_exe@16");
		m_pack_opt			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_pack_opt@16");
		m_pack_rt			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_pack_rt@16");
		m_hsc3_getsym		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc3_getsym@16");
		m_hsc3_make			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc3_make@16");
		m_hsc3_messize		= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc3_messize@16");
		m_hsc3_getruntime	= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc3_getruntime@16");
		m_hsc3_run			= (HSPDLLFUNC)::GetProcAddress(m_hDLL, "_hsc3_run@16");

		return IsLoaded();
	}

public:
	inline bool IsLoaded()
	{
		if ( m_bLoadedDll)
			return true;

		// 読み込み済みフラグが立っていないのでチェック
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

public:

	inline HSPDLLFUNC hsc_ini()			const { return m_hsc_ini; };
	inline HSPDLLFUNC hsc_refname()		const { return m_hsc_refname; };
	inline HSPDLLFUNC hsc_objname()		const { return m_hsc_objname; };
	inline HSPDLLFUNC hsc_comp()		const { return m_hsc_comp; };
	inline HSPDLLFUNC hsc_getmes()		const { return m_hsc_getmes;	};
	inline HSPDLLFUNC hsc_clrmes()		const { return m_hsc_clrmes; };
	inline HSPDLLFUNC hsc_ver()			const { return m_hsc_ver; };
	inline HSPDLLFUNC hsc_bye()			const { return m_hsc_bye; };
	inline HSPDLLFUNC pack_ini()		const { return m_pack_ini; };
	inline HSPDLLFUNC pack_make()		const { return m_pack_make; };
	inline HSPDLLFUNC pack_exe()		const { return m_pack_exe; };
	inline HSPDLLFUNC pack_opt()		const { return m_pack_opt; };
	inline HSPDLLFUNC pack_rt()			const { return m_pack_rt; };
	inline HSPDLLFUNC hsc3_getsym()		const { return m_hsc3_getsym; };
	inline HSPDLLFUNC hsc3_make()		const { return m_hsc3_make; };
	inline HSPDLLFUNC hsc3_messize()	const { return m_hsc3_messize; };
	inline HSPDLLFUNC hsc3_getruntime() const { return m_hsc3_getruntime; };
	inline HSPDLLFUNC hsc3_run()		const { return m_hsc3_run; };
	
};
