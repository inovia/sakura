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
	// pType->m_encoding.m_eDefaultCodetype = CODE_SJIS;					

	// 色設定初期値
	pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_bDisp				= true;
	pType->m_ColorInfoArr[COLORIDX_CARET].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_CARETLINEBG].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_COMMENT].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_CTRLCODE].m_bDisp					= false;
	pType->m_ColorInfoArr[COLORIDX_CURSORVLINE].m_bDisp					= false;
	pType->m_ColorInfoArr[COLORIDX_DIFF_APPEND].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_DIFF_CHANGE].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_DIFF_DELETE].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_EVENLINEBG].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_EOF].m_bDisp							= true;
	pType->m_ColorInfoArr[COLORIDX_EOL].m_bDisp							= false;
	pType->m_ColorInfoArr[COLORIDX_SEARCH2].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH3].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH4].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH5].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_HEREDOC].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_CARET_IME].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD1].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD2].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD3].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD4].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD5].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD6].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD7].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD8].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD9].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_KEYWORD10].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_GYOU].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_GYOU_MOD].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_MARK].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_NOTELINE].m_bDisp					= false;
	pType->m_ColorInfoArr[COLORIDX_DIGIT].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_PAGEVIEW].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_WRAP].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX1].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_REGEX2].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX3].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX4].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX5].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX6].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX7].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX8].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX9].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_REGEX10].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_RULER].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SELECT].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_SPACE].m_bDisp						= false;
	pType->m_ColorInfoArr[COLORIDX_SSTRING].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_TAB].m_bDisp							= false;
	pType->m_ColorInfoArr[COLORIDX_TEXT].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_UNDERLINE].m_bDisp					= true;
	pType->m_ColorInfoArr[COLORIDX_URL].m_bDisp							= true;
	pType->m_ColorInfoArr[COLORIDX_VERTLINE].m_bDisp					= false;
	pType->m_ColorInfoArr[COLORIDX_WSTRING].m_bDisp						= true;
	pType->m_ColorInfoArr[COLORIDX_ZENSPACE].m_bDisp					= false;

	pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_sColorAttr.m_cTEXT	= RGB(0xff, 0x9d, 0x3c);
	pType->m_ColorInfoArr[COLORIDX_CARET].m_sColorAttr.m_cTEXT			= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_CARETLINEBG].m_sColorAttr.m_cTEXT	= RGB(0x00, 0x00, 0x00);
	pType->m_ColorInfoArr[COLORIDX_COMMENT].m_sColorAttr.m_cTEXT		= RGB(0x6a, 0x99, 0x55);
	pType->m_ColorInfoArr[COLORIDX_CTRLCODE].m_sColorAttr.m_cTEXT		= RGB(0xff, 0xff, 0x00);
	pType->m_ColorInfoArr[COLORIDX_CURSORVLINE].m_sColorAttr.m_cTEXT	= RGB(0x85, 0x85, 0x85);
	pType->m_ColorInfoArr[COLORIDX_DIFF_APPEND].m_sColorAttr.m_cTEXT	= RGB(0x00, 0x00, 0xd2);
	pType->m_ColorInfoArr[COLORIDX_DIFF_CHANGE].m_sColorAttr.m_cTEXT	= RGB(0x00, 0x6f, 0x00);
	pType->m_ColorInfoArr[COLORIDX_DIFF_DELETE].m_sColorAttr.m_cTEXT	= RGB(0xd5, 0x6a, 0x00);
	pType->m_ColorInfoArr[COLORIDX_EVENLINEBG].m_sColorAttr.m_cTEXT		= RGB(0x00, 0x00, 0x00);
	pType->m_ColorInfoArr[COLORIDX_EOF].m_sColorAttr.m_cTEXT			= RGB(0x00, 0xff, 0xff);
	pType->m_ColorInfoArr[COLORIDX_EOL].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x80, 0xff);
	pType->m_ColorInfoArr[COLORIDX_SEARCH2].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_SEARCH3].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_SEARCH4].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_SEARCH5].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_SEARCH].m_sColorAttr.m_cTEXT			= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_HEREDOC].m_sColorAttr.m_cTEXT		= RGB(0xce, 0x91, 0x78);
	pType->m_ColorInfoArr[COLORIDX_CARET_IME].m_sColorAttr.m_cTEXT		= RGB(0xff, 0x00, 0x00);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD1].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD2].m_sColorAttr.m_cTEXT		= RGB(0xc5, 0x86, 0xb6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD3].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD4].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD5].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD6].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD7].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD8].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD9].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD10].m_sColorAttr.m_cTEXT		= RGB(0x56, 0x9c, 0xd6);
	pType->m_ColorInfoArr[COLORIDX_GYOU].m_sColorAttr.m_cTEXT			= RGB(0x85, 0x85, 0x85);
	pType->m_ColorInfoArr[COLORIDX_GYOU_MOD].m_sColorAttr.m_cTEXT		= RGB(0xff, 0xff, 0xff);
	pType->m_ColorInfoArr[COLORIDX_MARK].m_sColorAttr.m_cTEXT			= RGB(0xff, 0xfb, 0xf0);
	pType->m_ColorInfoArr[COLORIDX_NOTELINE].m_sColorAttr.m_cTEXT		= RGB(0xc0, 0xc0, 0xff);
	pType->m_ColorInfoArr[COLORIDX_DIGIT].m_sColorAttr.m_cTEXT			= RGB(0xb5, 0xce, 0xa8);
	pType->m_ColorInfoArr[COLORIDX_PAGEVIEW].m_sColorAttr.m_cTEXT		= RGB(0xff, 0xfb, 0xf0);
	pType->m_ColorInfoArr[COLORIDX_WRAP].m_sColorAttr.m_cTEXT			= RGB(0xff, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX1].m_sColorAttr.m_cTEXT			= RGB(0xff, 0xff, 0x80);
	pType->m_ColorInfoArr[COLORIDX_REGEX2].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX3].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX4].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX5].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX6].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX7].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX8].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX9].m_sColorAttr.m_cTEXT			= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_REGEX10].m_sColorAttr.m_cTEXT		= RGB(0x00, 0x00, 0xff);
	pType->m_ColorInfoArr[COLORIDX_RULER].m_sColorAttr.m_cTEXT			= RGB(0x85, 0x85, 0x85);
	pType->m_ColorInfoArr[COLORIDX_SELECT].m_sColorAttr.m_cTEXT			= RGB(0xc9, 0xd8, 0xf1);
	pType->m_ColorInfoArr[COLORIDX_SPACE].m_sColorAttr.m_cTEXT			= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_SSTRING].m_sColorAttr.m_cTEXT		= RGB(0xce, 0x91, 0x78);
	pType->m_ColorInfoArr[COLORIDX_TAB].m_sColorAttr.m_cTEXT			= RGB(0x80, 0x80, 0x80);
	pType->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cTEXT			= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_UNDERLINE].m_sColorAttr.m_cTEXT		= RGB(0x85, 0x85, 0x85);
	pType->m_ColorInfoArr[COLORIDX_URL].m_sColorAttr.m_cTEXT			= RGB(0xce, 0x91, 0x78);
	pType->m_ColorInfoArr[COLORIDX_VERTLINE].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);
	pType->m_ColorInfoArr[COLORIDX_WSTRING].m_sColorAttr.m_cTEXT		= RGB(0xce, 0x91, 0x78);
	pType->m_ColorInfoArr[COLORIDX_ZENSPACE].m_sColorAttr.m_cTEXT		= RGB(0xe1, 0xe1, 0xe1);

	pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_sColorAttr.m_cBACK	= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_CARET].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_CARETLINEBG].m_sColorAttr.m_cBACK	= RGB(0x3c, 0x3c, 0x3c);
	pType->m_ColorInfoArr[COLORIDX_COMMENT].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_CTRLCODE].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_CURSORVLINE].m_sColorAttr.m_cBACK	= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_DIFF_APPEND].m_sColorAttr.m_cBACK	= RGB(0xa2, 0xd0, 0xff);
	pType->m_ColorInfoArr[COLORIDX_DIFF_CHANGE].m_sColorAttr.m_cBACK	= RGB(0xbd, 0xfd, 0xc0);
	pType->m_ColorInfoArr[COLORIDX_DIFF_DELETE].m_sColorAttr.m_cBACK	= RGB(0xff, 0xe9, 0xac);
	pType->m_ColorInfoArr[COLORIDX_EVENLINEBG].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_EOF].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_EOL].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_SEARCH2].m_sColorAttr.m_cBACK		= RGB(0xa0, 0xff, 0xff);
	pType->m_ColorInfoArr[COLORIDX_SEARCH3].m_sColorAttr.m_cBACK		= RGB(0x99, 0xff, 0x99);
	pType->m_ColorInfoArr[COLORIDX_SEARCH4].m_sColorAttr.m_cBACK		= RGB(0xff, 0x99, 0x99);
	pType->m_ColorInfoArr[COLORIDX_SEARCH5].m_sColorAttr.m_cBACK		= RGB(0xff, 0x66, 0xff);
	pType->m_ColorInfoArr[COLORIDX_SEARCH].m_sColorAttr.m_cBACK			= RGB(0x80, 0x40, 0x40);
	pType->m_ColorInfoArr[COLORIDX_HEREDOC].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_CARET_IME].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD1].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD2].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD3].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD4].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD5].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD6].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD7].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD8].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD9].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_KEYWORD10].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_GYOU].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_GYOU_MOD].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_MARK].m_sColorAttr.m_cBACK			= RGB(0x00, 0x80, 0xc0);
	pType->m_ColorInfoArr[COLORIDX_NOTELINE].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_DIGIT].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_PAGEVIEW].m_sColorAttr.m_cBACK		= RGB(0xbe, 0xe6, 0xff);
	pType->m_ColorInfoArr[COLORIDX_WRAP].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX1].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX2].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX3].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX4].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX5].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX6].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX7].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX8].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX9].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_REGEX10].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_RULER].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_SELECT].m_sColorAttr.m_cBACK			= RGB(0x28, 0x57, 0xa4);
	pType->m_ColorInfoArr[COLORIDX_SPACE].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_SSTRING].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_TAB].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_TEXT].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_UNDERLINE].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_URL].m_sColorAttr.m_cBACK			= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_VERTLINE].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_WSTRING].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);
	pType->m_ColorInfoArr[COLORIDX_ZENSPACE].m_sColorAttr.m_cBACK		= RGB(0x1e, 0x1e, 0x1e);

	pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_sFontAttr.m_bBoldFont	= true;
	pType->m_ColorInfoArr[COLORIDX_CURSORVLINE].m_sFontAttr.m_bBoldFont		= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH2].m_sFontAttr.m_bBoldFont			= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH3].m_sFontAttr.m_bBoldFont			= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH4].m_sFontAttr.m_bBoldFont			= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH5].m_sFontAttr.m_bBoldFont			= true;
	pType->m_ColorInfoArr[COLORIDX_SEARCH].m_sFontAttr.m_bBoldFont			= true;
	pType->m_ColorInfoArr[COLORIDX_GYOU_MOD].m_sFontAttr.m_bBoldFont		= true;

	pType->m_ColorInfoArr[COLORIDX_URL].m_sFontAttr.m_bUnderLine			= true;

	// 正規表現キーワード
	int keywordPos = 0;
	wchar_t* pKeyword = pType->m_RegexKeywordList;
	pType->m_bUseRegexKeyword = true;
	// pType->m_ColorInfoArr[COLORIDX_REGEX1].m_bDisp = true;
	pType->m_ColorInfoArr[COLORIDX_REGEX1].m_nColorIdx = COLORIDX_REGEX1;
	::wcscpyn(&pKeyword[keywordPos],											// 正規表現キーワード
		L"/(?<!\\w)\\*(\\b[a-zA-Z_x7f-xff][a-zA-Z0-9_x7f-xff]*?\\b)/k",		// ラベルを検出するパターン
		_countof(pType->m_RegexKeywordList) - 1);
	keywordPos += ::wcslen(&pKeyword[keywordPos]) + 1;
	pKeyword[keywordPos] = L'\0';

	pType->m_eDefaultOutline = OUTLINE_HSP3;						/* アウトライン解析方法 */
	pType->m_eSmartIndent = SMARTINDENT_CPP;						/* スマートインデント種別 */
	// pType->m_ColorInfoArr[COLORIDX_DIGIT].m_bDisp = true;			//半角数値を色分け表示	//Mar. 10, 2001 JEPRO
	// pType->m_ColorInfoArr[COLORIDX_BRACKET_PAIR].m_bDisp = true;	//対括弧の強調をデフォルトONに	//Sep. 21, 2002 genta

	pType->m_bStringLineOnly = false;								// 文字列の改行を許す
	pType->m_bUseHokanByKeyword = true;
	pType->m_bHokanLoHiCase = true;

}

