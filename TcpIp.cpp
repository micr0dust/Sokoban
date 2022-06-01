// TcpIp.cpp: 實作檔案
//

#include "pch.h"
#include "Sokoban.h"
#include "afxdialogex.h"
#include "TcpIp.h"


// TcpIp 對話方塊

IMPLEMENT_DYNAMIC(TcpIp, CDialogEx)

TcpIp::TcpIp(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_IP(_T(""))
	, m_PORT(_T(""))
{

}

TcpIp::~TcpIp()
{
}

void TcpIp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IP, m_IP);
	DDX_Text(pDX, IDC_PORT, m_PORT);
}


BEGIN_MESSAGE_MAP(TcpIp, CDialogEx)
END_MESSAGE_MAP()


// TcpIp 訊息處理常式
