#pragma once
#include <afxcmn.h>
#include <afxwin.h>       // MFC ������

class CGoldSlider : public CSliderCtrl
{
public:
    CGoldSlider();
    virtual ~CGoldSlider();

public:
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

    DECLARE_DYNCREATE(CGoldSlider)
};