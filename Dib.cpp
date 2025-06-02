#include "pch.h"
#include "Dib.h"


CDib::CDib(void):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

}


CDib::CDib( CDib &Dib ):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{
	// initialized variables
	m_nBitCount = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nWidthBytes = 0;

	if(&Dib == NULL)
	{
		return;
	}
	if(!IsNull())
	{
		Destroy();
	}
	Create(Dib.GetWidth(),Dib.GetHeight(),Dib.GetBPP(),0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	if(IsIndexed())
	{
		int nColors=Dib.GetMaxColorTableEntries();
		if(nColors > 0)
		{
			RGBQUAD* pal{};
			pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0,nColors,pal);
			SetColorTable(0,nColors,pal);
			delete[] pal;
		} 
	}
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
	memcpy(m_pDibBits,Dib.m_pDibBits,m_nHeight*m_nWidthBytes);
}

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	if (m_pGrayValueCount != NULL)
	{
		delete []m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
}

void CDib::LoadFile( LPCTSTR lpszPathName )
{
	Load(lpszPathName);
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
}

void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i*m_nWidthBytes + j) = 255 - *(m_pDibBits + i*m_nWidthBytes + j);
		}
	}
}

long* CDib::GrayValueCount()
{
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return NULL;
	}
	long *pGrayValueCount = new long[nColors];
	memset(pGrayValueCount,0,nColors*sizeof(long));
	for (int i=0;i<m_nHeight;i++)
	{
		for (int j=0;j<m_nWidth;j++)
		{
			pGrayValueCount[*(m_pDibBits + i*m_nWidthBytes +j)]++;
		}
	}
	return pGrayValueCount;
}

cv::Mat CDib::applyTumorOverlay(const cv::Mat& image, const cv::Mat& mask, double alpha, cv::Vec3b color)
{
	CV_Assert(!image.empty() && !mask.empty());
	CV_Assert(image.rows == mask.rows && image.cols == mask.cols);
	CV_Assert(image.type() == CV_8UC3); // 保证是彩色图像
	CV_Assert(mask.type() == CV_8UC1);  // 保证是单通道掩码

	cv::Mat overlay = image.clone();

	for (int i = 0; i < image.rows; ++i) {
		const uchar* maskRow = mask.ptr<uchar>(i);
		const cv::Vec3b* imgRow = image.ptr<cv::Vec3b>(i);
		cv::Vec3b* outRow = overlay.ptr<cv::Vec3b>(i);

		for (int j = 0; j < image.cols; ++j) {
			if (maskRow[j] > 0) {
				outRow[j][0] = uchar(imgRow[j][0] * (1 - alpha) + color[0] * alpha);
				outRow[j][1] = uchar(imgRow[j][1] * (1 - alpha) + color[1] * alpha);
				outRow[j][2] = uchar(imgRow[j][2] * (1 - alpha) + color[2] * alpha);
			}
		}
	}

	return overlay;
}

cv::Mat CDib::fillMaskHoles(const cv::Mat& src) {
    // 确保输入是二值图像
    CV_Assert(src.type() == CV_8U);

    cv::Mat filled = src.clone();

    // 获取轮廓 - 只查找外部轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(filled, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 如果没有找到轮廓，直接返回
    if (contours.empty()) return filled;

    // 绘制填充的轮廓
    cv::drawContours(filled, contours, -1, cv::Scalar(255), cv::FILLED);

    return filled;
}

