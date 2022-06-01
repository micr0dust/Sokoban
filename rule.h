#pragma once
#include "afxdialogex.h"


// rule 對話方塊

class rule : public CDialogEx
{
	DECLARE_DYNAMIC(rule)

public:
	rule(CWnd* pParent = nullptr);   // 標準建構函式
	virtual ~rule();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RULE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
