
// BarTsDoc.cpp: CBarTsDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "BarTs.h"
#endif

#include "BarTsDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBarTsDoc

IMPLEMENT_DYNCREATE(CBarTsDoc, CDocument)

BEGIN_MESSAGE_MAP(CBarTsDoc, CDocument)
END_MESSAGE_MAP()


// CBarTsDoc 构造/析构

CBarTsDoc::CBarTsDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
    m_sliceIndices = { 75,75,75,75 };
}

CBarTsDoc::~CBarTsDoc()
{
}

BOOL CBarTsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


// CBarTsDoc 序列化

void CBarTsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CBarTsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CBarTsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CBarTsDoc::SetSearchContent(const CString& value)
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

// CBarTsDoc 诊断

#ifdef _DEBUG
void CBarTsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBarTsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

bool CBarTsDoc::LoadNiftiFile(const CString& path)
{
    if (m_pNiftiImg) {
        nifti_image_free(m_pNiftiImg);
        m_pNiftiImg = nullptr;
    }

    CStringA pathA(path);
    m_pNiftiImg = nifti_image_read(pathA.GetString(), 1);
    if (!m_pNiftiImg) return false;

    int w = m_pNiftiImg->nx;
    int h = m_pNiftiImg->ny;
    int d = m_pNiftiImg->nz;
    int c = std::max(1, m_pNiftiImg->nt);
    int middle = d / 2;

    float* data = reinterpret_cast<float*>(m_pNiftiImg->data);

    m_allModalImages.clear();
    m_allModalImages.resize(c);
    m_originalModalImages.clear();
    m_originalModalImages.resize(c);

    for (int t = 0; t < c; ++t)
    {
        m_allModalImages[t].clear();
        m_originalModalImages[t].clear();

        for (int z = 0; z < d; ++z)
        {
            // 读取原始 float 切片
            cv::Mat sliceFloat(h, w, CV_32F);
            for (int y = 0; y < h; ++y)
                for (int x = 0; x < w; ++x)
                    sliceFloat.at<float>(y, x) = data[x + y * w + z * w * h + t * w * h * d];

            // 保存原始 float 图
            m_originalModalImages[t].push_back(sliceFloat.clone());

            // 归一化转CV_8U
            double minVal, maxVal;
            cv::minMaxLoc(sliceFloat, &minVal, &maxVal);

            cv::Mat normSlice;
            if (fabs(maxVal - minVal) < 1e-6)
            {
                normSlice = cv::Mat::zeros(h, w, CV_8U);
            }
            else
            {
                cv::normalize(sliceFloat, normSlice, 0, 255, cv::NORM_MINMAX);
                normSlice.convertTo(normSlice, CV_8U);
            }

            m_allModalImages[t].push_back(normSlice);
        }
    }

    return true;
}


void CBarTsDoc::NotifyViewsRefresh()
{
    POSITION pos = GetFirstViewPosition();
    while (pos)
    {
        CView* pView = GetNextView(pos);
        if (pView) {
            pView->Invalidate();
            pView->UpdateWindow();
        }
    }
}

void CBarTsDoc::UpdateSlices(const std::vector<int>& sliceIndices)
{
    if (!m_pNiftiImg) return;

    int w = m_pNiftiImg->nx;
    int h = m_pNiftiImg->ny;
    int d = m_pNiftiImg->nz;
    int c = m_pNiftiImg->nt;

    float* data = (float*)m_pNiftiImg->data;

    // 初始化各模态图像数组
    if (m_allModalImages.size() != c)
        m_allModalImages.resize(c);

    m_sliceIndices = sliceIndices;

    for (int t = 0; t < c; ++t)
    {
        int z = (t < sliceIndices.size()) ? sliceIndices[t] : d / 2;

        // 更新当前切片索引
        if (m_allModalImages[t].size() != d)
            m_allModalImages[t].resize(d);  // 一次性保存所有切片也可，节省未来重新提取

        cv::Mat slice(h, w, CV_32F);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                slice.at<float>(y, x) = data[x + y * w + z * w * h + t * w * h * d];

        cv::Mat normSlice;
        cv::normalize(slice, normSlice, 0, 255, cv::NORM_MINMAX);
        normSlice.convertTo(normSlice, CV_8U);

        m_allModalImages[t][z] = normSlice.clone();  // 存储为当前切片
    }

    // 不触发分割，不触发 m_segMasks
    UpdateAllViews(nullptr);  // 让界面刷新新的切片
}



