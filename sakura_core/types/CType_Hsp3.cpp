/*! @file */
/*
	Copyright (C) 2008, kobake
	Copyright (C) 2018-2022, Sakura Editor Organization

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such,
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/

#include "StdAfx.h"
#include "types/CType.h"
#include "doc/CEditDoc.h"
#include "doc/CDocOutline.h"
#include "doc/logic/CDocLine.h"
#include "outline/CFuncInfoArr.h"
#include "view/Colors/EColorIndexType.h"
#include "CSelectLang.h"
#include "String_define.h"
#include "hsp/CHsp3.h"

/* HSP */
void CType_Hsp3::InitTypeConfigImp(STypeConfig* pType)
{
	//名前と拡張子
	wcscpy( pType->m_szTypeName, L"HSP3" );
	wcscpy( pType->m_szTypeExts, L"hsp,as" );

	//設定
	pType->m_cLineComment.CopyTo( 0, L"//", -1 );					/* 行コメントデリミタ */
	pType->m_cLineComment.CopyTo( 1, L";", -1);						/* 行コメントデリミタ */
	pType->m_cBlockComments[0].SetBlockCommentRule( L"/*", L"*/" );	/* ブロックコメントデリミタ */
	pType->m_nKeyWordSetIdx[0] = 17;								/* キーワードセット */
	pType->m_nKeyWordSetIdx[1] = 18;								/* キーワードセット */
	pType->m_nKeyWordSetIdx[2] = 19;								/* キーワードセット */
	pType->m_eDefaultOutline = OUTLINE_DEFAULT;						/* アウトライン解析方法 */
	pType->m_eSmartIndent = SMARTINDENT_CPP;						/* スマートインデント種別 */
	pType->m_ColorInfoArr[COLORIDX_DIGIT].m_bDisp = true;			//半角数値を色分け表示	//Mar. 10, 2001 JEPRO
	pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_bDisp = true;	//対括弧の強調をデフォルトONに	//Sep. 21, 2002 genta
	pType->m_bStringLineOnly = false;								// 文字列の改行を許す
	pType->m_bUseHokanByKeyword = true;
}

// ここのデータはダミー登録。
// 設定ファイルがない時や完全初期化タイミングで1度きりしか呼ばれないため、
// コンパイラからのキーワード登録を動的に書き換える必要がある。
const wchar_t* g_ppszKeywordsHSP3_FUNC[] = { L"" };
int g_nKeywordsHSP3_FUNC = _countof(g_ppszKeywordsHSP3_FUNC);

const wchar_t* g_ppszKeywordsHSP3_PRE[] = { L"" };
int g_nKeywordsHSP3_PRE = _countof(g_ppszKeywordsHSP3_PRE);

const wchar_t* g_ppszKeywordsHSP3_MACRO[] = { L"" };
int g_nKeywordsHSP3_MACRO = _countof(g_ppszKeywordsHSP3_MACRO);