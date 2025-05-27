#include "pch.h"
#include "Dib.h"
#include <vector>


CDib::CDib(void) :m_pDibBits(NULL), m_pGrayValueCount(NULL)
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


void CDib::CreateWhiteSquare(int width, int height, int pixel)
{
	// 1. 清除旧数据并创建新的 8-bit 灰度图像
	if (!IsNull())
	{
		Destroy();
	}
	Create(width, height, 8, 0); // 创建 8-bit 灰度图

	// 2. 重新获取图像的基本信息
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	// 3. 设置颜色表（灰度图像需要）
	if (IsIndexed())
	{
		int nColors = 256; // 灰度图 256 级灰度
		RGBQUAD* pal = new RGBQUAD[nColors];
		for (int i = 0; i < nColors; i++)
		{
			pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)i;
			pal[i].rgbReserved = 0;
		}
		SetColorTable(0, nColors, pal);
		delete[] pal;
	}

	// 4. 计算白色方块的区域
	int squareSizeX = pixel; 
	int squareSizeY = pixel;
	int startX = width / 2 - squareSizeX / 2;  // 方块左上角 X 坐标
	int startY = height / 2 - squareSizeY / 2; // 方块左上角 Y 坐标

	// 5. 填充像素数据
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// 获取当前像素指针
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;

			// 判断是否在白色方块区域
			if (x >= startX && x < startX + squareSizeX &&
				y >= startY && y < startY + squareSizeY)
			{
				*pPixel = 255; // 白色
			}
			else
			{
				*pPixel = 0;   // 黑色
			}
		}
	}
}

void CDib::CreateTestPage(int width, int height)
{
	// 1. 清除旧数据并创建新的 8-bit 灰度图像
	if (!IsNull())
	{
		Destroy();
	}
	Create(width, height, 8, 0); // 创建 8-bit 灰度图

	// 2. 重新获取图像的基本信息
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	// 3. 设置颜色表（灰度图像需要）
	if (IsIndexed())
	{
		int nColors = 256;
		RGBQUAD* pal = new RGBQUAD[nColors];
		for (int i = 0; i < nColors; i++)
		{
			pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)i;
			pal[i].rgbReserved = 0;
		}
		SetColorTable(0, nColors, pal);
		delete[] pal;
	}

	// 4. 清空背景
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;
			*pPixel = 50; // 50像素的背景
		}
	}

	// 5. 最外侧矩形（灰度值 50）
	int border = width / 10;
	for (int y = border; y < height - border; y++)
	{
		for (int x = border; x < width - border; x++)
		{
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;
			*pPixel = 50;
		}
	}

	// 6. 中间矩形（灰度值 130）
	int innerMargin = width / 6 ;
	for (int y = innerMargin; y < height - innerMargin; y++)
	{
		for (int x = innerMargin; x < width - innerMargin; x++)
		{
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;
			*pPixel = 130;
		}
	}

	// 7. 内部圆形（灰度值 200）
	int centerX = width / 2;
	int centerY = height / 2;
	int radius = width / 6;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int dx = x - centerX;
			int dy = y - centerY;
			if (dx * dx + dy * dy <= radius * radius)
			{
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;
				*pPixel = 200;
			}
		}
	}
}


CDib::~CDib(void) {
	m_pDibBits = NULL;
	if (m_pGrayValueCount != NULL) {
		delete[] m_pGrayValueCount;
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

int CDib::GetBitCount() const
{
	// 获取BitCount位数
	return m_nBitCount;
}

void CDib::GenerateRedImage()
{
	if (m_nBitCount == 24){
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidthBytes; j += 3)
			{
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j + 1) = 0;
			}
		}
	}

	else {
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{

			RGBQUAD* palette = new RGBQUAD[nColors];
			GetColorTable(0, nColors, palette);
			/*
			if (m_pOriginalPalette) delete[] m_pOriginalPalette;
			m_pOriginalPalette = new RGBQUAD[nColors];
			memcpy(m_pOriginalPalette, palette, nColors * sizeof(RGBQUAD));
			*/
			for (int i = 0; i < nColors; i++)
			{
				palette[i].rgbGreen = 0;
				palette[i].rgbBlue = 0;
			}

			SetColorTable(0, nColors, palette);
			delete[] palette;
		}

	}
}

void CDib::GenerateGreenImage()
{
	if (m_nBitCount == 24) {
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidthBytes; j += 3)
			{
				*(m_pDibBits + i * m_nWidthBytes + j) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j + 2) = 0;
			}
		}
	}
	else {
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{

			RGBQUAD* palette = new RGBQUAD[nColors];
			GetColorTable(0, nColors, palette);
			/*
			if (m_pOriginalPalette) delete[] m_pOriginalPalette;
			m_pOriginalPalette = new RGBQUAD[nColors];
			memcpy(m_pOriginalPalette, palette, nColors * sizeof(RGBQUAD));
			*/
			for (int i = 0; i < nColors; i++)
			{
				palette[i].rgbRed = 0;
				palette[i].rgbBlue = 0;
			}

			SetColorTable(0, nColors, palette);
			delete[] palette;
		}

	}
}

void CDib::GenerateBlueImage()
{
	if (m_nBitCount == 24) {
		for (int i = 0; i < m_nHeight; i++)
		{
			for (int j = 0; j < m_nWidthBytes; j += 3)
			{
				*(m_pDibBits + i * m_nWidthBytes + j + 1) = 0;
				*(m_pDibBits + i * m_nWidthBytes + j + 2) = 0;
			}
		}
	}

	else {
		int nColors = GetMaxColorTableEntries();
		if (nColors > 0)
		{

			RGBQUAD* palette = new RGBQUAD[nColors];
			GetColorTable(0, nColors, palette);
			/*
			if (m_pOriginalPalette) delete[] m_pOriginalPalette;
			m_pOriginalPalette = new RGBQUAD[nColors];
			memcpy(m_pOriginalPalette, palette, nColors * sizeof(RGBQUAD));
			*/
			for (int i = 0; i < nColors; i++)
			{
				palette[i].rgbGreen = 0;
				palette[i].rgbRed = 0;
			}

			SetColorTable(0, nColors, palette);
			delete[] palette;
		}

	}
}

void CDib::LinearTransformGrayScale() {
	for (int i = 0; i < m_nHeight; i++) {
		for (int j = 0; j < m_nWidthBytes; j++) {
			double tmp = *(m_pDibBits + i * m_nWidthBytes + j);
			if (tmp <= 100)
				*(m_pDibBits + i * m_nWidthBytes + j) =0;  // 100以下灰度变为0
			else if (tmp > 100 && tmp < 150)
				*(m_pDibBits + i * m_nWidthBytes + j) = (100 + (tmp-100) * (100/50));
			else
				*(m_pDibBits + i * m_nWidthBytes + j) = 255;
		}
	}
}

void CDib::Bit_Plane_Slicing_up(int k)
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i * m_nWidthBytes + j) = (*(m_pDibBits + i * m_nWidthBytes + j) & (1 << k)) ? 255 : 0;
		}
	}
}

void CDib::HistogramEqualization()
{
	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的直方图均衡化"));
		return;
	}

	// 计算直方图
	long histogram[256] = { 0 };
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			unsigned char pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
			histogram[pixelValue]++;
		}
	}

	// 计算累积直方图
	long cumulativeHistogram[256] = { 0 };
	cumulativeHistogram[0] = histogram[0];
	for (int i = 1; i < 256; i++)
	{
		cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
	}

	// 计算均衡化后的像素值映射
	unsigned char equalizedMap[256];
	int totalPixels = m_nWidth * m_nHeight;
	for (int i = 0; i < 256; i++)
	{
		equalizedMap[i] = static_cast<unsigned char>((cumulativeHistogram[i] * 255) / totalPixels);
	}

	// 应用均衡化后的像素值映射
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			unsigned char pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
			*(m_pDibBits + i * m_nWidthBytes + j) = equalizedMap[pixelValue];
		}
	}
}

void CDib::Smooth()
{
	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的平滑处理"));
		return;
	}

	// 创建一个临时缓冲区来存储平滑后的图像数据
	unsigned char* tempBuffer = new unsigned char[m_nHeight * m_nWidthBytes];
	memcpy(tempBuffer, m_pDibBits, m_nHeight * m_nWidthBytes);

	// 定义平滑滤波器的大小
	const int filterSize = 3;
	const int filterHalf = filterSize / 2;

	// 遍历图像的每个像素
	for (int y = filterHalf; y < m_nHeight - filterHalf; y++)
	{
		for (int x = filterHalf; x < m_nWidth - filterHalf; x++)
		{
			int sum = 0;
			int count = 0;

			// 遍历滤波器窗口
			for (int fy = -filterHalf; fy <= filterHalf; fy++)
			{
				for (int fx = -filterHalf; fx <= filterHalf; fx++)
				{
					int ny = y + fy;
					int nx = x + fx;

					// 检查窗口内的像素是否在图像范围内
					if (ny >= 0 && ny < m_nHeight && nx >= 0 && nx < m_nWidth)
					{
						sum += *(tempBuffer + ny * m_nWidthBytes + nx) * Smoth_Kernel[(fy + filterHalf) * filterSize + (fx + filterHalf)];
						count++;
					}
				}
			}

			// 计算平均值并存储在原始缓冲区中
			*(m_pDibBits + y * m_nWidthBytes + x) = static_cast<unsigned char>(sum / count);
		}
	}

	// 释放临时缓冲区
	delete[] tempBuffer;
}

void CDib::LaplaceTransform()
{
	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的拉普拉斯变换"));
		return;
	}

	// 创建一个临时缓冲区来存储拉普拉斯变换后的图像数据
	int* tempBuffer = new int[m_nHeight * m_nWidth];
	memset(tempBuffer, 0, m_nHeight * m_nWidth * sizeof(int));

	// 定义拉普拉斯滤波器的大小
	const int filterSize = 3;
	const int filterHalf = filterSize / 2;

	// 遍历图像的每个像素
	for (int y = filterHalf; y < m_nHeight - filterHalf; y++)
	{
		for (int x = filterHalf; x < m_nWidth - filterHalf; x++)
		{
			int sum = 0;

			// 遍历滤波器窗口
			for (int fy = -filterHalf; fy <= filterHalf; fy++)
			{
				for (int fx = -filterHalf; fx <= filterHalf; fx++)
				{
					int ny = y + fy;
					int nx = x + fx;

					// 检查窗口内的像素是否在图像范围内
					if (ny >= 0 && ny < m_nHeight && nx >= 0 && nx < m_nWidth)
					{
						sum += *(m_pDibBits + ny * m_nWidthBytes + nx) * Laplace_Kernel[(fy + filterHalf) * filterSize + (fx + filterHalf)];
					}
				}
			}

			// 存储拉普拉斯变换后的值到临时缓冲区
			*(tempBuffer + y * m_nWidth + x) = sum;
		}
	}

	// 找到拉普拉斯变换后像素值的最小值和最大值
	int minValue = INT_MAX;
	int maxValue = INT_MIN;
	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			int value = *(tempBuffer + y * m_nWidth + x);
			if (value < minValue) minValue = value;
			if (value > maxValue) maxValue = value;
		}
	}

	// 线性归一化到0到255并存储到原始图像缓冲区
	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			int value = *(tempBuffer + y * m_nWidth + x);
			if (maxValue != minValue)
			{
				// 线性归一化
				value = static_cast<int>((value - minValue) * 255.0 / (maxValue - minValue));
			}
			else
			{
				// 如果所有值相同，设置为128
				value = 128;
			}

			// 存储归一化后的值
			*(m_pDibBits + y * m_nWidthBytes + x) = static_cast<unsigned char>(value);
		}
	}

	// 释放临时缓冲区
	delete[] tempBuffer;
}


void CDib::LaplaceEnhance()
{
	if (m_nBitCount == 8)
	{
		// 灰度图增强处理
		unsigned char* tempBuffer = new unsigned char[m_nHeight * m_nWidthBytes];
		memcpy(tempBuffer, m_pDibBits, m_nHeight * m_nWidthBytes);

		const int filterSize = 3;
		const int filterHalf = filterSize / 2;
		const int Laplace_Kernel[9] = {
			0, -1, 0,
			-1, 4, -1,
			0, -1, 0
		};

		for (int y = filterHalf; y < m_nHeight - filterHalf; y++)
		{
			for (int x = filterHalf; x < m_nWidth - filterHalf; x++)
			{
				int sum = 0;

				for (int fy = -filterHalf; fy <= filterHalf; fy++)
				{
					for (int fx = -filterHalf; fx <= filterHalf; fx++)
					{
						int ny = y + fy;
						int nx = x + fx;
						sum += *(tempBuffer + ny * m_nWidthBytes + nx) * Laplace_Kernel[(fy + filterHalf) * filterSize + (fx + filterHalf)];
					}
				}

				int newVal = *(m_pDibBits + y * m_nWidthBytes + x) + sum;
				*(m_pDibBits + y * m_nWidthBytes + x) = static_cast<unsigned char>(min(max(newVal, 0), 255));
			}
		}

		delete[] tempBuffer;
	}
	else if (m_nBitCount == 24)
	{
		// 彩色图增强处理（对 R、G、B 三通道分别处理）
		unsigned char* tempBuffer = new unsigned char[m_nHeight * m_nWidthBytes];
		memcpy(tempBuffer, m_pDibBits, m_nHeight * m_nWidthBytes);

		const int filterSize = 3;
		const int filterHalf = filterSize / 2;
		const int Laplace_Kernel[9] = {
			0, -1, 0,
			-1, 4, -1,
			0, -1, 0
		};

		for (int y = filterHalf; y < m_nHeight - filterHalf; y++)
		{
			for (int x = filterHalf; x < m_nWidth - filterHalf; x++)
			{
				for (int c = 0; c < 3; c++) // c = 0:B, 1:G, 2:R
				{
					int sum = 0;
					for (int fy = -filterHalf; fy <= filterHalf; fy++)
					{
						for (int fx = -filterHalf; fx <= filterHalf; fx++)
						{
							int ny = y + fy;
							int nx = x + fx;
							unsigned char* pSrc = tempBuffer + ny * m_nWidthBytes + nx * 3 + c;
							sum += (*pSrc) * Laplace_Kernel[(fy + filterHalf) * filterSize + (fx + filterHalf)];
						}
					}

					unsigned char* pDst = m_pDibBits + y * m_nWidthBytes + x * 3 + c;
					int newVal = *pDst + sum;
					*pDst = static_cast<unsigned char>(min(max(newVal, 0), 255));
				}
			}
		}

		delete[] tempBuffer;
	}
	else
	{
		AfxMessageBox(_T("仅支持8位灰度图像或24位彩色图像的拉普拉斯增强"));
	}
}