//单模态更新代码
void CBarTsDoc::UpdateSingleModalSlice(int modalityIndex, int sliceIndex)
{
    if (!m_pNiftiImg) return;

    int w = m_pNiftiImg->nx;
    int h = m_pNiftiImg->ny;
    int d = m_pNiftiImg->nz;
    int c = m_pNiftiImg->nt;

    if (modalityIndex < 0 || modalityIndex >= c) return;
    if (sliceIndex < 0 || sliceIndex >= d) return;

    // 初始化缓存
    if (m_allModalImages.size() != c)
        m_allModalImages.resize(c);
    if (m_allModalImages[modalityIndex].size() != d)
        m_allModalImages[modalityIndex].resize(d);

    // 判断缓存是否已存在该切片，若没有则计算生成
    if (m_allModalImages[modalityIndex][sliceIndex].empty())
    {
        float* data = (float*)m_pNiftiImg->data;

        cv::Mat slice(h, w, CV_32F);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                slice.at<float>(y, x) = data[x + y * w + sliceIndex * w * h + modalityIndex * w * h * d];

        cv::Mat normSlice;
        cv::normalize(slice, normSlice, 0, 255, cv::NORM_MINMAX);
        normSlice.convertTo(normSlice, CV_8U);

        m_allModalImages[modalityIndex][sliceIndex] = normSlice;
    }

    // 记录当前显示的切片索引
    if (modalityIndex >= m_sliceIndices.size())
        m_sliceIndices.resize(modalityIndex + 1);
    m_sliceIndices[modalityIndex] = sliceIndex;
}


// CBarTsDoc 命令

// 恢复原图操作
void CBarTsDoc::Restore()
{
    LightAll = false;

    if (!m_originalModalImages.empty()) {
        // 确保 m_allModalImages 结构和 m_originalModalImages 一致
        if (m_allModalImages.size() != m_originalModalImages.size())
            m_allModalImages.resize(m_originalModalImages.size());

        for (int modal = 0; modal < m_originalModalImages.size(); ++modal) {
            if (m_allModalImages[modal].size() != m_originalModalImages[modal].size())
                m_allModalImages[modal].resize(m_originalModalImages[modal].size());

            for (int slice = 0; slice < m_originalModalImages[modal].size(); ++slice) {
                // 原始图是 float，要重新归一化成 uint8
                const cv::Mat& floatSlice = m_originalModalImages[modal][slice];

                double minVal, maxVal;
                cv::minMaxLoc(floatSlice, &minVal, &maxVal);

                cv::Mat normSlice;
                if (fabs(maxVal - minVal) < 1e-6)
                    normSlice = cv::Mat::zeros(floatSlice.size(), CV_8U);
                else {
                    cv::normalize(floatSlice, normSlice, 0, 255, cv::NORM_MINMAX);
                    normSlice.convertTo(normSlice, CV_8U);
                }

                m_allModalImages[modal][slice] = normSlice;
            }
        }

        UpdateAllViews(nullptr);  // 恢复完后刷新视图
    }
}

// 图像增强操作
void CBarTsDoc::ApplyCLAHEEnhancement(const std::vector<int>& sliceIndices)
{
    m_enhancedSlices.clear();
    m_enhancedSlices.resize(4);

    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4)
        return;

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->setTilesGridSize(cv::Size(8, 8));

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size())
            continue;

        const cv::Mat& src = m_allModalImages[i][sliceIdx];

        cv::Mat processedImage = src.clone();

        cv::Mat enhanced;
        clahe->apply(processedImage, enhanced);

        // 替换原始图像容器中的切片
        m_allModalImages[i][sliceIdx] = enhanced.clone();

        m_enhancedSlices[i] = enhanced;
    }
}


