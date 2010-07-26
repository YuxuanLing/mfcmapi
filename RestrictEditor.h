#pragma once
// RestrictEditor.h : header file
//

#include "Editor.h"

class CRestrictEditor : public CEditor
{
public:
	CRestrictEditor(
		_In_ CWnd* pParentWnd,
		_In_opt_ LPVOID lpAllocParent,
		_In_ LPSRestriction lpRes);
	virtual ~CRestrictEditor();

	_Check_return_ LPSRestriction DetachModifiedSRestriction();

private:
	void  OnEditAction1();
	_Check_return_ BOOL  OnInitDialog();
	_Check_return_ ULONG HandleChange(UINT nID);
	void  OnOK();

	_Check_return_ LPSRestriction GetSourceRes();

	// source variables
	LPSRestriction			m_lpRes;
	LPVOID					m_lpAllocParent;

	// output variable
	LPSRestriction			m_lpOutputRes;
	BOOL					m_bModified;
};


class CCriteriaEditor : public CEditor
{
public:
	CCriteriaEditor(
		_In_ CWnd* pParentWnd,
		_In_ LPSRestriction lpRes,
		_In_ LPENTRYLIST lpEntryList,
		ULONG ulSearchState);
	virtual ~CCriteriaEditor();

	_Check_return_ LPSRestriction DetachModifiedSRestriction();
	_Check_return_ LPENTRYLIST DetachModifiedEntryList();
	_Check_return_ ULONG GetSearchFlags();

private:
	// Use this function to implement list editing
	_Check_return_ BOOL  DoListEdit(ULONG ulListNum, int iItem, _In_ SortListData* lpData);
	void  OnEditAction1();
	_Check_return_ BOOL  OnInitDialog();
	_Check_return_ ULONG HandleChange(UINT nID);
	void  InitListFromEntryList(ULONG ulListNum, _In_ LPENTRYLIST lpEntryList);
	void  OnOK();

	_Check_return_ LPSRestriction GetSourceRes();

	LPSRestriction	m_lpSourceRes;
	LPSRestriction	m_lpNewRes;

	LPENTRYLIST		m_lpSourceEntryList;
	LPENTRYLIST		m_lpNewEntryList;

	ULONG			m_ulNewSearchFlags;
};