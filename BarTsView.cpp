
// BarTsView.cpp: CBarTsView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "BarTs.h"
#endif

#include "BarTsDoc.h"
#include "BarTsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBarTsView

IMPLEMENT_DYNCREATE(CBarTsView, CView)

BEGIN_MESSAGE_MAP(CBarTsView, CView)
    // 标准打印命令
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_WM_HSCROLL()
    ON_COMMAND_RANGE(ID_SEGMENT_BTN_BASE, ID_SEGMENT_BTN_BASE + 3, &CBarTsView::OnSegmentButtonClicked)
    ON_COMMAND_RANGE(ID_SEGMENT_BTN_BASE, ID_RESTORE_BTN_BASE + 3, &CBarTsView::OnRestoreButtonClicked)
    ON_COMMAND(ID_GRAYWHITESEGMENT, &CBarTsView::OnGrayWhiteClicked)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_COMMAND(ID_CLAHE_ENHANCE, &CBarTsView::OnClaheEnhance)
    ON_COMMAND(ID_RESTORE, &CBarTsView::OnRestore)
    ON_COMMAND(ID_SOBEL_EDGE, &CBarTsView::OnSobelEdge)
    ON_COMMAND(ID_AdaptiveNoise, &CBarTsView::OnAdaptivenoise)
    ON_COMMAND(ID_GaussianNoise, &CBarTsView::OnGaussiannoise)
    ON_COMMAND(ID_BilFilter, &CBarTsView::OnBilfilter)
    ON_COMMAND(ID_Light, &CBarTsView::OnLight)
    ON_COMMAND(ID_ALL_LIGHT, &CBarTsView::OnAllLight)
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

// CBarTsView 构造/析构

CBarTsView::CBarTsView() noexcept
{
	// TODO: 在此处添加构造代码

}

CBarTsView::~CBarTsView()
{
}

BOOL CBarTsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

    cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // 避免控件重绘冲突
    return CView::PreCreateWindow(cs);
}

