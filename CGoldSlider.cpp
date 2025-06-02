#include "pch.h"
#include "CGoldSlider.h"

IMPLEMENT_DYNCREATE(CGoldSlider, CSliderCtrl)

CGoldSlider::CGoldSlider() {}
CGoldSlider::~CGoldSlider() {}

BEGIN_MESSAGE_MAP(CGoldSlider, CSliderCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CGoldSlider::OnCustomDraw)
END_MESSAGE_MAP()

void CGoldSlider::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMCUSTOMDRAW pDraw = (LPNMCUSTOMDRAW)pNMHDR;

    switch (pDraw->dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:
    {
        CDC* pDC = CDC::FromHandle(pDraw->hdc);

        if (pDraw->dwItemSpec == TBCD_TICS || pDraw->dwItemSpec == TBCD_CHANNEL)
        {
            // 绘制通道（金色）
            CRect rc(pDraw->rc);
            pDC->FillSolidRect(rc, RGB(218, 165, 32)); // 金色通道
        }
        else if (pDraw->dwItemSpec == TBCD_THUMB)
        {
            // 绘制滑块按钮（金色+加粗）
            CBrush brush(RGB(255, 215, 0)); // 更亮的金色滑块
            pDC->FillRect(&pDraw->rc, &brush);
            pDC->DrawEdge(&pDraw->rc, EDGE_RAISED, BF_RECT);
        }

        *pResult = CDRF_SKIPDEFAULT;
    }
    break;

    default:
        *pResult = CDRF_DODEFAULT;
        break;
    }
}