void CBarTsDoc::SobelEdge(const std::vector<int>& sliceIndices)
{

    // 检查输入有效性
    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4) {
        TRACE("Error: Invalid input sizes\n");
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];

        // 检查索引有效性
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size()) {
            TRACE("Warning: Invalid slice index %d for modal %d\n", sliceIdx, i);
            continue;
        }

        // 获取当前切片（非const引用）
        cv::Mat& currentSlice = m_allModalImages[i][sliceIdx];

        // 检查图像是否有效
        if (currentSlice.empty()) {
            TRACE("Warning: Empty image for modal %d, slice %d\n", i, sliceIdx);
            continue;
        }

        // 处理图像副本
        cv::Mat processedImage = currentSlice.clone();

        // Sobel边缘检测
        cv::Mat edgeImage;
        try {
            // 1. 高斯模糊
            cv::Mat blurred;
            cv::GaussianBlur(processedImage, blurred, cv::Size(5, 5), 1.5);

            // 2. Sobel梯度计算
            cv::Mat gradX, gradY;
            cv::Sobel(blurred, gradX, CV_32F, 1, 0, 3);
            cv::Sobel(blurred, gradY, CV_32F, 0, 1, 3);

            // 3. 计算梯度幅值
            cv::Mat gradMag;
            cv::magnitude(gradX, gradY, gradMag);

            // 4. 归一化到0-255
            cv::normalize(gradMag, gradMag, 0, 255, cv::NORM_MINMAX);
            gradMag.convertTo(edgeImage, CV_8U);
        }
        catch (const cv::Exception& e) {
            TRACE("OpenCV error in Sobel edge detection: %s\n", e.what());
            continue;
        }

        // 更新图像容器
        currentSlice = edgeImage.clone();

        // 替换原始图像容器中的切片
        m_allModalImages[i][sliceIdx] = edgeImage.clone();
    }

    // 更新视图
    UpdateAllViews(NULL);
}

void CBarTsDoc::AdapativeNoise(const std::vector<int>& sliceIndices)
{
    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4)
        return;

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size())
            continue;

        cv::Mat& src = m_allModalImages[i][sliceIdx]; // 原始图像

        cv::Mat processedImage = src.clone();

        // 关键修复：使用临时变量存储中间结果
        cv::Mat temp1, temp2;

        // 1. 自适应中值滤波 - 使用临时变量存储结果
        cv::medianBlur(processedImage, temp1, 5);

        // 2. 非局部均值去噪 - 输入和输出不能是同一个矩阵
        cv::fastNlMeansDenoising(temp1, temp2, 10, 7, 21);

        // 3. 将最终结果复制回原始图像
        temp2.copyTo(src);

        // 替换原始图像容器中的切片
        m_allModalImages[i][sliceIdx] = src.clone();
    }

    // 更新视图
    UpdateAllViews(NULL);
}

void CBarTsDoc::GaussianNoise(const std::vector<int>& sliceIndices)
{
    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4) {
        TRACE("Error: Invalid input sizes\n");
        return;
    }

    // 高斯去噪参数
    const int kernelSize = 5;  // 高斯核大小（必须是奇数）
    const double sigma = 1.5;  // 标准差

    // 验证核大小是奇数
    if (kernelSize % 2 == 0) {
        TRACE("Error: Kernel size must be odd number\n");
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size()) {
            TRACE("Warning: Invalid slice index %d for modal %d\n", sliceIdx, i);
            continue;
        }

        cv::Mat& src = m_allModalImages[i][sliceIdx]; // 原始图像

        // 检查图像是否有效
        if (src.empty()) {
            TRACE("Warning: Empty image for modal %d, slice %d\n", i, sliceIdx);
            continue;
        }

        cv::Mat processedImage = src.clone();

        try {
            // 高斯去噪
            cv::Mat denoised;
            cv::GaussianBlur(processedImage, denoised, cv::Size(kernelSize, kernelSize), sigma);

            // 更新原始图像
            denoised.copyTo(src);

            // 替换原始图像容器中的切片
            m_allModalImages[i][sliceIdx] = denoised.clone();
        }
        catch (const cv::Exception& e) {
            TRACE("OpenCV error in Gaussian denoising: %s\n", e.what());
            continue;
        }
    }

    // 更新视图
    UpdateAllViews(NULL);
}