// CBarTsView 绘图
void CBarTsView::OnDraw(CDC* pDC)
{
    CBarTsDoc* pDoc = GetDocument();

    if (!pDoc || pDoc->m_allModalImages.empty())
    {
        // 隐藏所有 UI 控件
        for (auto& btn : m_segmentButtons)
            if (btn && ::IsWindow(btn->GetSafeHwnd()))
                btn->ShowWindow(SW_HIDE);

        if (m_GrayWhiteButtons == nullptr)
            AfxMessageBox(_T("按钮未初始化"));
        else if (!::IsWindow(m_GrayWhiteButtons->GetSafeHwnd()))
            AfxMessageBox(_T("窗口句柄无效"));
        else
            m_GrayWhiteButtons->ShowWindow(SW_HIDE);


        // 隐藏灰白质分割按钮（如果存在）
        if (m_GrayWhiteButtons && ::IsWindow(m_GrayWhiteButtons->GetSafeHwnd())) {
            m_GrayWhiteButtons->ShowWindow(SW_HIDE);
        }

        for (auto& slider : m_modalSliders)
            if (slider && ::IsWindow(slider->GetSafeHwnd()))
                slider->ShowWindow(SW_HIDE);

        if (::IsWindow(m_masterSlider.GetSafeHwnd()))
            m_masterSlider.ShowWindow(SW_HIDE);

        for (auto& btn : m_restoreButtons)
            if (btn && ::IsWindow(btn->GetSafeHwnd()))
                btn->ShowWindow(SW_HIDE);

        if (::IsWindow(m_masterLabel.GetSafeHwnd()))
            m_masterLabel.ShowWindow(SW_HIDE); 

        return;
    }

    std::vector<CString> titles = { _T("FLAIR"), _T("T1"), _T("T1c"), _T("T2") };
    int margin = 10, topMargin = 50;
    int imgW = pDoc->m_allModalImages[0][0].cols;
    int imgH = pDoc->m_allModalImages[0][0].rows;

    // 获取窗口绘制区域大小
    CRect rect;
    GetClientRect(&rect);

    // 1. 创建内存 DC 和兼容位图（双缓冲画布）
    // 双缓冲初始化
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap bmpMem;
    bmpMem.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldMemBmp = memDC.SelectObject(&bmpMem);
    memDC.FillSolidRect(&rect, RGB(0, 0, 0)); // 黑色背景

    // 设置字体
    CFont font;
    font.CreatePointFont(100, _T("Times New Roman"));
    CFont* pOldFont = memDC.SelectObject(&font);
    COLORREF white = RGB(255, 255, 255);
    COLORREF oldColor = memDC.SetTextColor(white);

    int nz = pDoc->m_pNiftiImg ? pDoc->m_pNiftiImg->nz : 1;



    // 4. 循环绘制每个模态图像及标题到内存DC
    for (int i = 0; i < std::min(4, (int)pDoc->m_allModalImages[0].size()); ++i)
    {
        cv::Mat img = pDoc->m_allModalImages[i][m_sliceIndices[i]];
        if (img.empty()) continue;

        // 如果是FLAIR通道且有blended图像，优先显示blended
        if (i == 0 && !pDoc->m_blendedFlair.empty() && m_flairSegActive)
            img = pDoc->m_blendedFlair;
        else if (i == 1 && !pDoc->m_blendedT1.empty() && m_t1SegActive)
            img = pDoc->m_blendedT1;
        else if (i == 2 && !pDoc->m_blendedT1C.empty() && m_t1cSegActive)
            img = pDoc->m_blendedT1C;
        else if (i == 3 && !pDoc->m_blendedT2.empty() && m_t2SegActive)
            img = pDoc->m_blendedT2;
        else
            img = pDoc->m_allModalImages[i][m_sliceIndices[i]];

        if (img.empty()) continue;

        cv::Mat colorImg;
        if (img.channels() == 1)
            cv::cvtColor(img, colorImg, cv::COLOR_GRAY2BGR);
        else
            colorImg = img.clone();

        // 转换 cv::Mat 到 HBITMAP
        HBITMAP hBmp = CDib::MatToHBITMAP(colorImg);
        if (!hBmp) continue;

        // 创建一个兼容 DC 用于位图绘制
        CDC imgDC;
        if (!imgDC.CreateCompatibleDC(&memDC))
        {
            DeleteObject(hBmp);
            continue;
        }

        HGDIOBJ oldImgBmp = imgDC.SelectObject(hBmp);
        if (!oldImgBmp)
        {
            DeleteObject(hBmp);
            continue;
        }

        // 计算绘制位置
        int drawX = margin + (i % 2) * (imgW + margin);
        int drawY = topMargin + margin + (i / 2) * (imgH + 140 + margin);

        // 绘制标题文本
        CString title = (i < titles.size()) ? titles[i] : CString(L"Modality") + std::to_wstring(i).c_str();
        CSize textSize = memDC.GetTextExtent(title);
        int textX = drawX + (imgW - textSize.cx) / 2;
        int textY = drawY - 25;
        memDC.TextOutW(textX, textY, title);

        // 图像
        int imgY = textY + textSize.cy + 10; // 标题下方10像素
        memDC.BitBlt(drawX, imgY, imgW, imgH, &imgDC, 0, 0, SRCCOPY);

        // 清理
        imgDC.SelectObject(oldImgBmp);
        DeleteObject(hBmp);



        // 滑块同步显示（注意此处是在屏幕上操作，保持原样）
        if (i < m_modalSliders.size() && m_modalSliders[i] && ::IsWindow(m_modalSliders[i]->GetSafeHwnd()))
        {
            m_modalSliders[i]->SetRange(0, nz - 1);
            int sliderY = imgY + imgH + 10; // 图像下方10像素
            m_modalSliders[i]->MoveWindow(drawX, sliderY, imgW, 20);
            m_modalSliders[i]->ShowWindow(SW_SHOW);
        

            // 按钮对齐到图像下方
            if (i < 4)
            {
                int btnW = 80, btnH = 40;
                int btnGap = 10; // 按钮之间的间距

                int totalBtnW = btnW * 2 + btnGap;
                int btnBaseX = drawX + (imgW - totalBtnW) / 2;
                int btnY = sliderY + 25; // 滑块下方25像素

                // 分割按钮位置
                if (m_segmentButtons[i] && ::IsWindow(m_segmentButtons[i]->GetSafeHwnd()))
                {
                    m_segmentButtons[i]->MoveWindow(btnBaseX, btnY, btnW, btnH);
                    m_segmentButtons[i]->ShowWindow(SW_SHOW);
                }

                // 还原按钮位置
                if (i < m_restoreButtons.size() && m_restoreButtons[i] && ::IsWindow(m_restoreButtons[i]->GetSafeHwnd()))
                {
                    int restoreBtnX = btnBaseX + btnW + btnGap;
                    m_restoreButtons[i]->MoveWindow(restoreBtnX, btnY, btnW, btnH);
                    m_restoreButtons[i]->ShowWindow(SW_SHOW);
                }
            }

        }
    }

    if (!pDoc->m_grayMatterSlices.empty() || !pDoc->m_whiteMatterSlices.empty())
    {
        // 计算分割结果的显示位置（在所有模态图像下方）
        const int SEG_MARGIN = 20;
        int segYOffset = topMargin + 2 * (imgH + 140) + SEG_MARGIN;
        int segXOffset = margin;

        // 绘制灰质分割结果
        if (!pDoc->m_grayMatterSlices.empty() && !pDoc->m_grayMatterSlices[m_sliceIndices[3]].empty())
        {
            cv::Mat grayImg = pDoc->m_grayMatterSlices[m_sliceIndices[3]];
            if (grayImg.channels() == 1)
                cv::cvtColor(grayImg, grayImg, cv::COLOR_GRAY2BGR);

            HBITMAP hGrayBmp = CDib::MatToHBITMAP(grayImg);
            if (hGrayBmp)
            {
                CDC grayDC;
                if (grayDC.CreateCompatibleDC(&memDC))
                {
                    HGDIOBJ oldGrayBmp = grayDC.SelectObject(hGrayBmp);
                    memDC.BitBlt(segXOffset, segYOffset, imgW, imgH, &grayDC, 0, 0, SRCCOPY);
                    grayDC.SelectObject(oldGrayBmp);
                }
                DeleteObject(hGrayBmp);
            }

            // 绘制灰质标签
            CString grayText = _T("灰质分割");
            CSize grayTextSize = memDC.GetTextExtent(grayText);
            int textX = segXOffset + (imgW - grayTextSize.cx) / 2;
            int textY = segYOffset - grayTextSize.cy - 5;
            memDC.TextOutW(textX, textY, grayText);
        }

        // 绘制白质分割结果（在灰质右侧）
        if (!pDoc->m_whiteMatterSlices.empty() && !pDoc->m_whiteMatterSlices[m_sliceIndices[3]].empty())
        {
            int whiteXOffset = segXOffset + imgW + margin;
            cv::Mat whiteImg = pDoc->m_whiteMatterSlices[m_sliceIndices[3]];
            if (whiteImg.channels() == 1)
                cv::cvtColor(whiteImg, whiteImg, cv::COLOR_GRAY2BGR);

            HBITMAP hWhiteBmp = CDib::MatToHBITMAP(whiteImg);
            if (hWhiteBmp)
            {
                CDC whiteDC;
                if (whiteDC.CreateCompatibleDC(&memDC))
                {
                    HGDIOBJ oldWhiteBmp = whiteDC.SelectObject(hWhiteBmp);
                    memDC.BitBlt(whiteXOffset, segYOffset, imgW, imgH, &whiteDC, 0, 0, SRCCOPY);
                    whiteDC.SelectObject(oldWhiteBmp);
                }
                DeleteObject(hWhiteBmp);
            }

            // 绘制白质标签
            CString whiteText = _T("白质分割");
            CSize whiteTextSize = memDC.GetTextExtent(whiteText);
            int textX = whiteXOffset + (imgW - whiteTextSize.cx) / 2;
            int textY = segYOffset - whiteTextSize.cy - 5;
            memDC.TextOutW(textX, textY, whiteText);
        }
    }

    // 3. 设置滑块范围和位置
    if (::IsWindow(m_masterSlider.GetSafeHwnd()))
    {
        const int MASTER_MARGIN = 20; // 右侧更大的边距
        const int SLIDER_WIDTH = 40;   // 更宽的滑块以便于操作
        const int LABEL_HEIGHT = 25;   // 标签高度
        const int TOP_MARGIN = 40;

        // 1. 计算标签位置
        CSize textSize = memDC.GetTextExtent(_T("总控"));
        int labelWidth = textSize.cx + 20; // 文本宽度加边距

        // 标签位置：右上角
        int labelX = 540;
        int labelY = TOP_MARGIN;
        m_masterLabel.MoveWindow(labelX, labelY, labelWidth, LABEL_HEIGHT);
        m_masterLabel.ShowWindow(SW_SHOW);

        // 2. 计算滑块位置
        int sliderX = 550;
        int sliderY = labelY + LABEL_HEIGHT + 10; // 标签下方10像素
        int sliderHeight = rect.Height() - sliderY - MASTER_MARGIN; // 使用窗口剩余高度

        // 限制滑块最大高度
        const int MAX_SLIDER_HEIGHT = 400;
        if (sliderHeight > MAX_SLIDER_HEIGHT) {
            sliderHeight = MAX_SLIDER_HEIGHT;
        }

        // 确保滑块有最小高度
        const int MIN_SLIDER_HEIGHT = 100;
        if (sliderHeight < MIN_SLIDER_HEIGHT) {
            sliderHeight = MIN_SLIDER_HEIGHT;
        }

        m_masterSlider.MoveWindow(sliderX, sliderY, SLIDER_WIDTH, sliderHeight);

        // 3. 确保滑块可见且在前景
        m_masterSlider.ShowWindow(SW_SHOW);
        m_masterSlider.BringWindowToTop();
    }
    // ====== 总控滑块修复结束 ======

    // ====== 在滑块下方绘制“灰白质分割”按钮 ======
    if (m_GrayWhiteButtons && ::IsWindow(m_GrayWhiteButtons->GetSafeHwnd()))
    {
        const int BUTTON_WIDTH = 150;
        const int BUTTON_HEIGHT = 50;
        const int BUTTON_MARGIN = 50;

        // 获取滑块位置
        CRect sliderRect;
        m_masterSlider.GetWindowRect(&sliderRect);
        ScreenToClient(&sliderRect); // 转为客户区坐标

        // 计算按钮位置（滑块底部 + 一定间距）
        int btnX = sliderRect.left + (sliderRect.Width() - BUTTON_WIDTH) / 2;
        int btnY = sliderRect.bottom + BUTTON_MARGIN;

        // 限制按钮不超过窗口底部
        if (btnY + BUTTON_HEIGHT > rect.Height()) {
            btnY = rect.Height() - BUTTON_HEIGHT - 5;
        }

        m_GrayWhiteButtons->MoveWindow(btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT);
        m_GrayWhiteButtons->ShowWindow(SW_SHOW);
        m_GrayWhiteButtons->BringWindowToTop();
    }

    // 恢复字体对象
    memDC.SelectObject(pOldFont);
    memDC.SetTextColor(oldColor);

    // 最后将内存DC内容一次性复制到屏幕DC
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldMemBmp);
}

