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
            // ����ͨ������ɫ��
            CRect rc(pDraw->rc);
            pDC->FillSolidRect(rc, RGB(218, 165, 32)); // ��ɫͨ��
        }
        else if (pDraw->dwItemSpec == TBCD_THUMB)
        {
            // ���ƻ��鰴ť����ɫ+�Ӵ֣�
            CBrush brush(RGB(255, 215, 0)); // �����Ľ�ɫ����
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
