
// SokobanDoc.h: CSokobanDoc 類別的介面
//

#include <iostream>
#include <cstring>
#include <vector>
#pragma once
using std::string;
using std::vector;

class CSokobanDoc : public CDocument
{
protected: // 僅從序列化建立
	CSokobanDoc() noexcept;
	DECLARE_DYNCREATE(CSokobanDoc)

// 屬性
public:

// 作業
public:

// 覆寫
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 程式碼實作
public:
	virtual ~CSokobanDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 為搜尋處理常式設定搜尋內容的 Helper 函式
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	enum Direction
	{
		LEFT,
		UP,
		DOWN,
		RIGHT
	};

	class CBlock
	{
	public:
		char value;
		bool dest;
		CBlock(char c = ' ', bool d = false) : value(c), dest(d) {};
	};
	vector<
		vector<CBlock>

	>
		map;
	int scale, level;
	int x, y;
	int destAmount, completed, boxAmount, step;
	void newLineAdd();
	void lineBlockAdd(int line, char block);
	char EMPTY;
	char PLAYER;
	char BOX;
	char WALL;
	int completedCount();
	void scaleModify(int modify);
	int get(int dy, int dx);
	int NextToWorker(Direction d, int n);
	void moveFn(Direction d, bool push);
	void MoveWorker(Direction d);
	void nextLevel();
	bool isEnd();
	Direction face;
	struct Save
	{
		vector<
			vector<CBlock>

		>
			map;
		int level;
		int destAmount, completed, boxAmount, step;
		Direction face;
	};
	Save local;
};