vector<float> CDib::GenerateGaussianKernel(int sigma)
{
	int kernelRadius = 3 * sigma;
	int kernelSize = 2 * kernelRadius + 1;
	vector<float> kernel(kernelSize);
	float sum = 0.0f;

	for (int i = -kernelRadius; i <= kernelRadius; ++i)
	{
		float value = exp(-(i * i) / (2 * sigma * sigma)) / (sqrt(2 * Pi) * sigma);
		kernel[i + kernelRadius] = value;
		sum += value;
	}

	// 归一化核
	for (int i = 0; i < kernelSize; ++i)
	{
		kernel[i] /= sum;
	}

	return kernel;
}

void CDib::GaussianSmoothPart(int startY, int endY, const vector<float>& kernel, int kernelRadius, unsigned char* tempBuffer)
{
	// 水平卷积
	for (int y = startY; y < endY; ++y)
	{
		for (int x = 0; x < m_nWidth; ++x)
		{
			float sum = 0.0f;
			for (int k = -kernelRadius; k <= kernelRadius; ++k)
			{
				int nx = x + k;
				if (nx >= 0 && nx < m_nWidth)
				{
					sum += tempBuffer[y * m_nWidthBytes + nx] * kernel[k + kernelRadius];
				}
			}
			m_pDibBits[y * m_nWidthBytes + x] = static_cast<unsigned char>(sum);
		}
	}

	// 垂直卷积
	memcpy(tempBuffer, m_pDibBits, m_nHeight * m_nWidthBytes);
	for (int y = startY; y < endY; ++y)
	{
		for (int x = 0; x < m_nWidth; ++x)
		{
			float sum = 0.0f;
			for (int k = -kernelRadius; k <= kernelRadius; ++k)
			{
				int ny = y + k;
				if (ny >= 0 && ny < m_nHeight)
				{
					sum += tempBuffer[ny * m_nWidthBytes + x] * kernel[k + kernelRadius];
				}
			}
			m_pDibBits[y * m_nWidthBytes + x] = static_cast<unsigned char>(sum);
		}
	}
}

void CDib::GaussianSmooth(int sigma)
{
	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的高斯平滑"));
		return;
	}

	vector<float> kernel = GenerateGaussianKernel(sigma);
	int kernelRadius = kernel.size() / 2;

	// 创建临时缓冲区来存储平滑后的图像数据
	unsigned char* tempBuffer = new unsigned char[m_nHeight * m_nWidthBytes];
	memcpy(tempBuffer, m_pDibBits, m_nHeight * m_nWidthBytes);

	// 单线程执行
	auto startSingle = std::chrono::high_resolution_clock::now();
	GaussianSmoothPart(0, m_nHeight, kernel, kernelRadius, tempBuffer);
	auto endSingle = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> singleThreadDuration = endSingle - startSingle;
	std::cout << "Single-threaded duration for sigma " << sigma << ": " << singleThreadDuration.count() << " seconds" << std::endl;

	// 多线程执行
	memcpy(m_pDibBits, tempBuffer, m_nHeight * m_nWidthBytes); // 恢复原始图像数据
	auto startMulti = std::chrono::high_resolution_clock::now();
	int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	int partHeight = m_nHeight / numThreads;

	for (int i = 0; i < numThreads; ++i)
	{
		int startY = i * partHeight;
		int endY = (i == numThreads - 1) ? m_nHeight : (startY + partHeight);
		threads.emplace_back(&CDib::GaussianSmoothPart, this, startY, endY, std::ref(kernel), kernelRadius, tempBuffer);
	}

	for (auto& t : threads)
	{
		t.join();
	}

	auto endMulti = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> multiThreadDuration = endMulti - startMulti;
	// 输出时间结果
	std::ostringstream oss;
	oss << "Multi-threaded duration for sigma " << sigma << ": " << multiThreadDuration.count() << " seconds";
	AfxMessageBox(CString(oss.str().c_str()));

	// 释放临时缓冲区
	delete[] tempBuffer;
}


void CDib::SobelTransform(int type)
{
	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的Sobel变换"));
		return;
	}

	unsigned char* tempBits = new unsigned char[m_nHeight * m_nWidthBytes];
	memcpy(tempBits, m_pDibBits, m_nHeight * m_nWidthBytes);

	int Gx[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};

	int Gy[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}
	};

	for (int i = 1; i < m_nHeight - 1; i++) {
		for (int j = 1; j < m_nWidth - 1; j++) {
			int sumX = 0;
			int sumY = 0;

			for (int ki = 0; ki < 3; ki++) {
				for (int kj = 0; kj < 3; kj++) {
					int pixel = *(tempBits + (i + ki - 1) * m_nWidthBytes + (j + kj - 1));
					sumX += pixel * Gx[ki][kj];
					sumY += pixel * Gy[ki][kj];
				}
			}

			int magnitude = static_cast<int>(std::sqrt(sumX * sumX + sumY * sumY));
			int angle = static_cast<int>(std::atan2(sumY, sumX) * 180 / Pi);

			if (magnitude > 255) magnitude = 255;
			if (magnitude < 0) magnitude = 0;
			if (angle > 255) angle = 255;
			if (angle < 0) angle = 0;

			if (type == 0) {
				*(m_pDibBits + i * m_nWidthBytes + j) = magnitude;
			}
			else if (type == 1) {
				*(m_pDibBits + i * m_nWidthBytes + j) = angle;
			}
			//*(m_pDibBits + i * m_nWidthBytes + j) = magnitude;

		}
	}

	delete[] tempBits;
}


void CDib::CLAHE(int clipLimit, int gridX, int gridY) {
	int width = GetWidth();
	int height = GetHeight();
	int pitch = GetPitch();
	int bpp = GetBPP();
	unsigned char* pBits = static_cast<unsigned char*>(GetBits());

	if (m_nBitCount != 8) // 确保图像是8位灰度图像
	{
		AfxMessageBox(_T("仅支持8位灰度图像的Sobel变换"));
		return;
	}

	int gridWidth = width / gridX;
	int gridHeight = height / gridY;
	int gridSize = gridWidth * gridHeight;
	// 用于存储每个网格的直方图和LUT
	std::vector<std::vector<int>> histograms(gridX * gridY, std::vector<int>(256, 0));
	std::vector<std::vector<unsigned char>> luts(gridX * gridY, std::vector<unsigned
		char>(256, 0));
	// 计算每个网格的直方图
	for (int gy = 0; gy < gridY; ++gy) {
		for (int gx = 0; gx < gridX; ++gx) {
			int gridIndex = gy * gridX + gx;
			auto& hist = histograms[gridIndex];
			for (int y = 0; y < gridHeight; ++y) {
				for (int x = 0; x < gridWidth; ++x) {
					int pixel = pBits[(gy * gridHeight + y) * pitch + (gx * gridWidth
						+ x)];
					hist[pixel]++;
				}
			}
			// 裁剪直方图
			int excess = 0;
			for (int i = 0; i < 256; ++i) {
				if (hist[i] > clipLimit) {
					excess += hist[i] - clipLimit;
					hist[i] = clipLimit;
				}
			}
			// 重新分配裁剪的像素
			int increment = excess / 256;
			int remainder = excess % 256;
			for (int i = 0; i < 256; ++i) {
				hist[i] += increment;
			}
			for (int i = 0; i < remainder; ++i) {
				hist[i]++;
			}
			// 计算累积分布函数（CDF）并生成LUT
			int sum = 0;
			auto& lut = luts[gridIndex];
			for (int i = 0; i < 256; ++i) {
				sum += hist[i];
				lut[i] = static_cast<unsigned char>(sum * 255 / gridSize);
			}
		}
	}
	// 对每个像素进行插值处理
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// 找到当前像素所在的网格
			int gx = x / gridWidth;
			int gy = y / gridHeight;
			// 确保网格索引不越界
			gx = (std::min)(gx, gridX - 1);
			gy = (std::min)(gy, gridY - 1);
			// 获取当前像素的四个邻近网格
			int gx1 = (std::min)(gx + 1, gridX - 1);
			int gy1 = (std::min)(gy + 1, gridY - 1);
			// 计算当前像素在网格中的相对位置
			float xRatio = static_cast<float>(x % gridWidth) / gridWidth;
			float yRatio = static_cast<float>(y % gridHeight) / gridHeight;
			// 获取四个邻近网格的LUT
			auto& lut00 = luts[gy * gridX + gx];
			auto& lut01 = luts[gy * gridX + gx1];
			auto& lut10 = luts[gy1 * gridX + gx];
			auto& lut11 = luts[gy1 * gridX + gx1];
			// 获取当前像素值
			unsigned char pixel = pBits[y * pitch + x];
			// 双线性插值
			float interpolatedValue =
				(1 - xRatio) * (1 - yRatio) * lut00[pixel] +
				xRatio * (1 - yRatio) * lut01[pixel] +
				(1 - xRatio) * yRatio * lut10[pixel] +
				xRatio * yRatio * lut11[pixel];
			// 更新像素值
			pBits[y * pitch + x] = static_cast<unsigned char>(interpolatedValue);
		}
	}
}

void CDib::FFT(complex<double>* pComplex, int nLength)
{
	// Base case
	if (nLength <= 1)
	{
		return;
	}
	// Split the array into even and odd parts
	complex<double>* pEven = new complex<double>[nLength / 2];
	complex<double>* pOdd = new complex<double>[nLength / 2];
	for (int i = 0; i < nLength / 2; i++)
	{
		pEven[i] = pComplex[2 * i];
		pOdd[i] = pComplex[2 * i + 1];
	}
	// Recursively compute FFT on even and odd parts
	FFT(pEven, nLength / 2);
	FFT(pOdd, nLength / 2);
	// Combine the results
	for (int i = 0; i < nLength / 2; i++)
	{
		double theta = -2 * Pi * i / nLength;
		complex<double> twiddle = complex<double>(cos(theta), sin(theta)) * pOdd[i];
		pComplex[i] = pEven[i] + twiddle;
		pComplex[i + nLength / 2] = pEven[i] - twiddle;
	}
	// Clean up
	delete[] pEven;
	delete[] pOdd;
}


void CDib::IFFT(complex<double>* pComplex, int nLength)
{
	// Base case
	if (nLength <= 1)
	{
		return;
	}
	// Split the array into even and odd parts
	complex<double>* pEven = new complex<double>[nLength / 2];
	complex<double>* pOdd = new complex<double>[nLength / 2];
	for (int i = 0; i < nLength / 2; i++)
	{
		pEven[i] = pComplex[2 * i];
		pOdd[i] = pComplex[2 * i + 1];
	}
	// Recursively compute IFFT on even and odd parts
	IFFT(pEven, nLength / 2);
	IFFT(pOdd, nLength / 2);
	// Combine the results
	for (int i = 0; i < nLength / 2; i++)
	{
		double theta = 2 * Pi * i / nLength;
		complex<double> twiddle = complex<double>(cos(theta), sin(theta)) * pOdd[i];
		pComplex[i] = pEven[i] + twiddle;
		pComplex[i + nLength / 2] = pEven[i] - twiddle;
	}
	// Clean up
	delete[] pEven;
	delete[] pOdd;
}

void CDib::FFT2D()
{
	// 获取图像尺寸
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 创建复数数组存储图像
	complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];

	normMagnitudeMap.resize(nHeight, vector<unsigned char>(nWidth));
	normPhaseMap.resize(nHeight, vector<unsigned char>(nWidth));


	// **预处理：中心化**
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			double pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
			pComplexImage[i * nWidth + j] = complex<double>(pixelValue * pow(-1, i + j), 0);
		}
	}

	// **多线程计算 1D FFT（行方向）**
	vector<thread> rowThreads;
	for (int i = 0; i < nHeight; i++)
	{
		rowThreads.emplace_back(&CDib::FFT, this, pComplexImage + i * nWidth, nWidth);
	}
	for (auto& t : rowThreads)
	{
		t.join();  // 确保所有线程完成
	}

	// **多线程计算 1D FFT（列方向）**
	vector<thread> colThreads;
	for (int i = 0; i < nWidth; i++)
	{
		colThreads.emplace_back([&, i]() {
			vector<complex<double>> pColumn(nHeight);  // **线程独立数组**
			for (int j = 0; j < nHeight; j++)
			{
				pColumn[j] = pComplexImage[j * nWidth + i];
			}
			FFT(pColumn.data(), nHeight);
			for (int j = 0; j < nHeight; j++)
			{
				pComplexImage[j * nWidth + i] = pColumn[j];
			}
			});
	}
	for (auto& t : colThreads)
	{
		t.join();  // 确保所有列 FFT 完成
	}

	// **计算幅度谱的最大值**
	double maxMagnitude = 0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			double magnitude = abs(pComplexImage[i * nWidth + j]);
			maxMagnitude = max(maxMagnitude, magnitude);
		}
	}

	// **归一化计算**
	double c = 255.0 / log(1 + maxMagnitude);
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			double magnitude = abs(pComplexImage[i * nWidth + j]);
			double phase = arg(pComplexImage[i * nWidth + j]);

			// **归一化幅值**
			normMagnitudeMap[i][j] = (float)(c * log(1 + magnitude));

			// **归一化相位到 0~255**
			normPhaseMap[i][j] = (float)(phase);
		}
	}

	// **将幅值图像存回 m_pDibBits 以显示 FFT 结果**
	for (int i = 0; i < nHeight; i++)
		for (int j = 0; j < nWidth; j++)
			*(m_pDibBits + i * m_nWidthBytes + j) = normMagnitudeMap[i][j];

	delete[] pComplexImage;
}


void CDib::IFFT2D()
{
	// Get the size of the image
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	// Create a complex array to store the image
	complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			pComplexImage[i * nWidth + j] = complex<double>(*(m_pDibBits + i * m_nWidthBytes + j), 0);
		}
	}
	// Perform 1D IFFT on each row
	for (int i = 0; i < nHeight; i++)
	{
		IFFT(pComplexImage + i * nWidth, nWidth);
	}
	// Perform 1D IFFT on each column
	complex<double>* pColumn = new complex<double>[nHeight];
	for (int i = 0; i < nWidth; i++)
	{
		for (int j = 0; j < nHeight; j++)
		{
			pColumn[j] = pComplexImage[j * nWidth + i];
		}
		IFFT(pColumn, nHeight);
		for (int j = 0; j < nHeight; j++)
		{
			pComplexImage[j * nWidth + i] = pColumn[j];
		}
	}
	// Normalize the magnitude of the IFFT
	double maxMagnitude = 0;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			double magnitude = abs(pComplexImage[i * nWidth + j]);
			if (magnitude > maxMagnitude)
			{
				maxMagnitude = magnitude;
			}
		}
	}
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			double magnitude = abs(pComplexImage[i * nWidth + j]);
			*(m_pDibBits + i * m_nWidthBytes + j) = (unsigned char)(255.0 * magnitude / maxMagnitude);
		}
	}
	// Clean up

	delete[] pComplexImage;
	delete[] pColumn;
}