void CBarTsDoc::BilateralFilter(const std::vector<int>& sliceIndices)
{
    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4) {
        TRACE("Error: Invalid input sizes\n");
        return;
    }

    // 双边滤波参数
    const int d = 9;                  // 邻域直径
    const double sigmaColor = 75.0;    // 颜色空间标准差
    const double sigmaSpace = 75.0;    // 坐标空间标准差

    // 验证参数有效性
    if (d < 1 || d > 50) {
        TRACE("Error: Invalid d parameter value %d\n", d);
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size()) {
            TRACE("Warning: Invalid slice index %d for modal %d\n", sliceIdx, i);
            continue;
        }

        cv::Mat& src = m_allModalImages[i][sliceIdx]; // 原始图像

        // 检查图像是否有效
        if (src.empty()) {
            TRACE("Warning: Empty image for modal %d, slice %d\n", i, sliceIdx);
            continue;
        }

        cv::Mat processedImage = src.clone();
        
        try {
            // 应用双边滤波
            cv::Mat filtered;
            cv::bilateralFilter(processedImage, filtered, d, sigmaColor, sigmaSpace);

            // 更新原始图像
            filtered.copyTo(src);

            // 替换原始图像容器中的切片
            m_allModalImages[i][sliceIdx] = filtered.clone();
        }
        catch (const cv::Exception& e) {
            TRACE("OpenCV error in bilateral filtering: %s\n", e.what());
            continue;
        }
    }

    // 更新视图
    UpdateAllViews(NULL);
}

void CBarTsDoc::HomomorphicCorrection(const std::vector<int>& sliceIndices)
{
    if (m_allModalImages.size() < 4 || sliceIndices.size() < 4)
        return;

    for (int i = 0; i < 4; ++i)
    {
        int sliceIdx = sliceIndices[i];
        if (sliceIdx < 0 || sliceIdx >= m_allModalImages[i].size())
            continue;

        cv::Mat& src = m_allModalImages[i][sliceIdx]; // 原始图像

        cv::Mat processedImage;
        // 自动处理不同类型图像
        if (src.channels() == 1) {
            if (src.depth() == CV_8U) {
                processedImage = src.clone(); // 创建副本
            }
            else {
                // 处理非8位图像（如16位转8位）
                double minVal, maxVal;
                cv::minMaxLoc(src, &minVal, &maxVal);
                src.convertTo(processedImage, CV_8U, 255.0 / (maxVal - minVal + 1e-5)); // 避免除以0
            }
        }
        else if (src.channels() == 3) {
            // 转换彩色图为灰度图
            cv::cvtColor(src, processedImage, cv::COLOR_BGR2GRAY);
        }
        else {
            TRACE("! Unsupported image type for modal %d\n", i);
            continue;
        }

        // 同态滤波参数
        const double gammaHigh = 2.0;    // 高频增益（增强细节）
        const double gammaLow = 0.5;      // 低频增益（抑制光照变化）
        const int filterRadius = 30;      // 滤波器半径（控制过渡区域大小）

        // 应用同态滤波
        cv::Mat result = ApplyHomomorphicFilter(processedImage, gammaHigh, gammaLow, filterRadius);

        // 更新原始图像
        result.copyTo(src);

        // 更新当前显示的切片
        //m_modalSlices[i] = result;

        // 替换原始图像容器中的切片
        m_allModalImages[i][sliceIdx] = result.clone();
    }

    // 更新视图
    UpdateAllViews(NULL);
}

