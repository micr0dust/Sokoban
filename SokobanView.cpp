﻿
// SokobanView.cpp: CSokobanView 類別的實作
//

#include <iostream>
#include <cstring>
#include <vector>

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "Sokoban.h"
#endif

#include "SokobanDoc.h"
#include "SokobanView.h"
#include "TcpIp.h"
#include "rule.h"

#include "protocols.h"
#include "mysocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using std::string;
using std::vector;

// CSokobanView

IMPLEMENT_DYNCREATE(CSokobanView, CView)

BEGIN_MESSAGE_MAP(CSokobanView, CView)
	// 標準列印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_KEYDOWN()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_local, &CSokobanView::Onlocal)
    ON_COMMAND(ID_server, &CSokobanView::Onserver)
    ON_COMMAND(ID_gamerule, &CSokobanView::Ongamerule)
END_MESSAGE_MAP()

// CSokobanView 建構/解構

CSokobanView::CSokobanView() noexcept
{
    // TODO: 在此加入建構程式碼
    errorTxt = "";
    level = -1;
    keydown = 0;
    originPos.x = 0;
    originPos.y = 0;
    currentPos.x = 0;
    currentPos.y = 0;
    dragging = false;
    follow = false;
    mode = 0;
    reqed = false;
    mapInit = false;
    loading = false;
    connected = false;
    seenRule = false;
}

CSokobanView::~CSokobanView()
{
}

BOOL CSokobanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CView::PreCreateWindow(cs);
}

// CSokobanView 繪圖

