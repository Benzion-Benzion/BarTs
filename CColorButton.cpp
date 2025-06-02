// CColorButton.cpp
#include "pch.h"
#include "CColorButton.h"

CColorButton::CColorButton() {}
CColorButton::~CColorButton() {}

void CColorButton::PreSubclassWindow()
{
    ModifyStyle(0, BS_OWNERDRAW);
    SetFontSize(100);
    CButton::PreSubclassWindow();
}

void CColorButton::SetFontSize(int pointSize, LPCTSTR fontName)
{
    m_font.DeleteObject();
    m_font.CreatePointFont(pointSize, fontName);
}

//重写构造函数
CColorButton::CColorButton(COLORREF Color) {
    m_bgColor = Color;
}

//处理按下状态
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItem)
{
    CDC* pDC = CDC::FromHandle(lpDrawItem->hDC);
    CRect rect = lpDrawItem->rcItem;

    BOOL bPressed = (lpDrawItem->itemState & ODS_SELECTED);

    // 颜色
    COLORREF bgColor = bPressed ? RGB(100, 100, 100) : m_bgColor;
    COLORREF borderColor = RGB(50, 50, 50);

    // 圆角区域
    const int radius = 15;
    CBrush brush(bgColor);
    CPen pen(PS_SOLID, 1, borderColor);

    CBrush* pOldBrush = pDC->SelectObject(&brush);
    CPen* pOldPen = pDC->SelectObject(&pen);

    pDC->SetBkMode(TRANSPARENT);
    pDC->RoundRect(&rect, CPoint(radius, radius));

    // 文字
    CString text;
    GetWindowText(text);

    CFont* pOldFont = pDC->SelectObject(&m_font);
    pDC->SetTextColor(m_textColor);

    pDC->DrawText(text, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 恢复设备上下文
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldFont);
}

BEGIN_MESSAGE_MAP(CColorButton, CButton)
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    CButton::OnLButtonUp(nFlags, point);

    // 关键：手动向父窗口发送点击消息
    if (GetSafeHwnd())
    {
        CWnd* pParent = GetParent();
        if (pParent)
        {
            pParent->SendMessage(WM_COMMAND,
                MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED),
                (LPARAM)GetSafeHwnd());
        }
    }
    
}