void CDib::LowFilter(CString filterType, int D0, bool zeropadding)
{
	if (m_pDibBits == nullptr) {
		AfxMessageBox(_T("m_pDibBits 未初始化！"));
		return;
	}

	int nOriginalWidth = GetWidth();
	int nOriginalHeight = GetHeight();

	// 根据是否启用补零调整图像尺寸
	int nWidth = nOriginalWidth;
	int nHeight = nOriginalHeight;

	if (zeropadding) {
		nWidth *= 2;
		nHeight *= 2;
	}

	const int N = nWidth * nHeight; // 总像素数
	complex<double>* pComplexImage = new complex<double>[N];
	memset(pComplexImage, 0, N * sizeof(complex<double>));

	// 将原图像数据复制到扩展图像的中心
	if (zeropadding) {
		int offsetX = (nWidth - nOriginalWidth) / 2;
		int offsetY = (nHeight - nOriginalHeight) / 2;

		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				double pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
				pComplexImage[(i + offsetY) * nWidth + (j + offsetX)] = complex<double>(pixelValue, 0);
			}
		}
	}
	else {
		// 不启用补零，直接复制原始图像数据
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				double pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
				pComplexImage[i * nWidth + j] = complex<double>(pixelValue, 0);
			}
		}
	}

	// 预处理：乘以 (-1)^(x+y) 进行中心化
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			pComplexImage[i * nWidth + j] = complex<double>(pComplexImage[i * nWidth + j].real() * pow(-1, i + j), 0);
		}
	}

	// 对每一行执行 1D FFT
	for (int i = 0; i < nHeight; i++) {
		FFT(pComplexImage + i * nWidth, nWidth);
	}

	// 对每一列执行 1D FFT
	complex<double>* pColumn = new complex<double>[nHeight];
	for (int i = 0; i < nWidth; i++) {
		for (int j = 0; j < nHeight; j++) {
			pColumn[j] = pComplexImage[j * nWidth + i];
		}
		FFT(pColumn, nHeight);
		for (int j = 0; j < nHeight; j++) {
			pComplexImage[j * nWidth + i] = pColumn[j];
		}
	}
	delete[] pColumn;

	// 低通滤波处理
	if (filterType == "Ideal") {
		IdealLowPassFilter(pComplexImage, nWidth, nHeight, D0);
	}
	else if (filterType == "ButterWorth") {
		ButterworthLowPassFilter(pComplexImage, nWidth, nHeight, D0);
	}
	else if (filterType == "Gaussian") {
		GaussianLowPassFilter(pComplexImage, nWidth, nHeight, D0);
	}

	// 逆变换 IFFT
	pColumn = new complex<double>[nHeight];
	for (int i = 0; i < nWidth; i++) {
		for (int j = 0; j < nHeight; j++) {
			pColumn[j] = pComplexImage[j * nWidth + i];
		}
		IFFT(pColumn, nHeight);
		for (int j = 0; j < nHeight; j++) {
			pComplexImage[j * nWidth + i] = pColumn[j];
		}
	}
	delete[] pColumn;

	for (int i = 0; i < nHeight; i++) {
		IFFT(pComplexImage + i * nWidth, nWidth);
	}

	// 反中心化处理并将结果裁剪回原来的尺寸
	const double dScale = 1.0 / N; // 归一化因子

	if (zeropadding) {
		// 创建一个新的数组来存储处理后的图像数据
		complex<double>* pComplexImage0 = new complex<double>[nOriginalWidth * nOriginalHeight];

		int offsetX = (nWidth - nOriginalWidth) / 2;
		int offsetY = (nHeight - nOriginalHeight) / 2;

		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				complex<double> value = pComplexImage[(i + offsetY) * nWidth + (j + offsetX)];
				double realValue = value.real() * pow(-1, i + j) * dScale;
				pComplexImage0[i * nOriginalWidth + j] = complex<double>(realValue, 0);
			}
		}

		// 将处理后的数据转换回图像数据
		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				double realValue = pComplexImage0[i * nOriginalWidth + j].real();
				*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)max(0.0, min(255.0, realValue));
			}
		}

		delete[] pComplexImage0;
	}
	else {
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				complex<double> value = pComplexImage[i * nWidth + j];
				double realValue = value.real() * pow(-1, i + j) * dScale;
				*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)max(0.0, min(255.0, realValue));
			}
		}
	}

	delete[] pComplexImage;
}

void CDib::HighFilter(CString filterType, int D0, bool zeropadding)
{
	if (m_pDibBits == nullptr) {
		AfxMessageBox(_T("m_pDibBits 未初始化！"));
		return;
	}

	int nOriginalWidth = GetWidth();
	int nOriginalHeight = GetHeight();

	// 根据是否启用补零调整图像尺寸
	int nWidth = nOriginalWidth;
	int nHeight = nOriginalHeight;

	if (zeropadding) {
		nWidth *= 2;
		nHeight *= 2;
	}

	const int N = nWidth * nHeight; // 总像素数
	complex<double>* pComplexImage = new complex<double>[N];
	memset(pComplexImage, 0, N * sizeof(complex<double>));

	// 将原图像数据复制到扩展图像的中心
	if (zeropadding) {
		int offsetX = (nWidth - nOriginalWidth) / 2;
		int offsetY = (nHeight - nOriginalHeight) / 2;

		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				double pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
				pComplexImage[(i + offsetY) * nWidth + (j + offsetX)] = complex<double>(pixelValue, 0);
			}
		}
	}
	else {
		// 不启用补零，直接复制原始图像数据
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				double pixelValue = *(m_pDibBits + i * m_nWidthBytes + j);
				pComplexImage[i * nWidth + j] = complex<double>(pixelValue, 0);
			}
		}
	}

	// 预处理：乘以 (-1)^(x+y) 进行中心化
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			pComplexImage[i * nWidth + j] = complex<double>(pComplexImage[i * nWidth + j].real() * pow(-1, i + j), 0);
		}
	}

	// 对每一行执行 1D FFT
	for (int i = 0; i < nHeight; i++) {
		FFT(pComplexImage + i * nWidth, nWidth);
	}

	// 对每一列执行 1D FFT
	complex<double>* pColumn = new complex<double>[nHeight];
	for (int i = 0; i < nWidth; i++) {
		for (int j = 0; j < nHeight; j++) {
			pColumn[j] = pComplexImage[j * nWidth + i];
		}
		FFT(pColumn, nHeight);
		for (int j = 0; j < nHeight; j++) {
			pComplexImage[j * nWidth + i] = pColumn[j];
		}
	}
	delete[] pColumn;

	// 高通滤波处理
	if (filterType == "Ideal") {
		IdealHighPassFilter(pComplexImage, nWidth, nHeight, D0);
	}
	else if (filterType == "ButterWorth") {
		ButterworthHighPassFilter(pComplexImage, nWidth, nHeight, D0);
	}
	else if (filterType == "Gaussian") {
		GaussianHighPassFilter(pComplexImage, nWidth, nHeight, D0);
	}

	// 逆变换 IFFT
	pColumn = new complex<double>[nHeight];
	for (int i = 0; i < nWidth; i++) {
		for (int j = 0; j < nHeight; j++) {
			pColumn[j] = pComplexImage[j * nWidth + i];
		}
		IFFT(pColumn, nHeight);
		for (int j = 0; j < nHeight; j++) {
			pComplexImage[j * nWidth + i] = pColumn[j];
		}
	}
	delete[] pColumn;

	for (int i = 0; i < nHeight; i++) {
		IFFT(pComplexImage + i * nWidth, nWidth);
	}

	// 反中心化处理并将结果裁剪回原来的尺寸
	const double dScale = 1.0 / N; // 归一化因子

	if (zeropadding) {
		// 创建一个新的数组来存储处理后的图像数据
		complex<double>* pComplexImage0 = new complex<double>[nOriginalWidth * nOriginalHeight];

		int offsetX = (nWidth - nOriginalWidth) / 2;
		int offsetY = (nHeight - nOriginalHeight) / 2;

		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				complex<double> value = pComplexImage[(i + offsetY) * nWidth + (j + offsetX)];
				double realValue = value.real() * pow(-1, i + j) * dScale;
				pComplexImage0[i * nOriginalWidth + j] = complex<double>(realValue, 0);
			}
		}

		// 将处理后的数据转换回图像数据
		for (int i = 0; i < nOriginalHeight; i++) {
			for (int j = 0; j < nOriginalWidth; j++) {
				double realValue = pComplexImage0[i * nOriginalWidth + j].real();
				*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)max(0.0, min(255.0, realValue));
			}
		}

		delete[] pComplexImage0;
	}
	else {
		for (int i = 0; i < nHeight; i++) {
			for (int j = 0; j < nWidth; j++) {
				complex<double> value = pComplexImage[i * nWidth + j];
				double realValue = value.real() * pow(-1, i + j) * dScale;
				*(m_pDibBits + i * m_nWidthBytes + j) = (BYTE)max(0.0, min(255.0, realValue));
			}
		}
	}

	delete[] pComplexImage;
}

//滤波函数实现

//理想滤波器
void CDib::IdealLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			// 计算频率距离
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));

			// 低通滤波
			if (D > D0)
			{
				pComplexImage[u * nWidth + v] = 0 ;  // 设为 0
			}
		}
	}
}


void CDib::IdealHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));
			if (D < D0)
			{
				pComplexImage[u * nWidth + v] = 0;
			}
		}
	}
}


//巴特沃斯滤波器
void CDib::ButterworthLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	int n = 4; //默认四阶滤波器
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));
			double H = 1 / (1 + pow(D / D0, 2 * n));
			pComplexImage[u * nWidth + v] *= H;
		}
	}
}

void CDib::ButterworthHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	int n = 4;//默认四阶滤波器
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));
			double H = 1 / (1 + pow(D0 / D, 2 * n));
			pComplexImage[u * nWidth + v] *= H;
		}
	}
}


//高斯滤波器
void CDib::GaussianLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));
			double H = exp(-pow(D, 2) / (2 * pow(D0, 2)));
			pComplexImage[u * nWidth + v] *= H;
		}
	}
}

void CDib::GaussianHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0)
{
	for (int u = 0; u < nHeight; u++)
	{
		for (int v = 0; v < nWidth; v++)
		{
			double D = sqrt(pow(u - nHeight / 2, 2) + pow(v - nWidth / 2, 2));
			double H = 1 - exp(-pow(D, 2) / (2 * pow(D0, 2)));
			pComplexImage[u * nWidth + v] *= H;
		}
	}
}

//几何变换函数实现
void CDib::Rotate(double angle)
{
	int centerX = m_nWidth / 2;
	int centerY = m_nHeight / 2;
	double radian = angle * Pi / 180.0;

	unsigned char* tempBuffer = new unsigned char[m_nHeight * m_nWidthBytes];
	memset(tempBuffer, 0, m_nHeight * m_nWidthBytes);

	for (int y = 0; y < m_nHeight; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			int newX = static_cast<int>((x - centerX) * cos(radian) - (y - centerY) * sin(radian) + centerX);
			int newY = static_cast<int>((x - centerX) * sin(radian) + (y - centerY) * cos(radian) + centerY);

			if (newX >= 0 && newX < m_nWidth && newY >= 0 && newY < m_nHeight)
			{
				// 双线性插值
				int x1 = floor(newX);
				int y1 = floor(newY);
				int x2 = ceil(newX);
				int y2 = ceil(newY);

				double a = newX - x1;
				double b = newY - y1;

				unsigned char p1 = m_pDibBits[y1 * m_nWidthBytes + x1];
				unsigned char p2 = m_pDibBits[y1 * m_nWidthBytes + x2];
				unsigned char p3 = m_pDibBits[y2 * m_nWidthBytes + x1];
				unsigned char p4 = m_pDibBits[y2 * m_nWidthBytes + x2];

				tempBuffer[y * m_nWidthBytes + x] = static_cast<unsigned char>((1 - a) * (1 - b) * p1 + a * (1 - b) * p2 + (1 - a) * b * p3 + a * b * p4);
			}
		}
	}

	memcpy(m_pDibBits, tempBuffer, m_nHeight * m_nWidthBytes);
	delete[] tempBuffer;
}


void CDib::ScaleImage(double scaleFactor)
{
	int newWidth = static_cast<int>(m_nWidth * scaleFactor);
	int newHeight = static_cast<int>(m_nHeight * scaleFactor);
	int newWidthBytes = ((newWidth + 3) / 4) * 4;  // 4字节对齐

	int N = newWidthBytes * newHeight;
	unsigned char* pNewBits = new unsigned char[N]();
	if (!pNewBits) {
		AfxMessageBox(_T("内存分配失败！"));
		return;
	}

	double dx = static_cast<double>(m_nWidth) / newWidth;
	double dy = static_cast<double>(m_nHeight) / newHeight;

	for (int y = 0; y < newHeight; y++) {
		for (int x = 0; x < newWidth; x++) {
			double srcX = x * dx;
			double srcY = y * dy;

			int x1 = static_cast<int>(srcX);
			int y1 = static_cast<int>(srcY);
			int x2 = min(x1 + 1, (int)(m_nWidth - 1));
			int y2 = min(y1 + 1, (int)(m_nHeight - 1));
			double u = srcX - x1;
			double v = srcY - y1;

			unsigned char p1 = *(m_pDibBits + y1 * m_nWidthBytes + x1);
			unsigned char p2 = *(m_pDibBits + y1 * m_nWidthBytes + x2);
			unsigned char p3 = *(m_pDibBits + y2 * m_nWidthBytes + x1);
			unsigned char p4 = *(m_pDibBits + y2 * m_nWidthBytes + x2);

			double gray = (1.0 - u) * (1.0 - v) * p1 +
				u * (1.0 - v) * p2 +
				(1.0 - u) * v * p3 +
				u * v * p4;

			pNewBits[y * newWidthBytes + x] = static_cast<unsigned char>(gray);
		}
	}

	// 创建新的 DIB 并复制数据
	CreateGrayDib(newWidth, newHeight, pNewBits);

	delete[] pNewBits;
}

void CDib::CreateGrayDib(int width, int height, unsigned char pImage[])
{
	if (width <= 0 || height <= 0) {
		AfxMessageBox(_T("错误：图像尺寸无效！"));
		return;
	}

	// 先销毁旧的 DIB 避免 `m_hBitmap` 断言失败
	Destroy();

	// 创建新的灰度图像
	if (!Create(width, height, 8, 0)) {
		AfxMessageBox(_T("DIB 图像创建失败！"));
		return;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	// 确保 `m_pDibBits` 有效
	if (!m_pDibBits) {
		AfxMessageBox(_T("错误：GetBits() 返回 NULL！"));
		return;
	}

	// 设置调色板（灰度图像）
	if (IsIndexed()) {
		RGBQUAD pal[256];
		for (int i = 0; i < 256; i++) {
			pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)i;
			pal[i].rgbReserved = 0;
		}
		SetColorTable(0, 256, pal);
	}

	// **复制图像数据，注意 DIB 是倒置存储的**
	memcpy(m_pDibBits, pImage, m_nHeight * m_nWidthBytes);
}

