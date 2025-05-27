#pragma once
#include "atlimage.h"

#include <complex>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include "pch.h"
#include <sstream>
#include <cmath>
#include <future> 
#include <algorithm>
#include <vector>

using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = {1,1,1,1,1,1,1,1,1};
const int sharpen_Kernel[9] = { 0,-1,0,-1,5,-1,0,-1,0 };
const int Sobel_X_Kernel[9] = { -1,0,1,-2,0,2,-1,0,1 };
const int Sobel_Y_Kernel[9] = { 1,2,1,0,0,0,-1,-2,-1 };
const int Laplace_Kernel[9] = { 0,-1,0,-1,4,-1,0,-1,0 };

#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);

public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};

public:
	void LoadFile(LPCTSTR lpszPathName);

public:
	void Invert();

public:
	long* GrayValueCount();
	int GetBitCount() const;
	void GenerateRedImage();
	void GenerateGreenImage();
	void GenerateBlueImage();
	void LinearTransformGrayScale();
	void Bit_Plane_Slicing_up(int k);
	void HistogramEqualization();
	void Smooth();
	void LaplaceTransform();
	void LaplaceEnhance();
	void GaussianSmooth(int sigma);
	void CLAHE(int clipLimit, int gridX, int gridY);
	void SobelTransform(int type = 0);
	void GaussianSmoothPart(int startY, int endY, const vector<float>& kernel, int kernelRadius, unsigned char* tempBuffer);

	//频率域的滤波：
	// 变换函数声明
	void FFT(complex<double >* pComplex, int length);
	void IFFT(complex<double >* pComplex, int length);
	void FFT2D();
	void IFFT2D();

	//新建图像
	void CreateWhiteSquare(int width, int height, int pixel);
	void CreateTestPage(int width, int height);

   //滤波实现
	void LowFilter(CString FilterType, int D0, bool zeropadding);
	void HighFilter(CString FilterType, int D0, bool zeropadding);

	//滤波函数实现
	void IdealLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void IdealHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void ButterworthLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void ButterworthHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void GaussianLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void GaussianHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);

	//几何变换函数实现
	void Rotate(double angle);
	void ScaleImage(double scaleFactor);
	void CreateGrayDib(int width, int height, unsigned char pImage[]);

	//图像恢复与重建函数
	void AddGaussianNoise(double stddev);//添加高斯噪声
	void AddSaltPapperNoise(double ratio);//添加椒盐噪声
	void AdaptiveMedianFilter(int maxWindowSize);	//自适应中值滤波
	void AdaptiveGaussianDenoise(int windowSize, float estimatedNoiseVar);//自适应高斯去噪
	void TurbulenceBlur(float k); //大气湍流模糊
	void MotionBlur(float thetaDeg, float speed);//运动模糊
	void AddPeriodicNoise(double amplitude, double freqX, double freqY);//添加周期噪声
	void RemovePeriodicNoise(double freqX, double freqY, double D0);//频率陷波器去除周期噪声
	void InverseFilter(float thetaDeg, float speed);//逆滤波，要与维纳滤波器做对比
	void WienerFilter(float thetaDeg, float speed, float K);//维纳滤波器
	void CSLF(float thetaDeg, float speed, double gamma);//最小约束二乘滤波器
	void WienerFilterTur(float k, float K);//对湍流退化进行维纳滤波处理

	//彩色图像处理
	void CreateRGBTestPage(int width, int height); //创建RGB测试图像
	void CalculateHue();//计算色调
	void CalculateSaturation();//计算饱和度
	void CalculateIntensity();//计算亮度
	void RGB2HSI();//RGB转HSI
	void HSI2RGB();//HSI转RGB
	void HSIHistogramEqualization();//彩色直方图均衡
	void ApplyPseudoColor(COLORREF startColor, COLORREF endColor);//设置伪彩图像

	//形态学变换
	void ConvertToBinary(double threshold); //二值化
	void Erosion(int kernelSize); //腐蚀
	void Dilation(int kernelSize); //膨胀
	void HoleFill(); //孔洞填充
	void Edge();//边缘检测
	void Outline();//轮廓检测

	//opencv库实现形态学变换函数
	void BinaryColor_cv(double thresh);//二值化
	void Erosion_cv(int KernelSize);//腐蚀
	void Dilation_cv(int kernelSize);//膨胀
	void Invert_cv();//求补
	void HoleFill_cv();//自动填洞算法
	void EdgeByErosion_cv(int kernelSize);//腐蚀推进的边缘检测
	void outline_cv();//描边

public:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char* m_pDibBits;

public:
	// 归一化幅值和相位，用于响应鼠标事件
	vector<vector<unsigned char>> normMagnitudeMap;  // 归一化幅值 (0~255)
	vector<vector<unsigned char>> normPhaseMap;      // 归一化相位 (0~255)
	
private:
	long *m_pGrayValueCount;
	vector<float> GenerateGaussianKernel(int sigma);
};
