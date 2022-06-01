// rule.cpp: 實作檔案
//

#include "pch.h"
#include "Sokoban.h"
#include "afxdialogex.h"
#include "rule.h"


// rule 對話方塊

IMPLEMENT_DYNAMIC(rule, CDialogEx)

rule::rule(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RULE, pParent)
{

}

rule::~rule()
{
}

void rule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(rule, CDialogEx)
END_MESSAGE_MAP()


// rule 訊息處理常式