//分水岭算法，提取增强肿瘤区域
cv::Mat CDib::applyWatershed_T1c(cv::Mat& image, int lowThresh, int highThresh) {
    if (image.empty()) {
        AfxMessageBox(_T("输入图像为空!"));
        return cv::Mat();
    }

    // 1. 转灰度图
    cv::Mat gray;
    if (image.channels() == 3)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image.clone();

    // 2. 提取增强区域亮边缘
    cv::Mat edge_binary = cv::Mat::zeros(gray.size(), CV_8U);
    for (int y = 0; y < gray.rows; ++y) {
        for (int x = 0; x < gray.cols; ++x) {
            uchar v = gray.at<uchar>(y, x);
            if (v >= lowThresh && v <= highThresh)
                edge_binary.at<uchar>(y, x) = 255;
        }
    }

    // 3. 平滑 + 连接边缘（中值滤波 + 闭运算）
    cv::medianBlur(edge_binary, edge_binary, 3); // 平滑孤立点
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(edge_binary, edge_binary, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 2); // 闭操作连接边缘
    cv::dilate(edge_binary, edge_binary, kernel, cv::Point(-1, -1), 1); // 膨胀增加边缘宽度

    // 4. 填充闭合区域（寻找轮廓并填充）
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edge_binary.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat filled = cv::Mat::zeros(edge_binary.size(), CV_8U);
    cv::drawContours(filled, contours, -1, cv::Scalar(255), cv::FILLED);

    // 闭运算 + 膨胀（再加强一次）
    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
    cv::morphologyEx(edge_binary, edge_binary, cv::MORPH_CLOSE, kernel2, cv::Point(-1, -1), 3);
    cv::dilate(edge_binary, edge_binary, kernel2, cv::Point(-1, -1), 1);

    // 连通区域填充
    cv::findContours(edge_binary.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    filled = cv::Mat::zeros(edge_binary.size(), CV_8U);

    // ✅ 忽略小区域（防止噪声干扰） 去除大区域（防止后面过大板块干扰）
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > 100.0)  // 可调阈值
            cv::drawContours(filled, contours, static_cast<int>(i), cv::Scalar(255), cv::FILLED);
    }

	// 最后腐蚀一轮，防止超出边界
    cv::erode(filled, filled, kernel2, cv::Point(-1, -1), 2);

    // 5. 计算 sure background（背景），对边缘二值图膨胀
    cv::Mat sure_bg;
    cv::dilate(filled, sure_bg, kernel, cv::Point(-1, -1), 3);

    // 6. 计算 sure foreground，利用距离变换获取核心区域
    cv::Mat dist_transform;
    cv::distanceTransform(edge_binary, dist_transform, cv::DIST_L2, 5);
    cv::normalize(dist_transform, dist_transform, 0, 1.0, cv::NORM_MINMAX);
    cv::Mat sure_fg;
    cv::threshold(dist_transform, sure_fg, 0.3, 1.0, cv::THRESH_BINARY);
    sure_fg.convertTo(sure_fg, CV_8U, 255);

    // 7. 未知区域
    cv::Mat unknown;
    cv::subtract(sure_bg, sure_fg, unknown);

    // 8. 连通域标记前景
    cv::Mat markers;
    cv::connectedComponents(sure_fg, markers);
    markers += 1;

    // 9. 未知区域标记为0
    for (int i = 0; i < unknown.rows; ++i) {
        for (int j = 0; j < unknown.cols; ++j) {
            if (unknown.at<uchar>(i, j) == 255) {
                markers.at<int>(i, j) = 0;
            }
        }
    }

    // 10. watershed 分水岭分割
    cv::Mat wsInput;
    if (image.channels() == 3)
        wsInput = image.clone();
    else
        cv::cvtColor(image, wsInput, cv::COLOR_GRAY2BGR);

    cv::watershed(wsInput, markers);

    return markers;
}