/* HSP3解析モード */
enum EFuncListHSP3Mode
{
	FL_HSP3_MODE_NORMAL = 0,
	// FL_HSP3_MODE_WORD = 1,
	// FL_HSP3_MODE_SYMBOL = 2,
	FL_HSP3_MODE_COMMENT = 8,						// 複数行コメント
	FL_HSP3_MODE_SINGLE_QUOTE = 20,
	FL_HSP3_MODE_DOUBLE_QUOTE = 21,
	FL_HSP3_MODE_DOUBLE_QUOTE_MULTILINE = 22,		//	{""} 形式の文字列
	FL_HSP3_MODE_DEFINE_LABEL = 100,				// 定義されているラベルのみ検出。gosub *Label みたいなのは検出しない。
	FL_HSP3_MODE_PREPROCESSOR = 101,				// #define, #func, #deffunc などプリプロセッサ全般を検出
	// FL_HSP3_MODE_TOO_LONG_WORD = 999
};

const int nMaxWordLeng = 255;		// 関数名/ラベル名の最大文字数

static void AppendData(CFuncInfoArr* pcFuncInfoArr, CEditDoc* pcDocRef, const wchar_t* szName, int x, int y, int nDataType)
{
	/*
	カーソル位置変換
	物理位置(行頭からのバイト数、折り返し無し行位置)
	→
	レイアウト位置(行頭からの表示桁位置、折り返しあり行位置)
	*/
	CLogicPoint  ptPosXY_Logic = CLogicPoint(CLogicInt(x), y);
	CLayoutPoint ptPosXY_Layout;
	pcDocRef->m_cLayoutMgr.LogicToLayout(
		ptPosXY_Logic,
		&ptPosXY_Layout
	);

	wchar_t	szNameCopy[nMaxWordLeng + 1];
	::wcscpy(szNameCopy, szName);

	int nInfo = -1;
	switch (nDataType)
	{
		case 0:			// ラベル
			nInfo = 0;
			break;
		case 1:			// #deffunc / #defcfunc
		{
			if ( ::_wcsnicmp(szNameCopy, L"#deffunc ", 9) == 0)
			{
				nInfo = 1;
				::wcscpy(szNameCopy, szNameCopy + 9);	// #deffunc を削る
			}
			else if ( ::_wcsnicmp(szNameCopy, L"#defcfunc ", 10) == 0)
			{
				nInfo = 2;
				::wcscpy(szNameCopy, szNameCopy + 10);	// #defcfunc を削る
			}
		}
		break;
	}

	pcFuncInfoArr->AppendData(
		ptPosXY_Logic.GetY2() + CLogicInt(1), ptPosXY_Layout.GetY2() + CLayoutInt(1), szNameCopy, nInfo);
}