void CSokobanView::OnDraw(CDC* /*pDC*/)
{
	CSokobanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    CRect rc;
    GetClientRect(rc);
    int nWidth = rc.Width();
    int nHeight = rc.Height();

    CDC* pDC = GetDC();
    CDC MemDC;
    CBitmap MemBitmap;

    MemDC.CreateCompatibleDC(pDC);
    MemBitmap.CreateCompatibleBitmap(pDC, nWidth, nHeight);
    CBitmap* pOldbitmap = MemDC.SelectObject(&MemBitmap);
    MemDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(220, 220, 220));

	// TODO: 在此加入原生資料的描繪程式碼
    CWnd* pwndParent = this->GetParent();
    pwndParent->GetClientRect(&screen);
    if (mode == 0) reqed = true;
    if (mode == 1 && !mapInit) {
        mapInit = true;
        request("R");
        request("R");
    }
    if (level != pDoc->level && reqed) {
        reqed = false;
        ReadMap(pDoc->level);
        if(follow)
            for (int i = 0; i < pDoc->map.size(); i++)
                for (int j = 0; j < pDoc->map[i].size(); j++)
                    if (pDoc->map[i][j].value == pDoc->PLAYER) {
                        currentPos.x = originPos.x + (pDoc->x - j) * pDoc->scale;
                        currentPos.y = originPos.y + (pDoc->y - i) * pDoc->scale;
                        originPos = currentPos;
                    }
    }else if ((player.x != pDoc->x || player.y != pDoc->y) && follow) {
        if (pDoc->face == pDoc->UP)
            currentPos.y += pDoc->scale;
        if (pDoc->face == pDoc->DOWN)
            currentPos.y -= pDoc->scale;
        if (pDoc->face == pDoc->LEFT)
            currentPos.x += pDoc->scale;
        if (pDoc->face == pDoc->RIGHT)
            currentPos.x -= pDoc->scale;
        originPos = currentPos;
    }

    player.x = pDoc->x;
    player.y = pDoc->y;

    for (int i = 0; i < pDoc->map.size(); i++)
            for (int j = 0; j < pDoc->map[i].size(); j++)
            {
                if (pDoc->map[i][j].value == pDoc->PLAYER)
                {
                    pDoc->x = j;
                    pDoc->y = i;
                }
                int color = 0;
                CBitmap aBitmap;

                if (pDoc->map[i][j].dest) {
                    if (pDoc->map[i][j].value == pDoc->PLAYER) {
                        if (pDoc->face == pDoc->UP)
                            aBitmap.LoadBitmap(IDB_BITMAP9);
                        else if (pDoc->face == pDoc->RIGHT)
                            aBitmap.LoadBitmap(IDB_BITMAP10);
                        else if (pDoc->face == pDoc->DOWN)
                            aBitmap.LoadBitmap(IDB_BITMAP11);
                        else if (pDoc->face == pDoc->LEFT)
                            aBitmap.LoadBitmap(IDB_BITMAP12);
                    }
                    else if (pDoc->map[i][j].value == pDoc->BOX)
                        aBitmap.LoadBitmap(IDB_BITMAP2);
                    else aBitmap.LoadBitmap(IDB_BITMAP8);
                }
                else if (pDoc->map[i][j].value == pDoc->WALL)
                    aBitmap.LoadBitmap(IDB_BITMAP3);
                else if (pDoc->map[i][j].value == pDoc->PLAYER) {
                    if(pDoc->face== pDoc->UP)
                        aBitmap.LoadBitmap(IDB_BITMAP4);
                    else if (pDoc->face == pDoc->RIGHT)
                        aBitmap.LoadBitmap(IDB_BITMAP5);
                    else if (pDoc->face == pDoc->DOWN)
                        aBitmap.LoadBitmap(IDB_BITMAP6);
                    else if (pDoc->face == pDoc->LEFT)
                        aBitmap.LoadBitmap(IDB_BITMAP7);
                }
                else if (pDoc->map[i][j].value == pDoc->BOX)
                    aBitmap.LoadBitmap(IDB_BITMAP1);
                
                CClientDC aDC(this);
                CDC memDC;
                memDC.CreateCompatibleDC(&aDC);
                memDC.SelectObject(&aBitmap);
                if (pDoc->map[i][j].value == pDoc->WALL || pDoc->map[i][j].value == pDoc->BOX) {
                    MemDC.StretchBlt(currentPos.x + pDoc->scale * j, currentPos.y + pDoc->scale * i, pDoc->scale, pDoc->scale, &memDC, 0, 0, 48, 48, SRCCOPY);
                    MemDC.StretchBlt(currentPos.x + pDoc->scale * j, currentPos.y + pDoc->scale * (i + 1), pDoc->scale, pDoc->scale / 3.0, &memDC, 0, 0, 48, 48, SRCCOPY);
                }
                else
                    MemDC.StretchBlt(currentPos.x + pDoc->scale * j, currentPos.y + pDoc->scale/3.0+ pDoc->scale * i, pDoc->scale, pDoc->scale, &memDC, 0, 0, 48, 48, SRCCOPY);

            }
    pDoc->completedCount();
    CString s;
    s.Format(L"Level: %d", pDoc->level);
    MemDC.TextOut(screen.right-200, screen.top +60,s);
    s.Format(L"Destination: %d", pDoc->destAmount);
    MemDC.TextOut(screen.right - 200, screen.top+80, s);
    s.Format(L"Arrival: %d", pDoc->completed);
    MemDC.TextOut(screen.right - 200, screen.top + 100, s);
    s.Format(L"Steps: %d", pDoc->step);
    MemDC.TextOut(screen.right - 200, screen.top + 120, s);
    if (!follow)
        MemDC.TextOut(screen.right - 200, screen.top + 140, L"Focus on: Map");
    else
        MemDC.TextOut(screen.right - 200, screen.top + 140, L"Focus on: Player");

    pDC->BitBlt(0, 0, nWidth, nHeight, &MemDC, 0, 0, SRCCOPY);
    MemDC.SelectObject(pOldbitmap);
    MemBitmap.DeleteObject();

    if (!seenRule) Ongamerule();
}


// CSokobanView 列印

BOOL CSokobanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 預設的準備列印程式碼
	return DoPreparePrinting(pInfo);
}

void CSokobanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印前額外的初始設定
}

void CSokobanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印後的清除程式碼
}


// CSokobanView 診斷

#ifdef _DEBUG
void CSokobanView::AssertValid() const
{
	CView::AssertValid();
}

void CSokobanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSokobanDoc* CSokobanView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSokobanDoc)));
	return (CSokobanDoc*)m_pDocument;
}
#endif //_DEBUG

// CSokobanView 訊息處理常式
bool CSokobanView::fileError(string err = "")
{
    CSokobanDoc* pDoc = GetDocument();
    errorTxt = err;
    //Invalidate();
    Invalidate();
    return false;
}


