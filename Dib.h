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

	//Ƶ������˲���
	// �任��������
	void FFT(complex<double >* pComplex, int length);
	void IFFT(complex<double >* pComplex, int length);
	void FFT2D();
	void IFFT2D();

	//�½�ͼ��
	void CreateWhiteSquare(int width, int height, int pixel);
	void CreateTestPage(int width, int height);

   //�˲�ʵ��
	void LowFilter(CString FilterType, int D0, bool zeropadding);
	void HighFilter(CString FilterType, int D0, bool zeropadding);

	//�˲�����ʵ��
	void IdealLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void IdealHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void ButterworthLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void ButterworthHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void GaussianLowPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);
	void GaussianHighPassFilter(complex<double>*& pComplexImage, int nWidth, int nHeight, double D0);

	//���α任����ʵ��
	void Rotate(double angle);
	void ScaleImage(double scaleFactor);
	void CreateGrayDib(int width, int height, unsigned char pImage[]);

	//ͼ��ָ����ؽ�����
	void AddGaussianNoise(double stddev);//��Ӹ�˹����
	void AddSaltPapperNoise(double ratio);//��ӽ�������
	void AdaptiveMedianFilter(int maxWindowSize);	//����Ӧ��ֵ�˲�
	void AdaptiveGaussianDenoise(int windowSize, float estimatedNoiseVar);//����Ӧ��˹ȥ��
	void TurbulenceBlur(float k); //��������ģ��
	void MotionBlur(float thetaDeg, float speed);//�˶�ģ��
	void AddPeriodicNoise(double amplitude, double freqX, double freqY);//�����������
	void RemovePeriodicNoise(double freqX, double freqY, double D0);//Ƶ���ݲ���ȥ����������
	void InverseFilter(float thetaDeg, float speed);//���˲���Ҫ��ά���˲������Ա�
	void WienerFilter(float thetaDeg, float speed, float K);//ά���˲���
	void CSLF(float thetaDeg, float speed, double gamma);//��СԼ�������˲���
	void WienerFilterTur(float k, float K);//�������˻�����ά���˲�����

	//��ɫͼ����
	void CreateRGBTestPage(int width, int height); //����RGB����ͼ��
	void CalculateHue();//����ɫ��
	void CalculateSaturation();//���㱥�Ͷ�
	void CalculateIntensity();//��������
	void RGB2HSI();//RGBתHSI
	void HSI2RGB();//HSIתRGB
	void HSIHistogramEqualization();//��ɫֱ��ͼ����
	void ApplyPseudoColor(COLORREF startColor, COLORREF endColor);//����α��ͼ��

	//��̬ѧ�任
	void ConvertToBinary(double threshold); //��ֵ��
	void Erosion(int kernelSize); //��ʴ
	void Dilation(int kernelSize); //����
	void HoleFill(); //�׶����
	void Edge();//��Ե���
	void Outline();//�������

	//opencv��ʵ����̬ѧ�任����
	void BinaryColor_cv(double thresh);//��ֵ��
	void Erosion_cv(int KernelSize);//��ʴ
	void Dilation_cv(int kernelSize);//����
	void Invert_cv();//��
	void HoleFill_cv();//�Զ���㷨
	void EdgeByErosion_cv(int kernelSize);//��ʴ�ƽ��ı�Ե���
	void outline_cv();//���

public:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char* m_pDibBits;

public:
	// ��һ����ֵ����λ��������Ӧ����¼�
	vector<vector<unsigned char>> normMagnitudeMap;  // ��һ����ֵ (0~255)
	vector<vector<unsigned char>> normPhaseMap;      // ��һ����λ (0~255)
	
private:
	long *m_pGrayValueCount;
	vector<float> GenerateGaussianKernel(int sigma);
};