void CDib::AddGaussianNoise(double stddev)
{
	double mean = 0.0;
	srand((unsigned int)time(NULL)); // 初始化随机数种子

	if (m_nBitCount == 8) {
		for (int y = 0; y < m_nHeight; y++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				// 1. 获取当前像素指针
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;

				// 2. 生成两个 [0,1) 的均匀分布随机数
				double u1 = (rand() + 1.0) / (RAND_MAX + 2.0); // 避免 ln(0)
				double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);

				// 3. Box-Muller 变换生成标准正态分布
				double z = sqrt(-2.0 * log(u1)) * cos(2 * Pi * u2);

				// 4. 转换为自定义高斯分布
				double noise = mean + stddev * z;

				// 5. 加到原始像素值，并裁剪到 [0, 255]
				int newVal = (int)(*pPixel + noise);
				if (newVal < 0) newVal = 0;
				if (newVal > 255) newVal = 255;

				*pPixel = (unsigned char)newVal;
			}
		}
	}
	else {
		double mean = 0.0;
		srand((unsigned int)time(NULL)); // 初始化随机数种子

		for (int y = 0; y < m_nHeight; y++) {
			for (int x = 0; x < m_nWidth; x++) {
				// 获取当前像素的指针
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;

				// 对每个通道添加不同的噪声
				for (int i = 0; i < 3; i++) { // 遍历R, G, B三个通道
					// 生成两个 [0,1) 的均匀分布随机数
					double u1 = (rand() + 1.0) / (RAND_MAX + 2.0); // 避免 ln(0)
					double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);

					// Box-Muller 变换生成标准正态分布
					double z = sqrt(-2.0 * log(u1)) * cos(2 * Pi * u2);

					// 转换为自定义高斯分布
					double noise = mean + stddev * z;

					// 获取原始像素值
					unsigned char original = pPixel[i];

					// 添加噪声并裁剪到 [0, 255]
					int newVal = original + (int)noise;
					if (newVal < 0) newVal = 0;
					if (newVal > 255) newVal = 255;

					// 更新像素值
					pPixel[i] = (unsigned char)newVal;
				}
			}
		}
	}
}


void CDib::AddSaltPapperNoise(double noise_ratio)
{
	srand((unsigned int)time(NULL)); // 初始化随机数种子
	if (m_nBitCount == 8) {
		for (int y = 0; y < m_nHeight; y++)
		{
			for (int x = 0; x < m_nWidth; x++)
			{
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x;

				double r = (rand() + 1.0) / (RAND_MAX + 2.0); // 生成 [0, 1) 的随机数

				if (r < noise_ratio)
				{
					// 再生成一个随机值决定是椒还是盐
					double salt_or_pepper = (rand() % 2 == 0) ? 0 : 255;
					*pPixel = (unsigned char)salt_or_pepper;
				}
			}
		}
	}
	else {
		for (int y = 0; y < m_nHeight; y++) {
			for (int x = 0; x < m_nWidth; x++) {
				// 获取当前像素的指针
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;

				// 对每个通道添加噪声
				for (int i = 0; i < 3; i++) { // 遍历R, G, B三个通道
					// 生成 [0, 1) 的随机数
					double r = (rand() + 1.0) / (RAND_MAX + 2.0);

					if (r < noise_ratio) {
						// 再生成一个随机值决定是椒（0）还是盐（255）
						double salt_or_pepper = (rand() % 2 == 0) ? 0 : 255;
						pPixel[i] = (unsigned char)salt_or_pepper;
					}
				}
			}
		}
	}
}

void CDib::AdaptiveMedianFilter(int maxWindowSize)
{
	// 拷贝原图用于参考
	if (m_nBitCount == 8) {
		{
			unsigned char* sourceBits = new unsigned char[m_nHeight * m_nWidthBytes];
			memcpy(sourceBits, m_pDibBits, m_nHeight * m_nWidthBytes);

			for (int y = 0; y < m_nHeight; y++)
			{
				for (int x = 0; x < m_nWidth; x++)
				{
					int S = 3;
					bool found = false;
					unsigned char Zxy = *(sourceBits + y * m_nWidthBytes + x);
					unsigned char Zmed = Zxy;

					while (S <= maxWindowSize && !found)
					{
						std::vector<unsigned char> window;
						int half = S / 2;

						for (int dy = -half; dy <= half; dy++)
						{
							int yy = y + dy;
							if (yy < 0 || yy >= m_nHeight) continue;

							for (int dx = -half; dx <= half; dx++)
							{
								int xx = x + dx;
								if (xx < 0 || xx >= m_nWidth) continue;

								unsigned char val = *(sourceBits + yy * m_nWidthBytes + xx);
								window.push_back(val);
							}
						}

						if (window.empty()) break;

						std::sort(window.begin(), window.end());
						unsigned char Zmin = window.front();
						unsigned char Zmax = window.back();
						Zmed = window[window.size() / 2];

						int A1 = Zmed - Zmin;
						int A2 = Zmed - Zmax;

						if (A1 > 0 && A2 < 0)
						{
							int B1 = Zxy - Zmin;
							int B2 = Zxy - Zmax;

							if (B1 > 0 && B2 < 0)
								*(m_pDibBits + y * m_nWidthBytes + x) = Zxy;
							else
								*(m_pDibBits + y * m_nWidthBytes + x) = Zmed;

							found = true;
						}
						else
						{
							S += 2;
						}
					}

					if (!found)
					{
						*(m_pDibBits + y * m_nWidthBytes + x) = Zmed;
					}
				}
			}

			delete[] sourceBits;
		}
	}
	else {
		// 拷贝原图用于参考
		unsigned char* sourceBits = new unsigned char[m_nHeight * m_nWidthBytes];
		memcpy(sourceBits, m_pDibBits, m_nHeight * m_nWidthBytes);

		// 对每个通道分别进行滤波
		for (int channel = 0; channel < 3; channel++) { // RGB三个通道
			for (int y = 0; y < m_nHeight; y++) {
				for (int x = 0; x < m_nWidth; x++) {
					int S = 3;
					bool found = false;
					unsigned char Zxy = *(sourceBits + y * m_nWidthBytes + x * 3 + channel);
					unsigned char Zmed = Zxy;

					while (S <= maxWindowSize && !found) {
						std::vector<unsigned char> window;
						int half = S / 2;

						for (int dy = -half; dy <= half; dy++) {
							int yy = y + dy;
							if (yy < 0 || yy >= m_nHeight) continue;

							for (int dx = -half; dx <= half; dx++) {
								int xx = x + dx;
								if (xx < 0 || xx >= m_nWidth) continue;

								unsigned char val = *(sourceBits + yy * m_nWidthBytes + xx * 3 + channel);
								window.push_back(val);
							}
						}

						if (window.empty()) break;

						std::sort(window.begin(), window.end());
						unsigned char Zmin = window.front();
						unsigned char Zmax = window.back();
						Zmed = window[window.size() / 2];

						int A1 = Zmed - Zmin;
						int A2 = Zmed - Zmax;

						if (A1 > 0 && A2 < 0) {
							int B1 = Zxy - Zmin;
							int B2 = Zxy - Zmax;

							if (B1 > 0 && B2 < 0)
								*(m_pDibBits + y * m_nWidthBytes + x * 3 + channel) = Zxy;
							else
								*(m_pDibBits + y * m_nWidthBytes + x * 3 + channel) = Zmed;

							found = true;
						}
						else {
							S += 2;
						}
					}

					if (!found) {
						*(m_pDibBits + y * m_nWidthBytes + x * 3 + channel) = Zmed;
					}
				}
			}
		}

		delete[] sourceBits;
	}
}

void CDib::AdaptiveGaussianDenoise(int windowSize, float estimatedNoiseVar)
{
	int half = windowSize / 2;
	float epsilon = 1e-5f;

	unsigned char* srcBits = new unsigned char[m_nHeight * m_nWidthBytes];
	memcpy(srcBits, m_pDibBits, m_nHeight * m_nWidthBytes);
	if (m_nBitCount == 8) {
		for (int y = 0; y < m_nHeight; ++y)
		{
			for (int x = 0; x < m_nWidth; ++x)
			{
				float sum = 0;
				float sumSq = 0;
				int count = 0;

				for (int dy = -half; dy <= half; ++dy)
				{
					int yy = y + dy;
					if (yy < 0 || yy >= m_nHeight) continue;

					for (int dx = -half; dx <= half; ++dx)
					{
						int xx = x + dx;
						if (xx < 0 || xx >= m_nWidth) continue;

						unsigned char val = *(srcBits + yy * m_nWidthBytes + xx);
						sum += val;
						sumSq += val * val;
						count++;
					}
				}

				if (count == 0) continue;

				float mean = sum / count;
				float var = sumSq / count - mean * mean;
				float current = *(srcBits + y * m_nWidthBytes + x);

				float weight = max(0.f, (var - estimatedNoiseVar) / (var + epsilon));
				float result = mean + weight * (current - mean);

				result = min(255.f, max(0.f, result));
				*(m_pDibBits + y * m_nWidthBytes + x) = (unsigned char)(result);
			}
		}

		delete[] srcBits;
	}
	else {

		// 对每个通道分别进行滤波
		for (int channel = 0; channel < 3; channel++) { // RGB三个通道
			for (int y = 0; y < m_nHeight; ++y) {
				for (int x = 0; x < m_nWidth; ++x) {
					float sum = 0;
					float sumSq = 0;
					int count = 0;

					for (int dy = -half; dy <= half; ++dy) {
						int yy = y + dy;
						if (yy < 0 || yy >= m_nHeight) continue;

						for (int dx = -half; dx <= half; ++dx) {
							int xx = x + dx;
							if (xx < 0 || xx >= m_nWidth) continue;

							unsigned char val = *(srcBits + yy * m_nWidthBytes + xx * 3 + channel);
							sum += val;
							sumSq += val * val;
							count++;
						}
					}

					if (count == 0) continue;

					float mean = sum / count;
					float var = sumSq / count - mean * mean;
					float current = *(srcBits + y * m_nWidthBytes + x * 3 + channel);

					float weight = max(0.0f, (var - estimatedNoiseVar) / (var + epsilon));
					float result = mean + weight * (current - mean);

					result = min(255.0f, max(0.0f, result));
					*(m_pDibBits + y * m_nWidthBytes + x * 3 + channel) = (unsigned char)(result);
				}
			}
		}

		delete[] srcBits;
	}
}

void CDib::TurbulenceBlur(float k = 0.001)
{

	if (m_nBitCount == 8) {
		// 获取图像尺寸
		int nWidth = GetWidth();
		int nHeight = GetHeight();

		// 初始化复数数组（包含相位调整）
		complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pComplexImage[i * nWidth + j] = complex<double>(
					(*(m_pDibBits + i * m_nWidthBytes + j)) * pow(-1, i + j), 0);
			}
		}

		// 执行2D FFT：先对行进行FFT，再对列进行FFT
		// 对每一行进行FFT
		for (int i = 0; i < nHeight; i++)
		{
			FFT(pComplexImage + i * nWidth, nWidth);
		}

		// 对每一列进行FFT
		complex<double>* pColumn = new complex<double>[nHeight];
		for (int col = 0; col < nWidth; col++)
		{
			for (int row = 0; row < nHeight; row++)
			{
				pColumn[row] = pComplexImage[row * nWidth + col];
			}
			FFT(pColumn, nHeight);
			for (int row = 0; row < nHeight; row++)
			{
				pComplexImage[row * nWidth + col] = pColumn[row];
			}
		}

		// 应用退化函数 e^{-k*(u² + v²)^{5/6}}
		int centerX = nWidth / 2;
		int centerY = nHeight / 2;
		for (int row = 0; row < nHeight; row++)
		{
			for (int col = 0; col < nWidth; col++)
			{
				// 计算坐标偏移
				double dx = col - centerX;
				double dy = row - centerY;
				double distanceSquared = dx * dx + dy * dy;

				// 计算指数项
				double exponent = -k * pow(distanceSquared, 5.0 / 6.0);
				double factor = exp(exponent);

				// 乘以退化系数
				pComplexImage[row * nWidth + col] *= factor;
			}
		}

		// 执行逆2D FFT：先对列进行IFFT，再对行进行IFFT
		// 对每一列进行IFFT
		for (int col = 0; col < nWidth; col++)
		{
			for (int row = 0; row < nHeight; row++)
			{
				pColumn[row] = pComplexImage[row * nWidth + col];
			}
			IFFT(pColumn, nHeight);
			for (int row = 0; row < nHeight; row++)
			{
				pComplexImage[row * nWidth + col] = pColumn[row];
			}
		}

		// 对每一行进行IFFT
		for (int row = 0; row < nHeight; row++)
		{
			IFFT(pComplexImage + row * nWidth, nWidth);
		}

		// 添加关键的归一化步骤（确保FFT/IFFT的缩放正确）
		// 假设FFT/IFFT未归一化，则需要除以图像总像素数
		const double normalizationFactor = 1.0 / (nWidth * nHeight);
		for (int row = 0; row < nHeight; row++)
		{
			for (int col = 0; col < nWidth; col++)
			{
				pComplexImage[row * nWidth + col] *= normalizationFactor;
			}
		}

		// 归一化并更新图像数据
		double maxMagnitude = 0.0;
		for (int row = 0; row < nHeight; row++)
		{
			for (int col = 0; col < nWidth; col++)
			{
				double mag = abs(pComplexImage[row * nWidth + col]);
				if (mag > maxMagnitude)
					maxMagnitude = mag;
			}
		}

		for (int row = 0; row < nHeight; row++)
		{
			for (int col = 0; col < nWidth; col++)
			{
				double mag = abs(pComplexImage[row * nWidth + col]);
				unsigned char value = static_cast<unsigned char>(
					255.0 * mag / maxMagnitude);
				*(m_pDibBits + row * m_nWidthBytes + col) = value;
			}
		}

		// 释放内存
		delete[] pComplexImage;
		delete[] pColumn;
	}
	else {
		int nWidth = GetWidth();
		int nHeight = GetHeight();

		const int channels = 3; // B, G, R
		const int totalPixels = nWidth * nHeight;

		// 为每个通道创建复数图像
		complex<double>* pComplexImage[3];
		for (int c = 0; c < channels; ++c)
			pComplexImage[c] = new complex<double>[totalPixels];

		// 将每个通道的像素值复制并乘上 (-1)^(i+j) 以实现中心化
		for (int i = 0; i < nHeight; ++i)
		{
			for (int j = 0; j < nWidth; ++j)
			{
				for (int c = 0; c < channels; ++c)
				{
					unsigned char pixelValue = *(m_pDibBits + i * m_nWidthBytes + j * 3 + c);
					pComplexImage[c][i * nWidth + j] = complex<double>(pixelValue * pow(-1, i + j), 0);
				}
			}
		}

		// 临时列向量用于列FFT
		complex<double>* pColumn = new complex<double>[nHeight];

		// 对每个通道分别进行频域处理
		for (int c = 0; c < channels; ++c)
		{
			// 1. 2D FFT
			for (int i = 0; i < nHeight; ++i)
				FFT(pComplexImage[c] + i * nWidth, nWidth);

			for (int col = 0; col < nWidth; ++col)
			{
				for (int row = 0; row < nHeight; ++row)
					pColumn[row] = pComplexImage[c][row * nWidth + col];
				FFT(pColumn, nHeight);
				for (int row = 0; row < nHeight; ++row)
					pComplexImage[c][row * nWidth + col] = pColumn[row];
			}

			// 2. 应用大气湍流退化函数
			int centerX = nWidth / 2;
			int centerY = nHeight / 2;
			for (int row = 0; row < nHeight; ++row)
			{
				for (int col = 0; col < nWidth; ++col)
				{
					double dx = col - centerX;
					double dy = row - centerY;
					double distanceSquared = dx * dx + dy * dy;
					double exponent = -k * pow(distanceSquared, 5.0 / 6.0);
					double factor = exp(exponent);
					pComplexImage[c][row * nWidth + col] *= factor;
				}
			}

			// 3. 2D IFFT
			for (int col = 0; col < nWidth; ++col)
			{
				for (int row = 0; row < nHeight; ++row)
					pColumn[row] = pComplexImage[c][row * nWidth + col];
				IFFT(pColumn, nHeight);
				for (int row = 0; row < nHeight; ++row)
					pComplexImage[c][row * nWidth + col] = pColumn[row];
			}
			for (int row = 0; row < nHeight; ++row)
				IFFT(pComplexImage[c] + row * nWidth, nWidth);

			// 4. 归一化缩放
			double normFactor = 1.0 / totalPixels;
			double maxMagnitude = 0.0;
			for (int i = 0; i < totalPixels; ++i)
			{
				pComplexImage[c][i] *= normFactor;
				double mag = abs(pComplexImage[c][i]);
				if (mag > maxMagnitude) maxMagnitude = mag;
			}

			// 5. 写回图像（值裁剪至 0-255）
			for (int i = 0; i < nHeight; ++i)
			{
				for (int j = 0; j < nWidth; ++j)
				{
					double mag = abs(pComplexImage[c][i * nWidth + j]);
					unsigned char value = static_cast<unsigned char>(min(255.0, 255.0 * mag / maxMagnitude));
					*(m_pDibBits + i * m_nWidthBytes + j * 3 + c) = value;
				}
			}
		}

		// 释放内存
		delete[] pColumn;
		for (int c = 0; c < channels; ++c)
			delete[] pComplexImage[c];
	}
}