// CBarTsView 打印

BOOL CBarTsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CBarTsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CBarTsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CBarTsView 诊断

#ifdef _DEBUG
void CBarTsView::AssertValid() const
{
	CView::AssertValid();
}

void CBarTsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBarTsDoc* CBarTsView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBarTsDoc)));
	return (CBarTsDoc*)m_pDocument;
}
#endif //_DEBUG


// CBarTsView 消息处理程序

void CBarTsView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // 只创建一次控件
    if (m_modalSliders.empty())
    {
        CRect clientRect;
        GetClientRect(&clientRect);

        const int IMG_SIZE = 240; // 匹配实际图像大小
        const int MARGIN = 10;

        m_modalSliders.resize(4, nullptr);
        m_sliceIndices.resize(4, 75);
        m_segmentButtons.resize(4, nullptr);
        m_restoreButtons.resize(4, nullptr);

        for (int i = 0; i < 4; ++i)
        {
            // 基于图像大小计算位置
            int col = i % 2;
            int row = i / 2;

            int xOffset = MARGIN + col * (IMG_SIZE + MARGIN);
            int yOffset = 50 + row * (IMG_SIZE + 80); // 50为顶部留白，80为图像下方空间

            // 创建滑块
            if (!m_modalSliders[i])
            {
                m_modalSliders[i] = new CGoldSlider();
                CRect sliderRect(xOffset, yOffset + IMG_SIZE + 10,
                    xOffset + IMG_SIZE, yOffset + IMG_SIZE + 30);
                m_modalSliders[i]->Create(WS_CHILD | WS_VISIBLE | TBS_HORZ,
                    sliderRect, this, 1000 + i);
            }
            m_modalSliders[i]->SetRange(0, 149);
            m_modalSliders[i]->SetPos(75);

            // 创建分割按钮
            if (!m_segmentButtons[i])
            {
                COLORREF BlueColor = RGB(189, 215, 238);
                m_segmentButtons[i] = new CColorButton(BlueColor);
                CRect segBtnRect(xOffset, yOffset + IMG_SIZE + 40,
                    xOffset + 80, yOffset + IMG_SIZE + 65);
                m_segmentButtons[i]->Create(_T("分割"), WS_CHILD | WS_VISIBLE,
                    segBtnRect, this, ID_SEGMENT_BTN_BASE + i);
            }

            // 创建还原按钮
            if (!m_restoreButtons[i])
            {
                COLORREF GreenColor = RGB(200, 229, 129);
                m_restoreButtons[i] = new CColorButton(GreenColor);
                CRect resetBtnRect(xOffset + 90, yOffset + IMG_SIZE + 40,
                    xOffset + 170, yOffset + IMG_SIZE + 65);
                m_restoreButtons[i]->Create(_T("还原"), WS_CHILD | WS_VISIBLE,
                    resetBtnRect, this, ID_RESTORE_BTN_BASE + i);
            }


        }

        // 创建总控滑块 - 使用浮动位置（在OnDraw中调整）
        if (!::IsWindow(m_masterSlider.GetSafeHwnd()))
        {
            // 临时矩形，实际位置在OnDraw中设置
            CRect sliderRect(0, 0, 30, 300);
            m_masterSlider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_AUTOTICKS,
                sliderRect, this, IDC_MASTER_SLIDER);
        }

        if (!::IsWindow(m_masterLabel.GetSafeHwnd()))
        {
            CRect labelRect(0, 0, 40, 20);
            m_masterLabel.Create(_T("总控"), WS_CHILD | WS_VISIBLE | SS_CENTER,
                labelRect, this, IDC_STATIC_MASTER_LABEL);
        }

        // 创建灰白质分割按钮
        if (m_GrayWhiteButtons == nullptr) {
            COLORREF CyanColor = RGB(212, 244, 242);
            m_GrayWhiteButtons = new CColorButton(CyanColor);

            CRect segBtnRect(400, 600, 500, 630);
            m_GrayWhiteButtons->Create(_T("灰白质分割"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                segBtnRect, this, ID_GRAYWHITESEGMENT);
        }

        // 设置初始范围（实际值在OnDraw中更新）
        m_masterSlider.SetRange(0, 149);
        m_masterSlider.SetPos(75);
    }
}