int CSokobanView::ReadMap(int n)
{
    CSokobanDoc* pDoc = GetDocument();
    level = pDoc->level;
    if (mode == 0) {
        if (n < 0 || n > 150)
            return fileError("Only accept map000 to map150");
        char filename[15];
        sprintf_s(filename, "map/map%03d.txt", n);
        FILE* fp;
        errno_t err = fopen_s(&fp, filename, "r");
        char word;
        if (fp == NULL || err != 0)
            return fileError("file not found. Press any key to quit.");
        else
        {
            pDoc->newLineAdd();
            for (int i = 0; (word = fgetc(fp)) != EOF;)
            {
                if (word == '\n')
                {
                    pDoc->newLineAdd();
                    i++;
                }
                else if (
                    word == 'H' ||
                    word == '0' ||
                    word == ' ' ||
                    word == '\t' ||
                    word == 'B' ||
                    word == 'C' ||
                    word == 'D' ||
                    word == 'W')
                    pDoc->lineBlockAdd(i, word);
                else if (word != '\r')
                    return fileError("Invalid syntax in map.");
            }

            fclose(fp);
            return 1;
        }
    }
    else if (mode == 1) {
            pDoc->map.clear();
            pDoc->newLineAdd();
            for (int i = 0,j=0; j<resMap.length();j++)
            {
                char word = resMap[j];
                if (word == 'N')
                {
                    pDoc->newLineAdd();
                    i++;
                }
                else if (
                    word == 'H' ||
                    word == '0' ||
                    word == ' ' ||
                    word == '\t' ||
                    word == 'B' ||
                    word == 'C' ||
                    word == 'D' ||
                    word == 'W' ||
                    word == 'w')
                    pDoc->lineBlockAdd(i, word);
                else
                    return fileError("Invalid syntax in map.");
            }
            return 1;
    }
}


bool CSokobanView::MapVarify()
{
    CSokobanDoc* pDoc = GetDocument();
    if (pDoc->boxAmount < pDoc->destAmount || pDoc->boxAmount == 0)
        return fileError("This map don't have enough box.");
    if (pDoc->x == -1 || pDoc->y == -1)
        return fileError("The worker seem disappear in map.");
    return true;
}


void CSokobanView::OnKeyDown(UINT key, UINT nRepCnt, UINT nFlags)
{
    if (!seenRule) return;
    keydown = key;
    CSokobanDoc* pDoc = GetDocument();
    if (mode == 0) {
        if (pDoc->isEnd())
            return pDoc->nextLevel();
        if (key == 'h' || key == 'H' || key == 37)
            return pDoc->MoveWorker(pDoc->LEFT);
        if (key == 'k' || key == 'K' || key == 38)
            return pDoc->MoveWorker(pDoc->UP);
        if (key == 'j' || key == 'J' || key == 40)
            return pDoc->MoveWorker(pDoc->DOWN);
        if (key == 'l' || key == 'L' || key == 39)
            return pDoc->MoveWorker(pDoc->RIGHT);
        if (pDoc->level < 150 && (key == 'x' || key == 'X'))
            return pDoc->nextLevel();
        if ((key == 'z' || key == 'Z'))
        {
            if ((pDoc->level -= 2) <= 0)
                pDoc->level = -1;
            return pDoc->nextLevel();
        }
    }
    else if (mode == 1) {
        if (key == 'h' || key == 'H' || key == 37) {
            pDoc->face = pDoc->LEFT;
            request("0");
        }
        else if (key == 'k' || key == 'K' || key == 38) {
            pDoc->face = pDoc->UP;
            request("1");
        }
        else if (key == 'j' || key == 'J' || key == 40) {
            pDoc->face = pDoc->DOWN;
            request("2");
        }
        else if (key == 'l' || key == 'L' || key == 39) {
            pDoc->face = pDoc->RIGHT;
            request("3");
        }
        else if (key == 'x' || key == 'X')
            request("x");
        else if (key == 'z' || key == 'Z')
            request("z");
    }
    
    if (key == 'r' || key == 'R') {
        currentPos.x = 0;
        currentPos.y = 0;
        originPos = currentPos;
        dragging = false;
        return Invalidate();
    }
    if (key == 'f' || key == 'F') {
        follow = follow ? false : true;
        return Invalidate();
    }
    if (key == 'm' || key == 'M') {
        mode = mode==0 ? 1 : 0;
        return;
    }
    /* x:goto last level, x:goto next level */
    
    CView::OnKeyDown(key, nRepCnt, nFlags);
}