//维纳滤波器处理湍流退化
void CDib::WienerFilterTur(float k = 0.001, float K = 0.01)
{
	if (m_nBitCount == 8 || m_nBitCount == 24)
	{
		int nWidth = GetWidth();
		int nHeight = GetHeight();
		int totalPixels = nWidth * nHeight;
		int channels = (m_nBitCount == 8) ? 1 : 3;

		// 为每个通道准备复数图像
		complex<double>* pComplex[3];
		for (int c = 0; c < channels; ++c)
			pComplex[c] = new complex<double>[totalPixels];

		// 初始化复数图像并中心化
		for (int i = 0; i < nHeight; ++i)
		{
			for (int j = 0; j < nWidth; ++j)
			{
				for (int c = 0; c < channels; ++c)
				{
					unsigned char value = (channels == 1)
						? *(m_pDibBits + i * m_nWidthBytes + j)
						: *(m_pDibBits + i * m_nWidthBytes + j * 3 + c);
					pComplex[c][i * nWidth + j] = complex<double>(value * pow(-1, i + j), 0);
				}
			}
		}

		// 临时列向量
		complex<double>* pColumn = new complex<double>[nHeight];

		for (int c = 0; c < channels; ++c)
		{
			// 1. 执行二维FFT
			for (int i = 0; i < nHeight; ++i)
				FFT(pComplex[c] + i * nWidth, nWidth);
			for (int j = 0; j < nWidth; ++j)
			{
				for (int i = 0; i < nHeight; ++i)
					pColumn[i] = pComplex[c][i * nWidth + j];
				FFT(pColumn, nHeight);
				for (int i = 0; i < nHeight; ++i)
					pComplex[c][i * nWidth + j] = pColumn[i];
			}

			// 2. 构建湍流退化函数 H 和 Wiener 滤波器 W
			int cx = nWidth / 2, cy = nHeight / 2;
			for (int i = 0; i < nHeight; ++i)
			{
				for (int j = 0; j < nWidth; ++j)
				{
					int idx = i * nWidth + j;
					double dx = j - cx, dy = i - cy;
					double d2 = dx * dx + dy * dy;

					double h = exp(-k * pow(d2, 5.0 / 6.0));
					complex<double> H(h, 0.0);
					complex<double> H_conj = conj(H);
					double H_abs2 = norm(H);

					// Wiener 滤波器
					complex<double> W = H_conj / (H_abs2 + K);

					// 滤波恢复
					pComplex[c][idx] *= W;
				}
			}

			// 3. 执行二维IFFT
			for (int j = 0; j < nWidth; ++j)
			{
				for (int i = 0; i < nHeight; ++i)
					pColumn[i] = pComplex[c][i * nWidth + j];
				IFFT(pColumn, nHeight);
				for (int i = 0; i < nHeight; ++i)
					pComplex[c][i * nWidth + j] = pColumn[i];
			}
			for (int i = 0; i < nHeight; ++i)
				IFFT(pComplex[c] + i * nWidth, nWidth);

			// 4. 归一化并写回图像
			double normFactor = 1.0 / totalPixels;
			double maxVal = 0.0;
			for (int i = 0; i < totalPixels; ++i)
			{
				pComplex[c][i] *= normFactor;
				double mag = abs(pComplex[c][i]);
				if (mag > maxVal) maxVal = mag;
			}

			for (int i = 0; i < nHeight; ++i)
			{
				for (int j = 0; j < nWidth; ++j)
				{
					double mag = abs(pComplex[c][i * nWidth + j]);
					unsigned char pixel = static_cast<unsigned char>(min(255.0, 255.0 * mag / maxVal));
					if (channels == 1)
						*(m_pDibBits + i * m_nWidthBytes + j) = pixel;
					else
						*(m_pDibBits + i * m_nWidthBytes + j * 3 + c) = pixel;
				}
			}
		}

		// 释放内存
		delete[] pColumn;
		for (int c = 0; c < channels; ++c)
			delete[] pComplex[c];
	}
}


void CDib::MotionBlur(float thetaDeg, float speed)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 转换角度为弧度
	double theta = thetaDeg * Pi / 180.0;

	// 初始化复数数组（含中心化处理）
	complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			pComplexImage[i * nWidth + j] = complex<double>(
				(*(m_pDibBits + i * m_nWidthBytes + j)) * pow(-1, i + j), 0);
		}
	}

	// FFT：先行后列
	for (int i = 0; i < nHeight; i++)
		FFT(pComplexImage + i * nWidth, nWidth);

	complex<double>* pColumn = new complex<double>[nHeight];
	for (int col = 0; col < nWidth; col++)
	{
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		FFT(pColumn, nHeight);
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}

	// 运动模糊退化函数
	int centerX = nWidth / 2;
	int centerY = nHeight / 2;
	double a = speed * cos(theta);
	double b = speed * sin(theta);

	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			double u = (col - centerX) / (double)nWidth;
			double v = (row - centerY) / (double)nHeight;
			double phi = Pi * (u * a + v * b);

			double sinc = (fabs(phi) < 1e-6) ? 1.0 : sin(phi) / phi;
			complex<double> H = sinc * complex<double>(cos(-phi), sin(-phi));

			pComplexImage[row * nWidth + col] *= H;
		}
	}

	// IFFT：先列后行
	for (int col = 0; col < nWidth; col++)
	{
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		IFFT(pColumn, nHeight);
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}
	for (int row = 0; row < nHeight; row++)
		IFFT(pComplexImage + row * nWidth, nWidth);

	// 归一化缩放
	const double scale = 1.0 / (nWidth * nHeight);
	for (int i = 0; i < nWidth * nHeight; i++)
		pComplexImage[i] *= scale;

	// 提取模长并归一化为[0,255]
	double maxMag = 0.0;
	for (int i = 0; i < nWidth * nHeight; i++)
	{
		double mag = abs(pComplexImage[i]);
		if (mag > maxMag) maxMag = mag;
	}

	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			double mag = abs(pComplexImage[row * nWidth + col]);
			unsigned char value = static_cast<unsigned char>(255.0 * mag / maxMag);
			*(m_pDibBits + row * m_nWidthBytes + col) = value;
		}
	}

	delete[] pComplexImage;
	delete[] pColumn;
}


template <typename T>
T clamp(T value, T low, T high) {
	return max(low, min(value, high));
}

void CDib::AddPeriodicNoise(double amplitude, double freqX, double freqY)
{
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	for (int y = 0; y < nHeight; y++)
	{
		for (int x = 0; x < nWidth; x++)
		{
			double noise = amplitude * sin(2 * Pi * (freqX * x + freqY * y));
			int offset = y * m_nWidthBytes + x;
			int value = *(m_pDibBits + offset);
			value = clamp(value + static_cast<int>(noise), 0, 255);
			*(m_pDibBits + offset) = static_cast<BYTE>(value);
		}
	}
}

void CDib::RemovePeriodicNoise(double freqX, double freqY, double D0)
{
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// === Step 1: 初始化频域图像（转为 complex，并中心化） ===
	complex<double>* m_pComplexImage = new complex<double>[nWidth * nHeight];
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			BYTE pixel = *(m_pDibBits + y * m_nWidthBytes + x);
			double centered = pixel * pow(-1.0, x + y);  // 频域中心化
			m_pComplexImage[y * nWidth + x] = complex<double>(centered, 0.0);
		}
	}

	// === Step 2: FFT（行 + 列） ===
	for (int i = 0; i < nHeight; i++)
		FFT(m_pComplexImage + i * nWidth, nWidth);
	for (int j = 0; j < nWidth; j++) {
		std::complex<double>* pColumn = new std::complex<double>[nHeight];
		for (int i = 0; i < nHeight; i++)
			pColumn[i] = m_pComplexImage[i * nWidth + j];
		FFT(pColumn, nHeight);
		for (int i = 0; i < nHeight; i++)
			m_pComplexImage[i * nWidth + j] = pColumn[i];
		delete[] pColumn;
	}

	// === Step 3: 陷波滤波器 Notch Filter ===
	double u0 = freqX * nWidth;
	double v0 = freqY * nHeight;

	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			double u = x - nWidth / 2.0;
			double v = y - nHeight / 2.0;

			double d1 = sqrt((u - u0) * (u - u0) + (v - v0) * (v - v0));
			double d2 = sqrt((u + u0) * (u + u0) + (v + v0) * (v + v0));

			double H = 1.0;
			if (d1 <= D0 || d2 <= D0)
				H = 0.0;

			m_pComplexImage[y * nWidth + x] *= H;
		}
	}

	// === Step 4: IFFT（列 + 行）===
	std::complex<double>* pColumn = new std::complex<double>[nHeight];
	for (int col = 0; col < nWidth; col++) {
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = m_pComplexImage[row * nWidth + col];
		IFFT(pColumn, nHeight);
		for (int row = 0; row < nHeight; row++)
			m_pComplexImage[row * nWidth + col] = pColumn[row];
	}
	delete[] pColumn;

	for (int row = 0; row < nHeight; row++)
		IFFT(m_pComplexImage + row * nWidth, nWidth);

	// === Step 5: 去中心化 + 归一化写入图像 ===
	double maxVal = 0.0;
	std::vector<double> magnitude(nWidth * nHeight);

	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			complex<double> val = m_pComplexImage[y * nWidth + x] * pow(-1.0, x + y);
			double realVal = val.real() / (nWidth * nHeight);
			magnitude[y * nWidth + x] = realVal;
			if (abs(realVal) > maxVal) maxVal = abs(realVal);
		}
	}

	if (maxVal == 0.0) maxVal = 1.0; // 避免除0

	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			double norm = magnitude[y * nWidth + x] / maxVal;
			BYTE value = static_cast<BYTE>(clamp(norm * 255.0, 0.0, 255.0));
			*(m_pDibBits + y * m_nWidthBytes + x) = value;
		}
	}

	delete[] m_pComplexImage;
}


void CDib::InverseFilter(float thetaDeg, float speed)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	double theta = thetaDeg * Pi / 180.0;
	double a = speed * cos(theta);
	double b = speed * sin(theta);

	// 储存频域图像
	complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];

	// 将图像数据转为复数，并中心化（乘 (-1)^{x+y}）
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			pComplexImage[i * nWidth + j] = complex<double>(
				(*(m_pDibBits + i * m_nWidthBytes + j)) * pow(-1, i + j), 0);
		}
	}

	// FFT（行 + 列）
	for (int i = 0; i < nHeight; i++)
		FFT(pComplexImage + i * nWidth, nWidth);

	complex<double>* pColumn = new complex<double>[nHeight];
	for (int col = 0; col < nWidth; col++)
	{
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		FFT(pColumn, nHeight);
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}

	// 构建逆滤波器：除以运动模糊退化函数 H(u,v)
	int centerX = nWidth / 2;
	int centerY = nHeight / 2;
	const double epsilon = 1e-3;  // 防止除以0

	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			double u = (col - centerX) / (double)nWidth;
			double v = (row - centerY) / (double)nHeight;
			double phi = Pi * (u * a + v * b);

			double sinc = (fabs(phi) < 1e-6) ? 1.0 : sin(phi) / phi;
			complex<double> H = sinc * complex<double>(cos(-phi), sin(-phi));

			// 防止除以0
			if (abs(H) < epsilon)
				H = epsilon;

			// 执行频域除法
			pComplexImage[row * nWidth + col] /= H;
		}
	}

	// IFFT（列 + 行）
	for (int col = 0; col < nWidth; col++)
	{
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		IFFT(pColumn, nHeight);
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}
	for (int row = 0; row < nHeight; row++)
		IFFT(pComplexImage + row * nWidth, nWidth);

	// 归一化缩放
	const double scale = 1.0 / (nWidth * nHeight);
	for (int i = 0; i < nWidth * nHeight; i++)
		pComplexImage[i] *= scale;

	// 获取最大模长，用于归一化显示
	double maxMag = 0.0;
	for (int i = 0; i < nWidth * nHeight; i++)
	{
		double mag = abs(pComplexImage[i]);
		if (mag > maxMag) maxMag = mag;
	}

	// 写回图像数据
	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			double mag = abs(pComplexImage[row * nWidth + col]);
			unsigned char value = static_cast<unsigned char>(
				255.0 * mag / maxMag);
			*(m_pDibBits + row * m_nWidthBytes + col) = value;
		}
	}

	delete[] pComplexImage;
	delete[] pColumn;
}