void CBarTsView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || !pDoc->m_pNiftiImg) return;

    HWND hScrollWnd = pScrollBar->GetSafeHwnd();

    //只需处理一个小滑块，大滑块是垂直总控来处理
    for (int i = 0; i < m_modalSliders.size(); ++i)
    {
        if (hScrollWnd == m_modalSliders[i]->GetSafeHwnd())
        {
            m_sliceIndices[i] = m_modalSliders[i]->GetPos();

            // 更新该模态对应切片
            pDoc->UpdateSingleModalSlice(i, m_sliceIndices[i]); // 推荐你实现这个函数（见下方）
            if (i == 0 && m_flairSegActive) {
                SegmentFlairSlice(m_sliceIndices[0]);
            }

            if (i == 1 && m_t1SegActive) {
                BlendSegmentMaskOntoT1(m_sliceIndices[1]);
            }

            if (i == 2 && m_t1cSegActive) {
                SegmentT1CSlice(m_sliceIndices[2]);
            }

            if (i == 3 && m_t2SegActive) {
                SegmentT2Slice(m_sliceIndices[3]);
            }

            // 只刷新当前模态的区域（你的代码保留，稍微整理）
            int margin = 10, topMargin = 40;
            int imgW = pDoc->m_allModalImages[0][0].cols;
            int imgH = pDoc->m_allModalImages[0][0].rows;
            int drawX = margin + (i % 2) * (imgW + margin);
            int drawY = topMargin + margin + (i / 2) * (imgH + 80 + margin);
            CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
            InvalidateRect(updateRect, FALSE);

            break;
        }
    }
    
    CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

