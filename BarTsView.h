
// BarTsView.h: CBarTsView 类的接口
//

#pragma once
#include "CColorButton.h"
#include "CGoldSlider.h"
#include "Dib.h"

class CBarTsView : public CView
{
protected: // 仅从序列化创建
	CBarTsView() noexcept;
	DECLARE_DYNCREATE(CBarTsView)

// 特性
public:
	CBarTsDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	std::vector<CGoldSlider*> m_modalSliders;
	std::vector<int> m_sliceIndices;
	bool m_updatingFromOverall = false;//添加滑块代码
	CGoldSlider m_masterSlider; // 大滑块控制器
	std::vector<CColorButton*> m_segmentButtons;//分割按钮
	std::vector<CColorButton*> m_restoreButtons;//还原按钮
	CColorButton* m_GrayWhiteButtons = nullptr;//灰白质分割按钮
	CStatic m_masterLabel;         // "总控"标签
	void SegmentFlairSlice(int flairSliceIdx);// 分割 FLAIR 切片，获得水肿区域
	void SegmentT1CSlice(int T1CSliceIdx);// 分割 T1C 切片，获得增强肿瘤区域
	void SegmentT2Slice(int T2CSliceIdx);// 分割 T2 切片，获得肿瘤坏死区域
	void BlendSegmentMaskOntoT1(int T1SliceIdx);//分割T1，但是其实是在分割相同索引的其他切片
	
	//T1切割得到肿瘤三个标签的展现，只需要调用上面三个函数即可实现
	bool m_flairSegActive = false;// 是否激活 FLAIR 分割
	bool m_t1cSegActive = false;// 是否激活 T1C 分割
	bool m_t2SegActive = false;// 是否激活 T2 分割
	bool m_t1SegActive = false;//是否激活T1分割，得到整体肿瘤区域


protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CBarTsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSegmentButtonClicked(UINT nID);
	afx_msg void OnRestoreButtonClicked(UINT nID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClaheEnhance();
	afx_msg void OnRestore();
	afx_msg void OnSobelEdge();
	afx_msg void OnAdaptivenoise();
	afx_msg void OnGaussiannoise();
	afx_msg void OnBilfilter();
	afx_msg void OnLight();
	afx_msg void OnAllLight();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGrayWhiteClicked();
};

#ifndef _DEBUG  // BarTsView.cpp 中的调试版本
inline CBarTsDoc* CBarTsView::GetDocument() const
   { return reinterpret_cast<CBarTsDoc*>(m_pDocument); }
#endif