void CBarTsDoc::LightAllCorrection() {
    if (m_allModalImages.size() < 4) {
        TRACE("Error: Not enough modalities available\n");
        return;
    }

    const double gammaHigh = 2.0;
    const double gammaLow = 0.5;
    const int filterRadius = 30;

    for (int modal = 0; modal < 4; modal++) {
        if (m_allModalImages[modal].empty()) {
            TRACE("Warning: Modal %d has no slices\n", modal);
            continue;
        }

        for (int sliceIdx = 0; sliceIdx < m_allModalImages[modal].size(); sliceIdx++) {
            cv::Mat& src = m_allModalImages[modal][sliceIdx];

            cv::Mat processedImage;
            if (src.channels() == 1) {
                if (src.depth() == CV_8U) {
                    processedImage = src.clone();
                }
                else {
                    double minVal, maxVal;
                    cv::minMaxLoc(src, &minVal, &maxVal);
                    src.convertTo(processedImage, CV_8U, 255.0 / (maxVal - minVal + 1e-5));
                }
            }
            else if (src.channels() == 3) {
                cv::cvtColor(src, processedImage, cv::COLOR_BGR2GRAY);
            }
            else {
                TRACE("! Unsupported image type for modal %d slice %d\n", modal, sliceIdx);
                continue;
            }

            cv::Mat result = ApplyHomomorphicFilter(processedImage, gammaHigh, gammaLow, filterRadius);

            // 直接替换缓存里的对应切片
            result.copyTo(m_allModalImages[modal][sliceIdx]);
        }
    }

    //// 更新当前显示的切片图（m_modalSlices）为缓存中对应的切片
    //for (int modal = 0; modal < 4; ++modal) {
    //    if (modal < m_sliceIndices.size() && m_sliceIndices[modal] < m_allModalImages[modal].size()) {
    //        m_modalSlices[modal] = m_allModalImages[modal][m_sliceIndices[modal]].clone();
    //    }
    //}

    UpdateAllViews(NULL);
}