//分割按钮被点击函数
void CBarTsView::OnSegmentButtonClicked(UINT nID)
{
    int index = nID - ID_SEGMENT_BTN_BASE;  // 计算是哪一个按钮被点击

    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || !pDoc->m_pNiftiImg) return;

    if (index == 0) {
        int flairSliceIdx = (m_sliceIndices.size() > 0) ? m_sliceIndices[0] : 75;
        SegmentFlairSlice(flairSliceIdx);
        m_flairSegActive = true;

        // 刷新第0个模态区域
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[0][0].cols;
        int imgH = pDoc->m_allModalImages[0][0].rows;
        int drawX = margin;
        int drawY = topMargin + margin;
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }

    if (index == 1) {
        int t1SliceIdx = (m_sliceIndices.size() > 0) ? m_sliceIndices[1] : 75;
        BlendSegmentMaskOntoT1(t1SliceIdx);
        m_t1SegActive = true;

        // 刷新第1个模态区域，同时调用三个分割
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[1][0].cols;
        int imgH = pDoc->m_allModalImages[1][0].rows;
        int drawX = margin + (1 % 2) * (imgW + margin);;
        int drawY = topMargin + margin;
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }

    if (index == 2) {
        int t1cSliceIdx = (m_sliceIndices.size() > 0) ? m_sliceIndices[2] : 75;
        SegmentT1CSlice(t1cSliceIdx);
        m_t1cSegActive = true;

        // 刷新第2个模态区域
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[2][0].cols;
        int imgH = pDoc->m_allModalImages[2][0].rows;
        int drawX = margin + (2 % 2) * (imgW + margin);
        int drawY = topMargin + margin + (2 / 2) * (imgH + 80 + margin);
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }

    if (index == 3) {
        int t2SliceIdx = (m_sliceIndices.size() > 0) ? m_sliceIndices[3] : 75;
        SegmentT2Slice(t2SliceIdx); // 调用T2分割
        m_t2SegActive = true;       // 如有需要

        // 刷新第4个模态区域
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[3][0].cols;
        int imgH = pDoc->m_allModalImages[3][0].rows;
        int drawX = margin + (3 % 2) * (imgW + margin);
        int drawY = topMargin + margin + (3 / 2) * (imgH + 80 + margin);
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }

}

void CBarTsView::OnRestoreButtonClicked(UINT nID) {
    int index = nID - ID_RESTORE_BTN_BASE;  // 计算是哪一个按钮被点击

    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || !pDoc->m_pNiftiImg) return;

    //关闭激活按钮
    if (index == 0) { 
        m_flairSegActive = false; //设置激活位为flase

        // 刷新第0个模态区域
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[0][0].cols;
        int imgH = pDoc->m_allModalImages[0][0].rows;
        int drawX = margin;
        int drawY = topMargin + margin;
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }
    else if (index == 1) { 
         m_t1SegActive = false; 

        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[1][0].cols;
        int imgH = pDoc->m_allModalImages[1][0].rows;
        int drawX = margin + (1 % 2) * (imgW + margin);;
        int drawY = topMargin + margin;
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }
    else if (index == 2) {
        m_t1cSegActive = false;

        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[2][0].cols;
        int imgH = pDoc->m_allModalImages[2][0].rows;
        int drawX = margin + (2 % 2) * (imgW + margin);
        int drawY = topMargin + margin + (2 / 2) * (imgH + 80 + margin);
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }
    else {
        m_t2SegActive = false;

        // 刷新第4个模态区域
        int margin = 10, topMargin = 40;
        int imgW = pDoc->m_allModalImages[3][0].cols;
        int imgH = pDoc->m_allModalImages[3][0].rows;
        int drawX = margin + (3 % 2) * (imgW + margin);
        int drawY = topMargin + margin + (3 / 2) * (imgH + 80 + margin);
        CRect updateRect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
        InvalidateRect(updateRect, FALSE);
    }

}


void CBarTsView::SegmentFlairSlice(int flairSliceIdx)
{
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || pDoc->m_allModalImages.empty() || pDoc->m_allModalImages[0].empty())
        return;

    if (flairSliceIdx < 0 || flairSliceIdx >= pDoc->m_allModalImages[0].size())
        return;

    // 关键改进：使用背景感知的亮度归一化
    const int referenceSliceIdx = 75; // 以第75张作为基准
    const int currentModal = 0;       // FLAIR模态

    // 只归一化当前切片（以参考切片为基准）
    pDoc->BackgroundAwareNormalization(
        currentModal,
        flairSliceIdx,
        referenceSliceIdx
    );

    // 获取当前切片
    const cv::Mat& flairSlice = pDoc->m_allModalImages[0][flairSliceIdx];
    if (flairSlice.empty())
        return;

    // 关键改进2：创建脑组织掩码用于分割
    cv::Mat brainMask = pDoc->CreateBrainMask(flairSlice);

    // 转换为灰度图
    cv::Mat graySlice;
    if (flairSlice.channels() == 3) {
        cv::cvtColor(flairSlice, graySlice, cv::COLOR_BGR2GRAY);
    }
    else {
        graySlice = flairSlice.clone();
    }

    // 关键改进3：只对脑组织区域进行归一化
    cv::Mat normSlice;
    double minVal, maxVal;
    cv::minMaxLoc(graySlice, &minVal, &maxVal, nullptr, nullptr, brainMask);

    if (fabs(maxVal - minVal) < 1e-6) {
        normSlice = cv::Mat::zeros(graySlice.size(), CV_8U);
    }
    else {
        // 使用掩码归一化脑组织区域
        normSlice = graySlice.clone();
        normSlice.convertTo(normSlice, CV_32F);

        for (int y = 0; y < normSlice.rows; y++) {
            for (int x = 0; x < normSlice.cols; x++) {
                if (brainMask.at<uchar>(y, x) > 0) {
                    float& pixel = normSlice.at<float>(y, x);
                    pixel = 255 * (pixel - minVal) / (maxVal - minVal);
                }
                else {
                    normSlice.at<float>(y, x) = 0; // 背景设为0
                }
            }
        }
        normSlice.convertTo(normSlice, CV_8U);
    }

    cv::Mat markers1;
    if (LightAll) {
        markers1 = CDib::applyWatershed(normSlice, 80);
    }
    else {
        markers1 = CDib::applyWatershed(normSlice, 120);
    }
    int tumor_label = -1, tumor_area = 0;
    cv::Mat tumor_mask = CDib::getTumorRegionMask(markers1, tumor_label, tumor_area);
    cv::Mat colorNormSlice;
    cv::cvtColor(normSlice, colorNormSlice, cv::COLOR_GRAY2BGR);
    cv::Mat blended = CDib::applyTumorOverlay(colorNormSlice, tumor_mask);

    pDoc->m_blendedFlair = blended;
}