void CSokobanView::statusBar()
{
    CSokobanDoc* pDoc = GetDocument();
    
}


BOOL CSokobanView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    CSokobanDoc* pDoc = GetDocument();
    if (nFlags== MK_CONTROL && zDelta > 0)
    {
        pDoc->scaleModify(1);
        //Invalidate()
        Invalidate();
    }
    if (nFlags == MK_CONTROL && zDelta < 0)
    {
        pDoc->scaleModify(-1);
        //Invalidate()
        Invalidate();
    }
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CSokobanView::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    //RECT rc;
    //GetClientRect(&rc);
    //pDC->SetBkColor(RGB(220, 220, 220));
    //pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    return FALSE;
    //return CView::OnEraseBkgnd(pDC);
}


void CSokobanView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    BeginPos = point;
    dragging = true;
    CView::OnLButtonDown(nFlags, point);
}


void CSokobanView::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    dragging = false;
    originPos = currentPos;
    CView::OnLButtonUp(nFlags, point);
}


void CSokobanView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    if (dragging) {
        currentPos.x = originPos.x+point.x - BeginPos.x;
        currentPos.y = originPos.y+point.y - BeginPos.y;
        Invalidate();
    }
    
    CView::OnMouseMove(nFlags, point);
}


void CSokobanView::request(string req)
{
    if (loading) return;
    loading = true;
    CSokobanDoc* pDoc = GetDocument();
    string res,strmap="";
    (*connection).Write(req);
    (*connection).Read(-1000, strmap);
    if (strmap[0] == '!') {
        fileError(strmap);
        return;
    }
    (*connection).Read(-50, res);
    int resn[4];
    sscanf_s(res.c_str(), "%d %d %d %d", resn, resn + 1, resn + 2, resn + 3);
    pDoc->level = resn[0];
    pDoc->destAmount = resn[1];
    pDoc->completed = resn[2];
    pDoc->step = resn[3];
    resMap = "";
    resMap = strmap;
    level = -1;
    reqed = true;
    connected = true;
    Invalidate();
    loading = false;
}


void CSokobanView::Onlocal()
{
    // TODO: 在此加入您的命令處理常式程式碼
    if (mode == 1) {
        mode = 0;
        reset();
    }
}


void CSokobanView::Onserver()
{
    // TODO: 在此加入您的命令處理常式程式碼
    TcpIp DialogWindow;
    CPoint point;
    vector<CString> data;
    CSokobanDoc* pDoc = GetDocument();
    pDoc->local.map.clear();
    pDoc->local.map.assign(pDoc->map.begin(), pDoc->map.end());
    pDoc->local.level = pDoc->level;
    pDoc->local.destAmount = pDoc->destAmount;
    pDoc->local.completed = pDoc->completed;
    pDoc->local.step = pDoc->step;
    pDoc->local.face = pDoc->face;
    if (!connected && DialogWindow.DoModal() == IDOK && mode==0) {
        if (DialogWindow.m_IP != L"") {
            CT2CA preIP(DialogWindow.m_IP);
            CT2CA prePORT(DialogWindow.m_PORT);
            string ip(preIP);
            string port(prePORT);
            static Endpoint epc(TCP | CLIENT, ip, port);
            connection = &epc;
            mode = 1;
            Invalidate();
        }else AfxMessageBox(_T("IP 是必填的項目"));
    }
    else if(connected) {
        mode = 1;
        Invalidate();
        request("R");
    }
}


void CSokobanView::reset()
{
    // TODO: 請在此新增您的實作程式碼.
    CSokobanDoc* pDoc = GetDocument();
    level = -1;
    pDoc->map.clear();
    pDoc->map.assign(pDoc->local.map.begin(), pDoc->local.map.end());
    pDoc->level = pDoc->local.level;
    level = pDoc->level;
    pDoc->destAmount = pDoc->local.destAmount;
    pDoc->completed = pDoc->local.completed;
    pDoc->step = pDoc->local.step;
    pDoc->face = pDoc->local.face;
    Invalidate();
}


void CSokobanView::Ongamerule()
{
    // TODO: 在此加入您的命令處理常式程式碼
    rule DialogWindow;
    DialogWindow.DoModal();
    seenRule = true;
    Invalidate();
}