// 同态滤波辅助函数
cv::Mat CBarTsDoc::ApplyHomomorphicFilter(const cv::Mat& input, double gammaHigh, double gammaLow, int radius)
{
    // 1. 转换为浮点型并取对数
    cv::Mat floatImage;
    input.convertTo(floatImage, CV_32F);

    // 添加1避免对0取对数
    cv::Mat logImage;
    cv::log(floatImage + 1, logImage);

    // 2. 扩展图像以便DFT处理
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(logImage.rows);
    int n = cv::getOptimalDFTSize(logImage.cols);
    cv::copyMakeBorder(logImage, padded, 0, m - logImage.rows, 0, n - logImage.cols,
        cv::BORDER_CONSTANT, cv::Scalar::all(0));

    // 3. 傅里叶变换
    cv::Mat complexImage;
    cv::dft(padded, complexImage, cv::DFT_COMPLEX_OUTPUT);

    // 4. 创建高斯同态滤波器
    cv::Mat filter = CreateHomomorphicFilter(complexImage.size(), gammaHigh, gammaLow, radius);

    // 5. 应用滤波器
    cv::Mat filteredComplex;
    std::vector<cv::Mat> planes;
    cv::split(complexImage, planes);

    // 应用滤波器到实部和虚部
    cv::multiply(planes[0], filter, planes[0]);
    cv::multiply(planes[1], filter, planes[1]);

    cv::merge(planes, filteredComplex);

    // 6. 逆傅里叶变换
    cv::Mat inverseTransform;
    cv::idft(filteredComplex, inverseTransform, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    // 7. 裁剪回原始大小
    cv::Mat cropped = inverseTransform(cv::Rect(0, 0, input.cols, input.rows));

    // 8. 指数变换
    cv::Mat expResult;
    cv::exp(cropped, expResult);
    expResult -= 1;

    // 9. 归一化并转换为8位
    cv::normalize(expResult, expResult, 0, 255, cv::NORM_MINMAX);
    cv::Mat finalResult;
    expResult.convertTo(finalResult, CV_8U);

    return finalResult;
}

// 创建同态滤波器
cv::Mat CBarTsDoc::CreateHomomorphicFilter(const cv::Size& size, double gammaHigh, double gammaLow, int radius)
{
    cv::Mat filter = cv::Mat::ones(size, CV_32F);
    cv::Point center(size.width / 2, size.height / 2);

    // 创建高斯滤波器
    for (int y = 0; y < size.height; y++) {
        for (int x = 0; x < size.width; x++) {
            double distance = std::sqrt(std::pow(x - center.x, 2) + std::pow(y - center.y, 2));
            double value = (gammaHigh - gammaLow) * (1 - std::exp(-(distance * distance) / (2 * radius * radius))) + gammaLow;
            filter.at<float>(y, x) = static_cast<float>(value);
        }
    }

    return filter;
}

void CBarTsDoc::BackgroundAwareNormalization(int currentModal, int currentSliceIdx, int referenceSliceIdx)
{
    // 检查参数有效性
    if (m_allModalImages.size() < 4 ||
        currentModal < 0 || currentModal >= 4 ||
        currentSliceIdx < 0 || currentSliceIdx >= m_allModalImages[currentModal].size() ||
        referenceSliceIdx < 0 || referenceSliceIdx >= m_allModalImages[currentModal].size()) {
        TRACE("Error: Invalid parameters for normalization\n");
        return;
    }

    // 如果是参考切片本身，不需要处理
    if (currentSliceIdx == referenceSliceIdx) {
        TRACE("Skipping normalization for reference slice\n");
        return;
    }

    // 1. 获取参考切片并创建脑组织掩码
    cv::Mat refSlice = m_allModalImages[currentModal][referenceSliceIdx].clone();
    cv::Mat refMask = CreateBrainMask(refSlice);

    // 2. 计算参考切片的前景统计
    double refMean, refStddev;
    ComputeForegroundStats(refSlice, refMask, refMean, refStddev);

    // 3. 获取当前切片
    cv::Mat& currentSlice = m_allModalImages[currentModal][currentSliceIdx];

    // 4. 创建当前切片的脑组织掩码
    cv::Mat currentMask = CreateBrainMask(currentSlice);

    // 5. 计算当前切片的前景统计
    double currMean, currStddev;
    ComputeForegroundStats(currentSlice, currentMask, currMean, currStddev);

    // 6. 应用亮度调整（只对前景区域）
    double scale = (currStddev < 1e-5) ? 1.0 : refStddev / currStddev;
    double shift = refMean - currMean * scale;

    // 创建调整后的图像（背景保持原样）
    cv::Mat adjustedSlice;
    currentSlice.convertTo(adjustedSlice, CV_32F);

    for (int y = 0; y < adjustedSlice.rows; y++) {
        for (int x = 0; x < adjustedSlice.cols; x++) {
            if (currentMask.at<uchar>(y, x) > 0) { // 前景区域
                float& pixel = adjustedSlice.at<float>(y, x);
                pixel = pixel * scale + shift;
            }
        }
    }

    // 7. 转换回原始类型并更新
    adjustedSlice.convertTo(currentSlice, currentSlice.type());

    TRACE("Normalized modal %d slice %d using slice %d as reference\n",
        currentModal, currentSliceIdx, referenceSliceIdx);
}

// 创建脑组织掩码（排除背景）
cv::Mat CBarTsDoc::CreateBrainMask(const cv::Mat& image)
{
    cv::Mat mask;

    // 步骤1: 转灰度
    cv::Mat gray;
    if (image.channels() == 3)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image.clone();

    int w = m_pNiftiImg->nx;
    int h = m_pNiftiImg->ny;

    //归一化操作显示
    cv::Mat Slice;
    double min, max;

    cv::minMaxLoc(gray, &min, &max);

    if (fabs(max - min) < 1e-6)
    {
        Slice = cv::Mat::zeros(h, w, CV_8U);
    }
    else
    {
        cv::normalize(gray, Slice, 0, 255, cv::NORM_MINMAX);
        Slice.convertTo(Slice, CV_8U);
    }

    gray = Slice;

    // 步骤2: 判断是否为纯黑或纯白图像
    double minVal, maxVal;
    cv::minMaxLoc(gray, &minVal, &maxVal);

    TRACE(_T("[BrainMask] 灰度图最小值: %.2f, 最大值: %.2f\n"), minVal, maxVal);

    if (maxVal - minVal < 10) {
        TRACE(_T("[BrainMask] 图像对比度极低，可能是空白图，直接返回空掩码。\n"));
        return cv::Mat::zeros(gray.size(), CV_8UC1);
    }

    // 步骤3: 快速大津法分割
    cv::Mat thresh;
    double otsuThresh = cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    TRACE(_T("[BrainMask] 大津法自动阈值: %.2f\n"), otsuThresh);

    // 步骤4: 形态学开运算去小噪声
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(thresh, mask, cv::MORPH_OPEN, kernel);

    // 步骤5: 孔洞填充
    cv::Mat filled = mask.clone();
    cv::floodFill(filled, cv::Point(0, 0), cv::Scalar(255));
    cv::bitwise_not(filled, filled);
    mask = mask | filled;

    // 步骤6: 掩码面积判断
    int maskPixels = cv::countNonZero(mask);
    TRACE(_T("[BrainMask] 掩码区域像素数: %d\n"), maskPixels);

    if (maskPixels < 100) {
        TRACE(_T("[BrainMask] 掩码过小，疑似无效。\n"));
        return cv::Mat::zeros(gray.size(), CV_8UC1);
    }

    return mask;
}


// 计算前景（脑组织）的统计量
void CBarTsDoc::ComputeForegroundStats(const cv::Mat& image, const cv::Mat& mask,
    double& mean, double& stddev)
{
    // 提取前景像素
    cv::Mat foreground;
    image.copyTo(foreground, mask);

    // 计算统计量（忽略背景）
    cv::Scalar meanVal, stddevVal;
    cv::meanStdDev(foreground, meanVal, stddevVal, mask);

    mean = meanVal[0];
    stddev = stddevVal[0];

    // 如果标准差太小（单色区域），设置安全值
    if (stddev < 1e-5) {
        stddev = 1.0;
    }
}

void CBarTsDoc::RemoveSmallRegions(cv::Mat& mask, int areaThreshold)
{
    // 1. 连通区域分析
    cv::Mat labels, stats, centroids;
    int nLabels = cv::connectedComponentsWithStats(mask, labels, stats, centroids);

    // 2. 创建新掩码（初始为全黑）
    cv::Mat newMask = cv::Mat::zeros(mask.size(), mask.type());

    // 3. 遍历所有区域（跳过背景标签0）
    for (int i = 1; i < nLabels; i++) {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);

        // 4. 只保留大于阈值的区域
        if (area >= areaThreshold) {
            // 标记当前区域为前景（255）
            cv::Mat regionMask = (labels == i);
            newMask.setTo(255, regionMask);
        }
    }

    // 5. 替换原始掩码
    mask = newMask;
}