void CBarTsView::SegmentT1CSlice(int T1CSliceIdx) {

    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || pDoc->m_allModalImages.empty() || pDoc->m_allModalImages[2].empty())
        return;
    if (T1CSliceIdx < 0 || T1CSliceIdx >= pDoc->m_allModalImages[2].size())
        return;
    const cv::Mat& t1cSlice = pDoc->m_allModalImages[2][T1CSliceIdx];
    if (t1cSlice.empty())
        return;

    // 步骤1：归一化
    // 关键改进：使用背景感知的亮度归一化
    const int referenceSliceIdx = 75; // 以第75张作为基准
    const int currentModal = 2;       // T1c模态

    // 只归一化当前切片（以参考切片为基准）
    pDoc->BackgroundAwareNormalization(
        currentModal,
        T1CSliceIdx,
        referenceSliceIdx
    );

    // 关键改进2：创建脑组织掩码用于分割
    cv::Mat brainMask = pDoc->CreateBrainMask(t1cSlice);

    // 转换为灰度图
    cv::Mat graySlice;
    if (t1cSlice.channels() == 3) {
        cv::cvtColor(t1cSlice, graySlice, cv::COLOR_BGR2GRAY);
    }
    else {
        graySlice = t1cSlice.clone();
    }

    // 关键改进3：只对脑组织区域进行归一化
    cv::Mat normSlice;
    double minVal, maxVal;
    cv::minMaxLoc(graySlice, &minVal, &maxVal, nullptr, nullptr, brainMask);

    if (fabs(maxVal - minVal) < 1e-6) {
        normSlice = cv::Mat::zeros(graySlice.size(), CV_8U);
    }
    else {
        // 使用掩码归一化脑组织区域
        normSlice = graySlice.clone();
        normSlice.convertTo(normSlice, CV_32F);

        for (int y = 0; y < normSlice.rows; y++) {
            for (int x = 0; x < normSlice.cols; x++) {
                if (brainMask.at<uchar>(y, x) > 0) {
                    float& pixel = normSlice.at<float>(y, x);
                    pixel = 255 * (pixel - minVal) / (maxVal - minVal);
                }
                else {
                    normSlice.at<float>(y, x) = 0; // 背景设为0
                }
            }
        }
        normSlice.convertTo(normSlice, CV_8U);
    }


    cv::Mat markers2;

    // 步骤2：分水岭
    if (LightAll) {
        markers2 = CDib::applyWatershed_T1c(normSlice, 100, 200);
    }
    else {
        markers2 = CDib::applyWatershed_T1c(normSlice, 150, 200);
    }

    // 步骤3：初步肿瘤掩码
    cv::Mat tumorMask = cv::Mat::zeros(markers2.size(), CV_8U);
    int tumor_label = 0, tumor_area = 0;
    for (int y = 0; y < markers2.rows; ++y) {
        for (int x = 0; x < markers2.cols; ++x) {
            int label = markers2.at<int>(y, x);
            if (label > 1) {
                tumorMask.at<uchar>(y, x) = 255;
                if (tumor_label == 0) tumor_label = label;
            }
        }
    }

    // 步骤4：填洞
    cv::Mat filledTumorMask = CDib::fillMaskHoles(tumorMask);

    // 步骤5：用填洞结果修正markers2
    for (int y = 0; y < markers2.rows; ++y) {
        for (int x = 0; x < markers2.cols; ++x) {
            if (filledTumorMask.at<uchar>(y, x) == 255 && tumorMask.at<uchar>(y, x) == 0) {
                int found_label = 0;
                // 寻找邻域已有的肿瘤标签
                for (int dy = -1; dy <= 1 && found_label == 0; dy++) {
                    for (int dx = -1; dx <= 1 && found_label == 0; dx++) {
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < markers2.rows && nx >= 0 && nx < markers2.cols) {
                            if (tumorMask.at<uchar>(ny, nx) == 255) {
                                found_label = markers2.at<int>(ny, nx);
                            }
                        }
                    }
                }
                if (found_label != 0)
                    markers2.at<int>(y, x) = found_label;
            }
        }
    }

    // 步骤6：重新生成肿瘤掩码
    cv::Mat updatedTumorMask = CDib::getTumorRegionMask(markers2, tumor_label, tumor_area);

    // 步骤7：形态学处理去噪点并恢复边缘
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::Mat erodedMask, processedMask;
    cv::erode(updatedTumorMask, erodedMask, kernel, cv::Point(-1, -1), 4);
    cv::dilate(erodedMask, processedMask, kernel, cv::Point(-1, -1), 5);
    updatedTumorMask = processedMask;

    // 步骤8：叠加黄色透明覆盖
    cv::Mat colorNormSlice;
    cv::cvtColor(normSlice, colorNormSlice, cv::COLOR_GRAY2BGR);
    cv::Vec3b color_t1c = cv::Vec3b(0, 255, 255);
    cv::Mat blended = CDib::applyTumorOverlay(colorNormSlice, updatedTumorMask, 0.5, color_t1c);

    // 保存到文档成员
    pDoc->m_blendedT1C = blended;
}