/*
	memo: 
		- #define などのプリプロセッサは 末尾が \ の場合は改行ができる
		  なお、\の後ろに空白やタブ文字は許容されないので、検出は割と簡単
		- ラベルは通常、列のはじめに *Label と記述するが、コロンの後でも認識できる（ mes "a" : *b ）
		  なので、 コロンは C++ や Java のセミコロン的な扱いにするのが良さそう
*/

void CDocOutline::MakeFuncList_HSP3(CFuncInfoArr* pcFuncInfoArr)
{
	const wchar_t*		pLine;
	CLogicInt			nLineLen;
	int					i;
	CLogicInt			nLineCount;
	int					nCharChars;
	EFuncListHSP3Mode	nMode;
	bool				bTopStatement;								// 先頭のステートメントか？(行が変更されるか、行内でコロンがある場合は先頭扱い)
																	//  mes "a" : *Label
																	//  ::: #deffunc test
																	// みたいなケースの検出用

	bool				bDetectSupportedPP;							// サポートしているプリプロセッサを検出フラグ
																	// 今回は #deffunc / #defcfunc の場合はフラグが立つ
	bool				bDetectLocalFunc;							// ローカル宣言されているかのフラグ
	bool				bExitExtractPP;								// プリプロセッサ内解析終了フラグ

	
	wchar_t				szFuncName[nMaxWordLeng + 1]	= {0};		// #deffunc / #defcfunc の名前
	wchar_t				szLabelName[nMaxWordLeng + 1]	= {0};		// ラベル名 *Label

	wchar_t				szTemp[16]						= { 0 };	// 一時用

	nMode = FL_HSP3_MODE_NORMAL;

	// 毎行スキャン
	for (nLineCount = CLogicInt(0); nLineCount < m_pcDocRef->m_cDocLineMgr.GetLineCount(); ++nLineCount)
	{
		// 1行分データ
		pLine = m_pcDocRef->m_cDocLineMgr.GetLine(nLineCount)->GetDocLineStrWithEOL(&nLineLen);
		bTopStatement = true;

		// 1行内の文字を1文字ずつスキャン
		for (i = 0; i < nLineLen; i += nCharChars)
		{
			// 指定した位置の文字がwchar_t何個分か
			nCharChars = CNativeW::GetSizeOfChar(pLine, nLineLen, i);

			// シングルクォーテーション文字列を読み取り中
			if ( FL_HSP3_MODE_SINGLE_QUOTE == nMode)
			{
				if (L'\'' == pLine[i])
				{
					nMode = FL_HSP3_MODE_NORMAL;
					continue;
				}
			}
			else
			// ダブルクォーテーション文字列を読み込み中
			if (FL_HSP3_MODE_DOUBLE_QUOTE == nMode)
			{
				if (L'"' == pLine[i])
				{
					nMode = FL_HSP3_MODE_NORMAL;
					continue;
				}
			}
			else
			// ダブルクォーテーション文字列({""} 形式の文字列)を読み込み中
			if (FL_HSP3_MODE_DOUBLE_QUOTE_MULTILINE == nMode)
			{
				if (i < nLineLen - 1 && L'"' == pLine[i] && L'}' == pLine[i + 1])
				{
					++i;
					nMode = FL_HSP3_MODE_NORMAL;
					continue;
				}
			}
			else
			// 複数行コメントを読み込み中
			if (FL_HSP3_MODE_COMMENT == nMode)
			{
				if (i < nLineLen - 1 && L'*' == pLine[i] && L'/' == pLine[i + 1])
				{
					++i;
					nMode = FL_HSP3_MODE_NORMAL;
					continue;
				}
			}
			else
			//// 長過ぎる単語無視中
			//if (FL_HSP3_MODE_TOO_LONG_WORD == nMode)
			//{
			//	// 空白やタブ記号等を飛ばす
			//	if ( WCODE::IsBlank(pLine[i]) || WCODE::IsLineDelimiter(pLine[i], false))
			//	{
			//		nMode = FL_HSP3_MODE_NORMAL;
			//		continue;
			//	}
			//}
			//else
			// 定義されたラベルを読み取り中
			if (FL_HSP3_MODE_DEFINE_LABEL == nMode)
			{
				// 終了判定は 改行、スペース、タブ、コメント(/*,//,;)、コロン、文字数オーバー
				if ( WCODE::IsBlank(pLine[i])
					|| WCODE::IsLineDelimiter(pLine[i], false)
					|| pLine[i] == ';'
					|| pLine[i] == ':'
					)
				{
					AppendData(pcFuncInfoArr, m_pcDocRef, szLabelName, 0, nLineCount, 0);
					nMode = FL_HSP3_MODE_NORMAL;
					szLabelName[0] = L'\0';
					continue;
				}
				else if (i < nLineLen - 1 && L'/' == pLine[i] && L'*' == pLine[i + 1])
				{
					AppendData(pcFuncInfoArr, m_pcDocRef, szLabelName, 0, nLineCount, 0);
					++i;
					nMode = FL_HSP3_MODE_NORMAL;
					szLabelName[0] = L'\0';
					continue;
				}
				else if (i < nLineLen - 1 && L'/' == pLine[i] && L'/' == pLine[i + 1])
				{
					AppendData(pcFuncInfoArr, m_pcDocRef, szLabelName, 0, nLineCount, 0);
					++i;
					nMode = FL_HSP3_MODE_NORMAL;
					szLabelName[0] = L'\0';
					continue;
				}
				else
				{
					// 厳密には記号なども弾くべきなんだろうけど、
					// どうせコンパイルエラーになるので、
					// 記述エラー箇所の見つけやすさというのためにあえてチェックしない

					// サロゲートペア分の文字も一緒に処理する
					for (int k = 0; k < nCharChars; k++)
					{
						szTemp[k] = pLine[i + k];
						if (k == nCharChars - 1)	// 終端チェック
							szTemp[k + 1] = L'\0';
					}
					
					::wcscat_s(szLabelName, nMaxWordLeng, szTemp);
				}
			}
			else
			// プリプロセッサを読み取り中
			if ( FL_HSP3_MODE_PREPROCESSOR == nMode)
			{
				// 継続判定は スペース、タブ、末尾の\、localという文字列、(#deffunc, #defcfunc以外)
				// 終了判定は 改行、コメント(/*,//,;)、文字数オーバー

				if (i < nLineLen - 2
					&& L'\\' == pLine[i]
					&& L'\r' == pLine[i + 1]
					&& L'\n' == pLine[i + 2])
				{
					// 継続判定：末尾の\マーク + (CRLF)
					i += 2;
					continue;
				}
				else if (i < nLineLen - 1
					&& L'\\' == pLine[i]
					&& WCODE::IsLineDelimiter(pLine[i + 1], false))
				{
					// 継続判定：末尾の\マーク + (CR OR LF)
					++i;
					continue;
				}
				else if ( WCODE::IsBlank(pLine[i]) )
				{
					//
					// 継続判定：スペース、タブ
					//

					if (!bDetectSupportedPP)	// サポート対象のプリプロセッサが未確定状態
					{
						if ( ::_wcsnicmp(szFuncName, L"#deffunc", 8) == 0)
						{
							// #deffunc?
							// 小文字に統一しつつ、末尾にスペースを付与
							::wcscpy_s(szFuncName, nMaxWordLeng, L"#deffunc ");
							bDetectSupportedPP = true;
							continue;
						}
						else if (::_wcsnicmp(szFuncName, L"#defcfunc", 9) == 0)
						{
							// #defcfunc?
							// 小文字に統一しつつ、末尾にスペースを付与
							::wcscpy_s(szFuncName, nMaxWordLeng, L"#defcfunc ");
							bDetectSupportedPP = true;
							continue;
						}
					}

					if (!bDetectLocalFunc)
					{
						// local 宣言かチェックする
						if (::_wcsnicmp(szFuncName, L"#deffunc local", 14) == 0)
						{
							// #deffunc local?
							// 小文字に統一しつつ、localを削除し、末尾にスペースを付与
							::wcscpy_s(szFuncName, nMaxWordLeng, L"#deffunc ");
							bDetectLocalFunc = true;
							continue;
						}
						else if (::_wcsnicmp(szFuncName, L"#defcfunc local", 15) == 0)
						{
							// #defcfunc local?
							// 小文字に統一しつつ、localを削除し、末尾にスペースを付与
							::wcscpy_s(szFuncName, nMaxWordLeng, L"#defcfunc ");
							bDetectLocalFunc = true;
							continue;
						}
					}

					if ( bDetectSupportedPP || bDetectLocalFunc)
					{
						// 末尾がスペースのままの場合はスキップ
						size_t len = ::wcslen(szFuncName);
						if (0 < len && szFuncName[len - 1] == L' ')
						{
							continue;
						}
					}
					else
					{
						szFuncName[0] = L'\0';
						bExitExtractPP = true;	// これ以上の解析は不要
						continue;
					}

					if (!bExitExtractPP && bDetectSupportedPP)
					{
						AppendData(pcFuncInfoArr, m_pcDocRef, szFuncName, 0, nLineCount, 1);
					}

					szFuncName[0] = L'\0';
					bExitExtractPP = true;	// これ以上の解析は不要
					continue;
				}
				else if ( WCODE::IsLineDelimiter(pLine[i], false))
				{
					if (!bExitExtractPP && bDetectSupportedPP)
					{
						AppendData(pcFuncInfoArr, m_pcDocRef, szFuncName, 0, nLineCount, 1);
					}
					nMode = FL_HSP3_MODE_NORMAL;
					szFuncName[0] = L'\0';
					continue;
				}
				else if ( L';' == pLine[i])
				{
					if (!bExitExtractPP && bDetectSupportedPP)
					{
						AppendData(pcFuncInfoArr, m_pcDocRef, szFuncName, 0, nLineCount, 1);
					}
					nMode = FL_HSP3_MODE_NORMAL;
					szFuncName[0] = L'\0';
					continue;
				}
				else if (i < nLineLen - 1
					&& L'/' == pLine[i]
					&& L'/' == pLine[i + 1])
				{
					if (!bExitExtractPP && bDetectSupportedPP)
					{
						AppendData(pcFuncInfoArr, m_pcDocRef, szFuncName, 0, nLineCount, 1);
					}
					nMode = FL_HSP3_MODE_NORMAL;
					szFuncName[0] = L'\0';
					continue;
				}
				else if (i < nLineLen - 1
					&& L'/' == pLine[i]
					&& L'*' == pLine[i + 1])
				{
					if (!bExitExtractPP && bDetectSupportedPP)
					{
						AppendData(pcFuncInfoArr, m_pcDocRef, szFuncName, 0, nLineCount, 1);
					}
					nMode = FL_HSP3_MODE_COMMENT;	// プリプロセッサ内でも複数行コメントは使えるが制限がある
													//  - ワンライナーの複数行コメントは使える ( #deffunc test int a /*a*/ )
													//  - ただし、下記のようなものは不可
													//    #deffunc test/*a*/int a
													//    #deffunc /*a*/test int a
					szFuncName[0] = L'\0';
					continue;
				}
				else if ( bExitExtractPP)
				{
					// 解析不要フラグ（対象外と分かっているので）
					continue;
				}
				else
				{
					// 厳密には記号なども弾くべきなんだろうけど、
					// どうせコンパイルエラーになるので、
					// 記述エラー箇所の見つけやすさというのためにあえてチェックしない

					// サロゲートペア分の文字も一緒に処理する
					for (int k = 0; k < nCharChars; k++)
					{
						szTemp[k] = pLine[i + k];
						if (k == nCharChars - 1)	// 終端チェック
							szTemp[k + 1] = L'\0';
					}

					::wcscat_s(szFuncName, nMaxWordLeng, szTemp);
				}
			}
			else
			// ノーマルモード
			if ( FL_HSP3_MODE_NORMAL == nMode)
			{
				// 空白やタブ記号、改行コード等を読み飛ばす
				if ( WCODE::IsBlank(pLine[i]) || WCODE::IsLineDelimiter(pLine[i], false))
				{
					continue;
				}
				else
				// 単一行コメントは行すべて読み飛ばす
				if (i < nLineLen - 1 && L'/' == pLine[i] && L'/' == pLine[i + 1])
				{
					break;
				}
				else
				// 単一行コメントは行すべて読み飛ばす
				if ( L';' == pLine[i])
				{
					break;
				}
				else
				// 複数行コメント
				if (i < nLineLen - 1 && L'/' == pLine[i] && L'*' == pLine[i + 1])
				{
					++i;
					nMode = FL_HSP3_MODE_COMMENT;
					continue;
				}
				else
				// シングルクォーテーション
				if (L'\'' == pLine[i])
				{
					nMode = FL_HSP3_MODE_SINGLE_QUOTE;
					continue;
				}
				else
				// ダブルクォーテーション
				if (L'"' == pLine[i])
				{
					nMode = FL_HSP3_MODE_DOUBLE_QUOTE;
					continue;
				}
				else
				// 複数行文字列
				if (i < nLineLen - 1 && L'{' == pLine[i] && L'"' == pLine[i + 1])
				{
					++i;
					nMode = FL_HSP3_MODE_DOUBLE_QUOTE_MULTILINE;
					continue;
				}
				else
				// コロン
				if (L':' == pLine[i])
				{
					bTopStatement = true;
					continue;
				}
				else
				// ラベル
				if ( bTopStatement && L'*' == pLine[i])
				{
					szLabelName[0] = L'*';
					szLabelName[1] = L'\0';
					nMode = FL_HSP3_MODE_DEFINE_LABEL;
					continue;
				}
				else
				// プリプロセッサ
				if ( bTopStatement && L'#' == pLine[i])
				{
					szFuncName[0] = L'#';
					szFuncName[1] = L'\0';
					nMode = FL_HSP3_MODE_PREPROCESSOR;
					bDetectSupportedPP = false;	// サポートしているプリプロセッサを検出フラグ
					bDetectLocalFunc = false;	// ローカル宣言検出フラグ
					bExitExtractPP = false;		// プリプロセッサ内解析終了フラグ
					continue;
				}
				else
				// それ以外
				{
					bTopStatement = false;	// 先頭ステートメントではないことが確定
					continue;
				}
			}
		}
	}

	// ラベルが残っている場合はここで出力（最終行に改行がないケース）
	if ( szLabelName[0] != L'\0')
	{
		AppendData(pcFuncInfoArr, m_pcDocRef,
			szLabelName, 0, nLineCount - 1/* デクリメント */, 0);
		szLabelName[0] = L'\0';
	}

	// 関数が残っている場合はここで出力（最終行に改行がないケース）
	if ( szFuncName[0] != L'\0')
	{
		if (!bExitExtractPP && bDetectSupportedPP)
		{
			AppendData(pcFuncInfoArr, m_pcDocRef,
				szFuncName, 0, nLineCount - 1/* デクリメント */, 1);
		}
		szFuncName[0] = L'\0';
	}

#ifdef _DEBUG
	pcFuncInfoArr->DUMP();
#endif
	return;
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