void CBarTsDoc::SegmentBrain(const cv::Mat& inputSlice,
    cv::Mat& grayMatterMask,
    cv::Mat& whiteMatterMask,
    int modal)
{
    // 输入检查：确保是单通道图像
    if (inputSlice.empty() || inputSlice.channels() != 1)
        return;

    // ==================== 预处理阶段 ====================
    cv::Mat processed = inputSlice.clone();

    // 应用两次CLAHE增强 + 两次高斯模糊
    for (int i = 0; i < 2; i++) {
        // 第一次CLAHE增强
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(2.0);  // 对比度限制
        clahe->setTilesGridSize(cv::Size(8, 8));  // 网格大小
        clahe->apply(processed, processed);

        // 第一次高斯模糊去噪
        cv::GaussianBlur(processed, processed, cv::Size(5, 5), 0);
    }

    // ==================== 分割阶段 ====================
    cv::Mat blurred = processed.clone();  // 保留处理后的图像

    // 获取像素强度范围
    double minVal, maxVal;
    cv::minMaxLoc(blurred, &minVal, &maxVal);

    // 定义灰质和白质的阈值
    float grayThreshold, whiteThreshold;

    if (modal == 0) { // FLAIR
        grayThreshold = 0.2 * maxVal;  // 灰质中等信号
        whiteThreshold = 0.4 * maxVal; // 白质高信号
    }
    else if (modal == 3) { // T2
        grayThreshold = 0.5 * maxVal;  // 灰质中等信号
        whiteThreshold = 0.1 * maxVal; // 白质低信号
    }
    else { // 其他模态（如 T1）
        grayThreshold = 0.7 * maxVal;  // 灰质中等信号
        whiteThreshold = 0.4 * maxVal; // 白质高信号
    }

    // 生成掩码
    cv::threshold(blurred, grayMatterMask, grayThreshold, 255, cv::THRESH_BINARY);
    cv::threshold(blurred, whiteMatterMask, whiteThreshold, 255, cv::THRESH_BINARY);

    // 去除白质掩码中的灰质区域
    cv::subtract(whiteMatterMask, grayMatterMask, whiteMatterMask);

    // 创建形态学核
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

    // 灰质形态学操作
    cv::morphologyEx(grayMatterMask, grayMatterMask, cv::MORPH_OPEN, kernel);
    cv::erode(grayMatterMask, grayMatterMask, kernel, cv::Point(-1, -1), 2); // 两次腐蚀
    cv::dilate(whiteMatterMask, whiteMatterMask, kernel, cv::Point(-1, -1), 1); // 三次膨胀
    // 白质形态学操作
    cv::erode(whiteMatterMask, whiteMatterMask, kernel, cv::Point(-1, -1), 3); // 三次腐蚀
    cv::dilate(whiteMatterMask, whiteMatterMask, kernel, cv::Point(-1, -1), 2); // 三次膨胀

    // 去除离散点
    int areaThreshold = inputSlice.rows * inputSlice.cols * 0.01;
    RemoveSmallRegions(whiteMatterMask, areaThreshold);
}

