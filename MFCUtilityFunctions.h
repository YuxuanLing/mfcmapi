#pragma once
// MFCUtilityFunctions.h : Common functions for MFC MAPI

#include "BaseDialog.h"
#include "ParentWnd.h"
#include "MAPIObjects.h"

enum ObjectType
{
	otDefault,
	otAssocContents,
	otContents,
	otHierarchy,
	otACL,
	otStatus,
	otReceive,
	otRules,
	otStore,
	otStoreDeletedItems,
};

_Check_return_ HRESULT DisplayObject(
					  _In_ LPMAPIPROP lpUnk,
					  ULONG ulObjType,
					  ObjectType Table,
					  _In_ CBaseDialog* lpHostDlg);

_Check_return_ HRESULT DisplayExchangeTable(
							 _In_ LPMAPIPROP lpMAPIProp,
							 ULONG ulPropTag,
							 ObjectType tType,
							 _In_ CBaseDialog* lpHostDlg);

_Check_return_ HRESULT DisplayTable(
					 _In_ LPMAPITABLE lpTable,
					 ObjectType tType,
					 _In_ CBaseDialog* lpHostDlg);

_Check_return_ HRESULT DisplayTable(
					 _In_ LPMAPIPROP lpMAPIProp,
					 ULONG ulPropTag,
					 ObjectType tType,
					 _In_ CBaseDialog* lpHostDlg);

void UpdateMenuString(_In_ CWnd* cWnd, UINT uiMenuTag, UINT uidNewString);

void DisplayContextMenu(UINT uiClassMenu, UINT uiControlMenu, _In_ CWnd* pParent, int x, int y);

_Check_return_ int GetEditHeight(_In_ HWND hwndEdit);
_Check_return_ int GetTextHeight(_In_ HWND hwndEdit);

_Check_return_ BOOL bShouldCancel(_In_opt_ CWnd* cWnd, HRESULT hRes);

void DisplayMailboxTable(_In_ CParentWnd*	lpParent,
						 _In_ CMapiObjects* lpMapiObjects);
void DisplayPublicFolderTable(_In_ CParentWnd* lpParent,
							  _In_ CMapiObjects* lpMapiObjects);