cv::Mat CDib::applyWatershed(cv::Mat& image, double thresh) {
    if (image.empty()) {
        AfxMessageBox(_T("输入图像为空!"));
        return cv::Mat();
    }

    cv::Mat processedImage;
    if (image.channels() == 1) {
        cv::cvtColor(image, processedImage, cv::COLOR_GRAY2BGR);
    }
    else {
        processedImage = image.clone();
    }

    // 高斯模糊
    cv::Mat blurred;
    int kernelSize = std::min(5, std::min(processedImage.cols, processedImage.rows) / 10 * 2 + 1);
    if (kernelSize < 3) kernelSize = 3;
    if (kernelSize % 2 == 0) kernelSize++;
    cv::GaussianBlur(processedImage, blurred, cv::Size(kernelSize, kernelSize), 0);

    // 灰度化
    cv::Mat gray;
    cv::cvtColor(blurred, gray, cv::COLOR_BGR2GRAY);

    // 类型转换确保为 8-bit
    if (gray.depth() != CV_8U) {
        double minVal, maxVal;
        cv::minMaxLoc(gray, &minVal, &maxVal);
        double scale = (maxVal > 0) ? 255.0 / maxVal : 1.0;
        gray.convertTo(gray, CV_8U, scale);
    }


    cv::Mat binary;
    //double thresh_val = cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    //int thresh = 100;
    cv::threshold(blurred, binary, thresh, 255, cv::THRESH_BINARY);
    CString msg;


    // 形态学开运算
    cv::Mat opening;
    try {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(binary, opening, cv::MORPH_OPEN, kernel);

        // 确保单通道8位
        if (opening.channels() != 1 || opening.depth() != CV_8U) {
            cv::Mat temp;
            if (opening.channels() > 1)
                cv::cvtColor(opening, temp, cv::COLOR_BGR2GRAY);
            else
                opening.convertTo(temp, CV_8U);
            opening = temp;
        }
    }
    catch (const cv::Exception& e) {
        msg.Format(_T("形态学错误: %S"), e.what());
        AfxMessageBox(msg);
        return cv::Mat();
    }

    // 计算前景比例
    int nonZeroCount = 0;
    try {
        nonZeroCount = cv::countNonZero(opening);
    }
    catch (...) {
        AfxMessageBox(_T("countNonZero失败"));
        return cv::Mat();
    }

    double totalPixels = static_cast<double>(opening.rows * opening.cols);
    double fgRatio = (totalPixels > 0) ? static_cast<double>(nonZeroCount) / totalPixels : 0.0;
    //msg.Format(_T("前景比例: %.2f%%"), fgRatio * 100);
    //AfxMessageBox(msg);

    // 膨胀获取背景区域
    cv::Mat sure_bg;
    cv::dilate(opening, sure_bg, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)), cv::Point(-1, -1), 2);

    // 距离变换获取前景
    cv::Mat dist_transform;
    cv::distanceTransform(opening, dist_transform, cv::DIST_L2, 5);
    cv::normalize(dist_transform, dist_transform, 0, 1.0, cv::NORM_MINMAX);

    cv::Mat sure_fg;
    cv::threshold(dist_transform, sure_fg, 0.2, 1.0, cv::THRESH_BINARY);
    sure_fg.convertTo(sure_fg, CV_8U, 255);

    // 未知区域
    cv::Mat unknown;
    cv::subtract(sure_bg, sure_fg, unknown);

    // 连通组件标记
    cv::Mat markers;
    cv::connectedComponents(sure_fg, markers);
    markers += 1;
    markers.setTo(0, unknown > 0);

    // 分水岭算法
    cv::watershed(processedImage, markers);

    // 返回标记矩阵（各区域标号）
    return markers;
}


// 获取最大肿瘤区域的掩码
cv::Mat CDib::getTumorRegionMask(const cv::Mat& markers, int& tumor_label, int& tumor_area) {
    std::map<int, int> label_area;
    for (int i = 0; i < markers.rows; ++i) {
        for (int j = 0; j < markers.cols; ++j) {
            int label = markers.at<int>(i, j);
            if (label > 1)
                label_area[label]++;
        }
    }

    tumor_area = 0;
    tumor_label = -1;
    for (const auto& entry : label_area) {
        if (entry.second > tumor_area) {
            tumor_area = entry.second;
            tumor_label = entry.first;
        }
    }

    cv::Mat mask = cv::Mat::zeros(markers.size(), CV_8U);
    for (int i = 0; i < markers.rows; ++i) {
        for (int j = 0; j < markers.cols; ++j) {
            if (markers.at<int>(i, j) == tumor_label)
                mask.at<uchar>(i, j) = 255;
        }
    }
    return mask;
}

HBITMAP CDib::MatToHBITMAP(const cv::Mat& mat)
{
    if (mat.empty()) {
        AfxMessageBox(_T("[MatToHBITMAP] 输入 mat 为空！"));
        return nullptr;
    }

    cv::Mat rgb;
    if (mat.channels() == 1) {
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2BGR);
    }
    else if (mat.channels() == 3) {
        rgb = mat.clone();
    }
    else {
        AfxMessageBox(_T("[MatToHBITMAP] 非法通道数！"));
        return nullptr;
    }

    if (rgb.empty() || !rgb.data) {
        AfxMessageBox(_T("[MatToHBITMAP] rgb 数据为空！"));
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rgb.cols;
    bmi.bmiHeader.biHeight = -rgb.rows; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC hdc = ::GetDC(NULL);
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    ::ReleaseDC(NULL, hdc);

    if (!hBitmap) {
        AfxMessageBox(_T("[MatToHBITMAP] CreateDIBSection 返回 NULL!"));
        return nullptr;
    }
    if (!bits) {
        AfxMessageBox(_T("[MatToHBITMAP] bits 是 NULL!"));
        return nullptr;
    }

    size_t size = rgb.total() * rgb.elemSize();
    if (size == 0) {
        AfxMessageBox(_T("[MatToHBITMAP] 计算 size 为 0!"));
        return nullptr;
    }

    memcpy(bits, rgb.data, size);

    return hBitmap;
}


