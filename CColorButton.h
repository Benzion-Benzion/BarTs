#pragma once  
#include <afxwin.h> // Ensure this header is included for MFC classes like CButton  
#include <afxext.h> // For CButton and other MFC controls

// CColorButton.h
class CColorButton : public CButton
{
public:
    CColorButton();
    CColorButton(COLORREF m_bgColor = RGB(189, 215, 238));
    virtual ~CColorButton();

    void SetFontSize(int pointSize, LPCTSTR fontName = _T("��������"));
    void SetTextColor(COLORREF color) { m_textColor = color; Invalidate(); }
    //��ϲ����ǳ��ɫ RGB(189, 215, 238)
    void SetBgColor(COLORREF color) { m_bgColor = color; Invalidate(); }

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItem);
    virtual void PreSubclassWindow();

private:
    CFont m_font;
    COLORREF m_textColor = RGB(0, 0, 0);

public:
    COLORREF m_bgColor = RGB(189, 215, 238);//����ť������ɫ
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
