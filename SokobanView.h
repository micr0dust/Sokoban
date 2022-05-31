
// SokobanView.h: CSokobanView 類別的介面
//
#include <iostream>
#include <cstring>
#include <vector>
#pragma once
using std::string;
using std::vector;

class CSokobanView : public CView
{
protected: // 僅從序列化建立
	CSokobanView() noexcept;
	DECLARE_DYNCREATE(CSokobanView)

// 屬性
public:
	CSokobanDoc* GetDocument() const;

// 作業
public:

// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 程式碼實作
public:
	virtual ~CSokobanView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()
public:
	string errorTxt;
	bool fileError(string err);
	int ReadMap(int n);
	bool MapVarify();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int level;
	void statusBar();
	int keydown;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CPoint originPos;
	CPoint currentPos;
	CPoint BeginPos;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	bool dragging;
	bool follow;
	RECT screen;
	CPoint player;
};

#ifndef _DEBUG  // 對 SokobanView.cpp 中的版本進行偵錯
inline CSokobanDoc* CSokobanView::GetDocument() const
   { return reinterpret_cast<CSokobanDoc*>(m_pDocument); }
#endif