void CDib::WienerFilter(float thetaDeg, float speed, float K)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nChannel = (m_nBitCount == 8) ? 1 : 3; // 判断通道数：8位灰度=1通道，24位彩色=3通道

	complex<double>* pComplexImage = new complex<double>[nWidth * nHeight];
	complex<double>* pColumn = new complex<double>[nHeight];

	for (int c = 0; c < nChannel; c++)
	{
		// ========== 1. 构造频域图像（中心化） ==========
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				BYTE value;
				if (nChannel == 1) // 灰度图
					value = *(m_pDibBits + i * m_nWidthBytes + j);
				else // 彩色图
					value = *(m_pDibBits + i * m_nWidthBytes + j * 3 + c);

				pComplexImage[i * nWidth + j] = complex<double>(value * pow(-1, i + j), 0);
			}
		}

		// ========== 2. 执行2D FFT ==========
		for (int i = 0; i < nHeight; i++)
			FFT(pComplexImage + i * nWidth, nWidth);

		for (int col = 0; col < nWidth; col++)
		{
			for (int row = 0; row < nHeight; row++)
				pColumn[row] = pComplexImage[row * nWidth + col];
			FFT(pColumn, nHeight);
			for (int row = 0; row < nHeight; row++)
				pComplexImage[row * nWidth + col] = pColumn[row];
		}

		// ========== 3. 构造 Wiener 滤波器 ==========
		float theta = thetaDeg * Pi / 180.0;
		double a = speed * cos(theta) / nWidth;
		double b = speed * sin(theta) / nHeight;

		for (int row = 0; row < nHeight; row++)
		{
			for (int col = 0; col < nWidth; col++)
			{
				int u = col - nWidth / 2;
				int v = row - nHeight / 2;

				double pi_term = Pi * (u * a + v * b);
				complex<double> H;
				if (fabs(pi_term) < 1e-6)
				{
					H = complex<double>(1.0, 0);  // sinc(0) = 1
				}
				else
				{
					double sinc = sin(pi_term) / pi_term;
					double phase = -pi_term;
					H = sinc * complex<double>(cos(phase), sin(phase));
				}

				complex<double> H_conj = conj(H);
				double H_mag2 = norm(H);
				complex<double> G = pComplexImage[row * nWidth + col];
				pComplexImage[row * nWidth + col] = (H_conj / (H_mag2 + K)) * G;
			}
		}

		// ========== 4. 执行2D IFFT ==========
		for (int col = 0; col < nWidth; col++)
		{
			for (int row = 0; row < nHeight; row++)
				pColumn[row] = pComplexImage[row * nWidth + col];
			IFFT(pColumn, nHeight);
			for (int row = 0; row < nHeight; row++)
				pComplexImage[row * nWidth + col] = pColumn[row];
		}

		for (int row = 0; row < nHeight; row++)
			IFFT(pComplexImage + row * nWidth, nWidth);

		// ========== 5. 归一化并写回图像 ==========
		const double normFactor = 1.0 / (nWidth * nHeight);
		double maxMag = 0;
		for (int i = 0; i < nWidth * nHeight; i++)
		{
			pComplexImage[i] *= normFactor;
			maxMag = max(maxMag, abs(pComplexImage[i]));
		}

		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				double val = abs(pComplexImage[i * nWidth + j]);
				val = 255.0 * val / maxMag;
				val = min(255.0, max(0.0, val));

				if (nChannel == 1)
					*(m_pDibBits + i * m_nWidthBytes + j) = static_cast<BYTE>(val);
				else
					*(m_pDibBits + i * m_nWidthBytes + j * 3 + c) = static_cast<BYTE>(val);
			}
		}
	}

	delete[] pComplexImage;
	delete[] pColumn;
}


void CDib::CSLF(float thetaDeg, float speed, double gamma)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 输入验证
	if (nWidth <= 0 || nHeight <= 0 || gamma < 0)
		return;

	// 转弧度
	double theta = thetaDeg * Pi / 180.0;
	double a = speed * cos(theta);
	double b = speed * sin(theta);

	// 图像中心化处理为复数数组
	std::unique_ptr<complex<double>[]> pComplexImage(new complex<double>[nWidth * nHeight]);
	for (int i = 0; i < nHeight; i++) {
		for (int j = 0; j < nWidth; j++) {
			// 中心化：乘以 (-1)^(i+j)
			double sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;
			pComplexImage[i * nWidth + j] = complex<double>(
				(*(m_pDibBits + i * m_nWidthBytes + j)) * sign, 0);
		}
	}

	// FFT2D：先行后列
	for (int i = 0; i < nHeight; i++)
		FFT(pComplexImage.get() + i * nWidth, nWidth);

	std::unique_ptr<complex<double>[]> pColumn(new complex<double>[nHeight]);
	for (int col = 0; col < nWidth; col++) {
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		FFT(pColumn.get(), nHeight);
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}

	// 构造运动模糊退化函数 H
	std::unique_ptr<complex<double>[]> H(new complex<double>[nWidth * nHeight]);
	int centerX = nWidth / 2;
	int centerY = nHeight / 2;

	// 直接在频域构造拉普拉斯算子 P
	std::unique_ptr<complex<double>[]> P(new complex<double>[nWidth * nHeight]);
	for (int row = 0; row < nHeight; row++) {
		for (int col = 0; col < nWidth; col++) {
			// 中心化频率坐标
			double u = (col - centerX) / static_cast<double>(nWidth);
			double v = (row - centerY) / static_cast<double>(nHeight);

			// 计算拉普拉斯频域响应（根据3x3核的精确傅里叶变换）
			double p_real = 4.0 - 2.0 * cos(2.0 * Pi * u) - 2.0 * cos(2.0 * Pi * v);
			P[row * nWidth + col] = complex<double>(p_real, 0.0);
		}
	}

	// 计算 H(u,v) 并进行约束最小二乘滤波
	const double epsilon = 1e-6; // 正则化参数防止除以零
	for (int row = 0; row < nHeight; row++) {
		for (int col = 0; col < nWidth; col++) {
			int idx = row * nWidth + col;
			double u = (col - centerX) / static_cast<double>(nWidth);
			double v = (row - centerY) / static_cast<double>(nHeight);

			// 计算运动模糊传递函数
			double phi = Pi * (u * a + v * b);
			complex<double> H_val;
			if (fabs(phi) < 1e-6) {
				H_val = complex<double>(1.0, 0);
			}
			else {
				H_val = complex<double>(sin(phi) / phi, 0) *
					complex<double>(cos(-phi), sin(-phi));
			}
			H[idx] = H_val;

			// CLSF 滤波公式
			complex<double> G = pComplexImage[idx];
			complex<double> H_conj = conj(H_val);
			double H2 = norm(H_val);
			double P2 = norm(P[idx]);
			double denom = H2 + gamma * P2 + epsilon;

			pComplexImage[idx] = (H_conj * G) / denom;
		}
	}

	// IFFT2D：先列后行，并归一化
	for (int col = 0; col < nWidth; col++) {
		for (int row = 0; row < nHeight; row++)
			pColumn[row] = pComplexImage[row * nWidth + col];
		IFFT(pColumn.get(), nHeight);
		// 列归一化
		for (int row = 0; row < nHeight; row++)
			pColumn[row] /= nHeight;
		for (int row = 0; row < nHeight; row++)
			pComplexImage[row * nWidth + col] = pColumn[row];
	}

	for (int row = 0; row < nHeight; row++) {
		IFFT(pComplexImage.get() + row * nWidth, nWidth);
		// 行归一化
		for (int col = 0; col < nWidth; col++)
			pComplexImage[row * nWidth + col] /= nWidth;
	}

	// 逆中心化：再次乘以 (-1)^(i+j)
	for (int row = 0; row < nHeight; row++) {
		for (int col = 0; col < nWidth; col++) {
			double sign = ((row + col) % 2 == 0) ? 1.0 : -1.0;
			pComplexImage[row * nWidth + col] *= sign;
		}
	}

	// 动态范围归一化（基于实部）
	double minVal = INFINITY, maxVal = -INFINITY;
	for (int i = 0; i < nWidth * nHeight; i++) {
		double realVal = real(pComplexImage[i]);
		if (realVal < minVal) minVal = realVal;
		if (realVal > maxVal) maxVal = realVal;
	}
	double range = maxVal - minVal;
	if (range < 1e-6) range = 1; // 防止除零

	for (int row = 0; row < nHeight; row++) {
		for (int col = 0; col < nWidth; col++) {
			double realVal = real(pComplexImage[row * nWidth + col]);
			realVal = (realVal - minVal) / range * 255.0;
			unsigned char value = static_cast<unsigned char>(clamp(realVal, 0.0, 255.0));
			*(m_pDibBits + row * m_nWidthBytes + col) = value;
		}
	}
}

//彩色图像处理
void CDib::CreateRGBTestPage(int width, int height) {
	// 1. 清除旧数据并创建新的 24-bit 彩色图像
	if (!IsNull()) {
		Destroy();
	}
	Create(width, height, 24); // 创建 24-bit 彩色图

	// 2. 重新获取图像的基本信息
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	// 3. 清空背景为黑色
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			pPixel[0] = 0; // Blue
			pPixel[1] = 0; // Green
			pPixel[2] = 0; // Red
		}
	}

	// 定义三个圆的中心和半径
	int centerX = width / 2;
	int centerY = height / 2;
	int radiusRed = width / 6;
	int radiusGreen = width / 6;
	int radiusBlue = width / 6;

	// 绘制红色圆形
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int dx = x - centerX;
			int dy = y - (centerY+radiusRed/2);
			double distance = sqrt(dx * dx + dy * dy);

			if (distance <= radiusRed) {
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
				pPixel[2] = 255; // Red component
			}
		}
	}

	// 绘制绿色圆形（偏移中心）
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int dx = x - (centerX + radiusGreen / 2);
			int dy = y - (centerY - radiusGreen / 2);
			double distance = sqrt(dx * dx + dy * dy);

			if (distance <= radiusGreen) {
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
				pPixel[1] = 255; // Green component
			}
		}
	}

	// 绘制蓝色圆形（偏移中心）
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int dx = x - (centerX - radiusBlue / 2);
			int dy = y - (centerY - radiusBlue / 2);
			double distance = sqrt(dx * dx + dy * dy);

			if (distance <= radiusBlue) {
				unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
				pPixel[0] = 255; // Blue component
			}
		}
	}
}

//计算色调
void CDib::CalculateHue()
{
	if (IsNull()) return;
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 遍历每个像素，计算色调并叠加到原图上
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			unsigned char B = pPixel[0];
			unsigned char G = pPixel[1];
			unsigned char R = pPixel[2];

			// 转换为0-1范围
			double r = R / 255.0;
			double g = G / 255.0;
			double b = B / 255.0;

			double maxVal = max(r, max(g, b));
			double minVal = min(r, min(g, b));
			double delta = maxVal - minVal;

			double hue = 0;
			if (delta != 0) {
				if (maxVal == r) {
					hue = fmod((g - b) / delta, 6.0);
				}
				else if (maxVal == g) {
					hue = (b - r) / delta + 2;
				}
				else {
					hue = (r - g) / delta + 4;
				}
				hue *= 60;
				if (hue < 0) hue += 360;
			}

			// 映射到0-255范围的灰度值
			unsigned char hueGrayscale = static_cast<unsigned char>(hue / 360.0 * 255.0);

			// 更新原图的RGB值（保持色调灰度值）
			pPixel[0] = static_cast<unsigned char>(hueGrayscale);
			pPixel[1] = static_cast<unsigned char>(hueGrayscale);
			pPixel[2] = static_cast<unsigned char>(hueGrayscale);
		}
	}
}

void CDib::CalculateSaturation() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 假设图像是 24 位 RGB 图像，计算每行字节数
	int nWidthBytes = ((nWidth * 3 + 3) / 4) * 4; // 对 4 字节边界对齐

	// 遍历每个像素，计算饱和度并写入原图
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			// 获取当前像素的指针
			unsigned char* pPixel = m_pDibBits + y * nWidthBytes + x * 3;
			unsigned char B = pPixel[0];
			unsigned char G = pPixel[1];
			unsigned char R = pPixel[2];

			// 将 RGB 值转换为浮点数
			double r = R;
			double g = G;
			double b = B;

			// 计算最小值
			double minVal = min(r, min(g, b));

			// 计算饱和度，避免除以零
			double saturation = 0.0;
			if (r + g + b > 0) {
				saturation = 1.0 - (3.0 / (r + g + b) * minVal);
			}
			else {
				saturation = 0.0; // 处理 R + G + B = 0 的情况
			}

			// 将饱和度映射到 0-255 范围
			unsigned char sat = static_cast<unsigned char>(saturation * 255.0);

			// 将饱和度值写入原图的 RGB 三个通道，形成灰度效果
			pPixel[0] = sat;
			pPixel[1] = sat;
			pPixel[2] = sat;
		}
	}
}

void CDib::CalculateIntensity() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int nWidthBytes = ((nWidth * 3 + 3) / 4) * 4; // 对 4 字节边界对齐

	// 遍历每个像素，计算亮度值并写入原图
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			// 获取当前像素的指针
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			unsigned char B = pPixel[0];
			unsigned char G = pPixel[1];
			unsigned char R = pPixel[2];

			// 计算亮度值（使用标准的亮度计算公式）
			unsigned char brightness = static_cast<unsigned char>((R + G + B)/3);

			// 将亮度值写入原图的 RGB 三个通道，形成灰度效果
			pPixel[0] = brightness;
			pPixel[1] = brightness;
			pPixel[2] = brightness;
		}
	}
}

void CDib::RGB2HSI() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 遍历每个像素，计算HSI并写入原图
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			// 获取当前像素的指针
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			unsigned char B = pPixel[0];
			unsigned char G = pPixel[1];
			unsigned char R = pPixel[2];

			// 将 RGB 值转换为 0-1 范围
			double r = R / 255.0;
			double g = G / 255.0;
			double b = B / 255.0;

			// 计算强度 I
			double I = (r + g + b) / 3.0;

			// 计算饱和度 S
			double S = 0.0;
			double minVal = min(r, min(g, b));
			if (I != 0) {
				S = 1.0 - (minVal / I);
			}

			// 计算色调 H
			double maxVal = max(r, max(g, b));
			double delta = maxVal - minVal;

			double H = 0;
			if (delta != 0) {
				if (maxVal == r) {
					H = fmod((g - b) / delta, 6.0);
				}
				else if (maxVal == g) {
					H = (b - r) / delta + 2;
				}
				else {
					H = (r - g) / delta + 4;
				}
				H *= 60;
				if (H < 0) H += 360;
			}

			// 将HSI分量映射到0-255范围
			unsigned char HValue = static_cast<unsigned char>((H / 360.0) * 255.0);
			unsigned char SValue = static_cast<unsigned char>(S * 255.0);
			unsigned char IValue = static_cast<unsigned char>(I * 255.0);

			// 将HSI分量写入原图的RGB通道
			pPixel[0] = HValue; // 色调映射到蓝色通道
			pPixel[1] = SValue; // 饱和度映射到绿色通道
			pPixel[2] = IValue; // 强度映射到红色通道
		}
	}
}