// FLAIR 模态：标准分水岭，无后处理
cv::Mat CDib::GetFlairTumorMask(const cv::Mat& flairSlice,bool Light)
{
    double minVal = 0.0, maxVal = 0.0;
    cv::minMaxLoc(flairSlice, &minVal, &maxVal);

    cv::Mat norm;
    if (fabs(maxVal - minVal) < 1e-6)
        norm = cv::Mat::zeros(flairSlice.size(), CV_8U);
    else {
        cv::normalize(flairSlice, norm, 0, 255, cv::NORM_MINMAX);
        norm.convertTo(norm, CV_8U);
    }

    cv::Mat markers;
    if (Light) {
        markers = CDib::applyWatershed(norm, 80);
    }
    else {
        markers = CDib::applyWatershed(norm, 120);
    }

    int label = -1, area = 0;
    return CDib::getTumorRegionMask(markers, label, area);
}

// T1C 模态：使用专用分水岭方法 + 填洞 + 修复 + 形态学处理
cv::Mat CDib::GetT1cTumorMask(const cv::Mat& t1cSlice, bool Light)
{
    double minVal = 0.0, maxVal = 0.0;
    cv::minMaxLoc(t1cSlice, &minVal, &maxVal);

    cv::Mat norm;
    if (fabs(maxVal - minVal) < 1e-6)
        norm = cv::Mat::zeros(t1cSlice.size(), CV_8U);
    else {
        cv::normalize(t1cSlice, norm, 0, 255, cv::NORM_MINMAX);
        norm.convertTo(norm, CV_8U);
    }

    cv::Mat markers;
    if (Light) {
        markers = CDib::applyWatershed_T1c(norm, 100.0, 200.0);
    }
    else {
        markers = CDib::applyWatershed_T1c(norm, 150.0, 200.0);
    }

    cv::Mat tumorMask = cv::Mat::zeros(markers.size(), CV_8U);
    int tumor_label = 0, tumor_area = 0;
    for (int y = 0; y < markers.rows; ++y) {
        for (int x = 0; x < markers.cols; ++x) {
            int label = markers.at<int>(y, x);
            if (label > 1) {
                tumorMask.at<uchar>(y, x) = 255;
                if (tumor_label == 0) tumor_label = label;
            }
        }
    }

    cv::Mat filledMask = CDib::fillMaskHoles(tumorMask);

    // 修复标记图
    for (int y = 0; y < markers.rows; ++y) {
        for (int x = 0; x < markers.cols; ++x) {
            if (filledMask.at<uchar>(y, x) == 255 && tumorMask.at<uchar>(y, x) == 0) {
                int found_label = 0;
                for (int dy = -1; dy <= 1 && found_label == 0; dy++) {
                    for (int dx = -1; dx <= 1 && found_label == 0; dx++) {
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < markers.rows && nx >= 0 && nx < markers.cols) {
                            if (tumorMask.at<uchar>(ny, nx) == 255)
                                found_label = markers.at<int>(ny, nx);
                        }
                    }
                }
                if (found_label != 0)
                    markers.at<int>(y, x) = found_label;
            }
        }
    }

    cv::Mat finalMask = CDib::getTumorRegionMask(markers, tumor_label, tumor_area);

    // 形态学细化
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::Mat eroded, refinedMask;
    cv::erode(finalMask, eroded, kernel, cv::Point(-1, -1), 4);
    cv::dilate(eroded, refinedMask, kernel, cv::Point(-1, -1), 5);
    return refinedMask;
}

// T2 模态：标准分水岭但阈值更高
cv::Mat CDib::GetT2TumorMask(const cv::Mat& t2Slice, bool Light)
{
    double minVal = 0.0, maxVal = 0.0;
    cv::minMaxLoc(t2Slice, &minVal, &maxVal);

    cv::Mat norm;
    if (fabs(maxVal - minVal) < 1e-6)
        norm = cv::Mat::zeros(t2Slice.size(), CV_8U);
    else {
        cv::normalize(t2Slice, norm, 0, 255, cv::NORM_MINMAX);
        norm.convertTo(norm, CV_8U);
    }

    cv::Mat markers;
    if (Light) {
        markers = CDib::applyWatershed(norm, 80.0);
    }
    else {
        markers = CDib::applyWatershed(norm, 140.0);
    }

    int label = -1, area = 0;
    return CDib::getTumorRegionMask(markers, label, area);
}