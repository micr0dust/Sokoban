
// SokobanDoc.cpp: CSokobanDoc 類別的實作
//
#include <iostream>
#include <cstring>
#include <vector>
#pragma once
using std::string;
using std::vector;
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "Sokoban.h"
#endif

#include "SokobanDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSokobanDoc

IMPLEMENT_DYNCREATE(CSokobanDoc, CDocument)

BEGIN_MESSAGE_MAP(CSokobanDoc, CDocument)
END_MESSAGE_MAP()


// CSokobanDoc 建構/解構

CSokobanDoc::CSokobanDoc() noexcept
{
	// TODO: 在此加入一次建構程式碼
	EMPTY = ' ';
	PLAYER = 'W';
	BOX = 'B';
	WALL = 'H';
	scale = 50;
	level = 0;
	x = -1;
	y=-1;
	destAmount = 0;
	completed = 0;
	boxAmount = 0;
	step = 0;
	face = Direction::DOWN;
}

CSokobanDoc::~CSokobanDoc()
{
}

BOOL CSokobanDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此加入重新初始化程式碼
	// (SDI 文件會重用此文件)

	return TRUE;
}




// CSokobanDoc 序列化

void CSokobanDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此加入儲存程式碼
	}
	else
	{
		// TODO: 在此加入載入程式碼
	}
}

#ifdef SHARED_HANDLERS

// 縮圖的支援
void CSokobanDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改這段程式碼以繪製文件的資料
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜尋處理常式的支援
void CSokobanDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 設定來自文件資料的搜尋內容。
	// 內容部分應該以 ";" 隔開

	// 範例:     strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSokobanDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSokobanDoc 診斷

#ifdef _DEBUG
void CSokobanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSokobanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSokobanDoc 命令


void CSokobanDoc::newLineAdd()
{
	map.push_back(vector<CBlock>());
}


void CSokobanDoc::lineBlockAdd(int line, char block)
{
	bool isdest = (block == 'C' || block == 'D' || block == 'w') ? true : false;
	if (isdest)
		destAmount++;
	if (block == '0' || block == 'D')
		block = EMPTY;
	else if (block == 'C')
		block = BOX;
	else if (block == 'w')
		block = PLAYER;
	if (block == 'B')
		boxAmount++;
	if (block == '\t') {
		block = EMPTY;
		for (int i = 0; i < 6; i++)
			map[line].push_back(CBlock(block, isdest));
	}
	map[line].push_back(CBlock(block, isdest));
}


int CSokobanDoc::completedCount()
{
	completed = 0;
	for (int i = 0; i < map.size(); i++)
		for (int j = 0; j < map[i].size(); j++)
			if (map[i][j].dest && map[i][j].value == BOX)
				completed++;
	return completed;
}


void CSokobanDoc::scaleModify(int modify)
{
	if (modify > 0)
		if(scale<50)
			scale += 1;
		else
			scale += 10;
	else if (modify < 0 && scale >= 0)
		if (scale < 50)
			scale -= 1;
		else
			scale -= 10;
}


int CSokobanDoc::get(int dy, int dx)
{
	if (y + static_cast<unsigned long long>(dy) >= map.size() || y + dy < 0)
		return WALL;
	if (x + static_cast<unsigned long long>(dx) >= map[y + static_cast<unsigned long long>(dy)].size() || x + dx < 0)
		return WALL;
	return map[y + static_cast<unsigned long long>(dy)][x + static_cast<unsigned long long>(dx)].value;
}


int CSokobanDoc::NextToWorker(Direction d, int n)
{
	if (d == LEFT)
		return get(0, -n);
	if (d == UP)
		return get(-n, 0);
	if (d == DOWN)
		return get(n, 0);
	if (d == RIGHT)
		return get(0, n);
	return 0;
}


void CSokobanDoc::moveFn(Direction d, bool push)
{
	step++;
	if (push)
	{
		if (d == LEFT)
			map[y][x - 2].value = map[y][x - 1].value;
		if (d == UP)
			map[y - 2][x].value = map[y - 1][x].value;
		if (d == DOWN)
			map[y + 2][x].value = map[y + 1][x].value;
		if (d == RIGHT)
			map[y][x + 2].value = map[y][x + 1].value;
	}
	if (d == LEFT)
		map[y][x - 1].value = map[y][x].value;
	if (d == UP)
		map[y - 1][x].value = map[y][x].value;
	if (d == DOWN)
		map[y + 1][x].value = map[y][x].value;
	if (d == RIGHT)
		map[y][x + 1].value = map[y][x].value;
	map[y][x].value = EMPTY;
	UpdateAllViews(NULL);
}


void CSokobanDoc::MoveWorker(Direction d)
{
	face = d;
	char next = NextToWorker(d, 1);
	char nextNext = NextToWorker(d, 2);
	if (next == BOX && nextNext == EMPTY)
		moveFn(d, true);
	if (next == EMPTY)
		moveFn(d, false);
	return;
}


void CSokobanDoc::nextLevel()
{
	level++;
	destAmount = 0;
	boxAmount = 0;
	completed = 0;
	step = 0;
	map.clear();
	UpdateAllViews(NULL);
}


bool CSokobanDoc::isEnd()
{
	if (completed >= destAmount)
		return true;
	return false;
}
