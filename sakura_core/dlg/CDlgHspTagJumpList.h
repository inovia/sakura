/*!	@file
	@brief HSPタグジャンプリストダイアログボックス

	@author MIK
	@date 2003.4.13
*/
/*
	Copyright (C) 2003, MIK
	Copyright (C) 2005, MIK
	Copyright (C) 2010, Moca
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

#ifndef SAKURA_CDLGTAGJUMPLIST_D44C3C98_9BF7_4B99_923B_9005CD54872F_H_
#define SAKURA_CDLGTAGJUMPLIST_D44C3C98_9BF7_4B99_923B_9005CD54872F_H_
#pragma once

#include "dlg/CDialog.h"
#include "mem/CNativeW.h"

typedef struct hsptagjump_info_t
{
	std::wstring	keyword;	//!< キーワード
	std::wstring	filename;	//!< ファイル名
	int				no;			//!< 行番号
	std::wstring	type;		//!< 種類
	std::wstring	note;		//!< 備考
} HspTagJumpInfo;

/*!	@brief HSPダイレクトタグジャンプ候補一覧ダイアログ

	ダイレクトタグジャンプで複数の候補がある場合のダイアログボックス制御
*/
class CDlgHspTagJumpList final : public CDialog
{
public:
	using TagJumpCallback = std::function<void(const HspTagJumpInfo*)>;
	using CloseCallback = std::function<void(void)>;

	/*
	||  Constructors
	*/
	CDlgHspTagJumpList();
	~CDlgHspTagJumpList();

	/*
	||  Attributes & Operations
	*/
	int DoModal(HINSTANCE hInstance, HWND hwndParent, LPARAM lParam);		/* モーダルダイアログの表示 */

	HWND DoModeless(
		HINSTANCE hInstance, HWND hwndParent, LPARAM lParam, 
		TagJumpCallback	tagJumpCallback,
		CloseCallback closeCallback);	/* モードレスダイアログの表示 */

public:

	inline void UpdateItem()
	{
		UpdateData();
	}

	inline void SetItem(const std::vector<HspTagJumpInfo>& tagList)
	{
		m_tagList.clear();
		std::copy(tagList.begin(), tagList.end(), std::back_inserter(m_tagList));
	}

	inline const HspTagJumpInfo* GetSelectedItem() const
	{
		if ( m_nIndex >= 0 && static_cast<std::size_t>(m_nIndex) < m_tagList.size())
			return &m_tagList[m_nIndex];
		else
			return nullptr;
	}

protected:
	/*
	||  実装ヘルパ関数
	*/
	BOOL	OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam) override;
	BOOL	OnDestroy( void ) override;
	BOOL	OnBnClicked(int wID) override;
	INT_PTR DispatchEvent( HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam ) override;
	BOOL	OnSize( WPARAM wParam, LPARAM lParam ) override;
	BOOL	OnMinMaxInfo( LPARAM lParam );
	BOOL	OnNotify(NMHDR* pNMHDR) override;

private:

	void	SetData( void ) override;	/* ダイアログデータの設定 */
	int		GetData( void ) override;	/* ダイアログデータの取得 */
	void	UpdateData( void );

private:

	int									m_nIndex;		//!< 選択された要素番号
	// const std::wstring					m_pszFileName;	//!< 編集中のファイル名
	std::vector<HspTagJumpInfo>			m_tagList;		//!< タグリスト

	// モードレス用
	TagJumpCallback		m_tagJumpCallback;
	CloseCallback		m_closeCallback;

	// ウィンドウ内のコントロールの位置管理用
	POINT				m_ptDefaultSize;
	RECT				m_rcItems[11];

	DISALLOW_COPY_AND_ASSIGN(CDlgHspTagJumpList);
};
#endif /* SAKURA_CDLGTAGJUMPLIST_D44C3C98_9BF7_4B99_923B_9005CD54872F_H_ */