void CDib::HSI2RGB() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 遍历每个像素，计算RGB并写入原图
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			// 获取当前像素的指针
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			unsigned char HValue = pPixel[0]; // 色调在蓝色通道
			unsigned char SValue = pPixel[1]; // 饱和度在绿色通道
			unsigned char IValue = pPixel[2]; // 强度在红色通道

			// 将HSI值转换为计算范围
			double H = HValue / 255.0 * 360.0; // 0-360度
			double S = SValue / 255.0;         // 0-1
			double I = IValue / 255.0;         // 0-1

			double R = 0, G = 0, B = 0;

			if (H >= 0 && H < 120) {
				B = I * (1 - S);
				R = I * (1 + (S * cos(H * Pi / 180.0)) / cos((60.0 - H) * Pi / 180.0));
				G = 3 * I - (R + B);
			}
			else if (H >= 120 && H < 240) {
				H -= 120;
				R = I * (1 - S);
				G = I * (1 + (S * cos(H * Pi / 180.0)) / cos((60.0 - H) * Pi / 180.0));
				B = 3 * I - (R + G);
			}
			else if (H >= 240 && H < 360) {
				H -= 240;
				G = I * (1 - S);
				B = I * (1 + (S * cos(H * Pi / 180.0)) / cos((60.0 - H) * Pi / 180.0));
				R = 3 * I - (G + B);
			}

			// 确保RGB值在有效范围内
			R = max(0.0, min(1.0, R));
			G = max(0.0, min(1.0, G));
			B = max(0.0, min(1.0, B));

			// 将RGB值映射到0-255范围并写入原图
			pPixel[0] = static_cast<unsigned char>(B * 255.0);
			pPixel[1] = static_cast<unsigned char>(G * 255.0);
			pPixel[2] = static_cast<unsigned char>(R * 255.0);
		}
	}
}

void CDib::HSIHistogramEqualization() {
	if (IsNull()) return;

	// Convert RGB to HSI
	RGB2HSI();

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// Extract the intensity component (I)
	std::vector<unsigned char> intensityValues;
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			intensityValues.push_back(pPixel[2]); // Intensity is stored in the red channel
		}
	}

	// Calculate the histogram
	std::vector<int> histogram(256, 0);
	for (unsigned char val : intensityValues) {
		histogram[val]++;
	}

	// Calculate the cumulative distribution function (CDF)
	std::vector<float> cdf(256, 0.0f);
	int totalPixels = nWidth * nHeight;
	cdf[0] = histogram[0] / static_cast<float>(totalPixels);
	for (int i = 1; i < 256; i++) {
		cdf[i] = cdf[i - 1] + histogram[i] / static_cast<float>(totalPixels);
	}

	// Map the intensity values
	std::vector<unsigned char> mappedIntensity(nWidth * nHeight);
	for (size_t i = 0; i < intensityValues.size(); i++) {
		unsigned char oldVal = intensityValues[i];
		unsigned char newVal = static_cast<unsigned char>(cdf[oldVal] * 255.0f);
		mappedIntensity[i] = newVal;
	}

	// Update the intensity component
	size_t index = 0;
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			pPixel[2] = mappedIntensity[index++]; // Update the intensity value
		}
	}

	// 增强饱和度（S）分量
	float enhancementFactor = 1.3; // 饱和度增强因子，可以根据需要调整
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			float H = static_cast<float>(pPixel[0]) / 255.0f; // 色调
			float S = static_cast<float>(pPixel[1]) / 255.0f; // 饱和度
			float I = static_cast<float>(pPixel[2]) / 255.0f; // 强度

			// 对饱和度进行增强处理
			S = min(S * enhancementFactor, 1.0f);

			// 更新饱和度值
			pPixel[1] = static_cast<unsigned char>(S * 255.0f);
		}
	}

	// Convert HSI back to RGB
	HSI2RGB();
}

void CDib::ApplyPseudoColor(COLORREF startColor, COLORREF endColor)
{
	if (IsNull()) return;

	if (m_nBitCount == 8)
	{
		// 处理8位调色板图像
		int nColors = GetMaxColorTableEntries();
		if (nColors <= 0) return;

		RGBQUAD* palette = new RGBQUAD[nColors];
		GetColorTable(0, nColors, palette);

		for (int i = 0; i < nColors; ++i)
		{
			// 计算颜色渐变比例
			float ratio = (nColors > 1) ? static_cast<float>(i) / (nColors - 1) : 0.5f;

			palette[i].rgbRed = static_cast<BYTE>(GetRValue(startColor) +
				(GetRValue(endColor) - GetRValue(startColor)) * ratio);
			palette[i].rgbGreen = static_cast<BYTE>(GetGValue(startColor) +
				(GetGValue(endColor) - GetGValue(startColor)) * ratio);
			palette[i].rgbBlue = static_cast<BYTE>(GetBValue(startColor) +
				(GetBValue(endColor) - GetBValue(startColor)) * ratio);
		}

		SetColorTable(0, nColors, palette);
		delete[] palette;
	}
	else if (m_nBitCount == 24)
	{
		// 处理24位真彩色图像
		int width = GetWidth();
		int height = GetHeight();

		// 创建颜色查找表
		std::vector<BYTE> redLut(256);
		std::vector<BYTE> greenLut(256);
		std::vector<BYTE> blueLut(256);

		for (int i = 0; i < 256; ++i)
		{
			redLut[i] = static_cast<BYTE>(GetRValue(startColor) +
				(GetRValue(endColor) - GetRValue(startColor)) * i / 255);
			greenLut[i] = static_cast<BYTE>(GetGValue(startColor) +
				(GetGValue(endColor) - GetGValue(startColor)) * i / 255);
			blueLut[i] = static_cast<BYTE>(GetBValue(startColor) +
				(GetBValue(endColor) - GetBValue(startColor)) * i / 255);
		}

		// 遍历所有像素
		for (int y = 0; y < height; ++y)
		{
			BYTE* pRow = m_pDibBits + y * m_nWidthBytes;
			for (int x = 0; x < width; ++x)
			{
				// 计算灰度值（使用标准亮度公式）
				BYTE* pPixel = &pRow[x * 3];
				BYTE gray = static_cast<BYTE>(
					0.299 * pPixel[2] +  // 红色分量
					0.587 * pPixel[1] +  // 绿色分量
					0.114 * pPixel[0]);   // 蓝色分量

				// 应用伪彩色映射（BGR存储格式）
				pPixel[0] = blueLut[gray];  // 蓝色通道
				pPixel[1] = greenLut[gray]; // 绿色通道
				pPixel[2] = redLut[gray];   // 红色通道
			}
		}
	}
	// 其他位深度不支持
}

void CDib::ConvertToBinary(double threshold) {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 遍历每个像素，计算RGB并写入原图
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			// 获取当前像素的指针
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			unsigned char B = pPixel[0]; 
			unsigned char G = pPixel[1]; 
			unsigned char R = pPixel[2]; 

			// 求解I值
			double I = (R + G + B) / 3;
			if (I > threshold) {
				pPixel[0] = 255; // 蓝色通道
				pPixel[1] = 255; // 绿色通道
				pPixel[2] = 255; // 红色通道
			}
			else
			{
				pPixel[0] = 0;   // 蓝色通道
				pPixel[1] = 0;   // 绿色通道
				pPixel[2] = 0;   // 红色通道
			}
		}
	}
}

void CDib::Erosion(int kernelSize) {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 检查内核大小是否合理，必须是正奇数
	if (kernelSize <= 0 || kernelSize % 2 == 0) {
		kernelSize = 3; // 默认使用3x3内核
	}
	int radius = (kernelSize - 1) / 2;

	// 创建临时缓冲区存储处理结果
	unsigned char* tempBuffer = new unsigned char[m_nWidthBytes * nHeight];
	if (!tempBuffer) return; // 内存分配失败处理

	// 遍历图像的每个像素
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			bool isWhite = true; // 假设当前像素区域全白

			// 遍历结构元素内的所有像素
			for (int i = -radius; i <= radius; i++) {
				for (int j = -radius; j <= radius; j++) {
					int nx = x + i;
					int ny = y + j;

					// 检查是否超出图像边界
					if (nx < 0 || nx >= nWidth || ny < 0 || ny >= nHeight) {
						isWhite = false; // 边界外视为黑色
						break;
					}

					// 获取相邻像素的指针
					unsigned char* pCheckPixel = m_pDibBits + ny * m_nWidthBytes + nx * 3;
					if (pCheckPixel[2] == 0) { // 检查R通道是否为黑色
						isWhite = false;
						break;
					}
				}
				if (!isWhite) break;
			}

			// 将结果写入临时缓冲区
			unsigned char* pTempPixel = tempBuffer + y * m_nWidthBytes + x * 3;
			pTempPixel[0] = isWhite ? 255 : 0; // B通道
			pTempPixel[1] = isWhite ? 255 : 0; // G通道
			pTempPixel[2] = isWhite ? 255 : 0; // R通道
		}
	}

	// 将处理结果复制回原图像
	memcpy(m_pDibBits, tempBuffer, m_nWidthBytes * nHeight);

	// 释放临时缓冲区
	delete[] tempBuffer;
}

void CDib::Dilation(int kernelSize) {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	// 检查内核大小是否合理，必须是正奇数
	if (kernelSize <= 0 || kernelSize % 2 == 0) {
		kernelSize = 3; // 默认使用3x3内核
	}
	int radius = (kernelSize - 1) / 2;

	// 创建临时缓冲区存储处理结果
	unsigned char* tempBuffer = new unsigned char[m_nWidthBytes * nHeight];
	if (!tempBuffer) return; // 内存分配失败处理

	// 遍历图像的每个像素
	for (int y = 0; y < nHeight; y++) {
		for (int x = 0; x < nWidth; x++) {
			bool foundWhite = false; // 默认当前像素区域全黑

			// 遍历结构元素内的所有像素
			for (int i = -radius; i <= radius; i++) {
				for (int j = -radius; j <= radius; j++) {
					int nx = x + i;
					int ny = y + j;

					// 检查是否超出图像边界
					if (nx < 0 || nx >= nWidth || ny < 0 || ny >= nHeight) {
						continue; // 跳过边界外像素
					}

					// 获取相邻像素的指针
					unsigned char* pCheckPixel = m_pDibBits + ny * m_nWidthBytes + nx * 3;
					if (pCheckPixel[2] == 255) { // 检查R通道是否为白色
						foundWhite = true;
						break;
					}
				}
				if (foundWhite) break;
			}

			// 将结果写入临时缓冲区
			unsigned char* pTempPixel = tempBuffer + y * m_nWidthBytes + x * 3;
			pTempPixel[0] = foundWhite ? 255 : 0; // B通道
			pTempPixel[1] = foundWhite ? 255 : 0; // G通道
			pTempPixel[2] = foundWhite ? 255 : 0; // R通道
		}
	}

	// 将处理结果复制回原图像
	memcpy(m_pDibBits, tempBuffer, m_nWidthBytes * nHeight);

	// 释放临时缓冲区
	delete[] tempBuffer;
}

void CDib::HoleFill()
{
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();

	int imageSize = m_nWidthBytes * nHeight;

	// Step 1: 提取灰度通道（只取 Red 分量）
	unsigned char* gray = new unsigned char[nWidth * nHeight];
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			int idx = y * nWidth + x;
			gray[idx] = pPixel[2]; // 取 R 分量（黑白图中RGB相等）
		}
	}

	// Step 2: 创建反图
	unsigned char* inverted = new unsigned char[nWidth * nHeight];
	for (int i = 0; i < nWidth * nHeight; ++i)
		inverted[i] = 255 - gray[i];

	// Step 3: 初始化 filled 图像（边缘为 inverted 的白）
	unsigned char* filled = new unsigned char[nWidth * nHeight];
	memset(filled, 0, nWidth * nHeight);

	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			if (x == 0 || x == nWidth - 1 || y == 0 || y == nHeight - 1) {
				int idx = y * nWidth + x;
				if (inverted[idx] == 255)
					filled[idx] = 255;
			}
		}
	}

	// Step 4: 迭代膨胀 + 掩模限制
	bool changed = true;
	const int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };
	const int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };

	while (changed) {
		changed = false;
		unsigned char* temp = new unsigned char[nWidth * nHeight];
		memcpy(temp, filled, nWidth * nHeight);

		for (int y = 1; y < nHeight - 1; ++y) {
			for (int x = 1; x < nWidth - 1; ++x) {
				int idx = y * nWidth + x;
				if (filled[idx] == 255)
					continue;

				if (inverted[idx] != 255)
					continue;

				for (int k = 0; k < 8; ++k) {
					int nx = x + dx[k];
					int ny = y + dy[k];
					int nIdx = ny * nWidth + nx;

					if (filled[nIdx] == 255) {
						temp[idx] = 255;
						changed = true;
						break;
					}
				}
			}
		}

		memcpy(filled, temp, nWidth * nHeight);
		delete[] temp;
	}

	// Step 5: 求反，得到最终图像
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			int idx = y * nWidth + x;
			unsigned char val = 255 - filled[idx];

			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			pPixel[0] = pPixel[1] = pPixel[2] = val;
		}
	}

	// 清理
	delete[] gray;
	delete[] inverted;
	delete[] filled;
}

void CDib::Edge() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int imageSize = m_nWidthBytes * nHeight;

	// 创建两个临时图像用于保存腐蚀结果
	unsigned char* originalBuffer = new unsigned char[imageSize];
	unsigned char* erodedBuffer = new unsigned char[imageSize];

	if (!originalBuffer || !erodedBuffer) {
		delete[] originalBuffer;
		delete[] erodedBuffer;
		return;
	}

	// 备份原图数据
	memcpy(originalBuffer, m_pDibBits, imageSize);

	// 执行两次腐蚀
	Erosion(3);  // 第一次腐蚀：作用于 m_pDibBits
	memcpy(erodedBuffer, m_pDibBits, imageSize);  // 保存第一次腐蚀的结果
	Erosion(3);  // 第二次腐蚀：再次作用于 m_pDibBits

	// Edge = 原图 - 二次腐蚀后的图
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			unsigned char* pOrig = originalBuffer + y * m_nWidthBytes + x * 3;
			unsigned char* pEroded = m_pDibBits + y * m_nWidthBytes + x * 3;

			for (int c = 0; c < 3; ++c) {
				int diff = pOrig[c] - pEroded[c];
				pOrig[c] = (unsigned char)(diff < 0 ? 0 : diff); // 防止负值
			}
		}
	}

	// 将边缘结果写回图像
	memcpy(m_pDibBits, originalBuffer, imageSize);

	// 清理
	delete[] originalBuffer;
	delete[] erodedBuffer;
}