// 灰质白质图分割
void CBarTsDoc::PerformSegmentation()
{
    m_grayMatterSlices.clear();
    m_whiteMatterSlices.clear();

    if (m_allModalImages.size() != 4) {
        AfxMessageBox(_T("错误：需要先加载4个模态图像！"));
        return;
    }

    int numSlices = (int)m_allModalImages[3].size(); // T2 模态切片数量
    m_grayMatterSlices.resize(numSlices);
    m_whiteMatterSlices.resize(numSlices);

    for (int i = 0; i < numSlices; ++i)
    {
        cv::Mat t2Slice = m_allModalImages[3][i];
        cv::Mat grayMask, whiteMask;

        SegmentBrain(t2Slice, grayMask, whiteMask, 3);

        if (grayMask.type() != CV_8U) grayMask.convertTo(grayMask, CV_8U);
        if (whiteMask.type() != CV_8U) whiteMask.convertTo(whiteMask, CV_8U);

        cv::resize(grayMask, grayMask, cv::Size(240, 240));
        cv::resize(whiteMask, whiteMask, cv::Size(240, 240));

        m_grayMatterSlices[i] = grayMask.clone();
        m_whiteMatterSlices[i] = whiteMask.clone();
    }
}

void CBarTsDoc::UpdateGrayWhiteMatterMask(int sliceIndex)
{
    // 假设你已有灰白质分割结果数组 m_grayMatterSlices 和 m_whiteMatterSlices

    if (sliceIndex < 0 || sliceIndex >= m_grayMatterSlices.size()) return;

    // 从预存分割结果取掩码
    cv::Mat m_currentGrayMask;
    cv::Mat m_currentWhiteMask;

    m_currentGrayMask = m_grayMatterSlices[sliceIndex];
    m_currentWhiteMask = m_whiteMatterSlices[sliceIndex];

}

