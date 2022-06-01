#pragma once
#include "afxdialogex.h"


// TcpIp 對話方塊

class TcpIp : public CDialogEx
{
	DECLARE_DYNAMIC(TcpIp)

public:
	TcpIp(CWnd* pParent = nullptr);   // 標準建構函式
	virtual ~TcpIp();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CString m_IP;
	CString m_PORT;
};