void CBarTsView::SegmentT2Slice(int T2SliceIdx)
{
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || pDoc->m_allModalImages.empty() || pDoc->m_allModalImages[3].empty())
        return;

    if (T2SliceIdx < 0 || T2SliceIdx >= pDoc->m_allModalImages[3].size())
        return;

    const cv::Mat& T2Slice = pDoc->m_allModalImages[3][T2SliceIdx];
    if (T2Slice.empty())
        return;

    double minVal = 0.0, maxVal = 0.0;
    cv::minMaxLoc(T2Slice, &minVal, &maxVal);

    cv::Mat normSlice;
    if (fabs(maxVal - minVal) < 1e-6)
        normSlice = cv::Mat::zeros(T2Slice.size(), CV_8U);
    else
    {
        cv::normalize(T2Slice, normSlice, 0, 255, cv::NORM_MINMAX);
        normSlice.convertTo(normSlice, CV_8U);
    }

    cv::Mat markers1;
    if(LightAll){ 
        markers1 = CDib::applyWatershed(normSlice, 80.0); 
    }
    else {
        markers1 = CDib::applyWatershed(normSlice, 140.0);
    }
    int tumor_label = -1, tumor_area = 0;
    cv::Mat tumor_mask = CDib::getTumorRegionMask(markers1, tumor_label, tumor_area);
    cv::Mat colorNormSlice;
    cv::cvtColor(normSlice, colorNormSlice, cv::COLOR_GRAY2BGR);
    cv::Vec3b color_t2 = cv::Vec3b(255, 0, 255);
    cv::Mat blended = CDib::applyTumorOverlay(colorNormSlice, tumor_mask,0.5, color_t2);

    pDoc->m_blendedT2 = blended;
}

//T1模态显示所有肿瘤掩码
void CBarTsView::BlendSegmentMaskOntoT1(int t1SliceIdx)
{
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc || pDoc->m_allModalImages.size() < 4)
        return;

    // 检查索引合法性
    for (int i = 0; i < 4; ++i)
    {
        if (pDoc->m_allModalImages[i].empty() || t1SliceIdx >= pDoc->m_allModalImages[i].size())
            return;
    }

    // Step 1：获取对应模态切片
    const cv::Mat& flairSlice = pDoc->m_allModalImages[0][t1SliceIdx];
    const cv::Mat& t1Slice = pDoc->m_allModalImages[1][t1SliceIdx];  // 用于最终显示
    const cv::Mat& t1cSlice = pDoc->m_allModalImages[2][t1SliceIdx];
    const cv::Mat& t2Slice = pDoc->m_allModalImages[3][t1SliceIdx];

    if (flairSlice.empty() || t1Slice.empty() || t1cSlice.empty() || t2Slice.empty())
        return;

    // Step 2：准备各模态肿瘤掩码
    auto getTumorMaskFromWatershed = [](const cv::Mat& slice, double threshold = 120.0) -> cv::Mat
        {
            double minVal = 0.0, maxVal = 0.0;
            cv::minMaxLoc(slice, &minVal, &maxVal);
            cv::Mat norm;
            if (fabs(maxVal - minVal) < 1e-6)
                norm = cv::Mat::zeros(slice.size(), CV_8U);
            else {
                cv::normalize(slice, norm, 0, 255, cv::NORM_MINMAX);
                norm.convertTo(norm, CV_8U);
            }

            cv::Mat markers = CDib::applyWatershed(norm, threshold);
            int label = -1, area = 0;
            return CDib::getTumorRegionMask(markers, label, area);
        };

    // Step 3：获取每个模态的肿瘤掩码
    cv::Mat flairMask = CDib::GetFlairTumorMask(flairSlice, LightAll);
    cv::Mat t1cMask = CDib::GetT1cTumorMask(t1cSlice, LightAll);
    cv::Mat t2Mask = CDib::GetT2TumorMask(t2Slice, LightAll);  // threshold 默认为 140.0，可微调


    // Step 4：归一化并转换 T1 图像为 BGR 彩图
    double minT1, maxT1;
    cv::minMaxLoc(t1Slice, &minT1, &maxT1);
    cv::Mat normT1;
    if (fabs(maxT1 - minT1) < 1e-6)
        normT1 = cv::Mat::zeros(t1Slice.size(), CV_8U);
    else {
        cv::normalize(t1Slice, normT1, 0, 255, cv::NORM_MINMAX);
        normT1.convertTo(normT1, CV_8U);
    }
    // Step 1：构建 colorT1 彩色灰阶图
    cv::Mat colorT1;
    cv::normalize(t1Slice, normT1, 0, 255, cv::NORM_MINMAX);
    normT1.convertTo(normT1, CV_8U);
    cv::cvtColor(normT1, colorT1, cv::COLOR_GRAY2BGR);

    // Step 2：创建彩色 mask 层，每个模态一个
    cv::Mat colorMask = cv::Mat::zeros(colorT1.size(), CV_8UC3);

    // 给每个模态掩码上色（按优先级写入）
    for (int y = 0; y < colorT1.rows; ++y) {
        for (int x = 0; x < colorT1.cols; ++x) {
            if (t2Mask.at<uchar>(y, x) > 0)
                colorMask.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 0, 255); // 紫色
            else if (t1cMask.at<uchar>(y, x) > 0)
                colorMask.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 255, 255); // 黄色
            else if (flairMask.at<uchar>(y, x) > 0)
                colorMask.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 0, 0);   // 红色
        }
    }

    // Step 3：使用掩码和 alpha 融合上色区域（非掩码区域保持灰度）
    cv::Mat blended = colorT1.clone();
    double alpha = 0.5;
    for (int y = 0; y < blended.rows; ++y) {
        for (int x = 0; x < blended.cols; ++x) {
            cv::Vec3b overlay = colorMask.at<cv::Vec3b>(y, x);
            if (overlay != cv::Vec3b(0, 0, 0)) {
                blended.at<cv::Vec3b>(y, x) =
                    (1.0 - alpha) * blended.at<cv::Vec3b>(y, x) + alpha * overlay;
            }
        }
    }

    // Step 4：保存
    pDoc->m_blendedT1 = blended;
}