void CDib::Outline() {
	if (IsNull()) return;

	int nWidth = GetWidth();
	int nHeight = GetHeight();
	int imageSize = m_nWidthBytes * nHeight;

	// Step 1: 二值化，求补图像
	unsigned char* colorBackup = new unsigned char[imageSize];
	memcpy(colorBackup, m_pDibBits, imageSize);

	// 转换为二值图并求反（假设已有相关函数）
	ConvertToBinary(218);  // 这里是阈值二值化
	Invert();  // 翻转二值图像（反转黑白）

	// Step 2: 进行两次腐蚀操作
	// 第一次腐蚀
	Erosion(3);  
	// 第二次腐蚀
	Erosion(3);

	// step 3：填洞
	//提取灰度通道（只取 Red 分量）
	unsigned char* gray = new unsigned char[nWidth * nHeight];
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			int idx = y * nWidth + x;
			gray[idx] = pPixel[2]; // 取 R 分量（黑白图中RGB相等）
		}
	}

	// 创建反图
	unsigned char* inverted = new unsigned char[nWidth * nHeight];
	for (int i = 0; i < nWidth * nHeight; ++i)
		inverted[i] = 255 - gray[i];

	// 初始化 filled 图像（边缘为 inverted 的白）
	unsigned char* filled = new unsigned char[nWidth * nHeight];
	memset(filled, 0, nWidth * nHeight);

	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			if (x == 0 || x == nWidth - 1 || y == 0 || y == nHeight - 1) {
				int idx = y * nWidth + x;
				if (inverted[idx] == 255)
					filled[idx] = 255;
			}
		}
	}

	//迭代膨胀 + 掩模限制
	bool changed = true;
	const int dx[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };
	const int dy[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };

	while (changed) {
		changed = false;
		unsigned char* temp = new unsigned char[nWidth * nHeight];
		memcpy(temp, filled, nWidth * nHeight);

		for (int y = 1; y < nHeight - 1; ++y) {
			for (int x = 1; x < nWidth - 1; ++x) {
				int idx = y * nWidth + x;
				if (filled[idx] == 255)
					continue;

				if (inverted[idx] != 255)
					continue;

				for (int k = 0; k < 8; ++k) {
					int nx = x + dx[k];
					int ny = y + dy[k];
					int nIdx = ny * nWidth + nx;

					if (filled[nIdx] == 255) {
						temp[idx] = 255;
						changed = true;
						break;
					}
				}
			}
		}

		memcpy(filled, temp, nWidth * nHeight);
		delete[] temp;
	}

	// 求反，回到填洞图像
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			int idx = y * nWidth + x;
			unsigned char val = 255 - filled[idx];

			unsigned char* pPixel = m_pDibBits + y * m_nWidthBytes + x * 3;
			pPixel[0] = pPixel[1] = pPixel[2] = val;
		}
	}

	// 清理
	delete[] gray;
	delete[] inverted;
	delete[] filled;

	// Step 4: 提取边缘：第一步，腐蚀+膨胀+腐蚀 并保存结果
	unsigned char* erosionBackup = new unsigned char[imageSize];
	memcpy(erosionBackup, m_pDibBits, imageSize);  // 先保存初始图像

	// 腐蚀一次，去掉图像左上角的一个白点
	Erosion(3);

	// 回头膨胀，防止腐蚀的边缘不对，边缘在里面
	Dilation(9);

	// 第一次腐蚀
	Erosion(3);
	// 将第一次腐蚀的结果保存到 erosionBackup
	memcpy(erosionBackup, m_pDibBits, imageSize);
	// 第二次腐蚀
	Erosion(3);

	// Step 4: 提取边缘（通过计算原图与二次腐蚀图的差异）
	for (int y = 0; y < nHeight; ++y) {
		for (int x = 0; x < nWidth; ++x) {
			unsigned char* pOrig = erosionBackup + y * m_nWidthBytes + x * 3;
			unsigned char* pEroded = m_pDibBits + y * m_nWidthBytes + x * 3;

			// 计算差异值
			bool isEdge = false;
			for (int c = 0; c < 3; ++c) {
				// 只要有差异，就认为是边缘
				if (pOrig[c] != pEroded[c]) {
					isEdge = true;
					break;
				}
			}

			// 如果是边缘，修改原图为绿色边缘
			if (isEdge) {
				unsigned char* pColor = colorBackup + y * m_nWidthBytes + x * 3;
				pColor[0] = 0;   // 蓝色通道
				pColor[1] = 255; // 绿色通道
				pColor[2] = 0;   // 红色通道
			}
		}
	}

	// Step 4: 将带边缘的彩色图复制回原图
	memcpy(m_pDibBits, colorBackup, imageSize);

	// Step 5: 清理临时缓冲区
	delete[] colorBackup;
	delete[] erosionBackup;
}

//openCV库实现彩色二值化
void CDib::BinaryColor_cv(double thresh)
{
	// 仅支持24位彩色图像
	if (m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持24位彩色图像的二值化！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;

	// 构造OpenCV的Mat（注意DIB数据是BGR格式，且每行可能有填充）
	cv::Mat src(height, width, CV_8UC3);
	for (int y = 0; y < height; ++y) {
		memcpy(src.ptr(y), m_pDibBits + y * stride, width * 3);
	}

	// 转灰度
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

	// 二值化
	cv::Mat binary;
	cv::threshold(gray, binary, thresh, 255, cv::THRESH_BINARY);

	// 用二值结果替换原图（变为8位灰度图）
	Destroy();
	Create(width, height, 8, 0);
	m_nWidth = width;
	m_nHeight = height;
	m_nWidthBytes = abs(GetPitch());
	m_nBitCount = 8;
	m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1) * GetPitch();

	// 设置灰度调色板
	if (IsIndexed()) {
		int nColors = 256;
		RGBQUAD* pal = new RGBQUAD[nColors];
		for (int i = 0; i < nColors; i++) {
			pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)i;
			pal[i].rgbReserved = 0;
		}
		SetColorTable(0, nColors, pal);
		delete[] pal;
	}

	// 拷贝二值化数据到DIB
	for (int y = 0; y < height; ++y) {
		memcpy(m_pDibBits + y * m_nWidthBytes, binary.ptr(y), width);
	}
}

void CDib::Erosion_cv(int kernelSize)
{
	if (m_nBitCount != 8 && m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持8位灰度或24位彩色图像的腐蚀！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;
	int ksize = (kernelSize > 0 && kernelSize % 2 == 1) ? kernelSize : 3;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));

	if (m_nBitCount == 8) {
		// 8位灰度图
		cv::Mat src(height, width, CV_8UC1);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width);
		}
		cv::Mat dst;
		cv::erode(src, dst, kernel);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width);
		}
	}
	else {
		// 24位彩色图
		cv::Mat src(height, width, CV_8UC3);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width * 3);
		}
		cv::Mat dst;
		cv::erode(src, dst, kernel);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width * 3);
		}
	}
}

void CDib::Dilation_cv(int kernelSize)
{

	if (m_nBitCount != 8 && m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持8位灰度或24位彩色图像的膨胀！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;
	int ksize = (kernelSize > 0 && kernelSize % 2 == 1) ? kernelSize : 3;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));

	if (m_nBitCount == 8) {
		// 8位灰度图
		cv::Mat src(height, width, CV_8UC1);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width);
		}
		cv::Mat dst;
		cv::dilate(src, dst, kernel);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width);
		}
	}
	else {
		// 24位彩色图
		cv::Mat src(height, width, CV_8UC3);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width * 3);
		}
		cv::Mat dst;
		cv::dilate(src, dst, kernel);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width * 3);
		}
	}
}

void CDib::Invert_cv()
{
	if (m_nBitCount != 8 && m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持8位灰度或24位彩色图像的求补！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;

	if (m_nBitCount == 8) {
		// 8位灰度图
		cv::Mat src(height, width, CV_8UC1);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width);
		}
		cv::Mat dst;
		cv::bitwise_not(src, dst);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width);
		}
	}
	else {
		// 24位彩色图
		cv::Mat src(height, width, CV_8UC3);
		for (int y = 0; y < height; ++y) {
			memcpy(src.ptr(y), m_pDibBits + y * stride, width * 3);
		}
		cv::Mat dst;
		cv::bitwise_not(src, dst);
		for (int y = 0; y < height; ++y) {
			memcpy(m_pDibBits + y * stride, dst.ptr(y), width * 3);
		}
	}
}

void CDib::HoleFill_cv()
{
	if (m_nBitCount != 8 && m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持8位灰度或24位彩色图像的自动填洞！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;

	// Step 1: 获取二值图像
	cv::Mat binImg;
	if (m_nBitCount == 8) {
		binImg = cv::Mat(height, width, CV_8UC1);
		for (int y = 0; y < height; ++y)
			memcpy(binImg.ptr(y), m_pDibBits + y * stride, width);
	}
	else {
		// 彩色图转灰度
		cv::Mat color(height, width, CV_8UC3);
		for (int y = 0; y < height; ++y)
			memcpy(color.ptr(y), m_pDibBits + y * stride, width * 3);
		cv::cvtColor(color, binImg, cv::COLOR_BGR2GRAY);
	}

	// Step 2: 取反
	cv::Mat invImg;
	cv::bitwise_not(binImg, invImg);

	// Step 3: floodFill 边界
	cv::Mat mask = cv::Mat::zeros(height + 2, width + 2, CV_8UC1);
	for (int x = 0; x < width; ++x) {
		if (invImg.at<uchar>(0, x) == 255)
			cv::floodFill(invImg, mask, cv::Point(x, 0), 128, 0, 0, 0, 4 | cv::FLOODFILL_MASK_ONLY | (255 << 8));
		if (invImg.at<uchar>(height - 1, x) == 255)
			cv::floodFill(invImg, mask, cv::Point(x, height - 1), 128, 0, 0, 0, 4 | cv::FLOODFILL_MASK_ONLY | (255 << 8));
	}
	for (int y = 0; y < height; ++y) {
		if (invImg.at<uchar>(y, 0) == 255)
			cv::floodFill(invImg, mask, cv::Point(0, y), 128, 0, 0, 0, 4 | cv::FLOODFILL_MASK_ONLY | (255 << 8));
		if (invImg.at<uchar>(y, width - 1) == 255)
			cv::floodFill(invImg, mask, cv::Point(width - 1, y), 128, 0, 0, 0, 4 | cv::FLOODFILL_MASK_ONLY | (255 << 8));
	}

	// Step 4: mask 里为0的地方就是“洞”
	cv::Mat holes = (mask(cv::Rect(1, 1, width, height)) == 0) & (binImg == 0);

	// Step 5: 填洞
	if (m_nBitCount == 8) {
		for (int y = 0; y < height; ++y) {
			uchar* pDst = m_pDibBits + y * stride;
			const uchar* pHole = holes.ptr(y);
			for (int x = 0; x < width; ++x) {
				if (pHole[x])
					pDst[x] = 255;
			}
		}
	}
	else {
		for (int y = 0; y < height; ++y) {
			uchar* pDst = m_pDibBits + y * stride;
			const uchar* pHole = holes.ptr(y);
			for (int x = 0; x < width; ++x) {
				if (pHole[x]) {
					pDst[x * 3 + 0] = 255;
					pDst[x * 3 + 1] = 255;
					pDst[x * 3 + 2] = 255;
				}
			}
		}
	}
}

void CDib::EdgeByErosion_cv(int kernelSize)
{
	if (m_nBitCount != 8 && m_nBitCount != 24) {
		AfxMessageBox(_T("仅支持8位灰度或24位彩色图像的边缘展现！"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;
	int ksize = (kernelSize > 0 && kernelSize % 2 == 1) ? kernelSize : 3;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));

	if (m_nBitCount == 8) {
		cv::Mat src(height, width, CV_8UC1);
		for (int y = 0; y < height; ++y)
			memcpy(src.ptr(y), m_pDibBits + y * stride, width);

		cv::Mat eroded;
		cv::erode(src, eroded, kernel, cv::Point(-1, -1), 2); // 腐蚀两次

		cv::Mat edge;
		cv::subtract(src, eroded, edge);

		for (int y = 0; y < height; ++y)
			memcpy(m_pDibBits + y * stride, edge.ptr(y), width);
	}
	else {
		cv::Mat src(height, width, CV_8UC3);
		for (int y = 0; y < height; ++y)
			memcpy(src.ptr(y), m_pDibBits + y * stride, width * 3);

		cv::Mat eroded;
		cv::erode(src, eroded, kernel, cv::Point(-1, -1), 2); // 腐蚀两次

		cv::Mat edge;
		cv::subtract(src, eroded, edge);

		for (int y = 0; y < height; ++y)
			memcpy(m_pDibBits + y * stride, edge.ptr(y), width * 3);
	}
}

void CDib::outline_cv()
{
	if (m_nBitCount != 24) {
		AfxMessageBox(_T("只支持24位彩色图像"));
		return;
	}

	int width = m_nWidth;
	int height = m_nHeight;
	int stride = m_nWidthBytes;

	// Step 1: 克隆原始图像数据
	cv::Mat original_color(height, width, CV_8UC3);
	for (int y = 0; y < height; ++y)
		memcpy(original_color.ptr(y), m_pDibBits + y * stride, width * 3);

	// Step 2: 灰度转换并二值化
	cv::Mat gray, binary;
	cv::cvtColor(original_color, gray, cv::COLOR_BGR2GRAY);
	cv::threshold(gray, binary, 218, 255, cv::THRESH_BINARY);

	// Step 3: 改进填洞处理
	cv::Mat filled = binary.clone();

	// 创建一个略大于图像的掩码
	cv::Mat flood_mask = cv::Mat::zeros(height + 2, width + 2, CV_8U);

	// 从图像的角点开始填充背景，确保整个背景被正确填充
	for (int y = 0; y < height; y += height - 1) {
		for (int x = 0; x < width; x += width - 1) {
			if (binary.at<uchar>(y, x) == 0) {  // 如果是背景点（假设背景为黑色）
				cv::floodFill(binary, flood_mask, cv::Point(x, y), 255);
			}
		}
	}

	// Step 4: 形态学操作提取边缘
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::Mat eroded;
	cv::erode(filled, eroded, kernel, cv::Point(-1, -1), 2);  // 腐蚀两次

	cv::Mat edge;
	cv::absdiff(filled, eroded, edge);  // 获取腐蚀差异边缘
	cv::threshold(edge, edge, 1, 255, cv::THRESH_BINARY);

	// Step 5: 在原图上绘制蓝色边缘
	for (int y = 0; y < height; ++y) {
		uchar* pColor = original_color.ptr<uchar>(y);
		const uchar* pEdge = edge.ptr<uchar>(y);
		for (int x = 0; x < width; ++x) {
			if (pEdge[x]) {
				// 只修改蓝色通道，保持其他通道原样
				pColor[x * 3] = 255;    // B通道
				pColor[x * 3 + 1] = 0;  // G通道保持不变
				pColor[x * 3 + 2] = 0;  // R通道保持不变
			}
		}
	}

	// Step 6: 回写处理后的数据
	for (int y = 0; y < height; ++y)
		memcpy(m_pDibBits + y * stride, original_color.ptr(y), width * 3);
}