BOOL CBarTsView::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(rect, RGB(0, 0, 0)); // 深色背景
    return TRUE; // 不再调用默认背景清除

    //return CView::OnEraseBkgnd(pDC);
}

//设置滑块深色
HBRUSH CBarTsView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CView::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性

    static HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 0)); // 深灰色背景

    if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_BTN)
    {
        pDC->SetBkMode(TRANSPARENT);
        pDC->SetBkColor(RGB(220, 220, 220));         // 背景色
        pDC->SetTextColor(RGB(220, 220, 220));    // 字体颜色
        return hBrush;
    }

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}

void CBarTsView::OnClaheEnhance()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->ApplyCLAHEEnhancement(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnRestore()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (pDoc) {
        pDoc->Restore();
    }

    Invalidate();  // 触发重绘
}

void CBarTsView::OnSobelEdge()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->SobelEdge(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnAdaptivenoise()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->AdapativeNoise(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnGaussiannoise()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->GaussianNoise(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnBilfilter()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->BilateralFilter(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnLight()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->HomomorphicCorrection(m_sliceIndices);  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}

void CBarTsView::OnAllLight()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    LightAll = true;
    pDoc->LightAllCorrection();
    Invalidate();  // 触发重绘
}

void CBarTsView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;
        // 判断是否是大滑块
    if (pScrollBar && pScrollBar->GetSafeHwnd() == m_masterSlider.GetSafeHwnd())
    {
        int value = m_masterSlider.GetPos();

        for (int i = 0; i < m_modalSliders.size(); ++i)
        {
            if (m_sliceIndices[i] == value) continue;

            m_sliceIndices[i] = value;
            if (m_modalSliders[i] && ::IsWindow(m_modalSliders[i]->GetSafeHwnd()))
                m_modalSliders[i]->SetPos(value);

            pDoc->UpdateSingleModalSlice(i, m_sliceIndices[i]);

            if (i == 0 && m_flairSegActive) {
                SegmentFlairSlice(m_sliceIndices[0]);
            }
            if (i == 1 && m_t1SegActive) {
                BlendSegmentMaskOntoT1(m_sliceIndices[1]);
            }
            if (i == 2 && m_t1cSegActive) {
                SegmentT1CSlice(m_sliceIndices[2]);
            }
            if (i == 3 && m_t2SegActive) {
                SegmentT2Slice(m_sliceIndices[3]);
            }
        }

        // 更新区域
        // 计算普通4模态区域
        CRect updateRect;
        for (int i = 0; i < 4; ++i) {
            int margin = 10, topMargin = 40;
            int imgW = pDoc->m_allModalImages[0][0].cols;
            int imgH = pDoc->m_allModalImages[0][0].rows;
            int drawX = margin + (i % 2) * (imgW + margin);
            int drawY = topMargin + margin + (i / 2) * (imgH + 80 + margin);
            CRect rect(drawX, drawY - 30, drawX + imgW, drawY + imgH + 50);
            if (i == 0) updateRect = rect;
            else updateRect.UnionRect(updateRect, rect);
        }

        // 计算灰白质掩码区域，叠加到 updateRect
        if (!pDoc->m_grayMatterSlices.empty() || !pDoc->m_whiteMatterSlices.empty())
        {
            const int SEG_MARGIN = 20;
            int segYOffset = 40 + 2 * (pDoc->m_allModalImages[0][0].rows + 140) + SEG_MARGIN;
            int segXOffset = 10;
            int imgW = pDoc->m_allModalImages[0][0].cols;
            int imgH = pDoc->m_allModalImages[0][0].rows;

            // 灰质区域
            CRect grayRect(segXOffset, segYOffset - 30, segXOffset + imgW, segYOffset + imgH + 50);
            updateRect.UnionRect(updateRect, grayRect);

            // 白质区域（在灰质右侧）
            CRect whiteRect(segXOffset + imgW + 10, segYOffset - 30, segXOffset + imgW * 2 + 10, segYOffset + imgH + 50);
            updateRect.UnionRect(updateRect, whiteRect);
        }

        InvalidateRect(updateRect, FALSE);

        // 灰白质区域更新
    }
    else {
        // 默认传给基类
        CView::OnVScroll(nSBCode, nPos, pScrollBar);
    }
}

void CBarTsView::OnGrayWhiteClicked()
{
    // TODO: 在此添加命令处理程序代码
    CBarTsDoc* pDoc = GetDocument();
    if (!pDoc) return;

    pDoc->PerformSegmentation();  // 传入当前四个模态的切片索引
    Invalidate();  // 触发重绘
}
